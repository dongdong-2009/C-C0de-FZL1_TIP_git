/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PvData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "StdAfx.h"
#include "PvData.h"


CPvData &CPvData::instance()
{
    FUNCTION_ENTRY( "instance" );

    static CPvData instance;

    FUNCTION_EXIT;
    return instance;
}


CPvData::CPvData()
{
    FUNCTION_ENTRY( "CPvData" );

    m_categories.push_back(new PV::Category("Normal Modes", "These plans are used in normal system operations"));
    {
        m_plans.push_back(new PV::Plan("System Startup", "This plan defines the procedure for starting the system", true, PV::Plan::ExecMode::EM_INTERACTIVE));
        {
            createDummySteps();
        }

        m_plans.push_back(new PV::Plan("System Shutdown", "This plan defines the procedure for shutting the system", true, PV::Plan::ExecMode::EM_INTERACTIVE));
        {
            createDummySteps();
        }

        m_plans.push_back(new PV::Plan("Line Opening", "This plan defines the procedure for opening the line", true, PV::Plan::ExecMode::EM_INTERACTIVE));
        {
            createDummySteps();
        }

        m_plans.push_back(new PV::Plan("Train Launch", "This plan defines the procedure for launching a train", true, PV::Plan::ExecMode::EM_INTERACTIVE));
        {
            createDummySteps();
        }

        m_plans.push_back(new PV::Plan("Train Wash", "This plan defines the procedure for washing a train", false, PV::Plan::ExecMode::EM_AUTOMATIC));
        {
            createDummySteps();
        }
    }


    m_categories.push_back(new PV::Category("Incident Modes", "These plans are used in response to incidents"));
    {
        m_plans.push_back(new PV::Plan("Tunnel Congestion", "This plan defines the procedure for dealing with tunnel congestion", true, PV::Plan::ExecMode::EM_AUTOMATIC));
        {
            createDummySteps();
        }

        m_plans.push_back(new PV::Plan("Train Overrun", "This plan defines the procedure for dealing with train overrun", false, PV::Plan::ExecMode::EM_INTERACTIVE));
        {
            createDummySteps();
        }

        m_plans.push_back(new PV::Plan("Door Obstruction", "This plan defines the procedure for dealing with door obstruction", true, PV::Plan::ExecMode::EM_AUTOMATIC));
        {
            createDummySteps();
        }
    }


    m_categories.push_back(new PV::Category("Emergency Modes", "These plans are used in response to emergencies"));
    {
        m_plans.push_back(new PV::Plan("Train Derailment", "This plan defines the procedure for dealing with train derailment", true, PV::Plan::ExecMode::EM_INTERACTIVE));
        {
            createDummySteps();
        }

        m_plans.push_back(new PV::Plan("Station on Fire", "This plan defines the procedure for dealing with a burning station", true, PV::Plan::ExecMode::EM_INTERACTIVE));
        {
            createDummySteps();
        }

        m_plans.push_back(new PV::Plan("Tunnel Collapse", "This plan defines the procedure for dealing with tunnel collapse", false, PV::Plan::ExecMode::EM_INTERACTIVE));
        {
            createDummySteps();
        }

        m_plans.push_back(new PV::Plan("Train Collision", "This plan defines the procedure for dealing with train collision", false, PV::Plan::ExecMode::EM_INTERACTIVE));
        {
            createDummySteps();
        }
    }


    createDummyActiveInstances();

    FUNCTION_EXIT;
}


CPvData::~CPvData()
{
    FUNCTION_ENTRY( "~CPvData" );

    for (PV::CategoryList::iterator citer = m_categories.begin(); citer != m_categories.end(); citer++)
    {
        PV::Category *category = *citer;
        delete category;
    }
    m_categories.clear();

    for (PV::PlanList::iterator piter = m_plans.begin(); piter != m_plans.end(); piter++)
    {
        PV::Plan *plan = *piter;
        delete plan;
    }
    m_plans.clear();

    for (PV::StepList::iterator siter = m_steps.begin(); siter != m_steps.end(); siter++)
    {
        PV::Step *step = *siter;
        delete step;
    }
    m_steps.clear();

    FUNCTION_EXIT;
}


