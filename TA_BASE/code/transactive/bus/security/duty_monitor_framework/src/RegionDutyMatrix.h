#if !defined(_REGIONDUTYMATRIX_H__F0E6DCCC_8415_434D_A32B_673042BC1628__INCLUDED_)
#define _REGIONDUTYMATRIX_H__F0E6DCCC_8415_434D_A32B_673042BC1628__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/RegionDutyMatrix.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class stores for each session the active region duties per region.
  * The duties are stored in the form of a two-dimensional matrix,
  * where rows represent sessions and columns represent regions.
  * Individual elements of the matrix contain a list of region duties.
  *
  */

namespace TA_Base_Bus
{
	class RegionDutyMatrixUpdater;
    class RegionDutyMatrix : public TA_Base_Core::Matrix<SessionId, RegionKey, RegionDutyMatrixElement>,
                             public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
                             public SeSubject<RegionDutyMatrix>							 
    {
    public:
        RegionDutyMatrix();
        virtual ~RegionDutyMatrix();

        /**
          * Retrieves from the Duty Agent a snapshot of region duty state and
          * fills the contents of the matrix.
          * Throws a DutyMonitorFrameworkException if relevant config data cannot 
          * be retrieved, or an error occurs while communicating with the agent or
          * while setting elements of the matrix.
          *
          */
        void populate(const RegionDutyList& regionDuties);

		void populateStaticData();

		void populateDynamicData(const RegionDutyList& regionDuties);
        /**
          * Returns the list of relevant column IDs based on region duties allocated
          * the specified session. The IDs are returned in ascending value order.
          * Throws a MatrixException if the session has no corresponding matrix row.
          *
          */
        RegionKeyList getColumnIdsByDuties(const SessionId& sessionId);

        /**
          * Returns duty update generated as the result of an Duty Agent comms message.
          * NULL when there is no update to be picked up.
          *
          */
        const RegionDutyUpdate* getDutyUpdate();       

        /**
          * TA_Base_Core::SpecialisedMessageSubscriber interface
          *
          */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);


	

    protected:
        /**
          * Processes session updates received from the SessionCache. Only logins and logouts
          * are of interest as they affect the matrix rows.
          *
          */
        void receiveSessionUpdate(const SessionUpdate& sessionUpdate);
        friend class SessionCache;

		void addOneRowToMatrix( const std::string & sessionId, bool fromActiveSessionList );

    private:
        RegionDutyMatrix(const RegionDutyMatrix& regionDutyMatrix);
        RegionDutyMatrix& operator=(const RegionDutyMatrix& regionDutyMatrix);

		//TD15069++
        /**
         * hasSessionEndedInDB
         *
         * Checks HAS_ENDED value in TA_SESSION
         *
         * @param   sessionId  The session Id of the active session.
         * @return  whether the session has ended (TRUE-1) or not (FALSE-0).
         *
         */
        bool hasSessionEndedInDB(const std::string& sessionId, bool& exceptionCaught); //TD15901

		//++TD15069

	public:
		void updateDuty( DutyAgentTypeCorbaDef::RegionDutySequence& regionDutySeq, EDutyUpdateTupe updateType );

		void removeLocationDuty(unsigned long locationKey);
 
    private:
        bool m_dutiesLoaded;

        RegionDutyUpdate* m_dutyUpdate;
        TA_Base_Core::NonReEntrantThreadLockable m_dutyUpdateLock;
		boost::shared_ptr<RegionDutyMatrixUpdater> m_updater;
    };
}

#endif // !defined(_REGIONDUTYMATRIX_H__F0E6DCCC_8415_434D_A32B_673042BC1628__INCLUDED_)
