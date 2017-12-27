/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/ModbusDpProcessor.cpp $
  * @author:	Liu Yong
  * @version:	$Revision: #12 $
  *
  * Last modification:	$DateTime: 2016/04/08 14:49:53 $
  * Last modified by:	$Author: Liu Yong $
  *
  * Description:
  *
  *	The ModbusDpProcessor object is responsible for processing input data polled with modbus block format.
  *
  */

#include <set>
#include <math.h>
#include "time.h"
#include "ace/OS.h"

#include "bus/scada/rtu_library/src/ModbusDpProcessor.h"
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
	ModbusDataBlock::ModbusDataBlock()
	:
    quality( TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON ),
	timestamp( TA_Base_Core::AlarmConstants::defaultTime )
	{
	}
	
	ModbusDpProcessor::ModbusDpProcessor( const std::string & dpProcessorName,
	                                unsigned long pollingStartAddress,
                                    unsigned long pollingEndAddress,
                                    unsigned long offsetAddress,
                                    bool 		  reverseDiBitOrder,
									int analogueDpUpdateInterval)  //TD16169
	:
	m_dpProcessorName( dpProcessorName ),
	m_startAddress( pollingStartAddress ),
	m_lastDpAddress( pollingEndAddress ),
    m_offsetAddress( offsetAddress ),
	m_threadTerminated( false ),
	m_reverseDiBitOrder(reverseDiBitOrder),
	m_queueSemaphore( 0 ),
	m_analogueDpUpdateInterval( DEFAULT_ANALOGUE_DP_UPDATE_INTERVAL ), //TD16169
	m_analogueDpUpdateCounter( 0 ),
	m_dataBlock(NULL),
	m_prevDataBlock(NULL)
	{
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Constructing ModbusDpProcessor for [%s], start/end/offset address [%u/%u/%u]", 
            dpProcessorName.c_str(), m_startAddress, m_lastDpAddress, m_offsetAddress); 

        //TD16169
		if ( analogueDpUpdateInterval > 0 )
		{
			m_analogueDpUpdateInterval = analogueDpUpdateInterval;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "DpProcessor %s: set the analogue datapoints update interval to a default value [%d].", 
				m_dpProcessorName.c_str(), m_analogueDpUpdateInterval);
		}
	}

	ModbusDpProcessor::~ModbusDpProcessor()
	{
        // terminate thread
        terminateAndWait();
    }

	bool ModbusDpProcessor::addDataPoint( TA_Base_Bus::DataPoint * dp )
	{
        TA_ASSERT( NULL != dp, "dp is NULL in RTUModbusTable::addDataPoint" );
        
        bool validAddress = false;
        
        if ( ( 0 <= m_startAddress ) && ( 0 < m_lastDpAddress ) )
        {
            if ( true == isDpMatched( dp ) )
            {
                // save the data point to internal list for further references
                sortAndSaveDataPointToList( dp );
                validAddress = true;
                
                addDataPointMap(dp);
            }
        }
       
        return validAddress;
    }

    // ++ TD15293 : AZ, optimize the updating of DPs by adding map of block address
	// TD16169 : seperate handling for analogue datapoints
	void ModbusDpProcessor::addDataPointMap( TA_Base_Bus::DataPoint * dp )
    {
        TA_Base_Core::ThreadGuard guard( m_datapointsLock );

		unsigned long registerAddress = dp->getDataPointRegisterAddress();

        // add to the map
        // should depend on datapoint data type
        if (dp->getIsDigitalDataPoint())
        {
            std::vector< TA_Base_Bus::DataPoint* > &dataPointList = m_blockDPs[registerAddress];
            dataPointList.push_back(dp);
        }
        else if (dp->getIsTextDataPoint())
        {
            //m_blockTextDPs.push_back(dp);
            // add this dp to all the register addresses that it occupied
            for(int i=0; i < dp->getMaxTextLength(); ++i)
            {
                std::vector< TA_Base_Bus::DataPoint* > &dataPointList = m_blockTextDPs[registerAddress+i];
                dataPointList.push_back(dp);
            }
        }
        // others are all analogue datapoints
        else
        {
            std::vector< TA_Base_Bus::DataPoint* > &dataPointList = m_blockAnalogueDPs[registerAddress];
            dataPointList.push_back(dp);
            
            if (use2Registers(dp))
            {
                std::vector< TA_Base_Bus::DataPoint* > & dataPointList = m_blockAnalogueDPs[registerAddress + 1];
                dataPointList.push_back(dp);
            }
        }
    }


	// TD16169: Seperate handling for analogue datapoint
    void ModbusDpProcessor::removeDataPoint( TA_Base_Bus::DataPoint * dp )
    {
        TA_ASSERT( NULL != dp, "dp is NULL in ModbusDpProcessor::removeDataPoint" );
		TA_Base_Core::ThreadGuard guard ( m_datapointsLock );

        unsigned long dataPointKey = dp->getEntityKey();

		TA_Base_Bus::DataPoint * dpRemoved = NULL;
		for ( std::vector< TA_Base_Bus::DataPoint * >::iterator itr = m_datapoints.begin(); itr != m_datapoints.end();/* ++itr*/ )
		{
			if ( dataPointKey == (*itr)->getEntityKey() )
			{
				dpRemoved = (*itr);
				itr = m_datapoints.erase( itr );	// must get return iterator

                // ++ TD15293 : AZ, optimize the updating of DPs
                // remove from map
				unsigned long registerAddress = dpRemoved->getDataPointRegisterAddress();
			
                // base on datapoint data type
                if (dp->getIsDigitalDataPoint())
                {
                    std::vector<TA_Base_Bus::DataPoint*> &blockIt = m_blockDPs[registerAddress];
                    std::vector<TA_Base_Bus::DataPoint*>::iterator it;
                    
                    for (it = blockIt.begin(); it != blockIt.end(); ++it)
                    {
                        if ((*it)->getEntityKey() == dataPointKey)
                        {
                            blockIt.erase(it);	// legal erase as break here
                            break;
                        }
                    }
                }
                else if (dp->getIsTextDataPoint())
                {
                    for(int i=0; i < dpRemoved->getMaxTextLength(); ++i)
                    {
                        std::vector< TA_Base_Bus::DataPoint* > &blockIt = m_blockTextDPs[registerAddress+i];
                        for (std::vector<TA_Base_Bus::DataPoint*>::iterator it = blockIt.begin();
                                    it != blockIt.end(); ++it)
                        {
                            if ((*it)->getEntityKey() == dataPointKey)
                            {
                                blockIt.erase(it);	// legal erase as break here
                                break;
                            }
                        }
                    }
                }
                // others are all analogue datapoints
                else
                {
                    std::vector<TA_Base_Bus::DataPoint*> &blockIt = m_blockAnalogueDPs[registerAddress];
                    std::vector<TA_Base_Bus::DataPoint*>::iterator it;
                    
                    for (it = blockIt.begin(); it != blockIt.end(); ++it)
                    {
                        if ((*it)->getEntityKey() == dataPointKey)
                        {
                            blockIt.erase(it); // legal erase as break here
                            break;
                        }
                    }
                    
                    if(use2Registers(dpRemoved))
                    {
                        std::vector< TA_Base_Bus::DataPoint* > &nextblockIt = m_blockAnalogueDPs[registerAddress + 1];
                        for (it = nextblockIt.begin(); it != nextblockIt.end(); ++it)
                        {
                            if ((*it)->getEntityKey() == dataPointKey)
                            {
                                blockIt.erase(it); // legal erase as break here
                                break;
                            }
                        }
                    }
                }
            }
			else
			{
				++itr;
			}
		}

	}

	void ModbusDpProcessor::run()
	{
        bool queueEmpty = true;
         m_prevDataBlock = new ModbusDataBlock;

		// enter the thread loop
		while ( false == m_threadTerminated )
		{
			m_queueSemaphore.wait();

			// if not running
			if ( m_threadTerminated )
			{
				// exit thread loop
				break;
			}

			// check the modbus table queue
            // if there is data available
            {
                TA_Base_Core::ThreadGuard guard( m_dataBlockLock );
                queueEmpty = m_dataBlockQueue.empty();
            }
                
            //TD15792, move the first logic into updateDataPoint() function.
			while (false == m_threadTerminated && false == queueEmpty)
            {
                // ++ TD15293 : AZ, optimize the updating of DPs
				unsigned long qSize = 0;
				{
					TA_Base_Core::ThreadGuard guard( m_dataBlockLock );
					qSize = m_dataBlockQueue.size();
					m_dataBlock = m_dataBlockQueue.front();
					m_dataBlockQueue.pop();
				}
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "DpProcessor %s start updating datapoints. now queue size is:%d.", m_dpProcessorName.c_str(),qSize);
                
                int lCount;

				lCount = updateDataPoints();
				delete m_prevDataBlock;
				m_prevDataBlock = m_dataBlock;
				m_dataBlock = NULL;
				
                {
                    TA_Base_Core::ThreadGuard guard( m_dataBlockLock );
                    queueEmpty = m_dataBlockQueue.empty();
                }
            }
        }

        if (NULL != m_prevDataBlock)
        {
			delete m_prevDataBlock;
			m_prevDataBlock = NULL;
        }

        if (NULL != m_dataBlock)
        {
			delete m_dataBlock;
			m_dataBlock = NULL;
        }

		// exit thread loop --> reset the flag
		m_threadTerminated = false;
	}

	void ModbusDpProcessor::terminate()
	{
		// the thread has been terminated, set the flag
		m_threadTerminated = true;

		// notify the running thread to break the thread loop
		m_queueSemaphore.post();
	}
	
	void ModbusDpProcessor::insertQueue(ModbusDataBlock* modbusDataBlock)
	{
		if (false == m_threadTerminated)
		{
			{
				TA_Base_Core::ThreadGuard guard( m_dataBlockLock );
				m_dataBlockQueue.push(modbusDataBlock);
			}

			// notify the running thread to process the modbus table queue
			m_queueSemaphore.post();	
		}
		else
		{
			delete modbusDataBlock;
		}
	}


	//TD16169: update analogue datapoint update interval
	void ModbusDpProcessor::updateAnalogueDpUpdateInterval(int analogueDpUpdateInterval)
	{
		if ( analogueDpUpdateInterval > 0 )
		{
			m_analogueDpUpdateInterval = analogueDpUpdateInterval;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New analogue data point update interval (%d) not in valid range.", analogueDpUpdateInterval );
		}
	}


    // ++ TD15293 : AZ, optimize the updating of DPs
	int ModbusDpProcessor::populateDataPoints()
	{
		TA_Base_Core::ThreadGuard guard( m_datapointsLock );

        int updateCount = 0;

		// update the DataPoint objects with the latest values in the modbus table
        for ( std::vector< TA_Base_Bus::DataPoint* >::iterator dpItr = m_datapoints.begin(); dpItr != m_datapoints.end(); ++dpItr )
		{
            processAddressDataPoint(*dpItr);
            ++updateCount;
		}
		
		updateCount += populateSpecialDataPoints();
        return updateCount;
	}

	//TD16169 : Update analogue datapoints according to the interval set
    int ModbusDpProcessor::updateDataPoints()
    {
        int updateCount = 0;
        std::vector<TA_Base_Bus::DataPoint*>::iterator dpItr;
		++m_analogueDpUpdateCounter;

        // should update all DPs when the quality has changed
		if ( m_dataBlock->quality != m_prevDataBlock->quality)
		{
			updateCount = populateDataPoints();
			m_analogueDpUpdateCounter = 0;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DpProcessor: %s: finish updating all %d datapoints.", m_dpProcessorName.c_str(), updateCount);
			return updateCount;
		}

        // to keep track of which DPs have been processed
        std::set<TA_Base_Bus::DataPoint*> processedAIDPs;
        std::set<TA_Base_Bus::DataPoint*> processedTextDPs;
		bool updateAnalogueDps = (0 == m_analogueDpUpdateCounter % m_analogueDpUpdateInterval);

	    TA_Base_Core::ThreadGuard guard( m_datapointsLock );

        unsigned long startAddress = std::max(m_startAddress, (unsigned long)m_dataBlock->data.start());
        unsigned long endAddress = std::min(m_lastDpAddress, (unsigned long)m_dataBlock->data.end());

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "startAddress[%lu],endAddress[%lu],updateAnalogueDps[%d]", 
		startAddress,endAddress,updateAnalogueDps);

		
        for (unsigned long i = startAddress; i <= endAddress; ++i)
        {
			if (m_dataBlock->data[i] == m_prevDataBlock->data[i])
			{
				// No changes; continue to check next DataBlock
				continue;
			}
			
			{
				// update dps which are not analogue or text dps
				BlockDpIt blockIt = m_blockDPs.find(i);
				if (blockIt != m_blockDPs.end())
				{
					for (dpItr = blockIt->second.begin(); dpItr != blockIt->second.end(); ++dpItr)
					{
						++updateCount;
						processAddressDataPoint(*dpItr);
					}	
				}
			}

			if ( false == updateAnalogueDps )
			{
				// update analogue dps
				BlockDpIt blockIt = m_blockAnalogueDPs.find(i);
				if (blockIt != m_blockAnalogueDPs.end())
				{
					for (dpItr = blockIt->second.begin(); dpItr != blockIt->second.end(); ++dpItr)
					{
						// special case for those require more than 1 register, avoid doing this twice
						if (use2Registers(*dpItr))
						{
							if (processedAIDPs.find(*dpItr) == processedAIDPs.end())
							{
								++updateCount;
								processAddressDataPoint(*dpItr);
								processedAIDPs.insert(*dpItr);
							}
						}
						else
						{
							++updateCount;
							processAddressDataPoint(*dpItr);
						}
					}
				}
			}

			{
				// udpate text datapoints
				BlockDpIt blockIt = m_blockTextDPs.find(i);
				if (blockIt != m_blockTextDPs.end())
				{
					for (dpItr = blockIt->second.begin(); dpItr != blockIt->second.end(); ++dpItr)
					{
						if (processedTextDPs.find(*dpItr) == processedTextDPs.end())
						{
							++updateCount;
							processAddressDataPoint(*dpItr);
							processedTextDPs.insert(*dpItr);
						}
					}
				}
			}
        }

		if ( true == updateAnalogueDps )
		{
			// update analogue dps
			for (BlockDpIt blockIt=m_blockAnalogueDPs.begin(); blockIt !=m_blockAnalogueDPs.end(); ++blockIt)
			{
				for (dpItr = blockIt->second.begin(); dpItr != blockIt->second.end(); ++dpItr)
				{
					++updateCount;
					processAddressDataPoint(*dpItr);
				}
			}
        }
		//process special datapoint which don't follow the address processing regularly.
		//The special processing should be implemented by the derived DpProcessor class.
		int specialUpdateCount = processSpecialDataPoint();
		
        if (updateCount > 0 || specialUpdateCount > 0)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DpProcessor %s: finish updating  %d address datapoints and %d special datapoint.", 
			m_dpProcessorName.c_str(), updateCount,specialUpdateCount);
        }
        
        return updateCount + specialUpdateCount;
    }


    bool ModbusDpProcessor::use2Registers(TA_Base_Bus::DataPoint * dp)
    {
        switch (dp->getDataPointDataType())
        {
        case TA_Base_Bus::DPT_IEEE32_TYPE:
		case TA_Base_Bus::DPT_INT32_TYPE:
		case TA_Base_Bus::DPT_INT32S_TYPE:
        case TA_Base_Bus::DPT_UINT32_TYPE:
            return true;
        default:
        	break;
        }

        return false;
    }
    

	void ModbusDpProcessor::processAddressDataPoint( TA_Base_Bus::DataPoint * dp)
    {
        switch ( dp->getDataPointDataType() )
        {
        // if the data point is an IEEE 32bit floating point number
        case TA_Base_Bus::DPT_IEEE32_TYPE:
            processIEEE32DataPoint( dp );
            break;
            
        // if the data point is a 32bit integer number, where the signed value is represented
		// in 2's Complement format
		case TA_Base_Bus::DPT_INT32_TYPE:
            processINT32DataPoint( dp );
            break;
            
        // if the data point is a 32bit integer number, where the signed value is represented
		// in the most significant bit
		case TA_Base_Bus::DPT_INT32S_TYPE:
            processINT32SDataPoint( dp );
			break;
        
        // if the data point is a 32bit unsigned integer number
        case TA_Base_Bus::DPT_UINT32_TYPE:
            processUINT32DataPoint( dp );
			break;
        
        // if the data point is a 16bit integer number, where the signed value is represented
		// in 2's Complement format
		case TA_Base_Bus::DPT_INT16_TYPE:
            processINT16DataPoint( dp );
            break;

        // if the data point is a 16bit integer number, where the signed value is represented
        // in the most significant bit
        case TA_Base_Bus::DPT_INT16S_TYPE:
            processINT16SDataPoint( dp );
            break;
            
        // if the data point is representing a 16bit unsigned integer number or an enum
        case TA_Base_Bus::DPT_UINT16_TYPE:
            processUINT16DataPoint( dp );
			break;

        // if the data point is a digital input
        case TA_Base_Bus::DPT_BOOL_TYPE:
            processDigitalDataPoint( dp );
            break;
        
        // if the data point is a text data point
		case TA_Base_Bus::DPT_TEXT_TYPE:
            processTextDataPoint( dp );
            break;

		// if the data point is a Enum (16bit integer number), where the signed value is represented
		// in 2's Complement format
		case TA_Base_Bus::DPT_ENUM_TYPE:
        
            processEnumDataPoint( dp );
            break;
            
        default:
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unable to process datapoint (%s) with unknown DataPointDataType (%d)",
                dp->getDataPointName().c_str(), dp->getDataPointDataType() );
            break;
        }
    }
    // TD15293 ++


	bool ModbusDpProcessor::getBooleanValue( int bitMask, WORD registerValue )
	{
		return ( 0 != ( bitMask & registerValue ) );
	}


	void ModbusDpProcessor::processIEEE32DataPoint( TA_Base_Bus::DataPoint * dp )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress); //TD16182
		// get the first register associated with the data point's register address
		WORD registerValue1 = m_dataBlock->data[ registerAddress ];

		// get the second register associated with the data point's register address,
		// make sure it is still within the address block
		registerAddress += 1;
		if ( ( registerAddress >= (unsigned long) m_dataBlock->data.start() ) && 
			 ( registerAddress <= (unsigned long) m_dataBlock->data.end() ) )
		{
			double IEEE_Value = 0.0;
			if (0 < dp->getBitWidth())
			{
				IEEE_Value = conversion2WordNumber(dp, *m_dataBlock, DPT_IEEE32_TYPE);
			}
			else
			{
				WORD registerValue2 = m_dataBlock->data[ registerAddress ];

				// TD# 3603: swap the high and low order 16-bit words
				// use the number conversion utility, extract the IEEE 32bit floating point number
				// stored in the registers
				if ( true == dp->getWordSwapIndication() )
				{
					IEEE_Value = m_numberConversionUtil.convertFromIEEE32_Value( registerValue2, registerValue1 );
				}
				else
				{
					IEEE_Value = m_numberConversionUtil.convertFromIEEE32_Value( registerValue1, registerValue2 );
				}
			}

			// set up DataPointState with the specified timestamp, quality and value
            TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( IEEE_Value, dp->getPrecision() );
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, m_dataBlock->quality );

			// update the datapoint
			dp->updateFieldState( newState );
		}
	}


	void ModbusDpProcessor::processINT32DataPoint( TA_Base_Bus::DataPoint * dp )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();
		timeb currentTimeStamp = getTimeStamp(registerAddress);  //TD16182

		// get the first register associated with the data point's register address
		WORD registerValue1 = m_dataBlock->data[ registerAddress ];

		// get the second register associated with the data point's register address,
		// make sure it is still within the address block
		registerAddress += 1;
		if ( registerAddress >= (unsigned long) m_dataBlock->data.start() && 
			 registerAddress <= (unsigned long) m_dataBlock->data.end() )
		{
			double int32Value = 0.0;
			if (0 < dp->getBitWidth())
			{
				int32Value = conversion2WordNumber(dp, *m_dataBlock, DPT_INT32_TYPE);
			}
			else
			{
				WORD registerValue2 = m_dataBlock->data[ registerAddress ];

				// TD# 3603: swap the high and low order 16-bit words
				// use the number conversion utility, extract the 32bit integer number
				// stored in the registers (signed value represented in 2's Complement format)
				
				if ( true == dp->getWordSwapIndication() )
				{
					int32Value = m_numberConversionUtil.convertFromINT32_Value( registerValue2, registerValue1 );
				}
				else
				{
					int32Value = m_numberConversionUtil.convertFromINT32_Value( registerValue1, registerValue2 );
				}
			}

			// set up DataPointState with the specified timestamp, quality and value
            TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( int32Value, dp->getPrecision() );
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, m_dataBlock->quality );
			
			// update the datapoint
            dp->updateFieldState( newState );
		}
	}


	void ModbusDpProcessor::processINT32SDataPoint( TA_Base_Bus::DataPoint * dp )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress);  //TD16182

		// get the first register associated with the data point's register address
		WORD registerValue1 = m_dataBlock->data[ registerAddress ];

		// get the second register associated with the data point's register address,
		// make sure it is still within the address block
		registerAddress = registerAddress + 1;
		if ( registerAddress >= (unsigned long) m_dataBlock->data.start() && 
			 registerAddress <= (unsigned long) m_dataBlock->data.end() )
		{
			double int32SValue = 0.0;
			if (0 < dp->getBitWidth())
			{
				int32SValue = conversion2WordNumber(dp, *m_dataBlock, DPT_INT32S_TYPE);
			}
			else
			{
				WORD registerValue2 = m_dataBlock->data[ registerAddress ];

				// TD# 3603: swap the high and low order 16-bit words
				// use the number conversion utility, extract the 32bit integer number
				// stored in the registers (signed value represented in the most significant bit)

				if ( true == dp->getWordSwapIndication() )
				{
					int32SValue = m_numberConversionUtil.convertFromINT32S_Value( registerValue2, registerValue1 );
				}
				else
				{
					int32SValue = m_numberConversionUtil.convertFromINT32S_Value( registerValue1, registerValue2 );
				}
			}
			// set up DataPointState with the specified timestamp, quality and value
            TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( int32SValue, dp->getPrecision() );
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, m_dataBlock->quality );

			// update the datapoint
			dp->updateFieldState( newState );
		}
	}


	void ModbusDpProcessor::processUINT32DataPoint( TA_Base_Bus::DataPoint * dp )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress);  //TD16182

		// get the first register associated with the data point's register address
		WORD registerValue1 = m_dataBlock->data[ registerAddress ];

		// get the second register associated with the data point's register address,
		// make sure it is still within the address block
		registerAddress = registerAddress + 1;
		if ( registerAddress >= (unsigned long) m_dataBlock->data.start() && 
			 registerAddress <= (unsigned long) m_dataBlock->data.end() )
		{
			double uint32Value = 0.0;
			if (0 < dp->getBitWidth())
			{
				uint32Value = conversion2WordNumber(dp, *m_dataBlock, DPT_UINT32_TYPE);
			}
			else
			{
				WORD registerValue2 = m_dataBlock->data[ registerAddress ];

				// TD# 3603: swap the high and low order 16-bit words
				// use the number conversion utility, extract the 32bit unsigned integer number
				// stored in the registers
				if ( true == dp->getWordSwapIndication() )
				{
					uint32Value = m_numberConversionUtil.convertFromUINT32_Value( registerValue2, registerValue1 );
				}
				else
				{
					uint32Value = m_numberConversionUtil.convertFromUINT32_Value( registerValue1, registerValue2 );
				}
			}

			// set up DataPointState with the specified timestamp, quality and value
            TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( uint32Value, dp->getPrecision() );
            TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, m_dataBlock->quality );

			// update the datapoint
			dp->updateFieldState( newState );
		}
	}


	void ModbusDpProcessor::processINT16DataPoint( TA_Base_Bus::DataPoint * dp )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress);   //TD16182

		// get the value of the register associated with the data point's register address
		WORD registerValue = m_dataBlock->data[ registerAddress ];

		int int16Value = 0;
		if (0 < dp->getBitWidth())
		{
			int16Value = conversionWordNumber(dp, *m_dataBlock, DPT_INT16_TYPE);
		}
		else
		{
			// use the number conversion utility, extract the 16bit integer number
			// stored in the registers (signed value represented in 2's Complement format)
			int16Value = m_numberConversionUtil.convertFromINT16_Value( registerValue );
		}

		// set up DataPointState with the specified timestamp, quality and value
        TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
		dpValue.setFloat( int16Value, dp->getPrecision() );
		TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, m_dataBlock->quality );

		// update the datapoint
		dp->updateFieldState( newState );
	}


	void ModbusDpProcessor::processEnumDataPoint( TA_Base_Bus::DataPoint * dp )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress);  //TD16182

		// get the value of the register associated with the data point's register address
		WORD registerValue = m_dataBlock->data[ registerAddress ];

		// use the number conversion utility, extract the 16bit integer number
		// stored in the registers (signed value represented in 2's Complement format)
		int int16Value = m_numberConversionUtil.convertFromINT16_Value( registerValue );

		// set up DataPointState with the specified timestamp, quality and value
		TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
        dpValue.setEnum( (unsigned long)int16Value );
		TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, m_dataBlock->quality );

		// update the datapoint
		dp->updateFieldState( newState );
	}

	void ModbusDpProcessor::processINT16SDataPoint( TA_Base_Bus::DataPoint * dp )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress);  //TD16182

		// get the value of the register associated with the data point's register address
		WORD registerValue = m_dataBlock->data[ registerAddress ];

		int int16SValue = 0;
		if (0 < dp->getBitWidth())
		{
			int16SValue = conversionWordNumber(dp, *m_dataBlock, DPT_INT16S_TYPE);
		}
		else
		{
			// use the number conversion utility, extract the 16bit integer number
			// stored in the registers (signed value represented in the most significant bit)
			int16SValue = m_numberConversionUtil.convertFromINT16S_Value ( registerValue );
		}

		// set up DataPointState with the specified timestamp, quality and value
        TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
		dpValue.setFloat( int16SValue, dp->getPrecision() );
		TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, m_dataBlock->quality );

		// update the datapoint itself
		dp->updateFieldState( newState );
	}

	void ModbusDpProcessor::processUINT16DataPoint( TA_Base_Bus::DataPoint * dp )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress);  //TD16182

		// get the first register associated with the data point's register address
		WORD registerValue = m_dataBlock->data[ registerAddress ];

		int uint16Value = 0;
		if (0 < dp->getBitWidth())
		{
			uint16Value = conversionWordNumber(dp, *m_dataBlock, DPT_UINT16_TYPE);
		}
		else
		{
			// use the number conversion utility, extract the 16bit unsigned integer number
			// stored in the register
			uint16Value = m_numberConversionUtil.convertFromUINT16_Value ( registerValue );
		}
		// set up DataPointState with the specified timestamp, quality and value
		TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
        dpValue.setFloat( uint16Value, dp->getPrecision() );
		TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, m_dataBlock->quality );

		// update the datapoint itself
		dp->updateFieldState( newState );
	}


	void ModbusDpProcessor::processDigitalDataPoint( TA_Base_Bus::DataPoint * dp )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress);  //TD16182

		// get the value of the register associated with the data point's register address
		WORD registerValue = m_dataBlock->data[ registerAddress ];

		// get the bit address of the digital data point
		int bitAddress = dp->getDataPointBitAddress();

		if (true == m_reverseDiBitOrder)
		{
			bitAddress = 15 - bitAddress;
		}

		WORD bitmask = 0x0001 << bitAddress;

		// set up DataPointState with the specified timestamp, quality and value
		TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
        dpValue.setBoolean( getBooleanValue( bitmask, registerValue ) );
        TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, m_dataBlock->quality );

		// update the datapoint
		dp->updateFieldState( newState );
	}


	void ModbusDpProcessor::processTextDataPoint( TA_Base_Bus::DataPoint * dp )
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

	WORD ModbusDpProcessor::getTheMixedWord( unsigned long startBitAddress, WORD lowWord, WORD highWord )
	{
		//      LowWord   | highWord;
		//0123456789ABCDEF0123456789ABCDEF
		//     0123456789ABCDEF	// if startBitAddress = 5;
		WORD resWord = 0;
		if (startBitAddress > 15)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "startBitAddress[%d] not in range.", startBitAddress);
			return resWord;
		}
		lowWord =  lowWord  >> startBitAddress;
		highWord = highWord << (16 - startBitAddress);
		resWord = lowWord | highWord;
		return resWord;
	}

	int ModbusDpProcessor::conversionWordNumber( TA_Base_Bus::DataPoint* dp, const ModbusDataBlock& dataBlock, TA_Base_Bus::EDataPointDataType dataType )
	{
		long bitWidth = dp->getBitWidth();
		if (16 < bitWidth || bitWidth <= 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "bit width: %d, is not in the range", bitWidth);
			return 0;	// not process.
		}

		unsigned long registerAddress = dp->getDataPointRegisterAddress();
		WORD registerValue = 0;

		registerValue = getTheMixedWord(
			dp->getDataPointBitAddress(),
			dataBlock.data[ registerAddress ],
			(registerAddress < dataBlock.data.end()) ? dataBlock.data[registerAddress + 1] : 0
			);

		registerValue = registerValue << (16 - bitWidth);

		int int16Value = 0;
		switch (dataType)
		{
		case TA_Base_Bus::DPT_INT16_TYPE:
			int16Value = m_numberConversionUtil.convertFromINT16_Value( registerValue);
			break;
		case TA_Base_Bus::DPT_UINT16_TYPE:
			int16Value = m_numberConversionUtil.convertFromUINT16_Value( registerValue);
			break;
		case TA_Base_Bus::DPT_INT16S_TYPE:
			int16Value = m_numberConversionUtil.convertFromINT16S_Value( registerValue);
			break;

		default:
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unable to process datapoint (%s) with unknown DataPointDataType (%d)",
				dp->getDataPointName().c_str(), dp->getDataPointDataType() );
			break;
		}

		int16Value = int16Value >> (16 - bitWidth);
		registerValue = registerValue >> (16 - bitWidth); // for log output.
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"conversionWordNumber: bitwidth[%d], register word[%X], in word[%X], result[%d]",
			bitWidth, dataBlock.data[registerAddress], registerValue, int16Value);

		return int16Value;
	}

	double ModbusDpProcessor::conversion2WordNumber( TA_Base_Bus::DataPoint* dp, const ModbusDataBlock& dataBlock, TA_Base_Bus::EDataPointDataType dataType )
	{
		long bitWidth = dp->getBitWidth();
		if (32 < bitWidth || bitWidth <= 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "bit width: %d, is not in the range.", bitWidth);
			return 0;	// not process.
		}

		unsigned long registerAddress = dp->getDataPointRegisterAddress();
		unsigned int bitAddress = dp->getDataPointBitAddress();	// 0 - 15;
		if (registerAddress > dataBlock.data.end() ||
			registerAddress < dataBlock.data.start() ||
			bitAddress < 0 || bitAddress > 15)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"bitAddress[%d], registerAddress[%d], valueis not in the range.", bitAddress, registerAddress);
			return 0;
		}
		// need low word in low register address.
		WORD registerValue = dataBlock.data[registerAddress];
		registerAddress = registerAddress + 1;
		WORD registerValue2 = (registerAddress > dataBlock.data.end())? 0 : dataBlock.data[registerAddress];
		registerAddress = registerAddress + 1;
		WORD registerValue3 = (registerAddress > dataBlock.data.end())? 0 : dataBlock.data[registerAddress];

		WORD lowWord = registerValue >> bitAddress;
		lowWord = lowWord | (registerValue2 << (16 - bitAddress));
		WORD highWord = registerValue2 >> bitAddress;
		highWord = highWord | (registerValue3 << (16 - bitAddress));

		unsigned long tempLong = highWord;
		tempLong = (tempLong << 16) | lowWord;
		tempLong = tempLong << (32 - bitWidth);

		lowWord = (WORD)((tempLong << 16) >> 16);
		highWord = (WORD)(tempLong >> 16);

		double resValue = 0.0;
		long longValue = 0;
		unsigned long uLongValue = 0;
		switch (dataType)
		{
		case TA_Base_Bus::DPT_INT32_TYPE:
			longValue = (long)m_numberConversionUtil.convertFromINT32_Value( highWord, lowWord);
			resValue = longValue >> (32 - bitWidth);
			break;
		case TA_Base_Bus::DPT_UINT32_TYPE:
			uLongValue = (unsigned long)m_numberConversionUtil.convertFromUINT32_Value( highWord, lowWord);
			resValue = uLongValue >> (32 - bitWidth);
			break;
		case TA_Base_Bus::DPT_INT32S_TYPE:
			longValue = (long)m_numberConversionUtil.convertFromINT32S_Value( highWord, lowWord);
			resValue = longValue >> (32 - bitWidth);
			break;
		case TA_Base_Bus::DPT_IEEE32_TYPE:
			longValue = (long)m_numberConversionUtil.convertFromIEEE32_Value( highWord, lowWord);
			resValue = longValue >> (32 - bitWidth);
			break;

		default:
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unable to process datapoint (%s) with unknown DataPointDataType (%d)",
				dp->getDataPointName().c_str(), dp->getDataPointDataType() );
			break;
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"conversion2WordNumber: bitwidth[%d], register word[%X], lowWord[%X], result[%lf]",
			bitWidth, dataBlock.data[registerAddress], lowWord, resValue);

		return resValue;
	}

	void ModbusDpProcessor::sortAndSaveDataPointToList( TA_Base_Bus::DataPoint * dp )
	{
        TA_ASSERT( NULL != dp, "dp is NULL in ModbusDpProcessor::sortAndSaveDataPointToList" );

        // stop multi threads accessing the list of DataPoint objects at the same time
        TA_Base_Core::ThreadGuard guard( m_datapointsLock );

		// if the list is empty
		if ( true == m_datapoints.empty() )
		{
			m_datapoints.push_back( dp );
            return;
		}

        //
        // check for low priority first
        //
        
        TA_Base_Bus::DataPoint * lastDp = m_datapoints.back();
        
        TA_ASSERT( NULL != lastDp, "lastDp is NULL in ModbusDpProcessor::sortAndSaveDataPointToList" );

        // if the priority level of the specified data point is less/equal the 
        // priority level of the last data point in the list
        if ( dp->getUpdatePriorityLevel() <= lastDp->getUpdatePriorityLevel() )
        {
            // insert the data point to the end of the list
            m_datapoints.push_back( dp );
            return;
        }

        //
        // if got here, the data point must have a high update priority, insert the
        // data point to the begining of the list
        //
        std::vector< TA_Base_Bus::DataPoint * >::iterator dpItr = m_datapoints.begin();

        // if found the first iterator
        if ( m_datapoints.end() != dpItr )
        {
            // insert the specified data point to the list, just before the data point
            // pointed to by the current iterator
            m_datapoints.insert( dpItr, dp );
            return;
        }
        
        // if none of the above 2 conditions have been met,
        // insert the data point to the end of the list anyway
        m_datapoints.push_back( dp );
    }
}
