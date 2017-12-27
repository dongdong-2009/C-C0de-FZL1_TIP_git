/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/SteRTUDpProcessor.h $
  * @author:	Liu Yong
  * @version:	$Revision: #8 $
  *
  * Last modification:	$DateTime: 2016/04/08 14:49:53 $
  * Last modified by:	$Author: Liu Yong $
  *
  * Description:
  *
  *	The SteRTUDpProcessor object is responsible for processing input data polled from the PMS RTU
  *
  */

#ifndef STE_RTU_DP_PROCESSOR_H
#define STE_RTU_DP_PROCESSOR_H

#include <sys/timeb.h>
#include <vector>
#include <queue>
#include <map>

#include "bus/scada/rtu_library/src/ModbusDpProcessor.h"

namespace TA_Base_Bus
{
	class SteRTUDpProcessor : public ModbusDpProcessor
	{
	public:

        SteRTUDpProcessor( const std::string & stationName,
                        const std::string & stationSystemName,
                        unsigned long pollingStartAddress,
                        unsigned long pollingEndAddress,
                        unsigned long offsetAddress,
						const std::vector<std::pair<unsigned long, unsigned long> >& blockAddresses,
                        int slaveID,
                        int statusWordNumber,
						int analogueDpUpdateInterval);  //TD16169

		virtual ~SteRTUDpProcessor();

		/**
		  * addDataPoint
		  *
		  * Add the specified TA_Base_Bus::DataPoint object to the list of data point
		  * monitored by the ModbusDpProcessor worker thread.
		  *
		  * @param theDataPoint	the TA_Base_Bus::DataPoint object that is monitored
		  *						by the ModbusDpProcessor worker thread
		  */
		virtual bool addDataPoint( TA_Base_Bus::DataPoint * dp );
		
		/**
		  * getSlaveID
		  *
		  * Get the ID of the Station System ID associate with this modbus table
		  *
		  */
        virtual int getSlaveID() const { return m_slaveID; }

		/**
		* getStatusWordNumber
		*
		* Get the status word number of the Station System , number associate with this modbus table
		*
		*/
		virtual int getStatusWordNumber() const { return m_statusWordNumber; }

		/**
		  * processDataBlock
		  *
		  * Update the address block associated with the SteRTUDpProcessor with the specified
		  * block
		  *
		  * @param addressBlock	The new address block that is assigned to the SteRTUDpProcessor
		  *						worker thread
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
        virtual void processDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, TA_Base_Bus::EQualityStatus newStatus);

	protected:

		/**
		  * processTextDataPoint
		  *
		  * Process and update the value of the Analogue typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processTextDataPoint( TA_Base_Bus::DataPoint * dp );

		virtual timeb getTimeStamp(unsigned long registerAddress);  //TD16182

        virtual bool isDpMatched( TA_Base_Bus::DataPoint * dp );
        
    private:
        // Disable Copy constructor and Assignment operator
		SteRTUDpProcessor( const SteRTUDpProcessor & obj );
		SteRTUDpProcessor & operator= ( const SteRTUDpProcessor & rhs );

 		timeb extractTimeStamp( unsigned long startTimeStampAddress, const TA_Base_Bus::DataBlock< WORD > & dataBlock );
        void checkBlockAddressRange(); //TD16182
		
		std::string		m_stationName;
		std::string 	m_stationSystemName;
		int 			m_slaveID;
		int 			m_statusWordNumber;

		std::vector<std::pair<unsigned long, unsigned long> > 	m_blockAddresses; 
		std::map<unsigned long, unsigned long> 					m_datapointsAddressBlock;
		
	};
}

#endif
