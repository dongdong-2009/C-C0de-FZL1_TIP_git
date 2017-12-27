/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/StationSystemOutputWriter.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2017/05/25 10:19:08 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The StationSystemOutputWriter object is responsible for processing write data commands to the PMS RTU
  *
  */

#include "bus/scada/rtu_library/src/RTU.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/rtu_library/src/NumberConversionUtil.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"


using TA_Base_Core::DebugUtil;

namespace 
{
    bool isSystemSessionID( const std::string& sessionID )//limin++, SystemSession should not access db
    {
        bool ret = ( TA_Base_Core::SessionAccessFactory::getSuperSessionId() == sessionID ||
                     TA_Base_Bus::DPT_INTERNAL_SESSION_ID == sessionID ||
                     TA_Base_Bus::NotLoaded == sessionID );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[sessionID=%s][return=%d]", sessionID.c_str(), ret );
        return ret;
    }
}


namespace TA_Base_Bus
{
	StationSystemOutputWriter::StationSystemOutputWriter ( RTU& rtu,
                                                           const std::string & stationSystemName,
                                                           const std::string & rtuServicePortNumber,
                                                           unsigned long startOutputAddress,
                                                           unsigned long endOutputAddress,
                                                           int slaveID,
														   const std::string& digitalFunCode,
														   unsigned long offsetAddress)
	:
    m_rtu (rtu),
	m_stationSystemName ( stationSystemName ),
	m_rtuServicePortNumber ( rtuServicePortNumber ),
    m_addressBlock ( startOutputAddress, endOutputAddress ),
    m_slaveID ( slaveID ),
	m_offsetAddress ( offsetAddress )
	{
		convertDigitalCodeToEType(digitalFunCode);
	}

	StationSystemOutputWriter::~StationSystemOutputWriter()
	{
	}


	bool StationSystemOutputWriter::processWriteRequest ( TA_Base_Bus::DataPointWriteRequest & newWrite )
	{
		// get the data point from the new request
		TA_Base_Bus::DataPoint * dp = newWrite.getDataPoint();

		TA_ASSERT( 0 != dp, "StationSystemOutputWriter::processWriteRequest: newWrite contains a NULL datapoint." );

        RTUOutputDataPointMap::iterator itr = m_dataPoints.find( dp->getEntityKey() );

        if ( itr == m_dataPoints.end() )
        {
            return false;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "%s, %s, slave ID %d: start writing process for %s",
            m_rtu.getName().c_str(),
            m_stationSystemName.c_str(),
            m_slaveID,
            dp->getDataPointName().c_str() );
        
        if ( dp->getIsDigitalDataPoint() )
        {
            writeDigitalDataPoint( newWrite );
        }
        else if ( dp->getIsTextDataPoint() )
        {
            // do nothing. To be implemented when required
        }
        else if ( dp->getIsAnalogueDataPoint() )
        {
            writeAnalogueDataPoint( newWrite );
        }
        else
        {
            // do nothing for the current design
        }
        
        return true;
    }


