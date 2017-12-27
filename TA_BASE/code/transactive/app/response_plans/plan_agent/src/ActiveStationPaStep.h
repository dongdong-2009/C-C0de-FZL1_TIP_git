#if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS )

#ifndef ACTIVE_STATION_PUBLIC_ADDRESS_STEP_HEADER_INCLUDED
#define ACTIVE_STATION_PUBLIC_ADDRESS_STEP_HEADER_INCLUDED

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
    class ActiveStationPaStep : public ActiveStep
    {
    // Public methods
    public:
        ActiveStationPaStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::StationPublicAddressParameters& params, IActivePlanFlow& flowControl );
        
    protected:
        virtual TA_Base_Core::EActiveStepState executeImpl( void );

    private:
        ActiveStationPaStep& operator=( const ActiveStationPaStep& );
        ActiveStationPaStep( const ActiveStationPaStep& );

        const TA_Base_Core::StationPublicAddressParameters m_params;        
    }; // ActiveStationPaStep
}; // TA_Base_App

#endif // ACTIVE_STATION_PUBLIC_ADDRESS_STEP_HEADER_INCLUDED

#endif //  STEPTYPE_STATION_PUBLIC_ADDRESS
