// RegionDutyMatrixUpdater.h: interface for the RegionDutyMatrixUpdater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGIONDUTYMATRIXUPDATER_H__4E7AA910_E697_4071_8691_59D9C42467CC__INCLUDED_)
#define AFX_REGIONDUTYMATRIXUPDATER_H__4E7AA910_E697_4071_8691_59D9C42467CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
	class RegionDutyMatrixUpdater: public ACE_Task<ACE_MT_SYNCH>  
	{
	public:
		RegionDutyMatrixUpdater(RegionDutyMatrix& matrix);
		virtual ~RegionDutyMatrixUpdater();
		
		virtual int open(void*);
		/**
		* @param flags
		* 
		*/
		virtual int close(u_long flags = 0 );
		/**
		* svc  This is the entry point for the callback from ACE_Task.
		* 
		*/
		virtual int svc(void);

		void updateSession(const std::string& sessionId, bool deleteFlag);
		void removeLocationDuty(unsigned long locationKey);

		void updateDuty(DutyAgentTypeCorbaDef::RegionDutySequence& regionDutySeq, EDutyUpdateTupe udpateType);
	private:
		ACE_Activation_Queue m_activationQueue;
		RegionDutyMatrix& m_matrix;
		
		static const ACE_Time_Value m_mqTimeout;
		static const ACE_Time_Value m_aqTimeout;
		
		static const unsigned long THREAD_COUNT;
	};
}
#endif // !defined(AFX_REGIONDUTYMATRIXUPDATER_H__4E7AA910_E697_4071_8691_59D9C42467CC__INCLUDED_)
