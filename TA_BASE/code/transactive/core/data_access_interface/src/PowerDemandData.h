/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/PowerDemandData.h $
  * @author:  wenching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PowerDemandData is an implementation of IPowerDemandData. It holds the power meter datapoint 30-min demand data.
  */

#if !defined(PowerDemandData_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define PowerDemandData_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IPowerDemandData.h"

namespace TA_Base_Core
{
    class PowerDemandData : public IPowerDemandData
    {

    public:

		/**
         * Constructor
         *
         * Construct an PowerDemandData .
         *
         * @param m_meterName The name of the meter data point
         * @param m_interval The 30 minute time period of the interval
         * @param m_recordedValue The recorded value of the power demand
         * @param m_updatedValue The updated value of the power demand
		 */
		PowerDemandData();

        /**
         * Constructor
         *
         * Construct an PowerDemandData .
         *
         * @param m_meterName The name of the meter data point
         * @param m_interval The 30 minute time period of the interval
         * @param m_recordedValue The recorded value of the power demand
         * @param m_updatedValue The updated value of the power demand
		 */
        PowerDemandData(const std::string& meterName, time_t interval, unsigned long recordedValue, unsigned long updatedValue);

        /**
         * Destructor
         */
        virtual ~PowerDemandData();

        /**
         * get power meter name
         *
         * Returns the string of the name of the meter power demand
		 *
         * @return The name for this data as a string.
         */
        virtual std::string getMeterName() const;

        /**
         * getInterval
         *
         * Returns the 30 minute period of the power demand
         *
         * @return The interval as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual time_t getInterval() const;


        /**
         * getRecordedValue
         *
         * Returns the recorded value of power demand
         *
         * @return The recorded value of the entity as a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual unsigned long getRecordedValue() const;

        
        /**
        * getUpdatedValue
         *
         * Returns the updated value of power demand
         *
         * @return The updated value of the entity as a unsigned long.
         *        
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual unsigned long getUpdatedValue() const;


		/**
         * setRecordedValue
         *
         * Set the recorded value of power demand
         *
         * @
         *        
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
		virtual void setRecordedValue(unsigned long recordedValue);


		/**
         * setUpdatedValue
         *
         * Set the updated value of power demand
         *
         * @
         *        
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
		virtual void setUpdatedValue(unsigned long updatedValue);



    private:
        // Copy constructor and assignment operator are not used so they are made private
        //PowerDemandData( const PowerDemandData& thePowerDemandData);            
		//PowerDemandData& operator=(const PowerDemandData &);


	private:

        std::string		m_meterName;
		time_t			m_interval;
		unsigned long	m_recordedValue;
		unsigned long	m_updatedValue;
    };
} // closes TA_Base_Core

#endif // !defined(QuickFindEntity_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
