/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/CommonDpProcessor.cpp $
  * @author:	Liu Yong
  * @version:	$Revision: #12 $
  *
  * Last modification:	$DateTime: 2016/04/08 14:49:53 $
  * Last modified by:	$Author: Liu Yong $
  *
  * Description:
  *
  *	The CommonDpProcessor object is responsible for processing input data polled without special handling.
  *
  */

#include <set>
#include <math.h>
#include "time.h"
#include "ace/OS.h"

#include "bus/scada/rtu_library/src/CommonDpProcessor.h"
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
	CommonDpProcessor::CommonDpProcessor( const std::string & dpProcessorName,
                                        unsigned long pollingStartAddress,
                                        unsigned long pollingEndAddress,
                                        unsigned long offsetAddress,
                                        bool          reverseDiBitOrder,                  
                      									int           analogueDpUpdateInterval)  //TD16169
	:
  ModbusDpProcessor( dpProcessorName, pollingStartAddress, pollingEndAddress, offsetAddress, reverseDiBitOrder, analogueDpUpdateInterval)
	{
        // start thread
        start();
	}

	CommonDpProcessor::~CommonDpProcessor()
	{
      
  }

  //Normally, adressblock don't have time information. we need get it from outside dpProcessor.
  void CommonDpProcessor::processDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, TA_Base_Bus::EQualityStatus newStatus )
  {
      ModbusDataBlock* pDataBlock = new ModbusDataBlock();
      
      LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "addressBlock: start(%lu) end(%lu) offset(%lu)", 
      (unsigned long)addressBlock.start(), (unsigned long)addressBlock.end(), (unsigned long)m_offsetAddress);
      

      pDataBlock->quality = newStatus;

      timeb timestamp;
      ftime( &timestamp );
      
      pDataBlock->timestamp = timestamp;
      // to process data block with station system offset address
      // so can process rtu status block as the same as other station systems
      int length = ( m_lastDpAddress - m_startAddress + 1 );
      pDataBlock->data.setStartAndLength( m_startAddress , length );
      
      // copy the data from phicial table to logical table
      for ( unsigned long loc = m_startAddress; loc <= m_lastDpAddress; ++ loc )
      {
          pDataBlock->data.set( loc, addressBlock[loc - m_offsetAddress] );
      }
      
      insertQueue(pDataBlock);
  }  

  bool CommonDpProcessor::isDpMatched( TA_Base_Bus::DataPoint * dp ) 
  {
      unsigned long registerAddress = dp->getDataPointRegisterAddress();

      if (m_lastDpAddress >= registerAddress && m_startAddress <= registerAddress)
	  {
      		return true; 
	  }
	  
	  return false;
  }   
}
