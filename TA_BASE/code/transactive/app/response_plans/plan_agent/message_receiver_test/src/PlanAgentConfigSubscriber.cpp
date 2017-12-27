/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/message_receiver_test/src/PlanAgentConfigSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "PlanAgentConfigSubscriber.h"
#include <iomanip>



PlanAgentConfigSubscriber::PlanAgentConfigSubscriber()
{
	std::cout << "PlanAgentConfigSubscriber started..." << std::endl;
}

void PlanAgentConfigSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
	std::cout << "Received config Message" << std::endl;
	std::ostringstream updt;

	TA_Base_Core::SessionId session = "";


	TA_Base_Core::PlanConfigUpdateData* configUpdateData=NULL;
	TA_Base_Core::PlanConfigUpdate* configUpdate = NULL;

	if ((message.messageState >>= configUpdateData))
	{
		
		session = configUpdateData->session;

		TA_Base_Core::NodeId plan;
		TA_Base_Core::NodeId category;
		TA_Base_Core::ApprovalStateUpdate* approvalState;
		TA_Base_Core::NodeSummary* nodeSummary = NULL;
		TA_Base_Core::DeleteNodeUpdate* deleteNode = NULL;
		TA_Base_Core::TransferNodeUpdate* xferNode = NULL;


		configUpdate = &(configUpdateData->configUpdate);


		switch (configUpdate->_d())
		{
		case TA_Base_Core::PCUT_APPROVAL_STATE:
			approvalState = &(configUpdate->approvalState());
			plan = approvalState->plan;

			if ( approvalState->approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE)
			{
				std::cout << "Plan approved: " << plan << std::endl;
			}
			else if (approvalState->approvalState == TA_Base_Core::UNAPPROVED_APPROVAL_STATE)
			{
				std::cout << "Plan unapproved: " << plan << std::endl;
			}
			else
				std::cout << "Undefined approvalstate" << std::endl;

			break;

		case TA_Base_Core::PCUT_UPDATE_CATEGORY:
			std::cout << "PCUT_UPDATE_CATEGORY config message received." << std::endl;
			nodeSummary = &(configUpdate->updateCategory());
			
			updt << "Parent: " << nodeSummary->node << " Category: " << nodeSummary->name.in(); 

			std::cout << updt.str().c_str() << std::endl;
			break;

		case TA_Base_Core::PCUT_INSERT_CATEGORY:
			std::cout << "PCUT_INSERT_CATEGORY config message received." << std::endl;
			nodeSummary = &(configUpdate->insertCategory());
			
			updt << "Parent: " << nodeSummary->parent << " Category: " << nodeSummary->name.in();

			std::cout << updt.str().c_str() << std::endl;
			break;

		case TA_Base_Core::PCUT_INSERT_CATEGORY_INTO_ROOT:
			std::cout << "PCUT_INSERT_CATEGORY config message received." << std::endl;
			nodeSummary = &(configUpdate->insertCategoryIntoRoot());
			
			updt << "Parent: " << "ROOT" << " Category: " << nodeSummary->name.in();

			std::cout << updt.str().c_str() << std::endl;
			break;


		case TA_Base_Core::PCUT_DELETE_CATEGORY:
			{
				deleteNode = &(configUpdate->deleteCategory());
				TA_Base_Core::NodeId parent = deleteNode->parent;
				int Len = deleteNode->nodes.length();
				category = (deleteNode->nodes)[0];
				std::cout << "DELETE_CATEGORY config message received for category: " << category << std::endl;
			}
			break;

		case TA_Base_Core::PCUT_DELETE_CATEGORY_FROM_ROOT:
			{
				category = configUpdate->deleteCategoryFromRoot();
				std::cout << "DELETE_CATEGORY_FROM_ROOT config message received for category: " << category << std::endl;
			}
			break;


		case TA_Base_Core::PCUT_UPDATE_PLAN:
			std::cout << "PCUT_UPDATE_PLAN config message received." << std::endl;
			break;

		case TA_Base_Core::PCUT_INSERT_PLAN:
			std::cout << "PCUT_INSERT_PLAN config message received." << std::endl;
			break;

		case TA_Base_Core::PCUT_DELETE_PLANS:
			{
				deleteNode = &(configUpdate->deletePlans());
				for (unsigned int i=0; i < deleteNode->nodes.length(); i++ )
				{
					plan = deleteNode->nodes[i];						
					std::cout << "PCUT_DELETE_PLAN:Plan:" << plan << " Parent: " << deleteNode->parent << std::endl;
				}
			}
			break;

		case TA_Base_Core::PCUT_MOVE_PLAN:
			xferNode = &(configUpdate->movePlan());
			std::cout << "PCUT_MOVE_PLAN: Plan " << xferNode->summary.name.in() << " OldParent: "
				<< xferNode->oldParent << " New Parent: " << xferNode->summary.parent << "." << std::endl;

			break;

		case TA_Base_Core::PCUT_MOVE_PLAN_FROM_ROOT:
			xferNode = &(configUpdate->movePlanFromRoot());
			std::cout << "PCUT_MOVE_PLAN_FROM_ROOT: Plan " << xferNode->summary.name.in() << " OldParent: "
				<< xferNode->oldParent << " New Parent: " << xferNode->summary.parent << "." << std::endl;

			break;

		case TA_Base_Core::PCUT_MOVE_PLAN_TO_ROOT:
			xferNode = &(configUpdate->movePlanToRoot());
			std::cout << "PCUT_MOVE_PLAN_TO_ROOT: Plan " << xferNode->summary.name.in() << " OldParent: "
				<< xferNode->oldParent << " New Parent: " << xferNode->summary.parent << "." << std::endl;

			break;

		case TA_Base_Core::PCUT_COPY_PLAN:
			std::cout << "PCUT_COPY_PLAN config message received." << std::endl;
			break;


		default:
			std::cout << "Undefined Discriminator" << std::endl;			
		}
	}
}

