/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PvCommonDefs.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
//PvCommonDefs.h

#if !defined(PLAN_VIEWER_COMMON_DEFINITIONS)
#define PLAN_VIEWER_COMMON_DEFINITIONS

//Get rid of a bunch of useless warnings
#pragma warning(disable:4100)   // unreferenced formal parameter
#pragma warning(disable:4355)   // 'this' : used in base member initializer list
#pragma warning(disable:4786)   // 255 character limit on mangled name
#pragma warning(disable:4512)    //'...' : assignment operator could not be generated in stl ostream
#pragma warning(disable:4245)    //'initializing' : conversion from 'const int' to 'const unsigned long', signed/unsigned mismatch
#pragma warning(disable:4127)    //conditional expression is constant (all those while(1)'s)

//DONT ADD THE FOLLOWING TO OTHER PROJECTS WITHOUT FIRST CHECKING
#pragma warning(disable:4510)    //default constructor could not be instantiated
#pragma warning(disable:4610)    //struct ** can never be instantiated
#pragma warning(disable:4097)  //typedef-name 'BaseT' used as synonym for class-name

#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <assert.h>
#include <iostream>

namespace PV
{
static unsigned long s_categoryPKey = 0;
static unsigned long s_planPKey     = 100;
static unsigned long s_stepPKey     = 1000;

struct Category;
struct Plan;
struct Step;
struct StepParameter;
struct PlanInstance;

typedef std::vector<PV::Category *> CategoryList;
typedef std::vector<PV::Plan *> PlanList;
typedef std::vector<PV::Step *> StepList;
typedef std::vector<PV::StepParameter *> StepParameterList;
typedef std::vector<PV::PlanInstance *> PlanInstanceList;

typedef std::map<unsigned long, PV::Step> StepMap;


struct Category
{
    Category() :
        m_name(""), m_description(""), m_pkey(++s_categoryPKey) {}

    Category(std::string name, std::string description) :
        m_name(name), m_description(description), m_pkey(++s_categoryPKey) {}

    std::string m_name;
    std::string m_description;

    unsigned long m_pkey;
};


struct PlanInstance
{
    typedef enum { ES_IDLE, ES_RUNNING, ES_PAUSED } ExecState;

    PlanInstance() :
        m_plan(NULL), m_instanceNumber(-1), m_owner(""), m_execState(ES_IDLE), m_isControlled(false), m_currentStepPkey(0) {}

    PlanInstance(Plan *plan, int instanceNumber, std::string owner, ExecState execState, bool isControlled, int currentStepPkey) :
        m_plan(plan), m_instanceNumber(instanceNumber), m_owner(owner), m_execState(execState), m_isControlled(isControlled), m_currentStepPkey(currentStepPkey) {}

    Plan *m_plan;
    int m_instanceNumber;
    std::string m_owner;
    ExecState m_execState;
    bool m_isControlled;
    unsigned long m_currentStepPkey;
};


struct Plan
{
    typedef enum { EM_AUTOMATIC, EM_INTERACTIVE, EM_UNDEFINED } ExecMode;

    Plan() :
        m_name(""), m_description(""), m_isApproved(false), m_execMode(EM_UNDEFINED), m_selectedInstance(0),
        m_pkey(++s_planPKey), m_categoryPkey(s_categoryPKey) {}

    Plan(std::string name, std::string description, bool isApproved, ExecMode execMode) :
        m_name(name), m_description(description), m_isApproved(isApproved), m_execMode(execMode), m_selectedInstance(0),
        m_pkey(++s_planPKey), m_categoryPkey(s_categoryPKey) {}

    ~Plan()
    {
        for (PlanInstanceList::iterator iter = m_instances.begin(); iter != m_instances.end(); iter++)
        {
            PlanInstance *instance = *iter;
            delete instance;
        }
        m_instances.clear();
    }

    void addInstance(PlanInstance *instance)
    {
        if (instance) m_instances.push_back(instance);
    }

    std::string m_name;
    std::string m_description;
    bool m_isApproved;
    ExecMode m_execMode;

    int m_selectedInstance;
    PlanInstanceList m_instances;

    unsigned long m_pkey;
    unsigned long m_categoryPkey;
};


struct StepParameter
{
    StepParameter(std::string name, std::string value) :
        m_name(name), m_value(value), m_isSelection(false), m_isNumeric(true)
    {
        for (int i = 0; i < m_value.length(); i++)
        {
            if ((m_value[i] < '0' || m_value[i] > '9') && !(m_value[i] == '.' || m_value[i] == '-'))
            {
                m_isNumeric = false;
                break;
            }
        }
    }

    StepParameter(std::string name, std::string value, std::vector<std::string> valueList) :
        m_name(name), m_value(value), m_valueList(valueList), m_isSelection(true), m_isNumeric(false) {}

    ~StepParameter()
    {
        m_valueList.clear();
    }

    std::string m_name;
    std::string m_value;
    std::vector<std::string> m_valueList;
    bool m_isSelection;
    bool m_isNumeric;
};


struct Step
{
    Step() :
        m_name(""), m_description(""), m_type(""), m_executionOrder(0), m_delay(0), m_skip(false),
        m_pkey(++s_stepPKey), m_planPkey(s_planPKey) {}

    Step(std::string name, std::string description, std::string type, unsigned long execOrder, unsigned long delay, bool skip) :
        m_name(name), m_description(description), m_type(type), m_executionOrder(execOrder), m_delay(delay), m_skip(skip),
        m_pkey(++s_stepPKey), m_planPkey(s_planPKey) {}

    Step(const Step &step)
    {
        operator=(step);
    }

    Step & operator=(const Step &step)
    {
        if (this != &step)
        {
            m_name = step.m_name;
            m_description = step.m_description;
            m_type = step.m_type;
            m_executionOrder = step.m_executionOrder;
            m_delay = step.m_delay;
            m_skip = step.m_skip;
            m_pkey = step.m_pkey;
            m_planPkey = step.m_planPkey;
        }

        return *this;
    }

    ~Step()
    {
        for (StepParameterList::iterator iter = m_parameters.begin(); iter != m_parameters.end(); iter++)
        {
            StepParameter *parameter = *iter;
            delete parameter;
        }
        m_parameters.clear();
    }

    void addParameter(std::string name, std::string value)
    {
        m_parameters.push_back(new StepParameter(name, value));
    }

    void addParameter(std::string name, std::string value, std::vector<std::string> valueList)
    {
        m_parameters.push_back(new StepParameter(name, value, valueList));
    }

    std::string m_name;
    std::string m_description;
    std::string m_type;
    unsigned long m_executionOrder;
    unsigned long m_delay;
    bool m_skip;

    StepParameterList m_parameters;

    unsigned long m_pkey;
    unsigned long m_planPkey;
};

} //end PV namespace


#endif //!defined(PLAN_VIEWER_COMMON_DEFINITIONS)


