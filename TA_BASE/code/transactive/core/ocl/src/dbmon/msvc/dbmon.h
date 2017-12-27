
#pragma once
#pragma pack(push, 8)

#include <comdef.h>

//
// Forward references and typedefs
//

struct TSQLParam;
struct TMonitorMessage;
typedef struct TSQLParam * PSQLParam;
struct __declspec(uuid("89f49e64-f6e0-11d6-9038-00c02631bdc7"))
/* interface */ IDBMonitor;
struct /* coclass */ DBMonitor;

//
// Smart pointer typedef declarations
//

_COM_SMARTPTR_TYPEDEF(IDBMonitor, __uuidof(IDBMonitor));

//
// Type library items
//

enum TEventType
{
    et_Connect = 2,
    et_Disconnect = 3,
    et_Commit = 4,
    et_Rollback = 5,
    et_Prepare = 6,
    et_Unprepare = 7,
    et_Execute = 8,
    et_Blob = 9,
    et_Error = 10,
    et_Misc = 11
};

enum TTracePoint
{
    tp_BeforeEvent = 0,
    tp_AfterEvent = 1
};

struct TSQLParam
{
    LPSTR Name;
    LPSTR DataType;
    LPSTR Paramtype;
    LPSTR Value;
};

struct TMonitorMessage
{
    long MessageID;
    enum TEventType EventType;
    enum TTracePoint TracePoint;
    long ObjectID;
    long OwnerID;
    LPSTR ObjectName;
    LPSTR OwnerName;
    LPSTR Description;
};

struct __declspec(uuid("89f49e64-f6e0-11d6-9038-00c02631bdc7"))
IDBMonitor : IUnknown
{
    //
    // Wrapper methods for error-handling
    //

    HRESULT GetVersion (
        LPSTR * pVersion );
    HRESULT SetCaption (
        LPSTR Caption );
    HRESULT IsMonitorActive (
        long * Active );
    HRESULT OnEvent (
        struct TMonitorMessage * Msg,
        LPSTR ParamStr );
    HRESULT OnExecute (
        struct TMonitorMessage * Msg,
        LPSTR SQL,
        struct TSQLParam Params[1000],
        long ParamCount,
        long RowsAffected );

    //
    // Raw methods provided by interface
    //

    virtual HRESULT __stdcall raw_GetVersion (
        LPSTR * pVersion ) = 0;
    virtual HRESULT __stdcall raw_SetCaption (
        LPSTR Caption ) = 0;
    virtual HRESULT __stdcall raw_IsMonitorActive (
        long * Active ) = 0;
    virtual HRESULT __stdcall raw_OnEvent (
        struct TMonitorMessage * Msg,
        LPSTR ParamStr ) = 0;
    virtual HRESULT __stdcall raw_OnExecute (
        struct TMonitorMessage * Msg,
        LPSTR SQL,
        struct TSQLParam Params[1000],
        long ParamCount,
        long RowsAffected ) = 0;
};

struct __declspec(uuid("89f49e65-f6e0-11d6-9038-00c02631bdc7"))
DBMonitor;
    // [ default ] interface IDBMonitor

//
// Named GUID constants initializations
//

extern "C" const GUID __declspec(selectany) LIBID_DBMonitorLib =
    {0x89f49e68,0xf6e0,0x11d6,{0x90,0x38,0x00,0xc0,0x26,0x31,0xbd,0xc7}};
extern "C" const GUID __declspec(selectany) IID_IDBMonitor =
    {0x89f49e64,0xf6e0,0x11d6,{0x90,0x38,0x00,0xc0,0x26,0x31,0xbd,0xc7}};
extern "C" const GUID __declspec(selectany) CLSID_DBMonitor =
    {0x89f49e65,0xf6e0,0x11d6,{0x90,0x38,0x00,0xc0,0x26,0x31,0xbd,0xc7}};

//
// Wrapper method implementations
//

#include "dbmon.inc"

#pragma pack(pop)
