/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUStatusWorker.cpp $
  * @author:	Liu Yong
  * @version:	$Revision: #5 $
  *
  * Last modification:	$DateTime: 2017/05/25 10:19:08 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The RTUStatusWorker object is responsible for processing PMS RTU status data
  *
  */

#if defined( WIN32 )
	#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "bus/scada/rtu_library/src/RTUStatusWorker.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    RTUStatusWorker::RTUStatusWorker()
    :
    m_isCommsOK(false)
    {
    }


    RTUStatusWorker::~RTUStatusWorker()
    {

    }


    void RTUStatusWorker::processRTUDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, 
                                               const bool isCommsOK )
    {
        m_isCommsOK = isCommsOK;
    }

    bool RTUStatusWorker::getIsCommsOK() const
    {
      return m_isCommsOK ;
    }

    bool RTUStatusWorker::getIsRTUSwitching() const
    {
      return false;
    }

    bool RTUStatusWorker::getIsPrimaryRTU() const
    {
      return m_isCommsOK;
    }    
}
