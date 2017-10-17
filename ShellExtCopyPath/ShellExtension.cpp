#include "Precompiled.h"
#include "ShellExtension.h"
#include "Clip.h"

using namespace Microsoft::WRL;

HRESULT ShellExtension::SetKeyState(DWORD) { return S_OK; }
HRESULT ShellExtension::SetParameters(LPCWSTR) { return S_OK; }
HRESULT ShellExtension::SetPosition(POINT) { return S_OK; }
HRESULT ShellExtension::SetShowWindow(int) { return S_OK; }
HRESULT ShellExtension::SetNoShowUI(BOOL) { return S_OK; }
HRESULT ShellExtension::SetDirectory(LPCWSTR) { return S_OK; }
HRESULT ShellExtension::Initialize(LPCWSTR, IPropertyBag *) { return S_OK; }

HRESULT ShellExtension::Execute()
{
	COM_EXCEPTION_GUARD_BEGIN

		if (!_pItems) {
			return E_UNEXPECTED;
		}
		ComPtr<IEnumShellItems> pEnumItems;
		HRESULT hr = _pItems->EnumItems(pEnumItems.GetAddressOf());

		if (S_OK != hr) {
			return hr;
		}

		std::wstring s{};
		size_t counter = 0;

		for (ComPtr<IShellItem> pItem; pEnumItems->Next(1, pItem.GetAddressOf(), nullptr) == S_OK; pItem.Reset()) {
			CComHeapPtr<wchar_t> pFilename;
			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pFilename);
			if (S_OK == hr) {
				if (counter > 0)
					s += L" ";
				s += pFilename.m_pData;
			}
			++counter;
		}
		clip::set_text(s);

		return hr;

	COM_EXCEPTION_GUARD_END
}

HRESULT ShellExtension::SetSelection(IShellItemArray *psia)
{
	_pItems = psia;
	return S_OK;
}

HRESULT ShellExtension::GetSelection(const IID &riid, void **ppv)
{
	if (_pItems) {
		return _pItems.CopyTo(riid, ppv);
	}
	*ppv = nullptr;
	return E_NOINTERFACE;
}
