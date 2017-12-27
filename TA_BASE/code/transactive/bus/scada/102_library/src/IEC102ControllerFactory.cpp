/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/102_library/src/IEC102ControllerFactory.cpp $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2015/01/21 14:56:06 $
  * Last modified by : $Author: dhanshri $
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "bus/scada/102_library/src/IEC102ControllerFactory.h"
#include "bus/scada/102_library/src/IEC102FlowController.h"


namespace TA_Base_Bus
{

    IEC102ControllerFactory*        IEC102ControllerFactory::m_ControllerSingleton = NULL;
    unsigned int                    IEC102ControllerFactory::m_refCounter = 0;
    TA_Base_Core::ReEntrantThreadLockable IEC102ControllerFactory::m_ControllerSingletonLock;

    IEC102ControllerFactory::IEC102ControllerFactory()
    {
		FUNCTION_ENTRY("IEC102ControllerFactory()");
		FUNCTION_EXIT;
    }


    IEC102ControllerFactory::~IEC102ControllerFactory()
    {
		FUNCTION_ENTRY("~IEC102ControllerFactory()");
		FUNCTION_EXIT;
    }
    
    /**
     * IEC102ControllerFactory::getInstance()
     *
     * Returns a pointer to the single instance
     * of SingeltonQueue, creating if necessary.
     *
     * @return  a pointer to the IEC102ControllerFactory
     *
     */
    IEC102ControllerFactory& IEC102ControllerFactory::getInstance()
    {
		FUNCTION_ENTRY("getInstance()");

        TA_THREADGUARD( m_ControllerSingletonLock );

        if ( m_ControllerSingleton == NULL )
        {
            m_ControllerSingleton = new IEC102ControllerFactory();
        }

        // increment reference counter
        m_refCounter++;
		FUNCTION_EXIT;
        return *m_ControllerSingleton;
    }   

    void IEC102ControllerFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance()");
		TA_THREADGUARD( m_ControllerSingletonLock );

        TA_ASSERT(m_refCounter > 0, "m_refCounter <= 0");

        // decrement reference counter
        --m_refCounter;

        // if not referenced anywhere
        if ( 0 == m_refCounter )
        {
            if ( m_ControllerSingleton != NULL )
            {
                delete m_ControllerSingleton;
                m_ControllerSingleton = NULL;
            }
        }
		FUNCTION_EXIT;
    }


    IEC102FlowController*  IEC102ControllerFactory::createIEC102Controller120(ChannelManager& channelManager,IEC102ConfigData& objConfigData,int startAddr, int endAddr, time_t startTime, time_t endTime, unsigned char rad)
	{
        FUNCTION_ENTRY("createIEC102Controller120()");
		FUNCTION_EXIT;
		return (new IEC102Controller120(channelManager,objConfigData,startAddr,  endAddr, startTime,  endTime, rad));
		
    }

    IEC102FlowController*  IEC102ControllerFactory::createIEC102Controller171(ChannelManager& channelManager,IEC102ConfigData& objConfigData,int startAddr, int endAddr) 
    {
		FUNCTION_ENTRY("createIEC102Controller171()");
		FUNCTION_EXIT;
		return (new IEC102Controller171(channelManager,objConfigData,startAddr, endAddr));
	}
	
    IEC102FlowController*  IEC102ControllerFactory::createIEC102Controller172(ChannelManager& channelManager,IEC102ConfigData& objConfigData,int startAddr, int endAddr, time_t startTime, time_t endTime, unsigned char rad) 
    {
		FUNCTION_ENTRY("createIEC102Controller172()");
		FUNCTION_EXIT;
		return (new IEC102Controller172(channelManager,objConfigData,startAddr, endAddr, startTime,  endTime, rad));
	}
    

	IEC102FlowController*  IEC102ControllerFactory::createIEC102ControllerInit(ChannelManager& channelManager,IEC102ConfigData& objConfigData,bool isInitLink)
	{
		FUNCTION_ENTRY("createIEC102ControllerInit()");
		FUNCTION_EXIT;
		return (new IEC102ControllerInit(channelManager,objConfigData,isInitLink));
    } 

};
