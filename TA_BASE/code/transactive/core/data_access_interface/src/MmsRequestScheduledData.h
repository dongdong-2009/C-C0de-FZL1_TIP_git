/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestScheduledData.h $
 * @author:  Luo HuiRong
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2009/12/29 14:17:48 $
 * Last modified by:  $Author: grace.koh $
 * 
 * It holds the data specific to a MmsRequestScheduledData
 * entry in the database.
 */

#if !defined(MmsRequestPreventiveData_EA3B2D7858_4ABD_478c_B0A0_211C51050158__INCLUDED_)
#define MmsRequestPreventiveData_EA3B2D7858_4ABD_478c_B0A0_211C51050158__INCLUDED_
 
#include <string>
#include <time.h>
#include <sys/timeb.h>

namespace TA_Base_Core
{
	

	class MmsRequestScheduledData  
	{
	public:

		enum ScheduledType 
		{
			NONE = 0 ,
			COUNTER, 
			MEASUREMENT
		};
        /**
		 * MmsRequestScheduledData (constructor)
		 *
		 * Construct a MmsRequestScheduledData class for the entry represented by the specified parameters.	 
		 */
		MmsRequestScheduledData(unsigned long key,
			ScheduledType scheduledType,
            unsigned long entityKey,         
			const std::string& equipmentID,
	    	const std::string& meterCode,
		    const std::string& equipmentDescription,
			double dpValue,	 
			timeb planDate,             			
			timeb createDate );
 

        /**
		 * MmsRequestScheduledData (deconstructor)
		 *
		 * Deconstruct a MmsRequestScheduledData class.
		 *
		 */
		virtual ~MmsRequestScheduledData();

	
		/**
         * getKey
         *
         * Returns the key for this MMS request.    
         */
		unsigned long getKey();

		/**
         * getScheduledType
         *
         * Returns the scheduled type for this MMS request.    
         */
		ScheduledType getScheduledType();
		 

		/**
		* getEntityKey
		*
		* Returns the entity pkey of data point    
		*/
		unsigned long getEntityKey();
		
	 
		
		/**
         * getEquipmentID
         *
         * Returns Identification name for the equipment
         */
		std::string getEquipmentID();
		
		/**
         * getMeterCode
         *
         * Returns Data point name: Eg. miiPWRC-KWHMeter  
         */
		std::string getMeterCode();
		
		/**
         * getDescription
         *
         * Returns description of the equipment  
         */
		std::string getEquipmentDescription();
		
	 
		
		/**
         * getProblemReport
         *
         * Returns the problem Report   
         */
		double getDpValue();
		
		
		/**
         * getPlanDate
         *
         * Returns the planning date time to collect the preventive request
         */
		timeb getPlanDate();

		/**
		* getCreateDate
		*
		* Returns the date time creating the preventive request 
		*/
		timeb getCreateDate();
	 

	private:
		MmsRequestScheduledData& operator=(const MmsRequestScheduledData &);
		MmsRequestScheduledData( const MmsRequestScheduledData& theMmsRequestPreventiveData);


		unsigned long m_key;
		ScheduledType m_scheduledType;
		unsigned long m_entityKey;         
		const std::string m_equipmentID;
		const std::string m_meterCode;
		const std::string m_equipmentDescription;
		double m_dpValue;		 
		timeb m_planDate;             			
		timeb m_createDate;

	 

		 
	};
} // closes TA_Base_Core

#endif // !defined(MmsRequestPreventiveData_EA3B2D7858_4ABD_478c_B0A0_211C51050158__INCLUDED_)
