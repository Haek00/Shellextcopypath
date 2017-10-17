#include "Precompiled.h"
#include "handle.h"

using namespace Handle;

typedef invalid_handle Transaction;
extern "C" IMAGE_DOS_HEADER __ImageBase;

Transaction CreateTransaction()
{
	return Transaction(CreateTransaction(nullptr, // default security descriptor
		nullptr, // reserved
		TRANSACTION_DO_NOT_PROMOTE,
		0, // reserved
		0, // reserved
		INFINITE,
		nullptr)); // description
}

struct RegistryKeyTraits {
	typedef HKEY pointer;

	static pointer invalid() throw()
	{
		return nullptr;
	}

	static void close(pointer value) throw()
	{
		HANDLE_VERIFY_(ERROR_SUCCESS, RegCloseKey(value))		;
	}
};

typedef unique_handle<RegistryKeyTraits> RegistryKey;

RegistryKey CreateRegistryKey(HKEY key,
	wchar_t const *path,
	Transaction const &transaction,
	REGSAM access)
{
	HKEY handle = nullptr;

	auto result = RegCreateKeyTransacted(key,
		path,
		0, // reserved
		nullptr, // class
		REG_OPTION_NON_VOLATILE,
		access,
		nullptr, // default security descriptor
		&handle,
		nullptr, // disposition
		transaction.get(),
		nullptr); // reserved

	if (ERROR_SUCCESS != result) {
		SetLastError(result);
	}

	return RegistryKey(handle);
}

RegistryKey OpenRegistryKey(HKEY key,
	wchar_t const *path,
	Transaction const &transaction,
	REGSAM access)
{
	HKEY handle = nullptr;

	auto result = RegOpenKeyTransacted(key,
		path,
		0, // reserved
		access,
		&handle,
		transaction.get(),
		nullptr); // reserved

	if (ERROR_SUCCESS != result) {
		SetLastError(result);
	}

	return RegistryKey(handle);
}

enum class EntryOption {
	None,
	Delete,
	FileName,
};

struct Entry {
	wchar_t const *Path;
	EntryOption Option;

	wchar_t const *Name;
	wchar_t const *Value;
};

static Entry Table[] =
{
	{
		L"Software\\Classes\\CLSID\\{60C0B5BE-6CB0-46B5-AB72-3E6F1A3D8F24}",
		EntryOption::Delete,
		nullptr,
		L"Copy Path"
	},
	{
		L"Software\\Classes\\CLSID\\{60C0B5BE-6CB0-46B5-AB72-3E6F1A3D8F24}\\InprocServer32",
		EntryOption::FileName
	},
	{
		L"Software\\Classes\\CLSID\\{60C0B5BE-6CB0-46B5-AB72-3E6F1A3D8F24}\\InprocServer32",
		EntryOption::None,
		L"ThreadingModel",
		L"Apartment"
	},
	{
		L"Software\\Classes\\*\\shell\\CopyPath",
		EntryOption::Delete,
		nullptr,
		L"Copy Path"
	},
	{
		L"Software\\Classes\\*\\shell\\CopyPath\\command",
		EntryOption::None,
		L"DelegateExecute",
		L"{60C0B5BE-6CB0-46B5-AB72-3E6F1A3D8F24}"
	},
	{
		L"Software\\Classes\\Directory\\shell\\CopyPath",
		EntryOption::Delete,
		nullptr,
		L"Copy Path"
	},
	{
		L"Software\\Classes\\Directory\\shell\\CopyPath\\command",
		EntryOption::None,
		L"DelegateExecute",
		L"{60C0B5BE-6CB0-46B5-AB72-3E6F1A3D8F24}"
	},
};

bool Unregister(Transaction const &transaction)
{
	for (auto const &entry : Table) {
		if (EntryOption::Delete == entry.Option) {

			auto key = OpenRegistryKey(HKEY_LOCAL_MACHINE,
				entry.Path,
				transaction,
				DELETE | KEY_ALL_ACCESS );

			if (!key) {
				if (ERROR_FILE_NOT_FOUND == GetLastError()) {
					continue;
				}
				return false;
			}
			auto result = RegDeleteTree(HKEY_LOCAL_MACHINE,
				entry.Path); 

			if (ERROR_SUCCESS != result) {
				SetLastError(result);
				return false;
			}
		}
	}

	return true;
}

bool Register(Transaction const &transaction)
{
	if (!Unregister(transaction)) {
		return false;
	}

	wchar_t filename[MAX_PATH];

	auto const length = GetModuleFileName(reinterpret_cast<HMODULE>(&__ImageBase), //s_serverModule,
		filename,
		_countof(filename));

	if (0 == length || _countof(filename) == length) {
		return false;
	}

	for (auto const &entry : Table) {
		auto key = CreateRegistryKey(HKEY_LOCAL_MACHINE,
			entry.Path,
			transaction,
			KEY_WRITE);

		if (!key) {
			return false;
		}

		if (EntryOption::FileName != entry.Option && !entry.Value) {
			continue;
		}

		auto value = entry.Value;

		if (!value) {
			ASSERT(EntryOption::FileName == entry.Option);
			value = filename;
		}

		auto result = RegSetValueEx(key.get(),
			entry.Name,
			0, // reserved
			REG_SZ,
			reinterpret_cast<BYTE const *>(value),
			static_cast<DWORD>(sizeof(wchar_t) * (wcslen(value) + 1)));

		if (ERROR_SUCCESS != result) {
			TRACE(L"RegSetValueEx failed %d\n", result);
			SetLastError(result);
			return false;
		}
	}

	return true;
}

extern "C" {

	HRESULT __stdcall DllRegisterServer()
	{
		auto transaction = CreateTransaction();

		if (!transaction) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (!Register(transaction)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (!CommitTransaction(transaction.get())) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		return S_OK;
	}

	HRESULT __stdcall DllUnregisterServer()
	{
		auto transaction = CreateTransaction();

		if (!transaction) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (!Unregister(transaction)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (!CommitTransaction(transaction.get())) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		return S_OK;
	}
}
