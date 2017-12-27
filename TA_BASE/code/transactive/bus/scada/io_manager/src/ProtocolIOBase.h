/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolIOBase.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_PROTOCOL_IO_BASE_H_
#define SCADA_PROTOCOL_IO_BASE_H_

#include "IDataPointProcessor.h"
#include "IProtocolIOBase.h"
#include "IProtocolManager.h"

namespace TA_Base_Bus
{
	/** Implement the interface of IProtocolIOBase.
     *  
     */
	class ProtocolIOBase : public IProtocolIOBase
	{
	public:

		/** io object constructor.
         *
         * @param name: object name.
         */
		ProtocolIOBase(std::string name);
		virtual ~ProtocolIOBase();

		/** determine if the DataPoint is belonging to this IO object.
         *
         * @param dp: DataPoint pointer.
         * @return bool: matched result.
         */
		virtual bool isDataPointMatched(DataPoint* dp) = 0;
		bool addDataPoint(DataPoint*);

		virtual void registerDPProcessor(IDataPointProcessor*);
		virtual void registerProtocolManager(IProtocolManager*);

		virtual bool processWriteRequest(DataPointWriteRequest&){return false;};

		virtual void submitDeviceData(IDeviceData*){};

		std::string	 getName(){return m_name;};

	protected:
		std::string				m_name;
		std::vector<DataPoint*> m_dataPoints;
		///< all the matched DataPoint s. 

		IDataPointProcessor*	m_dpProcessor;
		IProtocolManager*		m_protocolManager;
		TA_Base_Core::ReEntrantThreadLockable   m_lock;
	};
	
};

#endif
