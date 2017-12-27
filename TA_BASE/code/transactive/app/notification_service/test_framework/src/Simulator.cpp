/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/Simulator.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  Simulator.cpp
//  Implementation of the Class Simulator
//  Created on:      11-May-2004 08:39:50 AM
///////////////////////////////////////////////////////////

#include "Simulator.h"
#include "Common.h"
#include "CommsSupplier.h"
#include "CommsConsumer.h"
#include "AlarmSupplier.h"
#include "AlarmConsumer.h"

#include "core/utilities/src/TA_String.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/utilities/src/DebugSETranslator.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/PublicationManager.h"
#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/ConnectionChecker.h"


Simulator::Simulator( unsigned long simulatorID, SimpleDb& db, /*takes*/ ISimulationHandler* handler ) :
    m_simulatorID( simulatorID ),
    m_db(db),
    m_handler(handler)
{
    TA_Assert(m_handler.get() != NULL);
	m_handler->m_bGoingDown = false;
}


Simulator::~Simulator()
{
}


void Simulator::run()
{
    TA_Assert(m_handler.get() != NULL);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Simulator::run() called");

    populateSettings();

    for (unsigned int i = 0; i < m_numSteps; ++i)
    {
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Before Simulation Step");
        
		m_handler->simulationStep(i);

        TA_Base_Core::Thread::sleep( m_postStepDelay );

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "After Simulation Step");

    }

	m_handler->m_bGoingDown = true;
}


void Simulator::populateSettings()
{
    // Relevant columns from NST_SIMULATORCONFIG:
    // numSteps         - number of steps in the simulation
    // postStepDelay    - delay in ms after each step

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Simulator::populateSettings(): Retrieving settings");
    const int NUM_COLUMNS = 2;
    std::ostringstream sql;
    sql << "select numSteps, postStepDelay ";
    sql << "from NST_SIMULATORCONFIG where simulatorID = " << m_simulatorID;

    SimpleDb::Buffer data;
    m_db.Exec( sql.str().c_str(), NUM_COLUMNS, data );

    m_numSteps = atol(data[0][0].c_str());
    m_postStepDelay = atol(data[0][1].c_str());
}


ISimulationHandler* createHandler(
    const std::string& handlerFactory,
    unsigned long simulatorID, 
    SimpleDb& db)
{
    // Normally we would use a factory registry to decentralise knowledge of the 
    // types of factories, but a quick & dirty solution is used here.

    if (handlerFactory == "CommsSupplier")
    {
        return new CommsSupplierHandler(simulatorID, db, false);
    }
    else if (handlerFactory == "BCastCommsSupplier")
    {
        return new CommsSupplierHandler(simulatorID, db, true);
    }
    else if (handlerFactory == "CommsConsumer")
    {
        return new CommsConsumerHandler(simulatorID, db, false);
    }
    else if (handlerFactory == "BCastCommsConsumer")
    {
        return new CommsConsumerHandler(simulatorID, db, true);
    }
    else if (handlerFactory == "AlarmDetailSupplier")
    {
        return new AlarmSupplierHandler(simulatorID, db, AlarmSupplierHandler::ALARM_DETAIL);
    }
    else if (handlerFactory == "AlarmUpdateSupplier")
    {
        return new AlarmSupplierHandler(simulatorID, db, AlarmSupplierHandler::ALARM_UPDATE);
    }      
    else if (handlerFactory == "AlarmLoadedSupplier")
    {
        return new AlarmSupplierHandler(simulatorID, db, AlarmSupplierHandler::ALARM_LOADED);
    }
    else if (handlerFactory == "AlarmConsumer")
    {
        return new AlarmConsumerHandler(simulatorID, db);
    }
    else
    {
        TA_Assert(0);
        return NULL;
    }
}


// System specific event handlers.
#ifdef WIN32
BOOL WINAPI ConsoleHandler( DWORD CEvent )
{
    switch( CEvent )
    {
    case CTRL_C_EVENT:
//        s_semaphore.post();
        break;
    default:
        break;
    }
    return TRUE;
}
#endif


int main( int argc, char* argv[] )
{
    // Setup system specific event handlers.
#ifdef WIN32
    if( SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler,TRUE)==FALSE )
    {
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Unable to install handler. Aborting...");
        return -1;
    }
#endif

    if( false == TA_Base_Core::RunParams::getInstance().parseCmdLine( argc - 1, &argv[1] ) )
    {
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Unable to parse command line. Aborting...");
        exit(-1);
    }

    TA_Base_Core::gSetDebugUtilFromRunParams();
    TA_Base_Core::gInitStructuredExceptionHandler();   // Only initialises the calling thread

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Simulator.main(): Initializing Corba");
	TA_Base_Core::CorbaUtil::getInstance().initialise();
	TA_Base_Core::CorbaUtil::getInstance().activate();

    int retval = 0;
    try
    {
        // The simulatorID is supplied as the 2nd command line argument.
        unsigned long simulatorID = atol( argv[1] );

        // Open a connection to the database and retrieve relevant fields for this simulator.
    	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Simulator.main(): Opening database");
        std::string dbhost, user, pass;
        getDbConnectParams(dbhost, user, pass);

        SimpleDb db( dbhost.c_str(), user.c_str(), pass.c_str() ); 
        db.Open();

        // Relevant columns from NST_SIMULATORCONFIG:
        // simulatorLocationKey - the local location key that the simulator messaging framework will use 
        //                        (ie. LocationKey runparam).  It is unrelated to the location key that
        //                        messages are sent with or that received messages must have.
        // handlerFactory       - string name of the factory that will create an appropriate handler
        //                        (see createHandler() for more details).

        const int NUM_COLUMNS = 2;
        std::ostringstream sql;
        sql << "select simulatorLocationKey, handlerFactory ";
        sql << "from NST_SIMULATORCONFIG where simulatorID = " << simulatorID << " ";
        sql << "order by simulatorID";

        SimpleDb::Buffer data;
        db.Exec( sql.str().c_str(), NUM_COLUMNS, data );

        // Initialise some simulator-specific RunParams
    	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Simulator.main(): Initialising RunParams");
        std::string simulatorLocationKey = data[0][0];
        TA_Base_Core::RunParams::getInstance().set("LocationKey", simulatorLocationKey.c_str());

        TA_Base_Core::RunParams::getInstance().set("MgrPort", TA_Base_Core::gStringFromUnsignedLong(DEFAULT_MGRPORT).c_str());

        std::string notifyHosts = getNotifyHostsForLocation( atoi(simulatorLocationKey.c_str()) );
        TA_Base_Core::RunParams::getInstance().set("NotifyHosts", notifyHosts.c_str());

        // Create the appropriate type of simulator handler and start a simulator
    	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Simulator.main(): Starting simulator %d", simulatorID);

        // The database is used by the handler for adding records for each simulation step.
        ISimulationHandler* handler = createHandler(data[0][1], simulatorID, db);
        Simulator simulator(simulatorID, db, handler);
        simulator.run();

		// Wait for sometime so that the writing to DB can happen before destruction.
        // This also allows the BatchingQueue to flush itself (hopefully).
		Sleep(2000);
        db.Close();

    	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Simulator.main(): Simulator %d finished", simulatorID);
    }
    catch ( ... )
    {
        retval = -1;
    }

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Simulator.main(): Shutting down messaging singletons");
    TA_Base_Core::gDestroySubscriptionManager();
    TA_Base_Core::gTerminateAndDeleteStructuredEventSuppliers();
	TA_Base_Core::ChannelLocatorConnectionMgr::removeInstance();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Simulator.main(): Shutting down Corba");
    TA_Base_Core::CorbaUtil::getInstance().shutdown();
    TA_Base_Core::CorbaUtil::cleanup();

    return retval;
}


void storeMessageToBeSent(
    SimpleDb& db, 
    unsigned long supplierID,
    unsigned long stepID,
    const std::string messageContext,
    const std::string messageType,
    unsigned long entityKey,
    unsigned long locationKey,
    const char* data )
{
    std::ostringstream sql;
    sql << "insert into NST_MESSAGESSENT( srcEntityKey, supplierID, srcLocationKey, srcStepID, msgContextName, msgTypeName";

    if( NULL != data )
    {
        sql << ", data";
    }

    sql << " ) values (" 
        << entityKey << ", " << supplierID << ", " << locationKey << ", " 
        << stepID << ", '" << messageContext << "', '" << messageType;

    if( NULL != data )
    {
        sql << "', '" << data ;
    }

    sql << "')";
    
    db.Exec( sql.str().c_str() );
}


void storeMessageReceived( 
    SimpleDb& db,
    unsigned long consumerID,
    const std::string& supplierID,
    const std::string& srcStepID,
    unsigned long srcSeqNum,
    unsigned long expectedSeqNum,
    const std::string messageContext,
    const std::string messageType,
    const std::string& srcEntityKey,
    const std::string& srcLocationKey,
    const char* data )
{
    std::ostringstream sql;
    sql << "insert into NST_MESSAGESRECEIVED ( "
        << "consumerID, supplierID, srcLocationKey, srcStepID, srcSeqNum, expectedSeqNum, msgContextName, msgTypeName, srcEntityKey";

    if( NULL != data )
    {
        sql << ", data";
    }

    sql <<  " ) values ("
        << consumerID << ", " << supplierID << ", " << srcLocationKey << ", " << srcStepID << ", " << srcSeqNum << ", " 
        << expectedSeqNum << ", '" << messageContext << "', '" << messageType << "', " << srcEntityKey;
        
    if( NULL != data )
    {
        sql << ", '" << data << "'";
    }

    sql << ")";
    
    db.Exec( sql.str().c_str() );
}


