
#pragma once

//
// interface IDBMonitor wrapper method implementations
//

inline HRESULT IDBMonitor::GetVersion ( LPSTR * pVersion ) {
    HRESULT _hr = raw_GetVersion(pVersion);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}

inline HRESULT IDBMonitor::SetCaption ( LPSTR Caption ) {
    HRESULT _hr = raw_SetCaption(Caption);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}

inline HRESULT IDBMonitor::IsMonitorActive ( long * Active ) {
    HRESULT _hr = raw_IsMonitorActive(Active);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}

inline HRESULT IDBMonitor::OnEvent ( struct TMonitorMessage * Msg, LPSTR ParamStr ) {
    HRESULT _hr = raw_OnEvent(Msg, ParamStr);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}

inline HRESULT IDBMonitor::OnExecute ( struct TMonitorMessage * Msg, LPSTR SQL, struct TSQLParam Params[1000], long ParamCount, long RowsAffected ) {
    HRESULT _hr = raw_OnExecute(Msg, SQL, Params, ParamCount, RowsAffected);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}
