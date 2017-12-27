/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestAlarmData.h $
 * @author:  Luo HuiRong
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2009/12/29 14:17:48 $
 * Last modified by:  $Author: grace.koh $
 * 
 * It holds the data specific to a MmsRequestAlarmData
 * entry in the database.
 */

#if !defined(MmsRequestAlarmData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A1__INCLUDED_)
#define MmsRequestAlarmData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A1__INCLUDED_

#include <string>
#include <time.h>
#include <sys/timeb.h>

namespace TA_Base_Core
{

	class MmsRequestAlarmData  
	{
	public:

        /**
		 * MmsRequestAlarmData (constructor)
		 *
		 * Construct a MmsRequestAlarmData class for the entry represented by the specified parameters.	 
		 */
		MmsRequestAlarmData(unsigned long key,
            const std::string& alarmID,
            const timeb alarmDate, 
            unsigned long alarmSeverity,
			const std::string& equipmentID,
	    	const std::string& alarmFaultCode,
		    const std::string& alarmDescription,
			const std::string& requestor,
			const std::string& problemReport,
			const timeb targetDate,             			
			bool isAutoTrigger = true);
 

        /**
		 * MmsRequestAlarmData (deconstructor)
		 *
		 * Deconstruct a MmsRequestAlarmData class.
		 *
		 */
		virtual ~MmsRequestAlarmData();

	
		/**
         * getKey
         *
         * Returns the key for this alarm MMS request.    
         */
		unsigned long getKey();
		 

		/**
         * getAlarmID
         *
         * Returns the ID of alarm   
         */
		std::string getAlarmID();
		
		/**
         * getAlarmDate
         *
         * Returns the source time of alarm   
         */
		timeb getAlarmDate();
		
		/**
         * getAlarmServerity
         *
         * Returns the serverity of alarm   
         */
		unsigned long getAlarmSeverity();
		
		/**
         * getEquipmentID
         *
         * Returns the equipment id of alarm   
         */
		std::string getAlarmEquipmentID();
		
		/**
         * getFaultCode
         *
         * Returns the fault code id of alarm   
         */
		std::string getAlarmFaultCode();
		
		/**
         * getDescription
         *
         * Returns the description id of alarm   
         */
		std::string getAlarmDescription();
		
		/**
         * getRequestor
         *
         * Returns name of operator who raises the MMS request 
         */
		std::string getRequestor();
		
		/**
         * getProblemReport
         *
         * Returns the problem Report   
         */
		std::string getProblemReport();
		
		
		/**
         * getTargetDate
         *
         * Returns the target date time
         */
		timeb getTargetDate();

		/**
         * getIsAutoTrigger
         *
         * Returns the IsAutoTrigger
         */
		bool  getIsAutoTrigger();
	 

	private:
		MmsRequestAlarmData& operator=(const MmsRequestAlarmData &);
		MmsRequestAlarmData( const MmsRequestAlarmData& theMmsRequestAlarmData);
		
		unsigned long m_key;
        std::string m_alarmID;
        timeb m_alarmDate;
        unsigned long m_alarmSeverity;
	    std::string m_equipmentID;
	    std::string m_alarmFaultCode;
		std::string m_alarmDescription;
		std::string m_requestor;
		std::string m_problemReport;
	    timeb m_targetDate;             			
	    bool m_isAutoTrigger;

		 
	};
} // closes TA_Base_Core

#endif // !defined(MmsRequestAlarmData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A1__INCLUDED_)
