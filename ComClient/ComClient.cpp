#include <iostream>
#include "../ComProxyStub/IDoSomethingOnTheServer_h.h"
#include "winrt/base.h"

// {AC4A1DFF-C7E5-46CB-8A69-5ACEBFDC7BAD}
static const GUID CLSID_ServerObject =
{ 0xac4a1dff, 0xc7e5, 0x46cb, { 0x8a, 0x69, 0x5a, 0xce, 0xbf, 0xdc, 0x7b, 0xad } };

int main()
{
    winrt::check_hresult(CoInitialize(nullptr));

    winrt::com_ptr<IUnknown> serverObject;
    winrt::check_hresult(CoCreateInstance(CLSID_ServerObject, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(serverObject.put())));
    std::cout << "Instance of server object created\n";

    int result;
    winrt::check_hresult(serverObject.as<IDoSomethingOnTheServer>()->Add(5, 5, &result));

    std::cout << "The server says that 5 + 5 = " << result;
}
