/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/CommonDpProcessor.h $
  * @author:	Liu Yong
  * @version:	$Revision: #8 $
  *
  * Last modification:	$DateTime: 2016/04/08 14:49:53 $
  * Last modified by:	$Author: Liu Yong $
  *
  * Description:
  *
  *	The CommonDpProcessor object is responsible for processing input data without special handling.
  *
  */

#ifndef COMMON_DP_PROCESSOR_H
#define COMMON_DP_PROCESSOR_H

#include <sys/timeb.h>
#include <vector>
#include <queue>
#include <map>

#include "bus/scada/rtu_library/src/ModbusDpProcessor.h"

namespace TA_Base_Bus
{
	class CommonDpProcessor : public ModbusDpProcessor
	{
	public:

        CommonDpProcessor( const std::string & trainName,
                        unsigned long pollingStartAddress,
                        unsigned long pollingEndAddress,
                        unsigned long offsetAddress,
                        bool 		  reverseDiBitOrder,
						int analogueDpUpdateInterval);  //TD16169

		virtual ~CommonDpProcessor();

    virtual void processDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, 
                                  TA_Base_Bus::EQualityStatus newStatus );

    virtual int processSpecialDataPoint() {return 0;};
    virtual int populateSpecialDataPoints() {return 0;};
    virtual bool isDpMatched( TA_Base_Bus::DataPoint * dp );   

		virtual timeb getTimeStamp(unsigned long registerAddress) {return m_timeStamp;};  //TD16182



    private:
        // Disable Copy constructor and Assignment operator
		CommonDpProcessor( const CommonDpProcessor & obj );
		CommonDpProcessor & operator= ( const CommonDpProcessor & rhs );

		timeb		m_timeStamp;
	};
}

#endif
