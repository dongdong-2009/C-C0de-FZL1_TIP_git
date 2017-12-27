#pragma once

#import "msxml3.dll"
using namespace MSXML2;


#define XMLVAL_TRUE				_T("true")
#define XMLVAL_FALSE 			_T("false")

#define XMLNODE_ROOT			_T("Library")

// Messages
#define XMLNODE_MSG_ROOT		_T("Messages")
#define XMLNODE_MSG				_T("Message")
#define XMLNODE_MSG_OBSOLETE	_T("Obsolete")
#define XMLNODE_MSG_AUDIT		_T("AuditEvent")
#define XMLNODE_MSG_TYPE		_T("MessageType")
#define XMLNODE_MSG_ID			_T("ID")
#define XMLNODE_MSG_RESID       _T("ResID")
#define XMLNODE_MSG_HELPID		_T("HelpID")
#define XMLNODE_MSG_STYLE		_T("MsgBoxStyle")
#define XMLNODE_MSG_STRING		_T("String")
#define XMLNODE_MSG_LOCALE		_T("Locale")
#define XMLNODE_MSG_PARAM		_T("Parameter")

#define XPATH_MSG_USING_ID			_T("Message[ID = %ld]")
#define XPATH_MSG_USING_RESID       _T("Message[ResID = \"%s\"]")
#define XPATH_MSG_ACTIVE_MSGS		_T("Message[@Obsolete = \"false\"]")
#define XPATH_MSG_STR_USING_LOCALE	_T("String[@Locale = \"%d\"]")
#define XPATH_MSG_PARAMS_USING_ID	_T("Message/Parameter[../ID = %ld]")
#define XPATH_MSG_OBSOLETE_USING_ID _T("Message/@Obsolete[../ID = %ld]")
#define XPATH_MSG_TYPE_USING_ID		_T("Message/MessageType[../ID = %ld]")
#define XPATH_MSG_HELPID_USING_ID	_T("Message/HelpID[../ID = %ld]")
#define XPATH_MSG_STYLE_USING_ID	_T("Message/MsgBoxStyle[../ID = %ld]")
#define XPATH_MSG_STR_USING_ID_LOCALE	_T("Message/String[../ID = %ld and @Locale = \"%d\"]")
#define XPATH_MSG_MSG_IDS_USING_PARAMS	_T("Message/ID[../Parameter = \"%s\"]")

// Parameters
#define XMLNODE_PARAM_ROOT		_T("Parameters")
#define XMLNODE_PARAM			_T("ParameterDef")
#define XMLNODE_PARAM_TYPE		_T("Type")
#define XMLNODE_PARAM_OBSOLETE	_T("Obsolete")
#define XMLNODE_PARAM_NAME		_T("Name")
#define XMLNODE_PARAM_DESC		_T("Description")

#define XPATH_PARAM_USING_NAME	_T("ParameterDef[Name = \"%s\"]")

//Log
//#define XMLNODE_LOG_ROOT		_T("ChangeLog")
//#define XMLNODE_LOG				_T("Revision")
//#define XMLNODE_LOG_DATE		_T("UpdatedDate")
//#define XMLNODE_LOG_USER		_T("UpdatedUserID")
//#define XMLNODE_LOG_ENTRY		_T("Comments")

// Resource Source
#define XMLNODE_SRC_ROOT		_T("ResourceSource")

#define XMLNODE_SRC_RC			_T("RCSource")
#define XMLNODE_SRC_HEADER		_T("ResourceHeaderSource")
#define XMLNODE_SRC_STRING_TABLE _T("StringResourceSegment")

#define XMLNODE_SRC_DATA		_T("Data")
#define XMLNODE_SRC_MFCVERSION	_T("MFCVersion")

#define XMLNODE_RC_SRC_IDENTIFIERS	_T("ResourceIdentifiers")
#define XMLNODE_RC_SRC_IDENTIFIER	_T("ResourceIdentifier")
#define XMLNODE_RC_SRC_Locale		_T("Locale")
#define XMLNODE_RC_SRC_NAME			_T("Name")
#define XMLNODE_RC_SRC_DEFINITION	_T("Definition")
#define XMLNODE_RC_SRC_CODEPAGE		_T("CodePage")
#define XMLNODE_RC_SRC_AFXTARGET	_T("AfxTarget")

#define XPATH_RC_SRC_IDENTIFIER		_T("ResourceIdentifier[@Locale = \"%d\"]")

// Place Markers
#define TAG_ENA_LANG_INSERT			_T("<ENA_LANG_INSERT_PLACEMARKER>")
#define TAG_NEW_LANG_INSERT			_T("<NEW_LANG_INSERT_PLACEMARKER>")
#define TAG_INSERT_DEFINES			_T("<INSERT_DEFINES_PLACEMARKER>")
#define TAG_LANG_DEFINITION			_T("<LANG_DEFINITION>")
#define TAG_LANG_NAME				_T("<LANG_NAME>")
#define TAG_LANG_CODE_PAGE			_T("<LANG_CODE_PAGE>")
#define TAG_AFX_TARGET				_T("<AFX_TARGET>")
