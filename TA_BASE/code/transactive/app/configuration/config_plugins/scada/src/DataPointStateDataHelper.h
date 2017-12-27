/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Adam Radics
  * @version $Revision: 1.1.2.1 $
  * Last modification: $DateTime$
  * Last modified by: $Author: adamr $
  *
  * DataPointStateDataHelper provides access to DataPoint state data.
  */

#ifndef DATAPOINT_STATE_DATA_HELPER_H
#define DATAPOINT_STATE_DATA_HELPER_H

#include "bus/scada/common_library/src/ScadaPersistence.h"

namespace TA_Base_App
{
    class DataPointStateDataHelper
    {
	public:

        
        /** 
          * DataPointStateDataHelper
          *
          * constructor. Builds its own scada persistence
          */
        DataPointStateDataHelper();

        /** 
          * DataPointStateDataHelper
          *
          * constructor. Uses given scada persistence
          */
        DataPointStateDataHelper ( TA_Base_Bus::ScadaPersistencePtr persistence );

        virtual ~DataPointStateDataHelper();

        /**
         * getLoLoValue
         *
         * Returns the low low alarm value associated with the DataPoint entity
         *
         * @param entityKey the key of the datapoint being dealt with
         * @return the low low alarm value of this DataPoint entity
         */
		double getLoLoValue( unsigned long entityKey );

        /**
         * setLoLoValue
         *
         * Set the low low alarm value of this DataPoint entity to the specified value
		 *
		 * @param entityKey the key of the datapoint being dealt with
         * @param newValue  The new low low alarm value of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLoLoValue ( unsigned long entityKey, double newValue );

        /** 
          * getLoLoAlarmEnabled
          *
          * Returns whether the low low alarm associated with the DataPoint entity is enabled
          *
          * @param entityKey the key of the datapoint being dealt with
          *
          * @return true if the alarm is enabled
          */
        bool getLoLoAlarmEnabled( unsigned long entityKey );

        /** 
          * setLoLoAlarmEnabled
          *
          * sets whether the low low alarm associated with the DataPoint entity is enabled
          *
          * @param entityKey the key of the datapoint being dealt with
          * @param enabled true if the alarm is enabled
          *
          * @return True if the operation is successfull, false otherwise
          */
        bool setLoLoAlarmEnabled( unsigned long entityKey, bool enabled );


        //
        // Same as above for Lo alarms
        //
		double getLoValue( unsigned long entityKey );
        bool setLoValue ( unsigned long entityKey, double newValue );
        bool getLoAlarmEnabled( unsigned long entityKey );
        bool setLoAlarmEnabled( unsigned long entityKey,bool enabled);


        //
        // Same as above for Hi alarms
        //
		double getHiValue( unsigned long entityKey );
		bool setHiValue ( unsigned long entityKey, double newValue );
        bool getHiAlarmEnabled( unsigned long entityKey );
        bool setHiAlarmEnabled( unsigned long entityKey, bool enabled );


        //
        // Same as above for HiHi alarms
        //
		double getHiHiValue( unsigned long entityKey );
		bool setHiHiValue ( unsigned long entityKey, double newValue );
        bool getHiHiAlarmEnabled( unsigned long entityKey );
        bool setHiHiAlarmEnabled( unsigned long entityKey, bool enabled );


        //
        // Same as above for OHi alarms
        //
		double getOverScaleHiValue( unsigned long entityKey );
		bool setOverScaleHiValue ( unsigned long entityKey, double newValue );
        bool getOverScaleHiAlarmEnabled( unsigned long entityKey );
        bool setOverScaleHiAlarmEnabled( unsigned long entityKey, bool enabled );


        //
        // Same as above for OLo alarms
        //
		double getOverScaleLoValue( unsigned long entityKey );
		bool setOverScaleLoValue ( unsigned long entityKey, double newValue );
        bool getOverScaleLoAlarmEnabled( unsigned long entityKey );
        bool setOverScaleLoAlarmEnabled( unsigned long entityKey, bool enabled );
        

        //
        // Same as above for boolean alarms
        //
        bool getTrueAlarmEnabled( unsigned long entityKey );
        bool setTrueAlarmEnabled(  unsigned long entityKey, bool enabled );
        bool getFalseAlarmEnabled( unsigned long entityKey );
		bool setFalseAlarmEnabled( unsigned long entityKey, bool enabled );
		

    private:

        DataPointStateDataHelper ( const DataPointStateDataHelper & obj );
		DataPointStateDataHelper & operator= ( const DataPointStateDataHelper &);


        //
        // does the scada persistence work
        //

        double readDouble( TA_Base_Bus::EPersistedItem item, unsigned long entityKey );
        bool readBool(TA_Base_Bus::EPersistedItem item, unsigned long entityKey );

        bool writeDouble(TA_Base_Bus::EPersistedItem item, unsigned long entityKey, double value );
        bool writeBool(TA_Base_Bus::EPersistedItem item, unsigned long entityKey, bool value );


        TA_Base_Bus::ScadaPersistencePtr m_persistence;

        /** 
          * initialisePersistence
          *
          * Initialises m_persistence if not already.
          */
        void initialisePersistence();


    };
} //close namespace TA_Base_App

#endif
