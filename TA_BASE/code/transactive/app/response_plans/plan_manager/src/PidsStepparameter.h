/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PidsStepParameter.h $
* @author:  Bart Golab
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2010/06/09 22:25:17 $
* Last modified by:  $Author: qi.huang $
*
* Represents the Station parameter of a Station TIS/PA step.
*
**/

#if !defined(AFX_PIDSSTEPPARAMETER_H__2A75EF19_8FC6_404c_884D_B4878B37D72B__INCLUDED_)
#define AFX_PIDSSTEPPARAMETER_H__2A75EF19_8FC6_404c_884D_B4878B37D72B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// PidsStepParameter

	class PidsStepParameter : public PlanStepParameter
	{
		// Operations
	public:
		PidsStepParameter(PlanStep& parent, const std::string& name);
		PidsStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::StationTravellerInfoSystemParameters& stationtinfo);
		PidsStepParameter(PlanStep& parent, const PidsStepParameter& parameter);
		PidsStepParameter(const PidsStepParameter& parameter);
		virtual ~PidsStepParameter();

		virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

		TA_Base_Core::StationTravellerInfoSystemParameters getStationTravelInfo() const { return m_travelinfo; }
		void setStationTravellerInfo(const TA_Base_Core::TISPidList& pidlist, 
									 bool msgtype,
									 bool showtype,
									 std::wstring msgcontent);

		//argument 1 is refenrence type,value transmited will be modefied
		void setPidsDestinationList(TA_Base_Core::PidsDestinationList& destinationList,
									const TA_Base_Core::TISPidList& pidlist
									);

		void setStationTravellerInfo(const TA_Base_Core::StationTravellerInfoSystemParameters& stationtinfo);
	protected:
		virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

	private:
		PidsStepParameter& operator=(const PidsStepParameter& parameter);


		// Attributes
	private:
		TA_Base_Core::StationTravellerInfoSystemParameters m_travelinfo;
	};
}

#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#endif // !defined(AFX_PIDSSTEPPARAMETER_H__2A75EF19_8FC6_404c_884D_B4878B37D72B__INCLUDED_)
