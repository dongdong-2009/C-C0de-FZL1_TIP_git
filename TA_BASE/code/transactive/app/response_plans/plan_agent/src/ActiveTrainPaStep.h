#if defined ( STEPTYPE_TRAIN_PUBLIC_ADDRESS )

#ifndef ACTIVE_TRAIN_PUBLIC_ADDRESS_STEP_HEADER_INCLUDED
#define ACTIVE_TRAIN_PUBLIC_ADDRESS_STEP_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Mahaveer Pareek
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
    //Class declaration
    class ActiveTrainPaStep : public ActiveStep
    {
    // Public methods
    public:
        ActiveTrainPaStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::TrainPublicAddressParameters& params, IActivePlanFlow& flowControl );
        
    protected:
        virtual TA_Base_Core::EActiveStepState executeImpl( void );

    private:
        ActiveTrainPaStep& operator=( const ActiveTrainPaStep& );
        ActiveTrainPaStep( const ActiveTrainPaStep& );

        const TA_Base_Core::TrainPublicAddressParameters m_params;
    }; // ActiveTrainPaStep
}; // TA_Base_App

#endif // ACTIVE_TRAIN_PUBLIC_ADDRESS_STEP_HEADER_INCLUDED

#endif //  STEPTYPE_TRAIN_PUBLIC_ADDRESS
