#ifdef DEMO_VERSION

#include "DemoKeygen.h"
#include <atlbase.h>
#include <atlstr.h>
#include <comutil.h>
#include <wbemidl.h>
#include <Wincrypt.h>
#include <sstream>
#include <fstream>
#pragma comment(lib, "wbemuuid.lib")


DemoKeygen::DemoKeygen( const std::string& serial_number )
    : m_serial_number( serial_number)
{
    HRESULT hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
    ATLENSURE_SUCCEEDED(hr);
}


DemoKeygen::~DemoKeygen()
{
    ::CoUninitialize();
}


std::string DemoKeygen::generateKey()
{
    if ( m_serial_number.empty() )
    {
        m_serial_number = getPhysicalDriveSerialNumber();
    }

    return md5( createMachineCode( m_serial_number ) );
}


std::string DemoKeygen::getPhysicalDriveSerialNumber( int drive_number )
{
    try
    {
        // Format physical drive path (may be '\\.\PhysicalDrive0', '\\.\PhysicalDrive1' and so on).
        CString strDrivePath;
        strDrivePath.Format( _T("\\\\.\\PhysicalDrive%u"), drive_number );

        // 2. Set the default process security level
        // http://msdn.microsoft.com/en-us/library/windows/desktop/aa393617(v=vs.85).aspx
        HRESULT hr =  ::CoInitializeSecurity(
            NULL,                        // Security descriptor
            -1,                          // COM negotiates authentication service
            NULL,                        // Authentication services
            NULL,                        // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication level for proxies
            RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation level for proxies
            NULL,                        // Authentication info
            EOAC_NONE,                   // Additional capabilities of the client or server
            NULL);                       // Reserved
        ATLENSURE_SUCCEEDED(hr);

        // 3. Create a connection to WMI namespace
        // http://msdn.microsoft.com/en-us/library/windows/desktop/aa389749(v=vs.85).aspx

        // 3.1. Initialize the IWbemLocator interface
        CComPtr<IWbemLocator> pIWbemLocator;
        hr = ::CoCreateInstance( CLSID_WbemLocator, 0,  CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pIWbemLocator );
        ATLENSURE_SUCCEEDED(hr);

        // 3.2. Call IWbemLocator::ConnectServer for connecting to WMI
        CComPtr<IWbemServices> pIWbemServices;
        hr = pIWbemLocator->ConnectServer( L"ROOT\\CIMV2", NULL, NULL, 0, NULL, 0, 0, &pIWbemServices );
        ATLENSURE_SUCCEEDED(hr);

        // 4. Set the security levels on WMI connection
        // http://msdn.microsoft.com/en-us/library/windows/desktop/aa393619(v=vs.85).aspx
        hr = ::CoSetProxyBlanket(
            pIWbemServices,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            NULL,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE);
        ATLENSURE_SUCCEEDED(hr);

        // 5. Execute a WQL (WMI Query Language) query to get physical media info
        const BSTR szQueryLanguage = L"WQL";
        const BSTR szQuery =  L"SELECT Tag, SerialNumber FROM Win32_PhysicalMedia";
        CComPtr<IEnumWbemClassObject> pIEnumWbemClassObject;
        hr = pIWbemServices->ExecQuery(
            szQueryLanguage,                                       // Query language
            szQuery,                                               // Query
            WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,   // Flags
            NULL,                                                  // Context
            &pIEnumWbemClassObject);                               // Enumerator
        ATLENSURE_SUCCEEDED(hr);

        // 6. Get each enumerator element until find the desired physical drive
        ULONG uReturn = 0;

        while ( pIEnumWbemClassObject )
        {
            CComPtr<IWbemClassObject> pIWbemClassObject;
            hr = pIEnumWbemClassObject->Next( WBEM_INFINITE, 1, &pIWbemClassObject, &uReturn );

            if ( 0 == uReturn || FAILED(hr) )
            {
                break;
            }

            variant_t vtTag;           // unique tag, e.g. '\\.\PHYSICALDRIVE0'
            variant_t vtSerialNumber;  // manufacturer-provided serial number

            hr = pIWbemClassObject->Get( L"Tag", 0, &vtTag, NULL, NULL );
            ATLENSURE_SUCCEEDED(hr);

            CString strTag( vtTag.bstrVal );

            if ( !strTag.CompareNoCase(strDrivePath) ) // physical drive found
            {
                hr = pIWbemClassObject->Get( L"SerialNumber", 0, &vtSerialNumber, NULL, NULL );
                ATLENSURE_SUCCEEDED(hr);
                CString strSerialNumber;
                strSerialNumber = vtSerialNumber.bstrVal; // get the serial number
                strSerialNumber.Trim();
                return (const char*)strSerialNumber;
            }
        }
    }
    catch ( ... )
    {
        throw std::runtime_error( "failed to get physical drive serial number." );
    }

    return "";
}


std::string DemoKeygen::createMachineCode( const std::string& serial_number )
{
    return "TRANSACTIVE-MACHINE-CODE: " + serial_number;
}


std::string DemoKeygen::getKeyFromFile( const std::string& file_name )
{
    std::ifstream is( file_name.c_str() );
    std::stringstream strm;

    if ( !is )
    {
        strm << "can not open '" << file_name << "' for reading.";
        throw std::runtime_error( strm.str() );
    }

    strm << is.rdbuf();
    return strm.str();
}


std::string DemoKeygen::md5( const std::string& str )
{
    const size_t MD5LEN = 16;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = MD5LEN;
    CHAR rgbDigits[] = "0123456789abcdef";
    std::stringstream strm;

    if ( CryptAcquireContext( &hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) &&
         CryptCreateHash( hProv, CALG_MD5, 0, 0, &hHash ) &&
         CryptHashData( hHash, (BYTE*)str.c_str(), str.size(), 0 ) &&
         CryptGetHashParam( hHash, HP_HASHVAL, rgbHash, &cbHash, 0 ) )
    {
        for ( size_t i = 0; i < cbHash; i++ )
        {
            strm << rgbDigits[rgbHash[i] >> 4] << rgbDigits[rgbHash[i] & 0xf];
        }
    }

    if ( hHash )
    {
        CryptDestroyHash(hHash);
    }

    if ( hProv )
    {
        CryptReleaseContext(hProv, 0);
    }

    std::string md5_key = strm.str();

    if ( md5_key.empty() )
    {
        throw std::runtime_error( "failed to create md5 key" );
    }

    return md5_key;
}

#endif