	void StationSystemOutputWriter::writeDigitalDataPoint ( TA_Base_Bus::DataPointWriteRequest & newWrite )
	{
		switch(m_digitWriteCode)
		{
		case E_DIGIT_0F:
			writeDigitalDataPointWith0F(newWrite);
			break;
		case E_DIGIT_10:
			writeDigitalDataPointWith10(newWrite);
			break;
		default:
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "unknown digital write type:%d", m_digitWriteCode);
			break;
		}
	}

	void StationSystemOutputWriter::writeDigitalDataPointWith0F(TA_Base_Bus::DataPointWriteRequest & newWrite)
	{
		TA_Base_Bus::DataPoint * point = newWrite.getDataPoint();
		unsigned long registerAddress = point->getDataPointRegisterAddress() - m_offsetAddress; 
		// Standard Modbus device is 0-based address
		registerAddress = m_rtu.adjustAddress(registerAddress);

		bool isSystemSession = isSystemSessionID( newWrite.getSessionID() );//limin++, SystemSession should not access db

		TA_Base_Bus::DataBlock < bool > coilsDataBlock;
		// initialise coils data block, start at register address and length of 1 bit
		coilsDataBlock.setStartAndLength ( registerAddress, 1 );
		// set the value in data block
		coilsDataBlock.set( registerAddress, newWrite.getValue().getBoolean() );

		//
		// sending the command
		//

		// update control status of the data point to indicate that it
		// is waiting for confirmation from the field device
		point->updateControlState ( TA_Base_Bus::pending, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db

		bool sendCommandResult = false;
		{
    		TA_Base_Core::ThreadGuard guard(m_modbusLock);
    		sendCommandResult = m_rtu.processSendingData(coilsDataBlock,m_rtuServicePortNumber,m_slaveID);
		}
		// send command now
		if ( true == sendCommandResult )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
				"Successful to send command to %s with FC-0F, slave ID = %d. Start ABE timeout mechanism for %s",
				m_stationSystemName.c_str(),
				m_slaveID,
				point->getDataPointName().c_str() );

			// activate ABE process for this datapoint
			point->startABETimeout( newWrite.getSessionID() );
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
				"Failed to send command to %s, slave ID = %d with FC-0F, for %s",
				m_stationSystemName.c_str(),
				m_slaveID,
				point->getDataPointName().c_str() );

			// update control status of the data point to indicate that
			// the output command has failed
			point->updateControlState ( TA_Base_Bus::comms_failed, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db
		}

		point = 0;
	}

	void StationSystemOutputWriter::writeDigitalDataPointWith10(TA_Base_Bus::DataPointWriteRequest & newWrite)
	{
		TA_Base_Bus::DataPoint * point = newWrite.getDataPoint();
		unsigned long registerAddress = point->getDataPointRegisterAddress() - m_offsetAddress; 
		// Standard Modbus device is 0-based address
		registerAddress = m_rtu.adjustAddress(registerAddress);

		unsigned long bitAddress = point->getDataPointBitAddress();
		if (bitAddress > 15)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "wrong bit address:%u for %u", bitAddress, point->getEntityKey());
			return;
		}

		bool isSystemSession = isSystemSessionID( newWrite.getSessionID() );//limin++, SystemSession should not access db

		TA_Base_Bus::DataBlock < WORD > registerDataBlock;
		registerDataBlock.setStartAndLength ( registerAddress, 1 );
		WORD data = 0x0;
		if (newWrite.getValue().getBoolean())
		{
			data = 0x1 << bitAddress;
		}
		
		registerDataBlock.set( registerAddress, data);

		//
		// sending the command
		//

		// update control status of the data point to indicate that it
		// is waiting for confirmation from the field device
		point->updateControlState ( TA_Base_Bus::pending, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db

		bool sendCommandResult = false;
		{
    		TA_Base_Core::ThreadGuard guard(m_modbusLock);
    		sendCommandResult = m_rtu.processSendingData(registerDataBlock,m_rtuServicePortNumber,m_slaveID);
		}

		// send command now
		if ( true == sendCommandResult )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
				"Successful to send command to %s with FC-10, slave ID = %d. Start ABE timeout mechanism for %s",
				m_stationSystemName.c_str(),
				m_slaveID,
				point->getDataPointName().c_str() );

			// activate ABE process for this datapoint
			point->startABETimeout( newWrite.getSessionID() );
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
				"Failed to send command to %s with FC-10, slave ID = %d, for %s",
				m_stationSystemName.c_str(),
				m_slaveID,
				point->getDataPointName().c_str() );

			// update control status of the data point to indicate that
			// the output command has failed
			point->updateControlState ( TA_Base_Bus::comms_failed, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db
		}

		point = 0;
	}

	void StationSystemOutputWriter::writeAnalogueDataPoint ( TA_Base_Bus::DataPointWriteRequest & newWrite )
	{
		TA_Base_Bus::DataPoint * point = 0;
		unsigned long registerAddress = 0;
		TA_Base_Bus::DataBlock < WORD > registerDataBlock;
		bool isValueValid = true;

        bool isSystemSession = isSystemSessionID( newWrite.getSessionID() );//limin++, SystemSession should not access db

		// get the data point from the new request
		point = newWrite.getDataPoint();

		// get the data point's register address
		registerAddress = point->getDataPointRegisterAddress();

		registerAddress = registerAddress - m_offsetAddress;

		registerAddress = m_rtu.adjustAddress(registerAddress);

		switch ( point->getDataPointDataType() )
		{
			case TA_Base_Bus::DPT_INT16_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 1 );

					// use the number conversion utility, convert the data point value
					// to INT16 format, where the -ve value is represented in 2's
					// Complement format
					WORD convertedNumber = 0;
					if ( true == m_numberConversionUtil.convertToINT16_Value ( (int)newWrite.getValue().getFloat(), convertedNumber ) )
					{
						// set the value in data block
						registerDataBlock.set( registerAddress, convertedNumber );
					}
					else
					{
						isValueValid = false;
					}
					break;
				}

			case TA_Base_Bus::DPT_INT16S_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 1 );

					// use the number conversion utility, convert the data point value
					// to INT16S format, where the -ve value is represented in the most significant bit
					WORD convertedNumber = 0;
					if ( true == m_numberConversionUtil.convertToINT16S_Value ( (int)newWrite.getValue().getFloat(), convertedNumber ) )
					{
						// set the value in data block
						registerDataBlock.set( registerAddress, convertedNumber );
					}
					else
					{
						isValueValid = false;
					}
					break;
				}

			case TA_Base_Bus::DPT_UINT16_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 1 );

					// set the value in data block
					registerDataBlock.set( registerAddress, (unsigned int)newWrite.getValue().getFloat() );
					break;
				}

			case TA_Base_Bus::DPT_UINT32_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 2 );

					// get the unsigned integer
					unsigned long temp = (unsigned long)newWrite.getValue().getFloat();

					// set the low order 16 bit of the unsigned value in the first address field
					registerDataBlock.set( registerAddress, (WORD) (0x0000FFFF & temp) );

					// set the high order 16 bit of the unsigned value in the second address field
					registerDataBlock.set( registerAddress + 1, (WORD) ((0xFFFF0000 & temp) >> 16) );

					break;
				}

			case TA_Base_Bus::DPT_INT32_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 2 );

					// use the number conversion utility, convert the data point value
					// to INT32 format, where the -ve value is represented in 2's
					// Complement format
					double convertedNumber = 0;
					if ( true == m_numberConversionUtil.convertToINT32_Value ( newWrite.getValue().getFloat(), convertedNumber ) )
					{
						// set low order 16 bit of the value in first address of data block
						registerDataBlock.set( registerAddress, (WORD) (0x0000FFFF & (long) convertedNumber) );

						// set high order 16 bit of the value in second address of data block
						registerDataBlock.set( registerAddress, (WORD) ((0xFFFF0000 & (long) convertedNumber) >> 16) );
					}
					else
					{
						isValueValid = false;
					}
					break;
				}

			case TA_Base_Bus::DPT_INT32S_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 2 );

					// use the number conversion utility, convert the data point value
					// to INT32S format, where the -ve value is represented in the most significant bit
					double convertedNumber = 0;
					if ( true == m_numberConversionUtil.convertToINT32S_Value ( newWrite.getValue().getFloat(), convertedNumber ) )
					{
						// set low order 16 bit of the value in first address of data block
						registerDataBlock.set( registerAddress, (WORD) (0x0000FFFF & (long) convertedNumber) );

						// set high order 16 bit of the value in second address of data block
						registerDataBlock.set( registerAddress, (WORD) ((0xFFFF0000 & (long) convertedNumber) >> 16) );
					}
					else
					{
						isValueValid = false;
					}
					break;
				}

			case TA_Base_Bus::DPT_IEEE32_TYPE :
				{
					// not implemented yet

					isValueValid = false;
					break;
				}

            //TD11070, consider enum datatype as common int16
            case TA_Base_Bus::DPT_ENUM_TYPE :
                {
                    // initialise register data block
                    registerDataBlock.setStartAndLength ( registerAddress, 1 );
                    
                    // use the number conversion utility, convert the data point value
                    // to INT16 format, where the -ve value is represented in 2's
                    // Complement format
                    WORD convertedNumber = 0;
                    if ( true == m_numberConversionUtil.convertToINT16_Value ( (int)newWrite.getValue().getEnum(), convertedNumber ) )
                    {
                        // set the value in data block
                        registerDataBlock.set( registerAddress, convertedNumber );
                    }
                    else
                    {
                        isValueValid = false;
                    }
                    break;
                }
                
            default :
				{
					isValueValid = false;
					break;
				}
		}

		if ( isValueValid )
		{
			//
			// sending the command
			//

			// update control status of the data point to indicate that it
			// is waiting for confirmation
			point->updateControlState ( TA_Base_Bus::pending, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db

			bool sendCommandResult = false;
			{
        		TA_Base_Core::ThreadGuard guard(m_modbusLock);
        		sendCommandResult = m_rtu.processSendingData(registerDataBlock,m_rtuServicePortNumber,m_slaveID);
			}
			// send command now
			if ( true == sendCommandResult )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                    "Successful to send command to %s, slave ID = %d. Start ABE timeout mechanism for %s",
                    m_stationSystemName.c_str(),
                    m_slaveID,
                    point->getDataPointName().c_str() );

				// activate ABE process for this datapoint
				point->startABETimeout( newWrite.getSessionID() );
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                    "Failed to send command to %s, slave ID = %d, for %s",
                    m_stationSystemName.c_str(),
                    m_slaveID,
                    point->getDataPointName().c_str() );

				// update control status of the data point to indicate that
				// the output command has failed
				point->updateControlState ( TA_Base_Bus::comms_failed, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db
			}
		}
		else
		{
			// update control status of the data point to indicate that
			// the output command has failed because of faulty value
			point->updateControlState ( TA_Base_Bus::comms_failed, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                "Failed to send the control data to device.  Check the validity of the value..." );
		}

		point = 0;
	}


	bool StationSystemOutputWriter::addDataPoint( const TA_Base_Bus::DataPoint * dp )
	{
        TA_ASSERT( 0 != dp, "dp == NULL in StationSystemOutputWriter::addDataPoint." );
        TA_ASSERT( true == dp->isWriteable(), "StationSystemOutputWriter is only interested in writeable (output) datapoint." );
        
        if ( false == m_addressBlock.isInRange( dp->getDataPointRegisterAddress() ) )
        {
            return false;
        }

        bool dpAdded = false;
        
        if ( true == isDpStationSystemNameMatched( dp->getDataPointName() ))
        {
            // if the station sub system name of this output worker thread is not empty,
            // i.e this worker is for SWC sub systems
            // save the data point to internal list for further references
            m_dataPoints[ dp->getEntityKey() ] = dp;
            
            dpAdded = true;
        }
        
        return dpAdded;
    }
    

    void StationSystemOutputWriter::removeDataPoint( const TA_Base_Bus::DataPoint * dp )
	{
        TA_ASSERT( 0 != dp, "dp == NULL" );

        RTUOutputDataPointMap::iterator itr = m_dataPoints.find( dp->getEntityKey() );

        if ( itr != m_dataPoints.end() )
        {
            m_dataPoints.erase( itr );
		}
	}
    
    
    bool StationSystemOutputWriter::isDpStationSystemNameMatched( const std::string & dpName )
    {
        // find the station/system/subsystem/equip configuration from the member
        // m_stationSystemName, minus the "_CONFIGURATION"
        std::string configuredSystem;
        std::string::size_type pos = m_stationSystemName.find ( "_CONFIGURATION", 0 );
        if ( pos != std::string::npos )
        {
            configuredSystem = m_stationSystemName.substr ( 0, pos );
            
            // check if the data point's name has the configured system/subsystem/equip name
            return ( 0 == configuredSystem.compare( 0, configuredSystem.length(), dpName.substr( 0, configuredSystem.length() ) ) );
        }
        // if station system name doesn't include "_CONFIGURATION", just return true to indicate this check is not need
        else
        {
            return true;
        }
        
    }

	void StationSystemOutputWriter::convertDigitalCodeToEType(const std::string& funCode)
	{
		//default write code
		m_digitWriteCode = E_DIGIT_0F;

		if (funCode.empty() || funCode == "0F")
		{
			m_digitWriteCode = E_DIGIT_0F;
		}
		else if (funCode == "10")
		{
			m_digitWriteCode = E_DIGIT_10;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Wrong digital function code : %s, set to default", funCode.c_str());
		}
	}
}
