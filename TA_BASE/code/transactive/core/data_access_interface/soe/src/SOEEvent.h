#ifndef SOE_EVENT_INCLUDED
#define SOE_EVENT_INCLUDED

#include "IEventData.h"

namespace TA_Base_Core
{
	class SOEEvent : public IEventData
	{
	public:
		

        


		enum EnumColumnID
		{
			COL_ID_PKEY,
			COL_ID_SOURCE_TIME,
			COL_ID_ENTITY_KEY,
			COL_ID_ASSET_NAME
		};

		static const std::string COL_NAME_PKEY;
		static const std::string COL_NAME_SOURCE_TIME;
		static const std::string COL_NAME_TRANSACTIVE_TIME;
		static const std::string COL_NAME_ENTITY_KEY;
		static const std::string COL_NAME_ASSET_NAME;
		static const std::string COL_NAME_VALUE_DATA;
		static const std::string COL_NAME_DISPLAY_VALUE;
		static const std::string COL_NAME_LOCATION_KEY;
		static const std::string COL_NAME_LOCATION_NAME;
		static const std::string COL_NAME_DESCRIPTION;



		SOEEvent();
		SOEEvent( unsigned long nPKey );
		virtual ~SOEEvent();

		// property
		virtual unsigned long getPKey() const;
		virtual void setPKey( const unsigned long key );

		virtual timeb getSourceTime() const;
		virtual void setSourceTime( const timeb& t );

		virtual timeb getTransactiveTime() const;
		virtual void setTransactiveTime( const timeb& t );

		virtual unsigned long getEntityKey() const;
		virtual void setEntityKey( const unsigned long key );

		virtual std::string getAssetName() const;
		virtual void setAssetName( const std::string& name );
		
		virtual std::string getValueData() const;
		virtual void setValueData( const std::string& value );

		virtual std::string getDisplayValue() const;
		virtual void setDisplayValue( const std::string& value );

		virtual unsigned long getLocationKey() const;
		virtual void setLocationKey( const unsigned long key );

		virtual std::string getLocationName() const;
		virtual void setLocationName( const std::string& location );

		bool operator<( const SOEEvent& evt ) const;
		static void setSortStyle(EnumColumnID eCol = COL_ID_PKEY, bool bAsc = true);

		virtual std::string getDescription() const;
		virtual void setDescription( const std::string& description );


		// method
		bool isNew();

		SOEEvent(SOEEvent& ev);
	private:
		unsigned long m_nPKey;
		timeb m_timeSource;
		timeb m_timeTransactive;
		unsigned long m_nEntityKey;
		std::string m_strAssetName;
		std::string m_strValueData;
		std::string m_strDispValue;
		unsigned long m_nLocationKey;
		std::string m_strLocationName;
		std::string m_description;

		// sort style related
		static EnumColumnID m_eSortColumn;
		static bool m_bSortAsc;

		// attribute
		bool m_isNew;
	};
}



#endif // #ifndef SOE_EVENT_INCLUDED