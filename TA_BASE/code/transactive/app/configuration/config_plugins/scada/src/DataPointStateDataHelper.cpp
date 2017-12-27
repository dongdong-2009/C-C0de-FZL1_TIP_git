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


#include "app/configuration/config_plugins/scada/src/DataPointStateDataHelper.h"

namespace TA_Base_App
{

    DataPointStateDataHelper::DataPointStateDataHelper()
    {}


    DataPointStateDataHelper::DataPointStateDataHelper ( TA_Base_Bus::ScadaPersistencePtr persistence )
        : m_persistence(persistence)
    {}


    DataPointStateDataHelper::~DataPointStateDataHelper()
    {}


    //
    // For LoLo alarms
    //

	double DataPointStateDataHelper::getLoLoValue( unsigned long entityKey )
    {
        return readDouble( TA_Base_Bus::DP_LOLO_ALARM_VALUE, entityKey );
    }


	bool DataPointStateDataHelper::setLoLoValue ( unsigned long entityKey, double newValue )
    {
        return writeDouble( TA_Base_Bus::DP_LOLO_ALARM_VALUE, entityKey, newValue );
    }


    bool DataPointStateDataHelper::getLoLoAlarmEnabled( unsigned long entityKey )
    {
        return readBool( TA_Base_Bus::DP_LOLO_ALARM_ENABLED, entityKey );
    }


    bool DataPointStateDataHelper::setLoLoAlarmEnabled( unsigned long entityKey, bool enabled )
    {
        return writeBool( TA_Base_Bus::DP_LOLO_ALARM_ENABLED, entityKey, enabled );
    }


    //
    // For Lo alarms
    //

    double DataPointStateDataHelper::getLoValue( unsigned long entityKey )
    {
        return readDouble( TA_Base_Bus::DP_LO_ALARM_VALUE, entityKey );
    }


	bool DataPointStateDataHelper::setLoValue ( unsigned long entityKey, double newValue )
    {
        return writeDouble( TA_Base_Bus::DP_LO_ALARM_VALUE, entityKey, newValue );
    }


    bool DataPointStateDataHelper::getLoAlarmEnabled( unsigned long entityKey )
    {
        return readBool( TA_Base_Bus::DP_LO_ALARM_ENABLED, entityKey );
    }


    bool DataPointStateDataHelper::setLoAlarmEnabled( unsigned long entityKey, bool enabled )
    {
        return writeBool( TA_Base_Bus::DP_LO_ALARM_ENABLED, entityKey, enabled );
    }


    //
    // For Hi alarms
    //

    double DataPointStateDataHelper::getHiValue( unsigned long entityKey )
    {
        return readDouble( TA_Base_Bus::DP_HI_ALARM_VALUE, entityKey );
    }


	bool DataPointStateDataHelper::setHiValue ( unsigned long entityKey, double newValue )
    {
        return writeDouble( TA_Base_Bus::DP_HI_ALARM_VALUE, entityKey, newValue );
    }


    bool DataPointStateDataHelper::getHiAlarmEnabled( unsigned long entityKey )
    {
        return readBool( TA_Base_Bus::DP_HI_ALARM_ENABLED, entityKey );
    }


    bool DataPointStateDataHelper::setHiAlarmEnabled( unsigned long entityKey, bool enabled )
    {
        return writeBool( TA_Base_Bus::DP_HI_ALARM_ENABLED, entityKey, enabled );
    }


    //
    // For HiHi alarms
    //

    double DataPointStateDataHelper::getHiHiValue( unsigned long entityKey )
    {
        return readDouble( TA_Base_Bus::DP_HIHI_ALARM_VALUE, entityKey );
    }


	bool DataPointStateDataHelper::setHiHiValue ( unsigned long entityKey, double newValue )
    {
        return writeDouble( TA_Base_Bus::DP_HIHI_ALARM_VALUE, entityKey, newValue );
    }


    bool DataPointStateDataHelper::getHiHiAlarmEnabled( unsigned long entityKey )
    {
        return readBool( TA_Base_Bus::DP_HIHI_ALARM_ENABLED, entityKey );
    }


    bool DataPointStateDataHelper::setHiHiAlarmEnabled( unsigned long entityKey, bool enabled )
    {
        return writeBool( TA_Base_Bus::DP_HIHI_ALARM_ENABLED, entityKey, enabled );
    }


