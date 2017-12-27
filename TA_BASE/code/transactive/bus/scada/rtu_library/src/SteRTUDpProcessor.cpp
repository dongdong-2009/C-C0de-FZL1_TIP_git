/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/SteRTUDpProcessor.cpp $
  * @author:	Liu Yong
  * @version:	$Revision: #12 $
  *
  * Last modification:	$DateTime: 2016/04/08 14:49:53 $
  * Last modified by:	$Author: Liu Yong $
  *
  * Description:
  *
  *	The SteRTUDpProcessor object is responsible for processing input data polled from the PMS RTU
  *
  */

#include <set>
#include <math.h>
#include "time.h"
#include "ace/OS.h"

#include "bus/scada/rtu_library/src/SteRTUDpProcessor.h"
#include "bus/scada/rtu_library/src/NumberConversionUtil.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/exceptions/src/GenericAgentException.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

namespace TA_Base_Bus
{
	SteRTUDpProcessor::SteRTUDpProcessor( const std::string & stationName,
                                    const std::string & stationSystemName,
                                    unsigned long pollingStartAddress,
                                    unsigned long pollingEndAddress,
                                    unsigned long offsetAddress,
									const std::vector<std::pair<unsigned long, unsigned long> >& blockAddresses,
                                    int slaveID,
                                    int statusWordNumber,
									int analogueDpUpdateInterval)  //TD16169
	:
    ModbusDpProcessor(stationSystemName, pollingStartAddress, pollingEndAddress, offsetAddress,false, analogueDpUpdateInterval),
    m_stationName(stationName),
	m_stationSystemName( stationSystemName ),
    m_slaveID( slaveID ),
    m_statusWordNumber(statusWordNumber),
	m_blockAddresses( blockAddresses )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "stationName:%s,stationSystemName:%s, m_slaveID[%d]",m_stationName.c_str(), stationSystemName.c_str(), m_slaveID);

		checkBlockAddressRange();  //TD16182
		
        // start thread
        start();
	}

	SteRTUDpProcessor::~SteRTUDpProcessor()
	{
    }

	bool SteRTUDpProcessor::addDataPoint( TA_Base_Bus::DataPoint * dp )
	{
        TA_ASSERT( NULL != dp, "dp is NULL in RTUModbusTable::addDataPoint" );
        
        bool validAddress = false;
        unsigned long registerAddress = dp->getDataPointRegisterAddress();
        
        if ( ( 0 <= m_startAddress ) && ( 0 < m_lastDpAddress ) )
        {
            for(unsigned long index = 0; index < m_blockAddresses.size(); index++)
			{
				if( (m_blockAddresses[index].first <= registerAddress) &&
					  (m_blockAddresses[index].second >= registerAddress) )
				{
					validAddress = true;
					break;
				}
			}
            if ( validAddress == true )
            {
				validAddress = false;
                if ( true == isDpMatched( dp ) )
                {
                    // save the data point to internal list for further references
                    sortAndSaveDataPointToList( dp );
                    validAddress = true;
                    
                    addDataPointMap(dp);
                }
            }
        }

        
        return validAddress;
    }

	void SteRTUDpProcessor::processDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, TA_Base_Bus::EQualityStatus newStatus )
	{
        ModbusDataBlock* pDataBlock = new ModbusDataBlock();
		
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "addressBlock.start() = %lu and addressBlock.end() = %lu with m_offsetAddress = %lu", 
		(unsigned long)addressBlock.start(), (unsigned long)addressBlock.end(), (unsigned long)m_offsetAddress);
		
        pDataBlock->quality = newStatus;
        pDataBlock->timestamp = extractTimeStamp(m_startAddress - m_offsetAddress, addressBlock);

        // to process data block with station system offset address
        // so can process rtu status block as the same as other station systems
        int length = ( m_lastDpAddress - m_startAddress + 1 );
        pDataBlock->data.setStartAndLength( m_startAddress , length );
        
        // copy the data from phicial table to logical table
        for ( unsigned long loc = m_startAddress; loc <= m_lastDpAddress; ++ loc )
        {
            pDataBlock->data.set( loc, addressBlock[loc - m_offsetAddress] );
        }

		//wenching
		std::vector<std::pair<unsigned long, unsigned long> >::iterator blockAddressIt;

		for (blockAddressIt = m_blockAddresses.begin(); blockAddressIt != m_blockAddresses.end(); ++blockAddressIt)
		{
			pDataBlock->blockTimestamp[(*blockAddressIt).first] = extractTimeStamp((*blockAddressIt).first- m_offsetAddress, addressBlock);
		}
		
		insertQueue(pDataBlock);
	}


	void SteRTUDpProcessor::processTextDataPoint( TA_Base_Bus::DataPoint * dp )
	{
		TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );

		// get the size of the text data point, i.e number of chars/bytes
		int maxNumberOfBytes = dp->getMaxTextLength();

		// only proceed if the length is > 0
		if ( 0 >= maxNumberOfBytes )
		{
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Not processing text datapoint %s; MaxTextLength = %d",
                dp->getDataPointName().c_str(), maxNumberOfBytes );
            return;
        }

        unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress);  //TD16182

        std::string::size_type pos = std::string::npos;

        // special case: RTU config version tiiRTU-ConfigVersion
        pos = dp->getDataPointName().find( "tiiRTU-ConfigVersion" );
        if ( pos != std::string::npos )
        {
            // get RTU Config File version: word 9
            //TD15823, according to Dai Moi's comments, the version string should be:
            // [major version].[version].[index].[release]
            // bits 12-15: major version, if 0, need not display
            // bits 8-11: version number
            // bits 4-7: index number
            // bits 0-3: release number
			WORD registerValue = m_dataBlock->data[ registerAddress ];
            std::ostringstream ossValue;
            
            if ((( 0xF000 & registerValue ) >> 12) > 0)
            {
                // bits 12 - 15: major version number
                ossValue << ( ( 0xF000 & registerValue ) >> 12 ) << ".";
            }
            
            // bits 8 - 11: version number
            ossValue << ( ( 0x0F00 & registerValue ) >> 8 ) << ".";
            
            // bits 4 - 7: index number
            ossValue << ( ( 0x00F0 & registerValue ) >> 4 ) << ".";
            
            // bits 0 - 3: release number
            ossValue << ( 0x000F & registerValue );
            
            dpValue.setText ( ossValue.str() );
            
            TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, m_dataBlock->quality );  //TD16182
            dp->updateFieldState( newState );

            return;
        }

        // special case: RTU config down timestamp tiiRTU-ConfigDwnldTime
        pos = dp->getDataPointName().find( "tiiRTU-ConfigDwnldTime" );
        if ( pos != std::string::npos )
        {
            // get RTU Config File download timestamp: 2 words <word 10 (H) and 11 (L)>
            unsigned long timestamp;
            timestamp = m_dataBlock->data[ registerAddress ];
            timestamp <<= 16;
            timestamp |= m_dataBlock->data[ registerAddress + 1 ];
            
            //
            // convert RTU config file down load time stamp to string
            //
            time_t ltime = timestamp;
            struct tm tmstruct;
            ACE_OS::localtime_r(&ltime, &tmstruct);
            char str[30];
            sprintf(str, "%02d/%02d/%04d %02d:%02d:%02d", tmstruct.tm_mday, tmstruct.tm_mon + 1,
                tmstruct.tm_year + 1900, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);

            dpValue.setText ( str );
            
            TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, m_dataBlock->quality );  //TD16182
            dp->updateFieldState( newState );

            return;
        }

        // special case: RTU config software version tiiRTU-SoftwareVersion
        pos = dp->getDataPointName().find( "tiiRTU-SoftwareVersion" );
        if ( pos != std::string::npos )
        {
            // get RTU Software version: word 12
            // bits 0 - 3: Bug/Fault fixed
            // bits 4 - 7: Track Changes Request/Customised
            // bits 8 - 11: Release Version/New Feature
            // bits 12 - 15: 0x0D for Demo Release, 0x0E for Engineering Release, otherwise release number
			WORD registerValue = m_dataBlock->data[ registerAddress ];
            std::ostringstream ossValue;

            if ( 0x0D == ( ( 0xF000 & ( registerValue ) ) >> 12 ) )
            {
                ossValue << "D.";
            }
            else if ( 0x0E == ( ( 0xF000 & ( registerValue ) ) >> 12 ) )
            {
                ossValue << "E.";
            }
            else
            {
                ossValue << ( ( 0xF000 & ( registerValue ) ) >> 12 ) << ".";
            }
            
            // bits 8-11
            ossValue << ( ( 0x0F00 & ( registerValue ) ) >> 8 ) << ".";
            
            // bits 4-7
            ossValue << ( ( 0x00F0 & ( registerValue ) ) >> 4 ) << ".";
            
            // bits 0-3
            ossValue << ( 0x000F & ( registerValue ) );
            
            dpValue.setText ( ossValue.str() );
            
            TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, m_dataBlock->quality );  //TD16182
            dp->updateFieldState( newState );

            return;
        }


		// special case: PLC Version Number and PLC Config Type
		//

		// if the specified data point representing BMF data point, PLCA-ConfigType
        pos = dp->getDataPointName().find( "tiiPLCA-ConfigType" );
		if ( pos != std::string::npos ) //TD15153
		{
            std::ostringstream ossValue;
            char * configType[14] = {
                "1A",	// 0000
                "1B",	// 0001
			    "1C",	// 0010
			    "1D",	// 0011
			    "1E",	// 0100
			    "1F",	// 0101
			    "1G",	// 0110
    			"1H",	// 0111
			    "1I",	// 1000
			    "1J",	// 1001
			    "1K",	// 1010
			    "1L",	// 1011
			    "1M",	// 1100
			    "1N"	// 1101
            };

			//
			// occupy upper word of the register
			//
			WORD registerValue = m_dataBlock->data[ registerAddress ];

			// bits 12-15: PLC Configuration Type
            int plcConfigType = (0xF000 & registerValue) >> 12;

			if ( (plcConfigType >= 0 ) && (plcConfigType < 14 ) )
			{
				ossValue << configType[ plcConfigType ];

				// bits 10-11: not used

				// bits 8-9: number of MUXes
				ossValue << " with " << ( ( 0x0300 & ( registerValue ) ) >> 8 ) << " MUX(s)";
			}
			else
			{
				ossValue << "Unknown";
			}

			dpValue.setText ( ossValue.str() );

			TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, m_dataBlock->quality );  //TD16182
			dp->updateFieldState( newState );

			return;
		}

		// if the specified data point representing BMF data point, PLCA-VersionNumber
		pos = dp->getDataPointName().find( "tiiPLCA-VersionNumber" );
		if ( pos != std::string::npos ) //TD15153
		{
			std::ostringstream ossValue;

			//
			// occupy lower word of the register
			//

			WORD registerValue = m_dataBlock->data[ registerAddress ];

			// bit 7-4: Major version number
			ossValue << ( (0x00F0 & registerValue) >> 4 ) << ".";

			// bits 3-0: Minor version number
			ossValue << ( 0x000F & registerValue );

			dpValue.setText( ossValue.str() );

			TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, m_dataBlock->quality );   //TD16182
			dp->updateFieldState( newState );

			return;
		}

		//
		// if the data point is not one of the above, continue on
		//

		//
		// calculate the total number of words that makes up the
		// size of the text data point
		//
		int numberOfWords = 0;
		div_t divResult = div ( maxNumberOfBytes, 2 );
		numberOfWords = divResult.quot;
		if ( 0 < divResult.rem )
		{
			numberOfWords++;
		}

		//
		// extracting the data from the data block
		//
		std::ostringstream buffer;
		int currentByteIndex = 1;
		bool badQuality = false;
        WORD registerValue = 0;
		for ( int currentWord = 0; currentWord < numberOfWords; ++currentWord )
		{
			// only proceed if the queried address is within the range
			if ( true == m_dataBlock->data.isInRange( registerAddress + currentWord ) )
			{
				registerValue = m_dataBlock->data[ registerAddress + currentWord];

				//TD15784, don't swap the high and low bytes.
                //Add extra parameter to indicate if to swap bytes
                bool byteSwap = dp->getByteSwapIndication();
                if (byteSwap)
                {
                    buffer << (char) (0xFF & ( registerValue));	// low order byte
                }
                else
                {
                    buffer << (char) (0xFF & ( registerValue >> 8 ));	// high order byte
                }

                ++currentByteIndex;

				if ( currentByteIndex > maxNumberOfBytes )
				{
					break;
				}

                if (byteSwap)
                {
                    buffer << (char) (0xFF & ( registerValue >> 8 ));	// high order byte
                }
                else
                {
                    buffer << (char) (0xFF & registerValue);    // low order byte
                }

                ++currentByteIndex;

                if ( currentByteIndex > maxNumberOfBytes )
				{
					break;
				}
			}
            else
			{
                // if the address is not within the range of data block
				// set quality flag and then exit loop
				badQuality = true;
				break;
			}
		}

		dpValue.setText( buffer.str() );

		if ( true == badQuality )
		{
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR );  //TD16182
			dp->updateFieldState( newState );
		}
		else
		{
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, m_dataBlock->quality );   //TD16182
			dp->updateFieldState( newState );
		}
	}

    bool SteRTUDpProcessor::isDpMatched( TA_Base_Bus::DataPoint * dp)
    {
        // find the station/system/subsystem/equip configuration from the member
        // m_stationSystemName, minus the "_CONFIGURATION"
		std::string dpName = dp->getDataPointName();
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

	//TD16182
	void SteRTUDpProcessor::checkBlockAddressRange()
	{
        std::vector<std::pair<unsigned long, unsigned long> >::iterator blockAddressIt;

		if ( "" == m_stationSystemName )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RTUModbusTable::checkBlockAddressRange(): Station system data not initialised yet, return without checking block Addresses");

			return;
		}

		// Check if block addresses are empty
		if ( "" != m_stationSystemName && true == m_blockAddresses.empty() )
		{
			std::ostringstream desc;
			desc << "Subsystem [" 
				 << m_stationSystemName
				 << "] - block addresses undefined ";

			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}

		for (blockAddressIt=m_blockAddresses.begin(); blockAddressIt!=m_blockAddresses.end(); ++blockAddressIt)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RTUModbusTable[%s]::checkBlockAddressRange(): Sub block range - Start Address = %d, End Address = %d",
				m_stationSystemName.c_str(), (*blockAddressIt).first, (*blockAddressIt).second);
		}
		
		// Check the block address range, throw if it was configured wrongly
		for (blockAddressIt=m_blockAddresses.begin(); blockAddressIt!=m_blockAddresses.end(); ++blockAddressIt)
		{
			// Check if block start address out of polling address range
			if ( (*blockAddressIt).first < m_startAddress || (*blockAddressIt).first > m_lastDpAddress )
			{
				std::ostringstream desc;
				desc << "StationSystem [" 
					 << m_stationSystemName
					 << "] - start address is out of range. "
				     << "Polling start address = "
					 << m_startAddress
					 << " and end address = "
					 << m_lastDpAddress;

				TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
			}

			// Check if block end address out of polling address range
			if ( (*blockAddressIt).second < m_startAddress || (*blockAddressIt).second > m_lastDpAddress )
			{
				std::ostringstream desc;
				desc << "StationSystem [" 
					 << m_stationSystemName
					 << "] - end address is out of range. "
				     << "Polling start address = "
					 << m_startAddress
					 << " and end address = "
					 << m_lastDpAddress;

				TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
			}

			// Check if block start address greater than end address
			if ( (*blockAddressIt).second < (*blockAddressIt).first )
			{
				std::ostringstream desc;
				desc << "StationSystem [" 
					 << m_stationSystemName
					 << "] - start address is greater than end address for block with start address = "
				     << (*blockAddressIt).first
					 << " and end address = "
					 << (*blockAddressIt).second;

				TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
			}
		}

		// Check if there is any overlapped address range, throw if it was incorrect
		//TD17034
		if (false == m_blockAddresses.empty())
		{
			unsigned long prevStartAddress, prevEndAddress;
			blockAddressIt=m_blockAddresses.begin();
			prevStartAddress = (*blockAddressIt).first;
			prevEndAddress = (*blockAddressIt).second;
			++blockAddressIt;

			// Arrange the address range in order
			for (; blockAddressIt!=m_blockAddresses.end(); ++blockAddressIt)
			{
				if ( prevEndAddress >= (*blockAddressIt).first )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "RTUModbusTable[%s]::checkBlockAddressRange(): Block=[%d,%d] with Block=[%d,%d] are overlapped",
						m_stationSystemName.c_str(), prevStartAddress, prevEndAddress,
						(*blockAddressIt).first, (*blockAddressIt).second);

					std::ostringstream desc;
					desc << "StationSystem [" 
						 << m_stationSystemName
						 << "] - blocks address is overlapped.";

					TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
				}

				prevStartAddress = (*blockAddressIt).first;
				prevEndAddress = (*blockAddressIt).second;
			}
		}
	}

	timeb SteRTUDpProcessor::getTimeStamp(unsigned long registerAddress)
	{
		
		std::map<unsigned long, unsigned long>::iterator dpAddressBlockIt = m_datapointsAddressBlock.find(registerAddress);
		
		if ( dpAddressBlockIt != m_datapointsAddressBlock.end() )
		{
			std::map<unsigned long, timeb>::const_iterator iter = m_dataBlock->blockTimestamp.find(dpAddressBlockIt->second);

			if( m_dataBlock->blockTimestamp.end() != iter)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "SteRTUDpProcessor::getTimesStamp()[%s]: Register address - %d, found timestamp in datablock=%lu.%lu",
					m_stationSystemName.c_str(), registerAddress,  iter->second.time, iter->second.millitm);
				return iter->second;
			}
		}
	
		std::vector<std::pair<unsigned long, unsigned long> >::iterator blockAddressIt;

		for (blockAddressIt=m_blockAddresses.begin(); blockAddressIt!=m_blockAddresses.end(); ++blockAddressIt)
		{
			if ( registerAddress >= (*blockAddressIt).first && registerAddress <= (*blockAddressIt).second )
			{
				std::map<unsigned long, timeb>::const_iterator iter = m_dataBlock->blockTimestamp.find((*blockAddressIt).first);

				if( m_dataBlock->blockTimestamp.end() != iter)
				{
					m_datapointsAddressBlock[registerAddress] = (*blockAddressIt).first;

					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "SteRTUDpProcessor::getTimesStamp()[%s]: Register address - %d, found timestamp in datablock=%lu.%lu",
						m_stationSystemName.c_str(), registerAddress,  iter->second.time, iter->second.millitm);

					return iter->second;
				}
			}
		}
						
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "SteRTUDpProcessor::getTimeStamp[%s] - default timestamp returned [%lu.%lu].", 
				m_stationSystemName.c_str(), m_dataBlock->timestamp.time, m_dataBlock->timestamp.millitm);

	
		return TA_Base_Core::AlarmConstants::defaultTime;
		//return dataBlock.timestamp;
	}


     //TD16182
	timeb SteRTUDpProcessor::extractTimeStamp( unsigned long startTimeStampAddress, const TA_Base_Bus::DataBlock< WORD > & dataBlock )
	{
		timeb timestamp = TA_Base_Core::AlarmConstants::defaultTime;
		
		// RTU Timestamp format: first 2 words = UTC timestamp in secs in since start
		// of the epoch, last word (the 3rd) contains milliseconds.

		// get the UTC timestamp in secs
		unsigned long timestampInSeconds = dataBlock[ startTimeStampAddress ];
		timestampInSeconds <<= 16;
		timestampInSeconds |= dataBlock[ startTimeStampAddress + 1 ];

        // get the msecs.  Only accept value if it is between 0 and 1000 inclusive
        unsigned short timestampInMSeconds = 0;

        if ( dataBlock[ startTimeStampAddress + 2 ] <= 1000 )
		{
            timestampInMSeconds = dataBlock[ startTimeStampAddress + 2 ];
		}

		timestamp.time = timestampInSeconds;
   		timestamp.millitm = timestampInMSeconds;

		return timestamp;
	}
}
