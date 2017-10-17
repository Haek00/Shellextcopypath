#include "Precompiled.h"

using namespace Microsoft::WRL;

extern "C" {

	HRESULT __stdcall DllGetClassObject(const IID &rclsid, const IID &riid, void **ppv)
	{
		return Module<InProc>::GetModule().GetClassObject(rclsid, riid, ppv);
	}

	HRESULT __stdcall DllCanUnloadNow()
	{
		return Module<InProc>::GetModule().Terminate() ? S_OK : S_FALSE;
	}

	BOOL __stdcall DllMain(HINSTANCE hinst, DWORD reason, void *)
	{
		if (reason == DLL_PROCESS_ATTACH) {
			DisableThreadLibraryCalls(hinst);
		}
		return TRUE;
	}
}
