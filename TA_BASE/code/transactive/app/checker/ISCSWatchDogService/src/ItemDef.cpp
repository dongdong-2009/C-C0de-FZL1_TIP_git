// ItemDef.cpp: implementation of the CItemDef class.
//
//////////////////////////////////////////////////////////////////////

#include "ItemDef.h"
#include "opcda.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemDef::CItemDef() :
m_csName( "" ),
m_csPath( "" ),
m_hItem( INVALID_HANDLE_VALUE ),
m_dwAccessRights( 1 )
{
	VariantInit( &m_value );
	//	CoFileTimeNow( &TimeStamp );
	m_timestamp.dwHighDateTime = 0;
	m_timestamp.dwLowDateTime = 0;
	m_dwQuality = OPC_QUALITY_BAD;
}

CItemDef::~CItemDef()
{
	VariantClear( &m_value );
}
