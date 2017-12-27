/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigEquipReport.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigEquipReport is an implementation of the IConfigEquipReport interface
  * to allow access to the EquipReport table
  *
  */

#if !defined(ConfigEquipReport_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ConfigEquipReport_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>

#include "core/data_access_interface/src/IConfigEquipReport.h"

namespace TA_Base_Core
{
	// forward declarations
	class IData;
    class EquipReportHelper;

    class ConfigEquipReport : public IConfigEquipReport
    {
     
	public:
		// constructors
		ConfigEquipReport( );
        ConfigEquipReport( std::string& key );     
        ConfigEquipReport( std::string& key, const std::string& name, const unsigned long locationId );
        ConfigEquipReport( const ConfigEquipReport& theEquipReport );            	

		/**
         * Constructor
         *
         * Construct a ConfigEquipReport class based around dataset
         */
		ConfigEquipReport(const unsigned long row, TA_Base_Core::IData& data);

        // destructor
		virtual ~ConfigEquipReport();
		
		// support methods
        virtual ItemChanges getAllItemChanges() { return m_EquipReportChanges; }
        virtual bool hasChanged() { return !m_EquipReportChanges.empty(); }
        virtual bool isNew();
        virtual unsigned long getUniqueIdentifier() { return m_uniqueIdentifier; };
        virtual void applyChanges();
        virtual void deleteThisEquipReport();
		virtual void invalidate();

		// get methods for all fields within the table
        virtual unsigned long	getKey();
        virtual std::string     getId();
        virtual std::string		getName();
        virtual unsigned long	getLocationId();
        virtual unsigned long	getProfile();
		virtual bool			getReportTypeFilterFlag();
		virtual std::string		getReportTypeFilter();
		virtual bool			getAssetFilterFlag();
		virtual std::string		getAssetFilter();
		virtual bool			getDescFilterFlag();
		virtual std::string		getDescFilter();
		virtual bool			getDPNameFilterFlag();
		virtual std::string		getDPNameFilter();
		virtual bool			getDPTypeFilterFlag();
		virtual std::string		getDPTypeFilter();
		virtual bool			getDPValueFilterFlag();
		virtual std::string		getDPValueFilter();

		virtual bool			getStatusFilterFlag();
		virtual bool			getScanInhibitFlag();
		virtual bool			getAlarmInhibitFlag();
		virtual bool			getControlInhibitFlag();
		virtual bool			getMmsInhibitFlag();
		virtual bool			getTaggedFlag();
		virtual bool			getTaggedPTWFlag();
		virtual bool			getOverriddenFlag();

		virtual bool			getStatusChangeByFilterFlag();
		virtual std::string		getStatusChangeByFilter();
		virtual bool			getTimestampFilterFlag();
		virtual std::string		getTimestampFilter();

		// set methods for all fields within the table
        virtual void setName(const std::string& name);
        virtual void setLocationId(const unsigned long locationId);     
		virtual void setProfile(const unsigned long profile);
		virtual void setReportTypeFilterFlag(const bool flag);
		virtual void setReportTypeFilter(const std::string &reportFilter);
		virtual void setAssetFilterFlag(const bool flag);
		virtual void setAssetFilter(const std::string& assetFilter);
		virtual void setDescFilterFlag(const bool flag);
		virtual void setDescFilter(const std::string& descFilter);
		virtual void setDPNameFilterFlag(const bool flag);
		virtual void setDPNameFilter(const std::string& dpNameFilter);
		virtual void setDPTypeFilterFlag(const bool flag);
		virtual void setDPTypeFilter(const std::string& dpTypeFilter);
		virtual void setDPValueFilterFlag(const bool flag);
		virtual void setDPValueFilter(const std::string& dpValueFilter);

		virtual void setStatusFilterFlag(const bool flag);
		virtual void setScanInhibitFlag(const bool flag);
		virtual void setAlarmInhibitFlag(const bool flag);
		virtual void setControlInhibitFlag(const bool flag);
		virtual void setMmsInhibitFlag(const bool flag);
		virtual void setTaggedFlag(const bool flag);
		virtual void setTaggedPTWFlag(const bool flag);
		virtual void setOverriddenFlag(const bool flag);
		
		virtual void setStatusChangeByFilterFlag(const bool flag);
		virtual void setStatusChangeByFilter(const std::string& statusFilter);
		virtual void setTimestampFilterFlag(const bool flag);
		virtual void setTimestampFilter(const std::string& timestampFilter);

	private:
	
		virtual void updateEquipReportChanges(	const std::string& name, 
												const std::string& oldValue, 
												const std::string& newValue );
 
        // Assignment operator not used so it is made private
		ConfigEquipReport& operator=(const ConfigEquipReport&) {};

        static const std::string	NAME;
        static const std::string	LOCATIONID;
		static const std::string	ASSETFILTER;
		static const std::string	ASSETFILTERFLAG;

        EquipReportHelper*			m_EquipReportHelper;

        unsigned long				m_uniqueIdentifier; 
        static long					s_nextAvailableIdentifier; 

        ItemChanges					m_EquipReportChanges;
    };

} // TA_Base_Core

#endif // !defined(ConfigEquipReport_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
