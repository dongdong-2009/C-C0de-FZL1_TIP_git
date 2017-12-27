#if !defined(_SUBSYSTEMDUTYMATRIXELEMENT_H__B29B0A1A_139D_465D_B4CA_D4727B8A33F7__INCLUDED_)
#define _SUBSYSTEMDUTYMATRIXELEMENT_H__B29B0A1A_139D_465D_B4CA_D4727B8A33F7__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/SubsystemDutyMatrixElement.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is a representation of a single element of the subsystem duty matrix.
  * It is associated with a single subsystem in a single region.
  * It contains a boolean value indicating whether the subsystem belongs to
  * the region. It also contains a list of duties for the subsystem. 
  * If the subsystem is not part of the region, the duty list is always empty.
  *
  */

namespace TA_Base_Bus
{
    class SubsystemDutyMatrixElement
    {
    public:
        /**
          * Standard constructor
          *
          */
        SubsystemDutyMatrixElement();

        /**
          * Copy constructor
          *
          */
        SubsystemDutyMatrixElement(const SubsystemDutyMatrixElement& subsystemDutyMatrixElement);

        /**
          * Destructor
          *
          */
        virtual ~SubsystemDutyMatrixElement();

        /**
          * Assignment operator
          *
          */
        SubsystemDutyMatrixElement& operator=(const SubsystemDutyMatrixElement& subsystemDutyMatrixElement);

        /**
          * Returns true if subsystem belongs to the region, and false otherwise.
          *
          */
        bool isSubsystemInRegion() const;

        /**
          * Returns true if the list of active subsystem duties for the subsystem is not empty.
          *
          */
        bool dutiesExist() const;

        /**
          * Returns the list of all active subsystem duties in the subsystem.
          *
          */
        SubsystemDutyList getSubsystemDuties() const;

        /**
          * Returns the list of active subsystem duties for the specified session in the subsystem.
          *
          */
        SubsystemDutyList getSubsystemDuties(const SessionId& sessionId) const;

        /**
          * Sets the indicator showing if the subsystem belongs to the region.
          * If true, the subsystem duty list is purged.
          *
          */
        void setSubsystemInRegion(bool isSubsystemInRegion);

        /**
          * Adds the specified subsystem duty to the list, provided it is not already in there.
          * Returns true to indicate that the duty has been added, and false otherwise.
          * 
          */
        bool addDuty(const DutyAgentTypeCorbaDef::SubsystemDuty& subsystemDuty);

        /**
          * Removes the specified subsystem duty from the list, provided it is found.
          * Returns true to indicate that the duty has been removed, and false otherwise.
          * 
          */
        bool removeDuty(const DutyAgentTypeCorbaDef::SubsystemDuty& subsystemDuty);

        /**
          * Sets the state of the subsystem represented by this element.
          *
          */
        void setSubsystemState(const ESubsystemState subsystemState);

        /**
          * Gets the state of the subsystem represented by this element.
          *
          */
        ESubsystemState getSubsystemState() const;

		void clearDuty();

    private:
        /**
          * Compares two subsystem duties to determine their sort order in the SubsystemDutySet.
          *
          */
        struct SubsystemDutyComparator
        {
            bool operator()(const DutyAgentTypeCorbaDef::SubsystemDuty& duty1, 
                            const DutyAgentTypeCorbaDef::SubsystemDuty& duty2) const
            {
                int sessionCompareRes = ::strcmp(duty1.session.in(), duty2.session.in());

                if (sessionCompareRes == 0)
                {
                    return duty1.profileKey < duty2.profileKey;
                }

                return sessionCompareRes < 0;
            }
        };

        //typedef std::set<DutyAgentTypeCorbaDef::SubsystemDuty, SubsystemDutyComparator> SubsystemDutySet;
		typedef std::list< DutyAgentTypeCorbaDef::SubsystemDuty > SubsystemDutySet;


    private:
        bool m_isSubsystemInRegion;
        ESubsystemState m_subsystemState;
        SubsystemDutySet m_subsystemDuties;
    };

}

#endif // !defined(_SUBSYSTEMDUTYMATRIXELEMENT_H__B29B0A1A_139D_465D_B4CA_D4727B8A33F7__INCLUDED_)
