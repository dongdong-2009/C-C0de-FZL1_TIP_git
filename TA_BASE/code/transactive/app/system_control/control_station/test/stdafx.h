// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

// TODO: reference additional headers your program requires here
// TODO: reference additional headers your program requires here
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
//#include <boost/regex.hpp>
//#include <boost/filesystem.hpp>
//#include <boost/tokenizer.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/foreach.hpp>
//#include <boost/assign.hpp>
//#include <boost/preprocessor.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
//#include <boost/thread/latch.hpp>
//#include <boost/thread/barrier.hpp>
//#include <boost/thread/executors/basic_thread_pool.hpp>
//#include <boost/any.hpp>
//#include <boost/utility.hpp>
//#include <boost/type_traits.hpp>
#include <boost/format.hpp>
//#include <boost/function_types/result_type.hpp>
//#include <boost/signals2.hpp>
//#include <boost/tuple/tuple.hpp>
//#include <boost/tuple/tuple_comparison.hpp>
//#include <boost/tuple/tuple_io.hpp>
//#include <boost/algorithm/cxx11/one_of.hpp>
//#include <boost/algorithm/cxx11/none_of.hpp>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <set>
#include <algorithm>
#include <list>
#include <fstream>
