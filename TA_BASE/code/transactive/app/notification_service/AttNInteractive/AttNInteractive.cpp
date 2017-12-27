/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/AttNInteractive/AttNInteractive.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// AttNInteractive.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>

#include "core/corba/src/CorbaUtil.h"
#include "core/corba/IDL/src/IChannelLocatorCorbaDef.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/utilities/src/DebugSETranslator.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/StructuredEventConsumer.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/src/TA_CosUtility.h"

#include "omniNotify.h"
#include "COS/CosNotification.hh"


// This application allows a notification service (ie. IChannelLocator instance) to
// be interrogated interactively and remotely.  The event channels provided by the  
// IChannelLocator instance can be listed, and debugging and statistics information 
// about the channels can be obtained.  Also, all messages passing through an event 
// channel can be output.


std::string trimLeft(const std::string& str)
{
    std::string::const_iterator start = str.begin();

	while (start != str.end() && isspace(*start))
	{
		++start;
	}

    return str.substr(start - str.begin(), std::string::npos);
}

void WriteStringToLog(const std::string& str)
{
    static TA_Base_Core::NonReEntrantThreadLockable lock;

	TA_Base_Core::ThreadGuard guard( lock );

    // Always write the string to std::cout
    std::cout << str << std::endl;

    // Only write the string to the log file if a file has been set (otherwise it will
    // be written to std::cout twice).
    if (TA_Base_Core::RunParams::getInstance().isSet("DebugFile"))
    {
        TA_Base_Core::DebugUtil::getInstance().logLargeString(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, str);
    }
}


struct Handler : public TA_Base_Core::AbstractMessageSubscriber
{
    // Implementation of AbstractMessageSubscriber
	virtual void receiveMessage(const CORBA::Any& message) {TA_Assert(0);} // Dummy implementation is NOT CALLED
	virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events)
    {
        for (unsigned int i = 0; i < events.size(); ++i)
        {
            TA_Assert(events[i] != NULL);

            std::ostringstream stm;
            stm << "EVT " << i << ": " << TA_Base_Core::gStringFromEventHeader(*(events[i]));
            WriteStringToLog(stm.str());
        }

        return true;
    }
};

