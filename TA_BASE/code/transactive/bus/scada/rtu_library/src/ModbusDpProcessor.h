/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/ModbusDpProcessor.h $
  * @author:	Liu Yong
  * @version:	$Revision: #8 $
  *
  * Last modification:	$DateTime: 2017/03/08 14:49:53 $
  * Last modified by:	$Author: Liu Yong $
  *
  * Description:
  *
  *	The ModbusDpProcessor object is responsible for processing input data polled with modbus block format.
  *
  */

#ifndef MODBUS_DP_PROCESSOR_H
#define MODBUS_DP_PROCESSOR_H

#include <sys/timeb.h>
#include <vector>
#include <queue>
#include <map>

#include "bus/scada/common_library/src/CommonDefs.h"

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/modbus_comms/src/IModbus.h"
#include "bus/scada/rtu_library/src/NumberConversionUtil.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"



namespace TA_Base_Bus
{
	class DataPoint;
	class NumberConversionUtil;

    const int DEFAULT_ANALOGUE_DP_UPDATE_INTERVAL = 10; //TD16169
	
	class ModbusDataBlock
	{
	public:
		ModbusDataBlock();
		~ModbusDataBlock(){};
		
		TA_Base_Bus::DataBlock < WORD >	data;
        TA_Base_Bus::EQualityStatus 	quality;
		timeb 							timestamp;
		std::map<unsigned long, timeb> 	blockTimestamp; 
	private:
        // Disable Copy constructor and Assignment operator
		ModbusDataBlock( const ModbusDataBlock & obj );
		ModbusDataBlock & operator= ( const ModbusDataBlock & rhs );
		
	};

	class ModbusDpProcessor : public TA_Base_Core::Thread
	{
	public:

        ModbusDpProcessor( const std::string & dpProcessorName,
							unsigned long pollingStartAddress,
							unsigned long pollingEndAddress,
							unsigned long offsetAddress,
                            bool 		  reverseDiBitOrder,
							int 		  analogueDpUpdateInterval);  //TD16169

		virtual ~ModbusDpProcessor();

		// 
		// operations required by the TA_Core::TA_Thread interface
		//
		virtual void run();
		virtual void terminate();

		//
		// operations specific to ModbusDpProcessor
		//

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
		  * removeDataPoint
		  *
		  * Remove from the internal list the data point whose primary key matches
		  * with the specified primary key.  Returns the reference to the data point
		  * that has been removed.  Null pointer is returned if there is no
		  * matching
		  *
		  * @param	dataPointKey	The specified primary key of the data point
		  *							to be removed from the table
		  *
		  */
		virtual void removeDataPoint( TA_Base_Bus::DataPoint * dp );


		/**
		  * processRTUDataBlock
		  *
		  * Update the address block associated with the ModbusDpProcessor with the specified
		  * block
		  *
		  * @param addressBlock	The new address block that is assigned to the ModbusDpProcessor
		  *						worker thread
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
        virtual void processDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, TA_Base_Bus::EQualityStatus newStatus) = 0;
		
		void updateAnalogueDpUpdateInterval(int analogueDpUpdateInterval); //TD16169
		
	protected:

		/**
		  * processIEEE32DataPoint
		  *
		  * Process and update the value of the IEEE 32bit floating typed TA_Bus::TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Bus::TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Bus::TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processIEEE32DataPoint( TA_Base_Bus::DataPoint * dp);

		/**
		  * processINT32DataPoint
		  *
		  * Process and update the value of the 32bit integer typed TA_Bus::TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processINT32DataPoint( TA_Base_Bus::DataPoint * dp);

		/**
		  * processINT32SDataPoint
		  *
		  * Process and update the value of the 32bit signed integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processINT32SDataPoint( TA_Base_Bus::DataPoint * dp);

		/**
		  * processINT16DataPoint
		  *
		  * Process and update the value of the 16bit integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processINT16DataPoint( TA_Base_Bus::DataPoint * dp);

		/**
		  * processEnumDataPoint
		  *
		  * Process and update the value of the Enum (16bit integer typed) TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
        virtual void processEnumDataPoint( TA_Base_Bus::DataPoint * dp);

		/**
		  * processINT16SDataPoint
		  *
		  * Process and update the value of the 16bit signed integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processINT16SDataPoint( TA_Base_Bus::DataPoint * dp);

		/**
		  * processUINT16DataPoint
		  *
		  * Process and update the value of the 16bit unsigned integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processUINT16DataPoint( TA_Base_Bus::DataPoint * dp);

		/**
		  * processUINT32DataPoint
		  *
		  * Process and update the value of the 32bit unsigned integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processUINT32DataPoint( TA_Base_Bus::DataPoint * dp);

		/**
		  * processDigitalDataPoint
		  *
		  * Process and update the value of the Binary/Digital typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processDigitalDataPoint( TA_Base_Bus::DataPoint * dp);

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
		virtual void processTextDataPoint( TA_Base_Bus::DataPoint * dp);

		/**
		  * getBooleanValue
		  *
		  * Return the boolean value of the register value based on the specified bit mask
		  *
		  * @param bitMask			The specified bit mask to ANDGate
		  * @param registerValue	The register value
		  */
		bool getBooleanValue( int bitMask, WORD registerValue );

