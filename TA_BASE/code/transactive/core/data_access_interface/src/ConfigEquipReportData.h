/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigEquipReportData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigEquipReportData is an implementation of the IConfigEquipReportData interface
  * to allow access to the EquipReportData table
  *
  */

#if !defined(AFX_CONFIGEQUIPREPORTDATA_H__07FEC4FB_DCF8_451E_B299_9E4A719A0D94__INCLUDED_)
#define AFX_CONFIGEQUIPREPORTDATA_H__07FEC4FB_DCF8_451E_B299_9E4A719A0D94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <ctime>

#include "core/data_access_interface/src/IConfigEquipReportData.h"

namespace TA_Base_Core
{
	// forward declarations
	class IData;	
	class EquipReportDataHelper;
	
	class ConfigEquipReportData : public IConfigEquipReportData
	{	
	public:
		// constructors
		ConfigEquipReportData( );
        ConfigEquipReportData( std::string& key );     
        ConfigEquipReportData( std::string& key, std::string& reportKey );
        ConfigEquipReportData( const ConfigEquipReportData& theEquipReportData );

		/**
         * Constructor
         *
         * Construct a ConfigEquipReportData class based around dataset
         */
		ConfigEquipReportData(const unsigned long row, TA_Base_Core::IData& data);

        // destructor
		virtual ~ConfigEquipReportData();

		virtual ItemChanges getAllItemChanges() { return m_EquipReportDataChanges; }
        virtual bool hasChanged() { return !m_EquipReportDataChanges.empty(); }
        virtual bool isNew();
        virtual unsigned long getUniqueIdentifier() { return m_uniqueIdentifier; };
        virtual void applyChanges();
        virtual void deleteThisEquipReportData();
		virtual void invalidate();

		// get methods
        virtual unsigned long	getKey();
        virtual std::string     getId();
        virtual std::string 	getReportKey();
		virtual unsigned long	getDataNodeKey();
		virtual unsigned long	getDataPointKey();

		// set methods
        virtual void setReportKey(std::string& reportKey);     
		virtual void setDataNodeKey(const unsigned long dataNodeKey);
		virtual void setDataPointKey(const unsigned long dataPointKey);
		virtual void updateEquipReportDataChanges(	const std::string& name, 
													const std::string& oldValue, 
													const std::string& newValue );
 

		virtual void setName(const std::string &name) { };
		virtual std::string getName() { std::string blank(""); return blank; };	
	private:

        // Assignment operator not used so it is made private
		ConfigEquipReportData& operator=(const ConfigEquipReportData&) { };

        static const std::string	REPORTKEY;
		
        EquipReportDataHelper*		m_EquipReportDataHelper;

        unsigned long				m_uniqueIdentifier; 
        static long					s_nextAvailableIdentifier; 

        ItemChanges					m_EquipReportDataChanges;
	};

}
#endif // !defined(AFX_CONFIGEQUIPREPORTDATA_H__07FEC4FB_DCF8_451E_B299_9E4A719A0D94__INCLUDED_)
