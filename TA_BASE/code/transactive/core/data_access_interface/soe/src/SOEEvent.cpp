#include "SOEEvent.h"

namespace TA_Base_Core
{
	const std::string SOEEvent::COL_NAME_PKEY = "PKEY";
	const std::string SOEEvent::COL_NAME_SOURCE_TIME = "SOURCE_TIME";
	const std::string SOEEvent::COL_NAME_TRANSACTIVE_TIME = "TRANSACTIVE_TIME";
	const std::string SOEEvent::COL_NAME_ENTITY_KEY = "ENTITY_KEY";
	const std::string SOEEvent::COL_NAME_ASSET_NAME = "ASSET_NAME";
	const std::string SOEEvent::COL_NAME_VALUE_DATA = "VALUE_DATA";
	const std::string SOEEvent::COL_NAME_DISPLAY_VALUE = "DISPLAY_VALUE";
	const std::string SOEEvent::COL_NAME_LOCATION_KEY = "LOCATION_KEY";
	const std::string SOEEvent::COL_NAME_LOCATION_NAME = "LOCATION_NAME";
	const std::string SOEEvent::COL_NAME_DESCRIPTION="DESCRIPTION";

	SOEEvent::EnumColumnID SOEEvent::m_eSortColumn = SOEEvent::COL_ID_PKEY;
	bool SOEEvent::m_bSortAsc = true;

	SOEEvent::SOEEvent()
		: m_nPKey( 0 )
		, m_nEntityKey( 0 )
		, m_strAssetName("")
		, m_strValueData("")
		, m_strDispValue("")
		, m_nLocationKey( 0 )
		, m_strLocationName("")
		, m_isNew( true )
		, m_description("")
	{
		m_timeSource.time = 0;
		m_timeSource.millitm = 0;
		m_timeSource.dstflag = -1;
		m_timeSource.timezone = -1;
		
		m_timeTransactive.time = 0;
		m_timeTransactive.millitm = 0;
		m_timeTransactive.dstflag = -1;
		m_timeTransactive.timezone = -1;
	}


	SOEEvent::SOEEvent( unsigned long nPKey )
		: m_nPKey( nPKey )
		, m_nEntityKey( 0 )
		, m_strAssetName("")
		, m_strValueData("")
		, m_strDispValue("")
		, m_nLocationKey( 0 )
		, m_strLocationName("")
		, m_isNew( false )
		, m_description("")
	{
		m_timeSource.time = 0;
		m_timeSource.millitm = 0;
		m_timeSource.dstflag = -1;
		m_timeSource.timezone = -1;
		
		m_timeTransactive.time = 0;
		m_timeTransactive.millitm = 0;
		m_timeTransactive.dstflag = -1;
		m_timeTransactive.timezone = -1;
	}

	SOEEvent::~SOEEvent()
	{
	}
	

	unsigned long SOEEvent::getPKey() const
	{
		return m_nPKey;
	}
	
	void SOEEvent::setPKey(const unsigned long key)
	{
		m_nPKey = key;
	}
	
	timeb SOEEvent::getSourceTime() const
	{
		return m_timeSource;
	}
	
	void SOEEvent::setSourceTime( const timeb& t )
	{
		m_timeSource = t;
	}
	
	timeb SOEEvent::getTransactiveTime() const
	{
		return m_timeTransactive;
	}
	
	void SOEEvent::setTransactiveTime( const timeb& t )
	{
		m_timeTransactive = t;
	}
	
	unsigned long SOEEvent::getEntityKey() const
	{
		return m_nEntityKey;
	}
	
	void SOEEvent::setEntityKey( const unsigned long key )
	{
		m_nEntityKey = key;
	}
	
	std::string SOEEvent::getAssetName() const
	{
		return m_strAssetName;
	}
	
	void SOEEvent::setAssetName( const std::string& name )
	{
		m_strAssetName = name.c_str();
	}
	
	std::string SOEEvent::getValueData() const
	{
		return m_strValueData;
	}
	
	void SOEEvent::setValueData( const std::string& value )
	{
		m_strValueData = value.c_str();
	}
	
	std::string SOEEvent::getDisplayValue() const
	{
		return m_strDispValue;
	}
	
	void SOEEvent::setDisplayValue( const std::string& value )
	{
		m_strDispValue = value.c_str();
	}
	
	unsigned long SOEEvent::getLocationKey() const
	{
		return m_nLocationKey;
	}
	
	void SOEEvent::setLocationKey( const unsigned long key )
	{
		m_nLocationKey = key;
	}
	
	std::string SOEEvent::getLocationName() const
	{
		return m_strLocationName;
	}
	
	void SOEEvent::setLocationName( const std::string& location )
	{
		m_strLocationName = location.c_str();
	}




	bool SOEEvent::operator<( const SOEEvent& evt ) const
	{
		switch( m_eSortColumn )
		{

		case COL_ID_PKEY:
		default:
			return m_bSortAsc?m_nPKey < evt.m_nPKey : m_nPKey >= evt.m_nPKey;
		}
		return true;
	}


	void SOEEvent::setSortStyle(EnumColumnID eCol, bool bAsc /* = true */)
	{
		m_eSortColumn = COL_ID_PKEY;
		m_bSortAsc = bAsc;
	}

 

	bool SOEEvent::isNew()
	{
		return m_isNew;
	}

	std::string SOEEvent::getDescription() const
	{
         return m_description;
	}
	void SOEEvent::setDescription( const std::string& description )
	{
         m_description = description.c_str();
	}


 	SOEEvent::SOEEvent(SOEEvent& ev)
	{		 
		m_nPKey= ev.getPKey();
		m_timeSource=ev.getSourceTime();
		m_timeTransactive=ev.getTransactiveTime();
		m_nEntityKey=ev.getEntityKey();
		m_strAssetName=ev.getAssetName().c_str();
		m_strValueData=ev.getValueData().c_str();
		m_strDispValue=ev.getDisplayValue().c_str();
		m_nLocationKey=ev.getLocationKey();
		m_strLocationName=ev.getLocationName().c_str();		
		m_description=ev.getDescription().c_str();
		m_isNew=ev.isNew();

	}
}