		void sortAndSaveDataPointToList( TA_Base_Bus::DataPoint * dp );

        // ++ TD15293 : AZ, optimize the updating of DPs
		int populateDataPoints();

        int updateDataPoints();
			
        void addDataPointMap(TA_Base_Bus::DataPoint * dp);
		
		void insertQueue(ModbusDataBlock* modbusDataBlock);

		/**
		* getTheMixedWord
		*
		* dependent on the start address, get the word from low word and highword.
		*
		* @param startBitAddress	is the start bit address, from 0 to 15;
		* @param lowWord	The low  order number of the return word
		* @param highWord	The high order number of the return word
		*
		*/
		WORD getTheMixedWord(unsigned long startBitAddress, WORD lowWord, WORD highWord);

		/**
		* conversionWordNumber
		*
		* .
		*
		* @param startBitAddress	is the start bit address, from 0 to 15;
		* @param lowWord	The low  order number of the return word
		* @param highWord	The high order number of the return word
		*
		*/
		int conversionWordNumber(TA_Base_Bus::DataPoint* dp, const ModbusDataBlock& dataBlock, TA_Base_Bus::EDataPointDataType dataType);

		/**
		* conversion2WordNumber
		*
		* dependent on the start address, get the word from low word and highword.
		*
		* @param startBitAddress	is the start bit address, from 0 to 15;
		* @param lowWord	The low  order number of the return word
		* @param highWord	The high order number of the return word
		*
		*/
		double conversion2WordNumber(TA_Base_Bus::DataPoint* dp, const ModbusDataBlock& dataBlock, TA_Base_Bus::EDataPointDataType dataType);

 		/**
		  * isDpMatched
		  *
		  * Check whether the input data point is match for this ModbusDpProcessor.
		  * 
		  *
		  * @param theDataPoint	the TA_Base_Bus::DataPoint object that is monitored
		  *						by the ModbusDpProcessor worker thread
		  */
		virtual bool isDpMatched( TA_Base_Bus::DataPoint * dp ) { return true; };		

		virtual timeb getTimeStamp(unsigned long registerAddress) = 0;  //TD16182

        virtual int processSpecialDataPoint()	{return 0;};
		virtual int populateSpecialDataPoints() {return 0;};

        void processAddressDataPoint( TA_Base_Bus::DataPoint * dp);

        bool use2Registers(TA_Base_Bus::DataPoint * dp);

    private:
	
        // Disable Copy constructor and Assignment operator
		ModbusDpProcessor( const ModbusDpProcessor & obj );
		ModbusDpProcessor & operator= ( const ModbusDpProcessor & rhs );

        // TD15293 ++

    protected:
		std::string							m_dpProcessorName;
		unsigned long 						m_startAddress;
		unsigned long 						m_lastDpAddress;
        unsigned long 						m_offsetAddress;
        volatile bool 						m_threadTerminated;
		bool								m_reverseDiBitOrder;
        std::queue< ModbusDataBlock* > 		m_dataBlockQueue;

		std::vector< TA_Base_Bus::DataPoint* >	m_datapoints;
		TA_Base_Bus::NumberConversionUtil 		m_numberConversionUtil;
		TA_Base_Core::ReEntrantThreadLockable   m_datapointsLock;
		TA_Base_Core::ReEntrantThreadLockable	m_dataBlockLock;
		TA_Base_Core::Semaphore 				m_queueSemaphore;

		int 								m_analogueDpUpdateInterval; // TD16169
		unsigned long 						m_analogueDpUpdateCounter;  //TD16169

        // ++ TD15293 : AZ, optimize the updating of DPs
        // map the datapoints to the data block range
        typedef std::map< unsigned long, std::vector< TA_Base_Bus::DataPoint * > >::iterator BlockDpIt;
		
        std::map< unsigned long, std::vector< TA_Base_Bus::DataPoint * > > m_blockDPs;
		std::map< unsigned long, std::vector< TA_Base_Bus::DataPoint * > > m_blockAnalogueDPs;
        std::map< unsigned long, std::vector< TA_Base_Bus::DataPoint * > > m_blockTextDPs;

        ModbusDataBlock* m_dataBlock;
		ModbusDataBlock* m_prevDataBlock;
	};
}

#endif
