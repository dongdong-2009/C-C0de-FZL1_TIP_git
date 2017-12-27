#if defined ( STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM )

#ifndef ACTIVETRAINTISSTEP_H_INCLUDED
#define ACTIVETRAINTISSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Bart Golab
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Represents a Train Traveller Info System step in an executing plan.
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveTrainTisStep : public ActiveStep
	{
	// Public methods
	public:
        ActiveTrainTisStep(const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::TrainTravellerInfoSystemParameters& params, IActivePlanFlow& flowControl);

	protected:
        virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
        ActiveTrainTisStep& operator=( const ActiveTrainTisStep& );
        ActiveTrainTisStep( const ActiveTrainTisStep& );

        const TA_Base_Core::TrainTravellerInfoSystemParameters m_params;
	}; // ActiveTrainTisStep
}; // TA_Base_App

#endif // ACTIVETRAINTISSTEP_H_INCLUDED

#endif //  STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM
