/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/TA_BASE/transactive/bus/scada/wf_global_inhibit/src/WFGlobalInhibitState.h $
  * @author: Liu Yong
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  * 
  * This class provides a read and a write datapoint that represents the WF global 
  * inhibition state
  */

#if !defined(AFX_WFGlobalInhibitState_H__38ABA0F3_A9BE_449C_9AF8_397F6617127D__INCLUDED_)
#define AFX_WFGlobalInhibitState_H__38ABA0F3_A9BE_449C_9AF8_397F6617127D__INCLUDED_

#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	class LocalUpdateEventProcessor;
	class BooleanDataPoint;
	/*
	For WF Global Inhibit state datapoint,
	If the quality is bad, then WF is inhibit.operator no need to execute the WF verification. WF_EXIT
	If The quality is good. and the value is false, then WF is in non inhibit state
		operator should execute the WF verification. 		WF_IN_USE
	If The quality is good. and the value is true, then WF is in inhibit state
		operator should not execute the WF verification. 	WF_EXIT
	*/
	
	class WFGlobalInhibitState  
	{
	public:

		/**
		* getInstance
		* This class is a singleton
		*/
		static WFGlobalInhibitState* getInstance();

		/**
		* DTOR
		*
		*/
		virtual ~WFGlobalInhibitState();

		/**
		* initWFGlobalInhibitState
		*
		* This method will:
		* get the Global Inhibit state from the agent data 
		* and set the related datapoint to the value retrieved
		*/
		void initWFGlobalInhibitState();

		/**
		* isGlobalInhibitDataPoint
		*
		* This method will find the datapoints representing the global inhibit state.
		* @return true if find the datapoint
		*/
		bool isGlobalInhibitDataPoint(TA_Base_Bus::DataPoint* dataPoint);

		/**
		* setGlobalInhibitDataPoint
		*
		* set the WF global inhibit datapoint to the specified state
		*/
		void setGlobalInhibitState(bool applied);

		/**
		* getGlobalInhibit
		*
		* get the WF global inhibit state
		*/
		bool getGlobalInhibitState(bool& wfInhibitState);

		/**
		* getGlobalInhibitKey
		*
		* get the key to the datapoint representing the WF global inhibit state
		*/
		unsigned long getGlobalInhibitKey();


	private: //data members:	
		
		static WFGlobalInhibitState* 					m_instance;
		static TA_Base_Core::ReEntrantThreadLockable	m_lock;

		TA_Base_Bus::BooleanDataPoint*					m_globalInhibitPoint;

	private: //methods:	
		//this class is a singleton
		WFGlobalInhibitState();

	}; //end of class

} //end namespace TA_Base_Bus

#endif // !defined(AFX_WFGlobalInhibitState_H__38ABA0F3_A9BE_449C_9AF8_397F6617127D__INCLUDED_)