    //
    // For OHi alarms
    //

    double DataPointStateDataHelper::getOverScaleHiValue( unsigned long entityKey )
    {
        return readDouble( TA_Base_Bus::DP_OHI_ALARM_VALUE, entityKey );
    }


	bool DataPointStateDataHelper::setOverScaleHiValue ( unsigned long entityKey, double newValue )
    {
        return writeDouble( TA_Base_Bus::DP_OHI_ALARM_VALUE, entityKey, newValue );
    }


    bool DataPointStateDataHelper::getOverScaleHiAlarmEnabled( unsigned long entityKey )
    {
        return readBool( TA_Base_Bus::DP_OHI_ALARM_ENABLED, entityKey );
    }


    bool DataPointStateDataHelper::setOverScaleHiAlarmEnabled( unsigned long entityKey, bool enabled )
    {
        return writeBool( TA_Base_Bus::DP_OHI_ALARM_ENABLED, entityKey, enabled );
    }


    //
    // For OLo alarms
    //

    double DataPointStateDataHelper::getOverScaleLoValue( unsigned long entityKey )
    {
        return readDouble( TA_Base_Bus::DP_OLO_ALARM_VALUE, entityKey );
    }


	bool DataPointStateDataHelper::setOverScaleLoValue ( unsigned long entityKey, double newValue )
    {
        return writeDouble( TA_Base_Bus::DP_OLO_ALARM_VALUE, entityKey, newValue );
    }


    bool DataPointStateDataHelper::getOverScaleLoAlarmEnabled( unsigned long entityKey )
    {
        return readBool( TA_Base_Bus::DP_OLO_ALARM_ENABLED, entityKey );
    }


    bool DataPointStateDataHelper::setOverScaleLoAlarmEnabled( unsigned long entityKey, bool enabled )
    {
        return writeBool( TA_Base_Bus::DP_OLO_ALARM_ENABLED, entityKey, enabled );
    }


    //
    // For boolean alarms
    //

    bool DataPointStateDataHelper::getTrueAlarmEnabled( unsigned long entityKey )
    {
        return readBool( TA_Base_Bus::DP_TRUE_ALARM_ENABLED, entityKey );
    }


    bool DataPointStateDataHelper::setTrueAlarmEnabled( unsigned long entityKey, bool enabled )
    {
        return writeBool( TA_Base_Bus::DP_TRUE_ALARM_ENABLED, entityKey, enabled );
    }


    bool DataPointStateDataHelper::getFalseAlarmEnabled( unsigned long entityKey )
    {
        return readBool( TA_Base_Bus::DP_FALSE_ALARM_ENABLED, entityKey );
    }


    bool DataPointStateDataHelper::setFalseAlarmEnabled( unsigned long entityKey, bool enabled )
    {
        return writeBool( TA_Base_Bus::DP_FALSE_ALARM_ENABLED, entityKey, enabled );
    }


    void DataPointStateDataHelper::initialisePersistence()
    {
        // create ScadaPersistence if not already done so
        if ( m_persistence.get() == NULL )
        {
            m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistence( TA_Base_Bus::ScadaPersistence::DATAPOINT ) );
        }
    }


    double DataPointStateDataHelper::readDouble( TA_Base_Bus::EPersistedItem item, unsigned long entityKey )
    {
        initialisePersistence();

        double tempDouble(0);
        m_persistence->readPersistedValue( item, entityKey, tempDouble );

        return tempDouble;
    }


    bool DataPointStateDataHelper::readBool(TA_Base_Bus::EPersistedItem item, unsigned long entityKey )
    {
        initialisePersistence();

        bool tempBool(false);
        m_persistence->readPersistedValue( item, entityKey, tempBool );

        return tempBool;
    }


    bool DataPointStateDataHelper::writeDouble(TA_Base_Bus::EPersistedItem item, unsigned long entityKey, double value )
    {
        initialisePersistence();

        return m_persistence->writePersistedValue( item, entityKey, value );
    }


    bool DataPointStateDataHelper::writeBool(TA_Base_Bus::EPersistedItem item, unsigned long entityKey, bool value )
    {
        initialisePersistence();

        return m_persistence->writePersistedValue( item, entityKey, value );
    }


} //close namespace TA_Base_App
