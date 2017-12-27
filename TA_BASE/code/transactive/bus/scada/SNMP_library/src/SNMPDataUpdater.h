/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPDataUpdater.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__SNMP_DATA_UPDATER_H_
#define SNMP__SNMP_DATA_UPDATER_H_

#include <sys/timeb.h>
#include <vector>
#include <queue>
#include <map>

#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

#include "bus/scada/io_manager/src/ProtocolUpdater.h"

#include "SNMPData.h"

namespace TA_Base_Bus
{
	class DataPoint;

	class DataPointList
	{
	public:
		std::vector<DataPoint*> dataPoints;
		EQualityStatus			quality;
	};

	/** this class update IDeviceData submitted by ProtocolDevice to DataPoint.
	 * derived from ProtocolUpdater.
     *  
     */
	class SNMPDataUpdater : public ProtocolUpdater
	{
	public:

		/** constructor.
         *
         * @param name: object name.
         */
		SNMPDataUpdater(std::string name);
		virtual ~SNMPDataUpdater();
		
		virtual bool isDataPointMatched(DataPoint* dp );
		void finishedAddDataPoint();

		void submitDeviceData(IDeviceData*);
				
	
    private:
		        
        // Disable Copy constructor and Assignment operator
		SNMPDataUpdater( const SNMPDataUpdater & obj );
		SNMPDataUpdater & operator= ( const SNMPDataUpdater & rhs );

        
		void updateAllDataPointToBad();
		void updateDataPointsToBad(DataPointList*);

		void processVbData(const /*Snmp_pp::*/Vb& vb, bool isGoodQuality);

    protected:
		EQualityStatus	m_preBestQuality;
		///< the best good quality.

		std::map<std::string, DataPointList*> m_oidToDataPointsMap;
		///< oid to DataPointList map, one oid may have multy DataPoint.

		TA_Base_Core::ReEntrantThreadLockable   m_lock;
	};
}

#endif
