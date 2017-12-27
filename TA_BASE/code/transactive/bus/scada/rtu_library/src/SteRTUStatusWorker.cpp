/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/RTUStatusWorker.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #7 $
  *
  * Last modification:	$DateTime: 2016/03/15 15:30:22 $
  * Last modified by:	$Author: tianwai.teo $
  *
  * Description:
  *
  *	The RTUStatusWorker object is responsible for processing PMS RTU status data
  *
  */

#if defined( WIN32 )
	#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "ace/OS.h"
#include "time.h"

#include "bus/scada/rtu_library/src/SteRTUStatusWorker.h"
#include "bus/scada/rtu_library/src/RTUStatusTableConstants.h"



#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/alarm/src/AlarmConstants.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	const unsigned int PRIMARY = 1;
	const unsigned int SECONDARY = 2;


	SteRTUStatusWorker::SteRTUStatusWorker( unsigned long statusOffsetAddress,
											unsigned long pollingStartAddress,
											unsigned long pollingEndAddress,
											int slaveID )
	:
	m_slaveID(slaveID),
    m_firstScanOfData( true ),
    m_rtuIsDownloadRequired( false ),
	m_rtuSwitching( false ),
    m_SWCWeightage( 0 ),
	m_primaryStatus( SECONDARY ),
    m_startAddress(pollingStartAddress),
    m_endAddress(pollingEndAddress),
    m_statusOffsetAddress( statusOffsetAddress ),
	m_rtuConfigurationFileVersion( 0 ),
	m_rtuConfigurationFileDownLoadTimestamp( 0 ),
	m_rtuSoftwareVersion( 0 )
	{
        // ++ TD15293 : AZ, make the handling simpler
        for (int i = MIN_WORD; i <= MAX_WORD; ++i)
        {
            m_commsWithSubSystemAtWord[i] = false;
        }
        // TD15293 ++
	}


	SteRTUStatusWorker::~SteRTUStatusWorker()
	{

	}

	void SteRTUStatusWorker::processRTUDataBlock ( const TA_Base_Bus::DataBlock< WORD > & addressBlock, const bool isCommsOK )
	{
		// NOTE: RTU Status table range is 0-32 words, but the DataPoint entities
		// associated with the RTU status info is within [0 + statusOffsetAddress, 32 + statusOffsetAddress],
		// therefore we need to copy the data from [0,32] block to [0 + statusOffsetAddress, 32 + statusOffsetAddress] block
        
        // set communication state
        m_isCommsOK = isCommsOK;

        TA_Base_Bus::DataBlock<WORD> dataBlock;

		int length = ( m_endAddress - m_startAddress + 1 );
		dataBlock.setStartAndLength( m_startAddress , length );

		// copy the data from original table to new rtu status table
		for ( int statusAddress = 0; statusAddress < length; ++statusAddress )
		{
			dataBlock.set( statusAddress + m_statusOffsetAddress, addressBlock[statusAddress] );
		}

        // process RTU Status data
        processRTUStatus( dataBlock );
	}


	void SteRTUStatusWorker::processRTUStatus( const TA_Base_Bus::DataBlock< WORD > & addressBlock )
	{
		WORD registerValue = 0;
		unsigned long startAddress = addressBlock.start();

		// RTU Timestamp format: first 2 words = UTC timestamp in seconds since start
		// of the epoch, last word (the 3rd) contains miliseconds.
		// Not interested in milliseconds as yet

		// RTU ID: bit 0 & 1 of word 3, 1 = RTU1 and 2 = RTU2

		// RTU Manufacturing ID: bit 2 - 15 of word 3

		// RTU Location: word 4 (H) & 5 (L)

		// RTU Status: word 6

		// RTU Config download required: bit 2 of word 6,
		// download is required if bit 2 = 1

		// get redundant status: bit 4 & 5 of word 6
		registerValue = addressBlock [ startAddress + 6 ];

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTU status word = 0X%X.", registerValue );
		
		//TD10586
		//0000 0000 0000 0100, bit2 indicates isDownloadRequired
		m_rtuIsDownloadRequired = (0x0004 & registerValue) > 0;

		//0000 0000 0000 1000, bit3 indicates isSwitching
		m_rtuSwitching = (0x0008 & registerValue) > 0;

		//0000 0000 0001 0000, bit4 indicates isPrimary
		//0000 0000 0010 0000, bit5 indicates isStandby
		//set PRIMARY only when isPrimary and not isStandby.
		m_primaryStatus = SECONDARY;
		if (( (0x0010 & registerValue) > 0) && ( (0x0020 & registerValue) == 0))
		{
			m_primaryStatus = PRIMARY;
		}

		// SWC Notation (Weight): word 7
		m_SWCWeightage = addressBlock [ startAddress + 7 ];

		// SWC Link Check: word 8 don't use at this stage

		// get RTU Config File version: word 9
		//TD15823, according to Dai Moi's comments, the version string should be:
		// [major version].[version].[index].[release]
		// bits 12-15: major version, if 0, need not display
		// bits 8-11: version number
		// bits 4-7: index number
		// bits 0-3: release number
		m_rtuConfigurationFileVersion = addressBlock [ startAddress + 9 ];

		// get RTU Config File download timestamp: 2 words <word 10 (H) and 11 (L)>
		m_rtuConfigurationFileDownLoadTimestamp = addressBlock[ startAddress + 10 ];
		m_rtuConfigurationFileDownLoadTimestamp <<= 16;
		m_rtuConfigurationFileDownLoadTimestamp |= addressBlock[ startAddress + 11 ];

		// get RTU Software version: word 12
		// bits 0 - 3: Bug/Fault fixed
		// bits 4 - 7: Track Changes Request/Customised
		// bits 8 - 11: Release Version/New Feature
		// bits 12 - 15: 0x0D for Demo Release, 0x0E for Engineering Release, otherwise release number
		m_rtuSoftwareVersion = addressBlock[ startAddress + 12 ];

        // ++ TD15293 : AZ, simplify

		// get comms status with subsystem associated with status word 13-31
        for (int i = MIN_WORD; i <= MAX_WORD; ++i)
        {
		    registerValue = addressBlock [ startAddress + i ];

		    // if both bits 14 and 15 are 1 --> comms OK
		    if  ( ( 1 == ( 0x0001 & ( registerValue >> 14 ) ) ) && ( 1 == ( registerValue >> 15 ) ) )
		    {
			    m_commsWithSubSystemAtWord[i] = true;
		    }
		    // if any or both of bits 14 and 15 are not 1 --> comms not OK
		    else
		    {
			    m_commsWithSubSystemAtWord[i] = false;
		    }
        }

        // TD15293 ++

		// set the flag to indicate first scan of data has been processed
		m_firstScanOfData = false;
	}


	bool SteRTUStatusWorker::getIsRTUSwitching() const
	{
		return (m_isCommsOK && m_rtuSwitching);
	}


	//TD10586
	bool SteRTUStatusWorker::getIsDownloadRequired() const
	{
		return (m_isCommsOK && m_rtuIsDownloadRequired);
	}
	

	bool SteRTUStatusWorker::getIsPrimaryRTU() const
	{
		return (m_isCommsOK && PRIMARY == m_primaryStatus);
	}

    bool SteRTUStatusWorker::getIsCommsOK()
    {
        return m_isCommsOK;
    }
    
    // ++ TD15293 : AZ, make this a bit simpler
	bool SteRTUStatusWorker::getIsCommsValidAtWord(int index) const
	{
        TA_Assert(index >= MIN_WORD && index <= MAX_WORD);
        
		return (m_isCommsOK && m_commsWithSubSystemAtWord[index]);
	}

	bool SteRTUStatusWorker::getIsCommsWithStationSystemsValid ( int systemID ) const
	{
        // if station system id equil to rtu's slave id, just use communication status as valid indicator
        if (systemID == m_slaveID)
        {
            return m_isCommsOK;
        }

        bool retval = true;

		switch ( systemID )
		{
		case WORD13_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(13);
            break;

		case WORD14_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(14);
            break;

		case WORD15_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(15);
			break;

		case WORD16_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(16);
			break;

		case WORD17_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(17);
            break;

		case WORD18_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(18);
            break;

		case WORD19_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(19);
            break;

		case WORD20_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(20);
            break;

		case WORD21_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(21);
            break;

		case WORD22_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(22);
            break;

		case WORD23_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(23);
            break;

		case WORD24_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(24);
            break;

		case WORD25_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(25);
            break;

		case WORD26_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(26);
            break;

		case WORD27_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(27);
            break;

		case WORD28_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(28);
            break;

		case WORD29_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(29);
            break;

		case WORD30_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(30);
            break;

		case WORD31_RTU_STATUS_TABLE:
            retval = getIsCommsValidAtWord(31);
            break;

		default:
            retval = false;
		}

        return retval;
	}
    // TD15293 ++


	int SteRTUStatusWorker::getSWCWeightage() const
	{
        return m_isCommsOK ? m_SWCWeightage : 0;
	}

	bool SteRTUStatusWorker::getIsCommsWithStatusWordNumber( int wordIndex ) const
	{
		// if station system id equil to rtu's slave id, just use communication status as valid indicator
		if (wordIndex == m_slaveID)
		{
			return m_isCommsOK;
		}
		return getIsCommsValidAtWord(wordIndex);
	}
}
