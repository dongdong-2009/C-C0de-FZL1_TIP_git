/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestSchematicData.h $
 * @author:  Luo HuiRong
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2009/12/29 14:17:48 $
 * Last modified by:  $Author: grace.koh $
 * 
 * It holds the data specific to a MmsRequestSchematicData
 * entry in the database.
 */

#if !defined(MmsRequestSchematicData_EAEB545FAC_98B9_4e77_BE98_DE3DCC40FE8A__INCLUDED_)
#define MmsRequestSchematicData_EAEB545FAC_98B9_4e77_BE98_DE3DCC40FE8A__INCLUDED_
 
#include <string>
#include <time.h>
#include <sys/timeb.h>

namespace TA_Base_Core
{

	class MmsRequestSchematicData  
	{
	public:

        /**
		 * MmsRequestSchematicData (constructor)
		 *
		 * Construct a MmsRequestSchematicData class for the entry represented by the specified parameters.	 
		 */
		MmsRequestSchematicData(unsigned long key,
            unsigned long entityKey,      
			const std::string& equipmentID,	     
			const std::string& requestor,
			const std::string& problemReport,
			const timeb targetDate);
 

        /**
		 * MmsRequestSchematicData (deconstructor)
		 *
		 * Deconstruct a MmsRequestSchematicData class.
		 *
		 */
		virtual ~MmsRequestSchematicData();

	
		/**
         * getKey
         *
         * Returns the key for this MMS request.    
         */
		unsigned long getKey();
		 

		/**
         * getEntityKey
         *
         * Returns the pkey of entity   
         */
		unsigned long getEntityKey();
		
	 
		
		/**
         * getEquipmentID
         *
         * Returns the equipment id of alarm   
         */
		std::string getEquipmentID();
		
	 
		
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
 
	 

	private:
		MmsRequestSchematicData& operator=(const MmsRequestSchematicData &);
		MmsRequestSchematicData( const MmsRequestSchematicData& theMmsRequestSchematicData);
		
		unsigned long m_key;    
		unsigned long m_entityKey;
		std::string m_requestor;
	    std::string m_equipmentID;			 
		std::string m_problemReport;
	    timeb m_targetDate;             			
 

		 
	};
} // closes TA_Base_Core

#endif // !defined(MmsRequestSchematicData_EAEB545FAC_98B9_4e77_BE98_DE3DCC40FE8A__INCLUDED_)
