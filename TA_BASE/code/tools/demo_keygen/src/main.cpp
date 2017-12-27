#include "stdafx.h"
#include "DemoKeygen.h"
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")


int _tmain(int argc, _TCHAR* argv[])
{
    HRESULT hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
    ATLENSURE_SUCCEEDED(hr);

    boost::program_options::options_description desc( "Options", 120 );
    desc.add_options()
        ("help,?", "produce help message")
        ("serial-number,N", boost::program_options::value<std::string>(), "using given number instead of querying system info")
        ("key-file,F", boost::program_options::value<std::string>(), "key file path, default = c:\\transactive\\bin\\demo_key")
        ;

    boost::program_options::variables_map vm;
    boost::program_options::store( boost::program_options::parse_command_line( argc, argv, desc ), vm );
    boost::program_options::notify(vm);

    if ( vm.count( "help" ) )
    {
        std::stringstream strm;
        desc.print( strm );
        ::MessageBox( NULL, strm.str().c_str(), "Keygen", MB_OK | MB_ICONINFORMATION );
        return 0;
    }

    std::string serial_number;
    std::string key_file = "C:\\transactive\\bin\\demo_key";

    if ( vm.count( "serial-number" ) )
    {
        serial_number = vm["serial-number"].as<std::string>();
    }

    if ( vm.count( "key-file" ) )
    {
        key_file = vm["key-file"].as<std::string>();
    }

    try
    {
        DemoKeygen keygen( serial_number );
        std::string key = keygen.generate_key();
        keygen.save_key_to_file( key, key_file );

        std::stringstream strm;
        strm << "Successful!\n\n" << key_file;
        ::MessageBox( NULL, strm.str().c_str(), "Keygen", MB_OK | MB_ICONINFORMATION );
    }
    catch ( std::exception& e )
    {
        ::MessageBox( NULL, e.what(), "Keygen", MB_OK | MB_ICONERROR );
    }
    catch ( ... )
    {
        ::MessageBox( NULL, "Unknown error", "Keygen", MB_OK | MB_ICONERROR );
    }

    ::CoUninitialize();
    return 0;
}