void CPvData::createDummySteps()
{
    FUNCTION_ENTRY( "createDummySteps" );

    std::vector<std::string> stepValues;
    stepValues.push_back("2");
    stepValues.push_back("3");
    stepValues.push_back("4");
    stepValues.push_back("5");
    stepValues.push_back("6");
    stepValues.push_back("7");
    stepValues.push_back("8");
    stepValues.push_back("9");
    stepValues.push_back("10");

    std::vector<std::string> smsRecipientValues;
    smsRecipientValues.push_back("Emergency Services");
    smsRecipientValues.push_back("Jack");
    smsRecipientValues.push_back("Fred");
    smsRecipientValues.push_back("Jane");

    std::vector<std::string> modeValues;
    modeValues.push_back("Closed");
    modeValues.push_back("Open");
    modeValues.push_back("Citybound Closed");
    modeValues.push_back("Citybound Open");

    std::vector<std::string> sectionValues;
    sectionValues.push_back("Section 1");
    sectionValues.push_back("Section 4");
    sectionValues.push_back("Section 6");
    sectionValues.push_back("Section 8");
    sectionValues.push_back("Section G2");
    sectionValues.push_back("Section K12/B");
    sectionValues.push_back("Section C");

    std::vector<std::string> signValues;
    signValues.push_back("BB010");
    signValues.push_back("BB011");
    signValues.push_back("BB012");
    signValues.push_back("BB013");
    signValues.push_back("BB014");
    signValues.push_back("BB015");
    signValues.push_back("BB164");
    signValues.push_back("MS039");
    signValues.push_back("MS041");
    signValues.push_back("MS045");
    signValues.push_back("MS046");
    signValues.push_back("MS053");

    std::vector<std::string> vcrValues;
    vcrValues.push_back("<none>");
    vcrValues.push_back("Control Room VCR");

    std::vector<std::string> cameraValues;
    cameraValues.push_back("CA022");
    cameraValues.push_back("CA025");
    cameraValues.push_back("CA031");
    cameraValues.push_back("CA067");

    std::vector<std::string> monitorValues;
    monitorValues.push_back("<none>");
    monitorValues.push_back("Local");

    PV::Step *step = NULL;
    int execOrder = 1;

    m_steps.push_back(step = new PV::Step("Attendance check", "Are you the only operator on shift?", "Decision", execOrder++, 0, false));
    {
        step->addParameter("Timeout", "0");
        step->addParameter("YesStep", "2", stepValues);
        step->addParameter("NoStep", "3", stepValues);
        step->addParameter("MessageText", "Are you the only operator on shift?");
    }
    m_steps.push_back(step = new PV::Step("Contact transition personnel", "Ring whoever is currently on roster", "Prompt", execOrder++, 0, false));
    {
        step->addParameter("Timeout", "0");
        step->addParameter("MessageText", "Ring the rostered transition person.");
    }
    m_steps.push_back(step = new PV::Step("Notify emergency services", "Page emergency crews via SMS", "SMS", execOrder++, 0, false));
    {
        step->addParameter("Timeout", "60");
        step->addParameter("MessageRecipient", "Emergency Services", smsRecipientValues);
        step->addParameter("MessageText", "Fire inside the city tunnel");
    }
    m_steps.push_back(step = new PV::Step("Close section 8", "Set section 8 (citybound) to Closed.", "Change Mode", execOrder++, 0, false));
    {
        step->addParameter("Mode", "Closed", modeValues);
        step->addParameter("Section", "Section 8", sectionValues);
    }
    m_steps.push_back(step = new PV::Step("Close MS046", "Set MS046 to Closed.", "Sign", execOrder++, 0, true));
    {
        step->addParameter("Timeout", "90");
        step->addParameter("Scene", "Closed");
        step->addParameter("Sign", "MS046", signValues);
    }
    m_steps.push_back(step = new PV::Step("Close MS045 after 10 seconds", "Set MS045 to Closed.", "Sign", execOrder++, 10, false));
    {
        step->addParameter("Timeout", "60");
        step->addParameter("Scene", "Closed");
        step->addParameter("Sign", "MS045", signValues);
    }
    m_steps.push_back(step = new PV::Step("CA025 on monitor, pan to BB015", "Switch CA025 to local monitor and pan to BB015.", "CCTV", execOrder++, 0, true));
    {
        step->addParameter("RecordingUnit", "<none>", vcrValues);
        step->addParameter("Camera", "CA025", cameraValues);
        step->addParameter("Monitor", "Local", monitorValues);
        step->addParameter("Preset", "15");
    }
    m_steps.push_back(step = new PV::Step("Close BB012-014", "Set BB012, BB013 and BB014 to Closed.", "Sign Range", execOrder++, 0, false));
    {
        step->addParameter("Timeout", "90");
        step->addParameter("Scene", "Closed");
        step->addParameter("Range", "12-14");
    }
    m_steps.push_back(step = new PV::Step("Close MS039", "Set MS039 to Closed.", "Sign (No Confirm)", execOrder++, 0, false));
    {
        step->addParameter("Scene", "Closed");
        step->addParameter("Sign", "MS039", signValues);
    }
    m_steps.push_back(step = new PV::Step("Wait for MS039 to close", "Wait until MS039 has been closed.", "Wait Until", execOrder++, 0, false));
    {
        step->addParameter("Timeout", "30");
        step->addParameter("Scene", "Closed");
        step->addParameter("Sign", "MS039", signValues);
    }

    FUNCTION_EXIT;
}


