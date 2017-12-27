/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/Simulator.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  Simulator.h
//  Implementation of the Class Simulator
//  Created on:      11-May-2004 08:39:50 AM
///////////////////////////////////////////////////////////

#if !defined(Simulator_022B4103_DC04_4288_B6A4_22332CF7F941__INCLUDED_)
#define Simulator_022B4103_DC04_4288_B6A4_22332CF7F941__INCLUDED_

#include "core/threads/src/Thread.h"
#include "core/database/src/SimpleDB.h"
#include "core/message/src/BackwardsCompatibility.h"

#include <memory>
#include <vector>
#include <utility>


struct ISimulationHandler
{
    // TODO LPP NOW:  What the?  This is a complete hack.  What's it doing?
	bool m_bGoingDown;
    
	// Called once for each simulation step.  stepID will increment by one for each call.
    virtual void simulationStep(unsigned long stepID) = 0;
    virtual ~ISimulationHandler() {}
};


class Simulator 
{
public:                              
	Simulator( unsigned long simulatorID, SimpleDb& db, /*takes*/ ISimulationHandler* handler );
	virtual ~Simulator();

    // Executes the simulation by calling ISimulationHandler::simulationStep
    // repeatedly m_numSteps times.
	void run();


private:
    // Initialises m_numSteps and m_postStepDelay from NST_SIMULATORCONFIG.
    void populateSettings();

    unsigned long m_simulatorID;
    SimpleDb& m_db;
    std::auto_ptr<ISimulationHandler> m_handler;

    // Relevant columns from NST_SIMULATORCONFIG:
    // numSteps         - number of steps in the simulation
    // postStepDelay    - delay in ms after each step
    unsigned int m_numSteps;
    unsigned int m_postStepDelay;
};


// Helper functions for storing records in NST_MESSAGESENT and NST_MESSAGERECEIVED
void storeMessageToBeSent(
    SimpleDb& db, 
    unsigned long supplierID,
    unsigned long stepID,
    const std::string messageContext,
    const std::string messageType,
    unsigned long entityKey,
    unsigned long locationKey,
    const char* data );

void storeMessageReceived( 
    SimpleDb& db,
    unsigned long consumerID,          // simulatorID of the consumer
    const std::string& supplierID,     // simulatorID of the supplier
    const std::string& srcStepID,      // simulation stepID
    unsigned long srcSeqNum,           // seqNum added by SeqNumAppender
    unsigned long expectedSeqNum,      
    const std::string messageContext,
    const std::string messageType,
    const std::string& srcEntityKey,
    const std::string& srcLocationKey,
    const char* data );

#endif // !defined(Simulator_022B4103_DC04_4288_B6A4_22332CF7F941__INCLUDED_)
