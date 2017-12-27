#ifndef __OTL_CONF_H__
#define __OTL_CONF_H__



//////////////////////////////////////////////////////////////////////////
#define USING_OTL
//#define OTL_ORA10G // Compile OTL 4/OCI10
#define OTL_ORA11G
//#define OTL_ODBC // Compile OTL ODBC    odbc32.lib odbccp32.lib
#define OTL_STREAM_READ_ITERATOR_ON
#define OTL_STL

#if (defined(OTL_ODBC))
	#if (defined(WIN32))
		#pragma comment(lib,"odbc32.lib")
		#pragma comment(lib,"odbccp32.lib")
	#else
		#define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
	#endif
#endif
//////////////////////////////////////////////////////////////////////////


#endif//__OTL_CONF_H__



