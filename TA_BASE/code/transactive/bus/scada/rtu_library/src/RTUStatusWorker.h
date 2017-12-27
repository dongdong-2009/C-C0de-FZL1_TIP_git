/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUStatusWorker.h $
  * @author:	HoaVu
  * @version:	$Revision: #6 $
  *
  * Last modification:	$DateTime: 2017/05/25 10:19:08 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The RTUStatusWorker object is responsible for processing PMS RTU status data
  *
  */

#ifndef RTUSTATUSWORKER_H
#define RTUSTATUSWORKER_H

#include <vector>
#include <string>

#include "bus/modbus_comms/src/IModbus.h"

namespace TA_Base_Bus
{
	class RTUStatusWorker
	{
  	// operations
  	public:
    		RTUStatusWorker();
    		virtual ~RTUStatusWorker();

    		//
    		virtual void processRTUDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, const bool isCommsOK );
        virtual bool getIsCommsOK() const;
        virtual bool getIsRTUSwitching() const;
        virtual bool getIsPrimaryRTU() const;
          
  	private:
          // disabled constructors
      		RTUStatusWorker( const RTUStatusWorker & obj );
      		RTUStatusWorker & operator= ( const RTUStatusWorker & rhs );

  	// attributes
  	protected:
          bool m_isCommsOK;
  	};
}

#endif
