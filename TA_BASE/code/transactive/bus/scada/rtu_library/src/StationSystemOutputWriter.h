/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/StationSystemOutputWriter.h $
  * @author:	Raymond Pau
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

#ifndef STATIONSYSTEMOUTPUTWRITER_H
#define STATIONSYSTEMOUTPUTWRITER_H

#include <map>

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/rtu_library/src/NumberConversionUtil.h"

#include "core/sockets/src/SocketOptions.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"
#include "bus/scada/rtu_library/src/CommandQueue.h"

namespace TA_Base_Bus
{
    // forward declaration
    class DataPointWriteRequest;
	class DataPoint;
    class RTU;
    typedef std::map < unsigned long, const TA_Base_Bus::DataPoint * > RTUOutputDataPointMap;

	enum EDIGIT_WITE_CODE
	{
		E_DIGIT_0F = 0,
		E_DIGIT_10,
	};

	class StationSystemOutputWriter
	{
    public:

		//Construct for outputwriter with the different port from the polling port.
		StationSystemOutputWriter ( RTU& rtu,
                                    const std::string & stationSystemName,
                                    const std::string & rtuServicePortNumber,
                                    unsigned long startOutputAddress,
                                    unsigned long endOutputAddress,
                                    int slaveID,
									const std::string& digitalFunCode = "0F",
									unsigned long offsetAddress = 0);


		virtual ~StationSystemOutputWriter();

		/**
		  * addDataPoint
		  *
		  * Add the specified DataPoint object to the list of data points to be
		  * monitored by the worker thread
		  *
		  * @param theDataPoint	the DataPoint object that is monitored by the worker thread
		  *
		  */
		virtual bool addDataPoint( const TA_Base_Bus::DataPoint * dp );

		virtual void removeDataPoint( const TA_Base_Bus::DataPoint * dp );

		/**
		  * processWriteRequest
		  *
		  * Process the specified DataPoint write request
		  *
		  */
		virtual bool processWriteRequest( TA_Base_Bus::DataPointWriteRequest & newWrite );
		
        virtual int getSlaveID() const { return m_slaveID; };

    protected:

		/**
		  * writeDigitalDataPoint
		  *
		  * Send the specified write request to a digital DataPoint
		  *
		  */
		void writeDigitalDataPoint ( TA_Base_Bus::DataPointWriteRequest & newWrite );

		void writeDigitalDataPointWith0F(TA_Base_Bus::DataPointWriteRequest & newWrite);

		void writeDigitalDataPointWith10(TA_Base_Bus::DataPointWriteRequest & newWrite);
		/**
		  * writeAnalogueDataPoint
		  *
		  * Send the specified write request to an analogue DataPoint
		  *
		  */
		void writeAnalogueDataPoint ( TA_Base_Bus::DataPointWriteRequest & newWrite );
        bool isDpStationSystemNameMatched( const std::string & dpName );
		void convertDigitalCodeToEType(const std::string& funCode);
	private:
        // disabled constructor
		StationSystemOutputWriter();
		StationSystemOutputWriter ( const StationSystemOutputWriter & obj );
		StationSystemOutputWriter & operator= ( const StationSystemOutputWriter & rhs );

    // attributes
	protected:
        RTU & 								m_rtu;
		std::string							m_stationSystemName;
        std::string                         m_rtuServicePortNumber;
        TA_Base_Bus::DataBlock < WORD >     m_addressBlock;		
        int                                 m_slaveID;
		
        RTUOutputDataPointMap                   m_dataPoints;
        TA_Base_Core::WriteReadThreadLockable   m_modbusLock;
		TA_Base_Bus::NumberConversionUtil       m_numberConversionUtil;

		unsigned long						m_offsetAddress;
        EDIGIT_WITE_CODE					m_digitWriteCode;
    };
}
#endif
