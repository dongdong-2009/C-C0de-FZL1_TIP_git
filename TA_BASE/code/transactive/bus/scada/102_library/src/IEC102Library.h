#ifndef IEC102LIBRARY_H
#define IEC102LIBRARY_H

//#include "I102EventHandler.h"
#include "bus/scada/102_library/src/ChannelManager.h"
#include "bus/scada/102_library/src/IEC102FlowController.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/scada/102_library/src/IEC102ObserverInterface.h"
#include "boost/shared_ptr.hpp"
#include "bus/scada/102_library/src/IEC102ConfigData.h"

#include <string>

namespace TA_Base_Bus
{
	class ChannelManager;
	//class ConfigData;
	
	class IEC102ControllerFactory;
	
	class IEC102Library: public TA_Base_Bus::IIEC102Channel, public TA_Base_Core::Thread
	{
	friend class ChannelManager;
		
	public:
		IEC102Library();
		virtual ~IEC102Library();

		void startProcess();
		void stopProcess();

		/**
		 * getPowerEnergy_C_CI_NR_2(120)
		 *
		 * get history power energy with predefined address range
		 *
		 * @return true if operation success
		 *
		 * @param startAddr start address power energy
		 * @param endAddr end address of power energy
		 * @param startTime start time
		 * @param endTime end time
		 *
		 */
		FrameEnergyData getPowerEnergy_C_CI_NR_2(unsigned short startAddr, unsigned short endAddr, time_t startTime, time_t endTime, unsigned char rad = 0x00);

		/**
		 * getRemoteMeasurement_C_YC_NA_2(171)
		 *
		 * get current remote measurement with predefined address range
		 *
		 * @return true if operation success
		 *
		 * @param startAddr start address of remote measurement
		 * @param endAddr end address of remote measurement
		 *
		 */
		FrameEnergyData getRemoteMeasurement_C_YC_NA_2(unsigned short startAddr, unsigned short endAddr);

		/**
		 * getRemoteMeasurement_C_YC_TA_2(172)
		 *
		 * get history remote measurement with predefined address range
		 *
		 * @return true if operation success
		 *
		 * @param startAddr start address of remote measurement
		 * @param endAddr end address of remote measurement
		 * @param startTime start time
		 * @param endTime end time
		 *
		 */
		FrameEnergyData getRemoteMeasurement_C_YC_TA_2(unsigned short startAddr, unsigned short endAddr, time_t startTime, time_t endTime, unsigned char rad  = 0x00);

		void callLinkStatus();
		
		void setupConfiguration(const IEC102ConfigData& obj);
	
		//Interface functions for Agent observer
		virtual void registerIEC102Observer(IEC102Observer * observer);
		virtual void removeIEC102Observer(const IEC102Observer *observer);
		ChannelManager*	getChannelManager();
		void addEnergyData(const std::vector<PowerEnergyData>& energyData);
	protected:		
		void processStatusForObserver(const bool & status);
		void	initControllerFactory();

	private:
		virtual void terminate();
		virtual void run();		
		/**
         * operator= - Hidden as not used
         */
        IEC102Library& operator=(const IEC102Library& rhs);

        /**
         * Copy constructor - Hidden as not used
         */
        IEC102Library(const IEC102Library&);

		void	UpdateChannelStatus(const bool & status);	
		
		void startFlow();
		
		bool initLink();
		bool getChannelStatus();
		void handleStopProcess();
		void handleConnectionEstablished();
		
		// delete the controller after using.
		void destroyController();

		// Notify controller for connection lost to stop current processFlow
		void handleConnectionLost();

		// Notify controller for writing failed.
		void handleWriteFailed();

		// Pass incomingBuffer to controller to process the data
		void processData(std::vector<unsigned char>& buffer);

		void clearEnergyData();

		TA_Base_Core::ReEntrantThreadLockable m_controllerLock;
		TA_Base_Core::ReEntrantThreadLockable m_usingControllerLock;
		
		IEC102FlowController*		m_iec102Controller;
		ChannelManager*				m_channelManager;
		
		TA_Base_Core::Semaphore 	m_initSemphore;
		volatile bool 				m_linkInitSuccess, m_bChannelStatus, m_running;
		//For observers
		IEC102Observer* 			m_observer; 
		IEC102ConfigData 			m_objConfigData;

		//For ControllerFactory object
		TA_Base_Bus::IEC102ControllerFactory* m_controllerFactory;
		FrameEnergyData 			m_energyData;
	};
}

#endif

