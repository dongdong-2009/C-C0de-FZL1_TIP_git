#ifndef _IDECISIONSTEP_H__
#define _IDECISIONSTEP_H__

namespace TA_Base_App
{
    class IDecisionStep
    {
    public:
        virtual ~IDecisionStep(){}
        virtual TA_Base_Core::StepNumber getNextDecisionStep(const int status) = 0;
    };
}

#endif