void CPvData::createDummyActiveInstances()
{
    FUNCTION_ENTRY( "createDummyActiveInstances" );

    srand((unsigned int)time(NULL));

    for (PV::PlanList::iterator piter = m_plans.begin(); piter != m_plans.end(); piter++)
    {
        PV::Plan *plan = *piter;
        bool isInteractive = plan->m_execMode == PV::Plan::ExecMode::EM_INTERACTIVE;

        // Unapproved plans cannot become active.
        if (plan->m_isApproved == true)
        {
            // Populate the active instances list with new randomly generated entries
            int activeInstances = rand() % 3;
            for (int i = 1; i <= activeInstances; i++)
            {
                // Select randomly the current step for this plan instance
                PV::StepList steps;
                CPvData::instance().getStepsForPlan(steps, plan->m_pkey);
                PV::Step *step = steps[rand() % steps.size()];

                // Select the execution state and owner. For non-system plans these are random.
                // Give myself a slightly better chance at being selected ;-)
                static const char* DUMMY_OPERATORS[] = { "Me !!!", "Me !!!", "Me !!!", "Fred", "Bob", "Jane", "System" };
                int NUM_DUMMY_OPERATORS = isInteractive? 6 : 7;
                std::string owner = DUMMY_OPERATORS[rand() % NUM_DUMMY_OPERATORS];

                PV::PlanInstance::ExecState execState = rand() % 2 && owner != "System" ?
                                                            PV::PlanInstance::ExecState::ES_PAUSED :
                                                            PV::PlanInstance::ExecState::ES_RUNNING;;

                // Create a representation of this plan instance
                plan->addInstance(new PV::PlanInstance(plan, i, owner, execState, isInteractive, step->m_pkey));
            }
        }
    }

    FUNCTION_EXIT;
}


void CPvData::getAllCategories(PV::CategoryList &categories)
{
    FUNCTION_ENTRY( "getAllCategories" );

    categories = m_categories;

    FUNCTION_EXIT;
}


