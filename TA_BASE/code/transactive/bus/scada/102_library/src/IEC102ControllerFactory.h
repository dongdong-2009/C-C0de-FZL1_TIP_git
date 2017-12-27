/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/102_library/src/IEC102ControllerFactory.h $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2015/01/21 14:56:06 $
  * Last modified by : $Author: dhanshri $
  *
  */

#ifndef IEC102CONTROLLERFACTORY_H
#define IEC102CONTROLLERFACTORY_H

#include <string>
#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/102_library/src/IEC102FlowController.h"
#include "bus/scada/102_library/src/IEC102Controller120.h"
#include "bus/scada/102_library/src/IEC102Controller171.h"
#include "bus/scada/102_library/src/IEC102Controller172.h"
#include "bus/scada/102_library/src/IEC102ControllerInit.h"


namespace TA_Base_Bus
{
    // Forward declarations
	class ChannelManager;
	class IEC102FlowController;

	class IEC102ControllerFactory
    {

    public:
        /**
         * getInstance()
         *
         * Returns a pointer to the single instance
         * of IEC102ControllerFactory, creating if necessary.
         *
         * @return  a pointer to a IEC102ControllerFactory
         *
         */
         static IEC102ControllerFactory& getInstance();


        /**
         * removeInstance()
         *
         * Terminates singleton instance of factory, releasing all resources associated
         * with it.
         *
         */
         static void removeInstance();
		 /**
         *
         * createIEC102Controller120
         *
         * create a new monitored data point proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
		 IEC102FlowController* createIEC102Controller120(ChannelManager& channelManager,IEC102ConfigData& objConfigData,int startAddr, int endAddr, time_t startTime, time_t endTime, unsigned char rad);

        /**
         *
         * IEC102Controller171
         *
         * create a new monitored data point proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
		IEC102FlowController*  createIEC102Controller171(ChannelManager& channelManager,IEC102ConfigData& objConfigData,int startAddr, int endAddr); 

        /**
         *
         * IEC102Controller172
         *
         * create a new monitored data point proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
		IEC102FlowController*  createIEC102Controller172(ChannelManager& channelManager,IEC102ConfigData& objConfigData,int startAddr, int endAddr, time_t startTime, time_t endTime, unsigned char rad); 
		
        /**
         *
         * IEC102ControllerInit
         *
         * create a new monitored data node proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
        IEC102FlowController*  createIEC102ControllerInit(ChannelManager& channelManager,IEC102ConfigData& objConfigData,bool isInitLink = true);	
    
    protected:

        IEC102ControllerFactory();
        virtual ~IEC102ControllerFactory();

        // Singleton
        static IEC102ControllerFactory *                m_ControllerSingleton;
        static unsigned int                             m_refCounter;
        static TA_Base_Core::ReEntrantThreadLockable    m_ControllerSingletonLock;
        
    };
}
#endif
