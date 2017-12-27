/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/pa/pa_agent_proxy/src/PaAgentProxy.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/23 16:48:56 $
  * Last modified by:  $Author: qi.huang $
  *
  * This is used to get access to the PA agent interface and keep a reference
  * to it.
  */

#if !defined(PA_AGENT_PROXY_H)
#define PA_AGENT_PROXY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_Bus
{
    class PaAgentProxy
    {
    public:
        /**
         * PaAgentProxy
         *
         * Standard constructor.
         */
        PaAgentProxy();

        /**
         * ~PaAgentProxy
         *
         * Standard destructor.
         */
        virtual ~PaAgentProxy();

        // Throw exception: TA_Base_Bus::IPAAgentCorbaDef::PaErrorException, TA_Base_Core::OperationModeException
        static void broadcastSigTriggeredStationDva( unsigned long ulAgentLocationKey, unsigned long ulEventType, unsigned long ulSrcStationLocationKey,
            unsigned long ulDestStationLocationKey, unsigned long ulDestPlatformId, bool bIsLastTrain, bool bTrainInfoValid, time_t tmStartTime );

        // Throw exception: TA_Base_Bus::IPAAgentCorbaDef::PaErrorException, TA_Base_Core::OperationModeException
        static void broadcastFasTriggeredStationDva( unsigned long ulAgentLocationKey, unsigned long ulEventType, unsigned long ulSrcStationLocationKey );

    private:
        // Disable operator= and copy constructor
        PaAgentProxy& operator=(const PaAgentProxy &);
        PaAgentProxy( const PaAgentProxy& );
    };

} // end namespace TA_Base_Bus

#endif // define PA_AGENT_PROXY_H