void CPvData::getAllPlans(PV::PlanList &plans)
{
    FUNCTION_ENTRY( "getAllPlans" );

    plans = m_plans;

    FUNCTION_EXIT;
}


void CPvData::getApprovedPlans(PV::PlanList &plans)
{
    FUNCTION_ENTRY( "getApprovedPlans" );

    for (PV::PlanList::iterator piter = m_plans.begin(); piter != m_plans.end(); piter++)
    {
        PV::Plan *plan = *piter;

        if (plan->m_isApproved == true)
        {
            plans.push_back(plan);
        }
    }

    FUNCTION_EXIT;
}


void CPvData::getUnapprovedPlans(PV::PlanList &plans)
{
    FUNCTION_ENTRY( "getUnapprovedPlans" );

    for (PV::PlanList::iterator piter = m_plans.begin(); piter != m_plans.end(); piter++)
    {
        PV::Plan *plan = *piter;

        if (plan->m_isApproved == false)
        {
            plans.push_back(plan);
        }
    }

    FUNCTION_EXIT;
}


void CPvData::getActivePlans(PV::PlanList &plans)
{
    FUNCTION_ENTRY( "getActivePlans" );

    for (PV::PlanList::iterator piter = m_plans.begin(); piter != m_plans.end(); piter++)
    {
        PV::Plan *plan = *piter;

        if (plan->m_instances.size() > 0)
        {
            plans.push_back(plan);
        }
    }

    FUNCTION_EXIT;
}


void CPvData::getPlansForCategory(PV::PlanList &plans, unsigned long categoryPKey)
{
    FUNCTION_ENTRY( "getPlansForCategory" );

    for (PV::PlanList::iterator piter = m_plans.begin(); piter != m_plans.end(); piter++)
    {
        PV::Plan *plan = *piter;

        if (plan->m_categoryPkey == categoryPKey)
        {
            plans.push_back(plan);
        }
    }

    FUNCTION_EXIT;
}


void CPvData::getStepsForPlan(PV::StepList &steps, unsigned long planPKey)
{
    FUNCTION_ENTRY( "getStepsForPlan" );

    for (PV::StepList::iterator siter = m_steps.begin(); siter != m_steps.end(); siter++)
    {
        PV::Step *step = *siter;

        if (step->m_planPkey == planPKey)
        {
            steps.push_back(step);
        }
    }

    FUNCTION_EXIT;
}


PV::Category *CPvData::getCategory(unsigned long categoryPKey)
{
    FUNCTION_ENTRY( "getCategory" );

    // Too few categories to bother with maps, do a linear search instead...
    for (PV::CategoryList::iterator citer = m_categories.begin(); citer != m_categories.end(); citer++)
    {
        PV::Category *category = *citer;

        if (category->m_pkey == categoryPKey)
        {
            FUNCTION_EXIT;
            return category;
        }
    }

    FUNCTION_EXIT;
    return NULL;
}


PV::Plan *CPvData::getPlan(unsigned long planPKey)
{
    FUNCTION_ENTRY( "getPlan" );

    // Too few plans to bother with maps, do a linear search instead...
    for (PV::PlanList::iterator piter = m_plans.begin(); piter != m_plans.end(); piter++)
    {
        PV::Plan *plan = *piter;

        if (plan->m_pkey == planPKey)
        {
            FUNCTION_EXIT;
            return plan;
        }
    }

    FUNCTION_EXIT;
    return NULL;
}


PV::Step *CPvData::getStep(unsigned long stepPKey)
{
    FUNCTION_ENTRY( "getStep" );

    // Too few steps to bother with maps, do a linear search instead...
    for (PV::StepList::iterator siter = m_steps.begin(); siter != m_steps.end(); siter++)
    {
        PV::Step *step = *siter;

        if (step->m_pkey == stepPKey)
        {
            FUNCTION_EXIT;
            return step;
        }
    }

    FUNCTION_EXIT;
    return NULL;
}


