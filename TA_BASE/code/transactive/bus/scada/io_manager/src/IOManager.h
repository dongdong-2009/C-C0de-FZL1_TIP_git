/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/IOManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_IO_MANAGER_H_
#define SCADA_IO_MANAGER_H_

#include "IIOManager.h"
#include <map>
#include <vector>

#include "DataPointQueueProcessor.h"
#include "OutputCommandSender.h"

namespace TA_Base_Bus
{

	/** implement the IIOManager interface.
     * 
	 * __NOTE__: the defualt return value of function getEntityGroup is 0.
	 * if the entity data have group concept, should return the real group value.
     */
	class IOManager : public IIOManager
	{
	public:
		IOManager();
		virtual ~IOManager();
		virtual IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

		/** get entity group from entityData
         *
         * @param entityData: the entity data.
         * @return unsigned long: group id, if no group id, value is 0.
         */
		virtual unsigned long getEntityGroup(TA_Base_Core::IEntityDataPtr entityData){return 0;};

		virtual void addDataPoint(DataPoint* dp);
		virtual void finishedAddDataPoint();

		virtual void setOperationMode(TA_Base_Core::EOperationMode mode);
		void updateMoreParas(std::map<std::string, std::string>& paras);

	protected:

		typedef std::vector<IProtocolManager*> ProtocolManagerList;

		std::map<unsigned long, ProtocolManagerList*> m_groupManagersMap;
		///< group id to ProtocolManagerList map.

		ProtocolManagerList	m_protocolManagers;

		std::map<unsigned long, IEntity*> m_entitys;
		///< <entity key, IEntity*>

		std::map<unsigned long, IProtocolIOBase*> m_dpToIOBaseMap;
		///< <datapoint pkey, IProtocolIOBase*>

		DataPointQueueProcessor*	m_dpProcessor;
		OutputCommandSender*		m_commandSender;

	};

};

#endif
