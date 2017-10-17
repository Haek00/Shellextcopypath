#pragma once

class __declspec(uuid("60C0B5BE-6CB0-46B5-AB72-3E6F1A3D8F24"))
	ShellExtension : public Microsoft::WRL::RuntimeClass<
		Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
		IExecuteCommand, IInitializeCommand, IObjectWithSelection> {
public:
	ShellExtension() = default;
	HRESULT __stdcall SetKeyState(DWORD grfKeyState) override;
	HRESULT __stdcall SetParameters(LPCWSTR pszParameters) override;
	HRESULT __stdcall SetPosition(POINT pt) override;
	HRESULT __stdcall SetShowWindow(int nShow) override;
	HRESULT __stdcall SetNoShowUI(BOOL fNoShowUI) override;
	HRESULT __stdcall SetDirectory(LPCWSTR pszDirectory) override;
	HRESULT __stdcall Execute() override;
	HRESULT __stdcall SetSelection(IShellItemArray *psia) override;
	HRESULT __stdcall GetSelection(const IID &riid, void **ppv) override;
	HRESULT __stdcall Initialize(LPCWSTR pszCommandName, IPropertyBag *ppb) override;

private:
	Microsoft::WRL::ComPtr<IShellItemArray> _pItems;
};

CoCreatableClass(ShellExtension);