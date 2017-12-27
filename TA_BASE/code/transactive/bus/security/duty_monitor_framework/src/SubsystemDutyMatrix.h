#if !defined(_SUBSYSTEMDUTYMATRIX_H__263034E0_6125_4BE3_8F1F_0CD10648A63A__INCLUDED_)
#define _SUBSYSTEMDUTYMATRIX_H__263034E0_6125_4BE3_8F1F_0CD10648A63A__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/SubsystemDutyMatrix.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class stores the active subsystem duties per region.
  * The duties are stored in the form of a two-dimensional matrix,
  * where rows represent physical subsystems and columns represent regions.
  * Individual elements of the matrix contain a list of subsystem duties, 
  * as well as a boolean value indicating whether the corresponding subsystem 
  * is part of the corresponding region.
  * Where this is set to false, the duty list is always empty.
  *
  */

namespace TA_Base_Bus
{
    class SubsystemDutyMatrix : public TA_Base_Core::Matrix<SubsystemKey, RegionKey, SubsystemDutyMatrixElement>,
                                public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
                                public SeSubject<SubsystemDutyMatrix>
    {
    public:
        SubsystemDutyMatrix();
        virtual ~SubsystemDutyMatrix();

        /**
          * Retrieves from the Duty Agent a snapshot of subsystem duty state and
          * fills the contents of the matrix.
          * Throws a DutyMonitorFrameworkException if relevant config data cannot 
          * be retrieved, or an error occurs while communicating with the agent or
          * while setting elements of the matrix.
          *
          */
        void populate(const SubsystemStateList& subsystemStates, const SubsystemDutyList& subsystemDuties);

		void populateStaticData();

		void populateDynamicData(const SubsystemStateList& subsystemStates, const SubsystemDutyList& subsystemDuties);

        /**
          * Returns the list of relevant row IDs based on subsystem access rights 
          * of the specified profiles. The IDs are sorted in ascending value order.
          * Throws a DutyMonitorFrameworkException if relevant config or session
          * data cannot be retrieved.
          * 
          */
        SubsystemKeyList getRowIdsByRights(const ProfileKeyList& profileKeys);

        /**
          * Returns duty update generated as the result of an Duty Agent comms message.
          * NULL when there is no update to be picked up.
          *
          */
        const SubsystemDutyUpdate* getDutyUpdate();       

        /**
          * TA_Base_Core::SpecialisedMessageSubscriber interface
          *
          */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

	private:
		void updateSubystemDuty( DutyAgentTypeCorbaDef::SubsystemDutySequence& subsystemDutySeq, EDutyUpdateTupe updateType );
		void removeLocationDuty(unsigned long locationKey);
    private:
        SubsystemDutyMatrix(const SubsystemDutyMatrix& SubsystemDutyMatrix);
        SubsystemDutyMatrix& operator=(const SubsystemDutyMatrix& SubsystemDutyMatrix);

 
    private:
        bool m_dutiesLoaded;

        SubsystemDutyUpdate* m_dutyUpdate;
        TA_Base_Core::NonReEntrantThreadLockable m_dutyUpdateLock;        
    };
}

#endif // !defined(_SUBSYSTEMDUTYMATRIX_H__263034E0_6125_4BE3_8F1F_0CD10648A63A__INCLUDED_)