int main(int argc, char* argv[])
{
    if( false == TA_Base_Core::RunParams::getInstance().parseCmdLine( argc - 1, &argv[1] ) )
    {
	    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
            "Unable to parse command line. Aborting." );
        return 1;
    }

    TA_Base_Core::gSetDebugUtilFromRunParams();
    TA_Base_Core::gInitStructuredExceptionHandler();   // Only initialises the calling thread

	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "AttNInteractive.main(): Initializing Corba");
	TA_Base_Core::CorbaUtil::getInstance().initialise();
	TA_Base_Core::CorbaUtil::getInstance().activate();


    // The IChannelLocator interface is used to access a particular event channel.
    // A corbaloc url to the IChannelLocator implementation has to be specified
    // first, then the event channel of interest.
    TA_Base_Core::IChannelLocator_var channelLocator; 
	CosNotifyChannelAdmin::EventChannel_var channel;
    AttNotification::Interactive_ptr targetObj;
    
    bool exit = false;
    do
    {
        std::cout << "?> " << std::flush;

        // Note that operator>> stops parsing as soon as whitespace is encountered - so 
        // it will only read a single token
        std::string cmd;
        std::cin >> cmd;

        if (TA_Base_Core::RunParams::getInstance().isSet("DebugFile"))
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "?> %s", cmd.c_str());
        }

        try
        {
            if (cmd == "help")
            {
                WriteStringToLog(
                    "Client commands: \n" 
                    " help \n" 
                    " exit \n" 
                    " setnsa <url>  (eg. setnsa corbaloc::callisto:9991/ChannelLocator) \n" 
                    " listchannels \n" 
                    " setchannel <channelName> \n" 
                    " svr <server command>\n"
                    " attach - displays messages passing through the current channel\n" 
                    "\n" 
                    "Additional commands for server object: \n" 
                    " svr help \n" 
                    " svr up \n" 
                    " svr go <object> \n" 
                    " svr flags \n" ); 
            }
            else if (cmd == "exit")
            {
                exit = true;
            }
            else if (cmd == "setnsa")
            {
                char buf[1024];
                std::cin.getline(buf, 1024);
                std::string objrefStr(buf);
                objrefStr = trimLeft(objrefStr);

                if (objrefStr.empty())
                {
                    WriteStringToLog("Error: Empty object reference");
                    continue;
                }

                // The remainder of the command string is an object reference
                // of the form corbaloc::hostname:port/interface
                // (eg. "corbaloc::callisto:9991/ChannelLocator").
			    TA_Base_Core::IChannelLocator_var newChannelLocator = TA_Base_Core::IChannelLocator::_narrow(
                    TA_Base_Core::CorbaUtil::getInstance().stringToObject(objrefStr) );

			    if ( !CORBA::is_nil( newChannelLocator ) )
			    {
                    channelLocator = newChannelLocator;
                    channel = CosNotifyChannelAdmin::EventChannel::_nil();
                }
                else
                {
                    std::ostringstream stm;
                    stm << "Error: Invalid object reference " << objrefStr;
                    WriteStringToLog(stm.str());
                }
            }
            else if (cmd == "listchannels" && !CORBA::is_nil(channelLocator))
            {
			    TA_Base_Core::ChannelMappingSeq* channels = channelLocator->getChannels( );
                std::ostringstream stm;
			    for ( unsigned int i = 0; i < channels->length(); i++)
			    {
                    TA_Base_Core::ChannelMapping& mapping = (*channels)[i];
                    stm << mapping.channelName.in() << std::endl;
			    }

                WriteStringToLog(stm.str());
				
                delete channels;						
            }
            else if (cmd == "setchannel" && !CORBA::is_nil(channelLocator))
            {
                char buf[1024];
                std::cin.getline(buf, 1024);
                std::string arg(buf);
                arg = trimLeft(arg);

                CosNotifyChannelAdmin::EventChannel_var newChannel;
                AttNotification::Interactive_ptr newTargetObj = AttNotification::Interactive::_nil();

			    TA_Base_Core::ChannelMappingSeq* channels = channelLocator->getChannels( );

			    for ( unsigned int i = 0; i < channels->length(); i++)
			    {
                    TA_Base_Core::ChannelMapping& mapping = (*channels)[i];
                    if (mapping.channelName.in() == arg)
                    {
                        newChannel = CosNotifyChannelAdmin::EventChannel::_duplicate(mapping.channel);
                        newTargetObj = AttNotification::Interactive::_narrow(newChannel);
                        break;
                    }
			    }

                if (!CORBA::is_nil(newTargetObj))
                {
                    TA_Assert(!CORBA::is_nil(newChannel));

                    channel = newChannel;
                    targetObj = newTargetObj;
                }
                else
                {
                    WriteStringToLog("Error: Invalid channel name");
                }

				delete channels;						
            }
            else if (cmd == "attach")
            {
                if (CORBA::is_nil(channel))
                {
                    WriteStringToLog("Error: Channel has not been set");
                    continue;
                }

                // TODO LPP: Which part of the dual event channel should this attach to?

		        TA_Base_Core::StructuredEventConsumer* consumer = new TA_Base_Core::StructuredEventConsumer(new TA_Base_Core::DefaultAdminProvider);
                consumer->addConnection("dummy", channel);

        		CosNotification::EventTypeSeq seq;
 		        seq.length(1);
                seq[0].domain_name = CORBA::string_dup(TA_Base_Core::WILDCARD_STR);
                seq[0].type_name = CORBA::string_dup(TA_Base_Core::WILDCARD_STR);
		        TA_Base_Core::SubscriptionFilter* filter = new TA_Base_Core::SubscriptionFilter(seq, "true", NULL);
                TA_Base_Core::IEventHandler* handler = new Handler;

        		consumer->attachSubscriberIfCompatible(handler, filter);

                // LPP TODO: HACK - how do we do "Press Any Key"?
                std::string cmd;
                std::cin >> cmd;

                consumer->detachSubscriber(filter);
                consumer->disconnectAllEventChannels();
                delete consumer;
                
                delete filter;
                delete handler;
            }
            else if (cmd == "svr")
            {
                if (CORBA::is_nil(channel))
                {
                    WriteStringToLog("Error: Channel has not been set");
                    continue;
                }

                TA_Assert(!CORBA::is_nil(channelLocator));

                char buf[1024];
                std::cin.getline(buf, 1024);
                std::string svrCmd(buf);
                svrCmd = trimLeft(svrCmd);

                // Pass the command to the current target object
                CORBA::Boolean success = false;
                CORBA::Boolean target_changed = false;
                AttNotification::Interactive_ptr newTargetObj;
            
                char* result = targetObj->do_command(svrCmd.c_str(), success, target_changed, newTargetObj);
                WriteStringToLog(result);
                CORBA::string_free(result);

                if (target_changed)
                {
                    targetObj = newTargetObj;
                }
            }
            else
            {
                WriteStringToLog("Error: Invalid command");

                // discard the rest of the line
                char buf[1024];
                std::cin.getline(buf, 1024);
            }
        }
        catch ( const CORBA::Exception& cex )
        {
            std::ostringstream stm;
            stm << "Error: CORBA::Exception caught: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex);
            WriteStringToLog(stm.str());
        }
    }
    while (!exit);

	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "AttNInteractive.main(): Shutting down Corba");
	TA_Base_Core::CorbaUtil::getInstance().shutdown();
	TA_Base_Core::CorbaUtil::getInstance().cleanup();

	return 0;
}


