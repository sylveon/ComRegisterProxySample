#include <iostream>
#include "../ComProxyStub/IDoSomethingOnTheServer_h.h"
#include "winrt/base.h"

struct ServerObject : winrt::implements<ServerObject, IDoSomethingOnTheServer>
{
    HRESULT STDMETHODCALLTYPE Add(int a, int b, int* result) override try
    {
        std::cout << "The client asked us to add " << a << " and " << b << '\n';

        *result = a + b;

        return S_OK;
    }
    catch (...)
    {
        return winrt::to_hresult();
    }
};

struct Factory : winrt::implements<Factory, IClassFactory>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override try
    {
        *ppvObject = nullptr;

        if (!pUnkOuter)
        {
            return winrt::make<ServerObject>().as(riid, ppvObject);
        }
        else
        {
            return CLASS_E_NOAGGREGATION;
        }
    }
    catch (...)
    {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override
    {
        return S_OK;
    }
};

// {AC4A1DFF-C7E5-46CB-8A69-5ACEBFDC7BAD}
static const GUID CLSID_ServerObject =
{ 0xac4a1dff, 0xc7e5, 0x46cb, { 0x8a, 0x69, 0x5a, 0xce, 0xbf, 0xdc, 0x7b, 0xad } };


int main()
{
    winrt::check_hresult(CoInitialize(nullptr));


    DWORD objectCookie;
    winrt::check_hresult(CoRegisterClassObject(CLSID_ServerObject, winrt::make<Factory>().get(), CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &objectCookie));
    std::cout << "Server object registered\n";
    
    HINSTANCE lib = LoadLibrary(L"ComProxyStub.dll");
    winrt::check_bool(lib);

    auto getClassObject = reinterpret_cast<decltype(&DllGetClassObject)>(GetProcAddress(lib, "DllGetClassObject"));
    winrt::check_bool(getClassObject);

    // by default, the UUID of the PSFactory is the one of the first interface encountered by MIDL - in this case our only interface.
    winrt::com_ptr<IUnknown> psFactory;
    winrt::check_hresult(getClassObject(__uuidof(IDoSomethingOnTheServer), IID_PPV_ARGS(psFactory.put())));

    DWORD psCookie;
    winrt::check_hresult(CoRegisterClassObject(__uuidof(IDoSomethingOnTheServer), psFactory.get(), CLSCTX_INPROC_SERVER, REGCLS_MULTIPLEUSE, &psCookie));
    winrt::check_hresult(CoRegisterPSClsid(__uuidof(IDoSomethingOnTheServer), __uuidof(IDoSomethingOnTheServer)));
    std::cout << "Proxy/stub registered\n";

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
