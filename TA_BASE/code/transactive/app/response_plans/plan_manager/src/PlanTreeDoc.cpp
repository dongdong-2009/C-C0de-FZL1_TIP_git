//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DataConversion.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "bus/mfc_extensions/src/message_box/XMessageBox.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
//TD17888 marc_bugfix for save as dialog
#include "bus/mfc_extensions/src/file_dialog_no_folder_change/FileDialogArialDisableFolders.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/PlanManagerEntityData.h"
#include "PlanManager.h"
#include "PlanTreeDoc.h"
#include "TreeNode.h"
#include "PlanNode.h"
#include "CategoryNode.h"
#include "RootNode.h"
#include "PlanStep.h"
#include "MessageBox.h"
#include "PlanAgentUpdateProcessor.h"
#include "PlanManagerCommonDefs.h"

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "ChangeImportPlanName.h"

#include "PlanAgentAccess.h"

// CL 15087 (3) ++
#include "core/message/types/PlanAgentAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
// CL 15087 (3) ++
#include "core/utilities/src/DateFormat.h" // TD12474
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeDoc

IMPLEMENT_DYNCREATE(CPlanTreeDoc, CPlanBrowserDoc)

BEGIN_MESSAGE_MAP(CPlanTreeDoc, CPlanBrowserDoc)
    // {{AFX_MSG_MAP(CPlanTreeDoc)
    ON_UPDATE_COMMAND_UI(ID_PLAN_EDIT, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_CONTROL, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_APPROVE, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_UNAPPROVE, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_SCHEDULE, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_NEW, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_DELETE, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_CUT, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_COPY, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_PASTE, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_CATEGORY_NEW, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_CATEGORY_DELETE, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_CATEGORY_RENAME, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_CATEGORY_OR_PLAN_DELETE, OnUpdateTreeNodeCmd)
     ON_UPDATE_COMMAND_UI(ID_IMPORT_PLAN, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_EXPORT_PLAN, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_VISIBLE, OnUpdateTreeNodeCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_INVISIBLE, OnUpdateTreeNodeCmd)
    ON_COMMAND(ID_PLAN_EDIT, OnEditPlan)
    ON_COMMAND(ID_PLAN_CONTROL, OnControlPlan)
    ON_COMMAND(ID_PLAN_APPROVE, OnApprovePlan)
    ON_COMMAND(ID_PLAN_UNAPPROVE, OnUnapprovePlan)
    ON_COMMAND(ID_PLAN_SCHEDULE, OnSchedulePlan)
    ON_COMMAND(ID_PLAN_NEW, OnNewPlan)
    ON_COMMAND(ID_PLAN_DELETE, OnDeletePlan)
    ON_COMMAND(ID_PLAN_CUT, OnCutPlan)
    ON_COMMAND(ID_PLAN_COPY, OnCopyPlan)
    ON_COMMAND(ID_PLAN_PASTE, OnPastePlan)
    ON_COMMAND(ID_CATEGORY_NEW, OnNewCategory)
    ON_COMMAND(ID_CATEGORY_DELETE, OnDeleteCategory)
    ON_COMMAND(ID_CATEGORY_OR_PLAN_DELETE, OnDeleteCategoryOrPlan)
    ON_COMMAND(ID_STEP_EDIT, OnEditStep)
    ON_COMMAND(ID_PLAN_VISIBLE, OnMakePlanVisible)
    ON_COMMAND(ID_PLAN_INVISIBLE, OnMakePlanInvisible)
    ON_COMMAND(ID_IMPORT_PLAN, OnImportPlan)
    ON_COMMAND(ID_EXPORT_PLAN, OnExportPlan)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeDoc construction/destruction

CPlanTreeDoc::CPlanTreeDoc() : m_copyNodeAction(CNA_NO_ACTION)
{
    FUNCTION_ENTRY( "CPlanTreeDoc" );

    // TODO: add one-time construction code here

    FUNCTION_EXIT;
}


CPlanTreeDoc::~CPlanTreeDoc()
{
    FUNCTION_ENTRY( "~CPlanTreeDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanTreeDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    if (!CPlanBrowserDoc::OnNewDocument())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanTreeDoc::OnGenericGuiInitCompleted()
{
    FUNCTION_ENTRY( "OnGenericGuiInitCompleted" );

    // Register for active plan, plan customisation and plan configuration updates.
    PlanAgentUpdateProcessor::getInstance().subscribeForActivePlanUpdates(this);
    PlanAgentUpdateProcessor::getInstance().subscribeForPlanCustomisationUpdates(this);
    PlanAgentUpdateProcessor::getInstance().subscribeForPlanConfigUpdates(this);

    // Register for ExecPlanId, LockCategory and SessionId runtime parameter updates.
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_FWDEXECPLANID);
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_FWDVIEWPLANID);
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_LOCKCATEGORY);
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_FWDASSOCALARMID);

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnCloseDocument()
{
    FUNCTION_ENTRY( "OnCloseDocument" );

    // TODO: Add your specialized code here and/or call the base class
    PlanAgentUpdateProcessor::getInstance().unsubscribeFromActivePlanUpdates(this);
    PlanAgentUpdateProcessor::getInstance().unsubscribeFromPlanCustomisationUpdates(this);
    PlanAgentUpdateProcessor::getInstance().unsubscribeFromPlanConfigUpdates(this);

    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);

    CPlanBrowserDoc::OnCloseDocument();

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeDoc diagnostics

#ifdef _DEBUG
void CPlanTreeDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CPlanBrowserDoc::AssertValid();

    FUNCTION_EXIT;
}


void CPlanTreeDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CPlanBrowserDoc::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeDoc commands

void CPlanTreeDoc::OnUpdateTreeNodeCmd(CCmdUI* pCmdUI)
{
    FUNCTION_ENTRY( "OnUpdateTreeNodeCmd" );

    switch (pCmdUI->m_nID)
    {
        case ID_PLAN_PASTE:
        {
            if (m_copyNodeAction == CNA_NO_ACTION)
            {
                pCmdUI->Enable(FALSE);
                break;
            }
            // drop through for normal processing
        }

        default:
        {
            try
            {
                // All selected nodes should make a collective decision whether to enable the command.
                // The command can be enabled only if all nodes agree to it. If the sole selected node
                // is the current plan tree node, then the decision rests with this one node.
                CCmdUIProbe cmdUIProbe(pCmdUI->m_nID);
                TreeNodeList selectedNodes = GetSelectedNodes();

                for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
                {
                    TreeNode *treeNode = *iter;
                    treeNode->updateCmdUI(cmdUIProbe);
                }

                pCmdUI->Enable(cmdUIProbe.m_enabled);
            }
            catch (...)
            {
                pCmdUI->Enable(FALSE);
            }
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnImportPlan ()
{
    FUNCTION_ENTRY( "OnImportPlan" );

    LPCTSTR defFileExt = ".pln";
    DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY;
    LPCTSTR fileFilter = "Plan Files (*.pln)|*.pln|";
    // TD17888 marc_bugfix
    // CFileDialog impFileDlg (TRUE, defFileExt, NULL, dwFlags, fileFilter, NULL);
    TA_Base_Bus::FileDialogArialDisableFolders impFileDlg (TRUE, defFileExt, NULL, dwFlags, fileFilter, NULL);
    CWaitCursor waitCursor;
    CString m_DefaultExportDirectory;
    TCHAR szMaxpath[MAX_PATH];


    TA_Base_Core::IEntityData* planManagerEntityData =
        TA_Base_Core::EntityAccessFactory::getInstance().getEntity("PlanManager");

    TA_Base_Core::PlanManagerEntityData *planEntityData = dynamic_cast<TA_Base_Core::PlanManagerEntityData *>(planManagerEntityData);

    m_DefaultExportDirectory = planEntityData->getExportDirectory().c_str();

    impFileDlg.m_ofn.lpstrTitle = "Import Selected Files";

    SHGetSpecialFolderPath(NULL, szMaxpath, CSIDL_PERSONAL, FALSE);

    if (m_DefaultExportDirectory.IsEmpty())
    {
        impFileDlg.m_ofn.lpstrInitialDir = szMaxpath;
    }
    else
    {
        CString fileName = "Untitled";
        CFile outFile;

        if ( 0 == outFile.Open(m_DefaultExportDirectory + fileName, CFile::modeCreate | CFile::modeReadWrite) )
        {
            impFileDlg.m_ofn.lpstrInitialDir = szMaxpath;
        }
        else
        {
            // remove the file if it is created
            if (outFile != NULL)
            {
                outFile.Close();
                CFile::Remove(m_DefaultExportDirectory + fileName);
            }
            impFileDlg.m_ofn.lpstrInitialDir = m_DefaultExportDirectory;
        }
    }

    if ( impFileDlg.DoModal() == IDOK)
    {
        // for every selected plan file
        // import plan details into newly
        // created plan nodes
        waitCursor.Restore ();
        CategoryNode* categoryNode = dynamic_cast<CategoryNode*>(GetCurrentNode());
        POSITION pos = impFileDlg.GetStartPosition ();
        if (pos != NULL)
        {
            do
            {
                CString impFilePath = impFileDlg.GetNextPathName(pos);
                try
                {
                    TA_Base_Core::AuxPlanDetail_var   plan = new TA_Base_Core::AuxPlanDetail;

                    // read plan detail from user-selected file
                    // and store it in plan
                    CFile inFile;
                    if ( inFile.Open(impFilePath, CFile::modeRead) )
                    {
                        // Variables used repeatedly to read each piece of information to the file.
                        DWORD nBufSize = inFile.GetLength();
                        char *lpBuffer = new char[nBufSize];
						memset(lpBuffer, 0, nBufSize);

                        inFile.Read (lpBuffer, nBufSize);
                        CString fileBuffer (lpBuffer);
                        inFile.Close();
                        delete []lpBuffer;

                        // find indicies of all new line characters in the file
                        CUIntArray newLineIdxArray;
                        int startFrom = 0;
                        while (true)
                        {
                            startFrom = fileBuffer.Find ("\n", startFrom);
                            if (startFrom >= 0)
                            {
                                newLineIdxArray.Add (startFrom);
                                startFrom++;
                            }
                            else
                            {
                                break;
                            }
                        }

                        CString currentLine, tmp;
                        CUIntArray newDetailPos;
                        for (int curLine = 0; curLine < newLineIdxArray.GetSize (); curLine++)
                        {
                            tmp = fileBuffer.Right (fileBuffer.GetLength () - newLineIdxArray.GetAt (curLine) - 1);
                            currentLine = tmp.Left (tmp.Find ("\n"));

                            if (currentLine.Compare ("Plan_Details") == 0)
                            {
                                curLine++;
                                tmp = fileBuffer.Right (fileBuffer.GetLength () - newLineIdxArray.GetAt (curLine) - 1);
                                currentLine = tmp.Left (tmp.Find ("\n"));
                                startFrom = 0;
                                while (true)
                                {
                                    startFrom = currentLine.Find ("==", startFrom);
                                    if (startFrom >= 0)
                                    {
                                        newDetailPos.Add (startFrom);
                                        startFrom++;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }

                                for (int curValue = 0; curValue < newDetailPos.GetSize (); curValue++)
                                {
                                    tmp = currentLine.Right (currentLine.GetLength () - newDetailPos.GetAt (curValue) - 2);//2 is for the length of delimiter
                                    std::string tmpValue = tmp.Left (tmp.Find (';')).GetBuffer (1);
                                    switch (curValue)
                                    {
                                    case 0:
                                        {
                                            plan->name = CORBA::string_dup (tmpValue.c_str ());
                                            // need to check if the plan with the same name already exists
                                            // in the curent category
                                            TreeNodeList childNodes;
                                            categoryNode->getChildren(childNodes);

                                            for (TreeNodeList::iterator iter = childNodes.begin(); iter != childNodes.end(); iter++)
                                            {
                                                TreeNode *childNode = (TreeNode *) *iter;
                                                if (tmpValue.compare (childNode->getNodeName ()) == 0)
                                                {
                                                    CString msg;
                                                    msg.Format ("The selected plan cannot be imported as the plan - %s already exists in the selected category! Please enter a new name.", tmpValue.c_str ());
                                                    CChangeImportPlanName dlg (*categoryNode, msg);
                                                    if (dlg.DoModal () == IDOK)
                                                    {
                                                        plan->name = CORBA::string_dup (dlg.GetNewPlanName ());
                                                        waitCursor.Restore ();
                                                    }
                                                    else
                                                    {
                                                        FUNCTION_EXIT;
                                                        return;//if they cancel do not import any of the plans
                                                    }
                                                }
                                            }
                                            break;
                                        }
                                    case 1:
                                        plan->description = CORBA::string_dup (tmpValue.c_str ());
                                        break;
                                    default:
                                        break;
                                    }
                                }
                            }
                            else if (currentLine.Compare ("Steps") == 0)
                            {
                                curLine++;
                                // get number of steps
                                tmp = fileBuffer.Right (fileBuffer.GetLength () - newLineIdxArray.GetAt (curLine) - 1);
                                currentLine = tmp.Left (tmp.Find ("\n"));
                                int numSteps = atoi (currentLine.GetBuffer (1));

                                // for each step store the details
                                plan->steps.length (numSteps);
                                for (int step = 0; step < numSteps; step++)
                                {
                                    curLine++;
                                    CUIntArray newStepDetailPos;
                                    tmp = fileBuffer.Right (fileBuffer.GetLength () - newLineIdxArray.GetAt (curLine) - 1);
                                    currentLine = tmp.Left (tmp.Find ("\n"));
                                    startFrom = 0;
                                    while (true)
                                    {
                                        startFrom = currentLine.Find ("==", startFrom);
                                        if (startFrom >= 0)
                                        {
                                            newStepDetailPos.Add (startFrom);
                                            startFrom++;
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }

                                    // step details
                                    for (int curValue = 0; curValue < newStepDetailPos.GetSize (); curValue++)
                                    {
                                        tmp = currentLine.Right (currentLine.GetLength () - newStepDetailPos.GetAt (curValue) - 2);
                                        std::string tmpValue = tmp.Left (tmp.Find (';')).GetBuffer (1);
                                        switch (curValue)
                                        {
                                        case 0:
                                            plan->steps[step].name = CORBA::string_dup (tmpValue.c_str ());
                                            break;
                                        case 1:
                                            plan->steps[step].type = static_cast <TA_Base_Core::EStep> (atol (tmpValue.c_str ()));
                                            break;
                                        case 2:
                                            plan->steps[step].description = CORBA::string_dup (tmpValue.c_str ());
                                            break;
                                        case 3:
                                            plan->steps[step].position = static_cast <TA_Base_Core::StepNumber> (atol (tmpValue.c_str ()));
                                            break;
                                        case 4:
                                            if (tmpValue.compare ("0") == 0)
                                            {
                                                plan->steps[step].skip = false;
                                            }
                                            else
                                            {
                                                plan->steps[step].skip = true;
                                            }
                                            break;
                                        case 5:
                                            if (tmpValue.compare ("0") == 0)
                                            {
                                                plan->steps[step].skippable = false;
                                            }
                                            else
                                            {
                                                plan->steps[step].skippable = true;
                                            }
                                            break;
                                        case 6:
                                            if (tmpValue.compare ("0") == 0)
                                            {
                                                plan->steps[step].ignoreFailure = false;
                                            }
                                            else
                                            {
                                                plan->steps[step].ignoreFailure = true;
                                            }
                                            break;
                                        case 7:
                                            if (tmpValue.compare ("0") == 0)
                                            {
                                                plan->steps[step].nowait = false;
                                            }
                                            else
                                            {
                                                plan->steps[step].nowait = true;
                                            }
                                            break;
                                        case 8:
                                            plan->steps[step].delay = static_cast <TA_Base_Core::Seconds> (atol (tmpValue.c_str ()));
                                        default:
                                            break;
                                        }
                                    }

                                    // step parameters
                                    curLine++;
                                    tmp = fileBuffer.Right (fileBuffer.GetLength () - newLineIdxArray.GetAt (curLine) - 1);
                                    currentLine = tmp.Left (tmp.Find ("\n"));
                                    if (currentLine.Compare ("Step_Parameters") == 0)
                                    {
                                        curLine++;
                                    }
                                    else
                                    {//error reading .pln file
                                    TA_Base_Bus::TransActiveMessage userMsg;
                                    userMsg << std::string (impFilePath.GetBuffer (1));
                                    userMsg.showMsgBox(IDS_UE_210119);
									// CL 15087 (3) ++
									{
										std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
										std::ostringstream strOut;
										strOut << "Imported Pos:";
										CategoryNode* parentNode = dynamic_cast<CategoryNode*>(categoryNode->getParentNode());
										std::vector<std::string> vecPlanPath;
										while (parentNode)
										{
											vecPlanPath.push_back(parentNode->getNodeName());
											parentNode = dynamic_cast<CategoryNode*>(parentNode->getParentNode());
										}
										for( int i = vecPlanPath.size() - 1; i >= 0; i--)
										{
											strOut << vecPlanPath[i] << "/";
										}
										vecPlanPath.clear();
										strOut << categoryNode->getNodeName() << " File:" 
											<< impFilePath.GetBuffer(1) << " Fail";
										
										TA_Base_Core::NameValuePair nvp0("Value", "Fail" );
										TA_Base_Core::NameValuePair nvp1("AuditMessage",strOut.str() );
										TA_Base_Core::DescriptionParameters dp;
										dp.push_back(&nvp0);
										dp.push_back(&nvp1);
										TA_Base_Core::AuditMessageSender *messageSender = 
											TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
											TA_Base_Core::PlanAgentAudit::Context );
										
										//TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
										unsigned long entityKey = planManagerEntityData->getKey();
										messageSender->sendAuditMessage(TA_Base_Core::PlanAgentAudit::PlanImportExport,
											entityKey, dp, "", session, "", "", "");
										delete messageSender;
									}
									// ++ CL 15087 (3)
                                    return;
                                    }

                                    tmp = fileBuffer.Right (fileBuffer.GetLength () - newLineIdxArray.GetAt (curLine) - 1);
                                    currentLine = tmp.Left (tmp.Find ("\n"));
                                    int numParams = atoi (currentLine.GetBuffer (1));
                                    plan->steps[step].stepParams.length (numParams);

                                    curLine++;
                                    CUIntArray newParamDetailPos;
                                    tmp = fileBuffer.Right (fileBuffer.GetLength () - newLineIdxArray.GetAt (curLine) - 1);
                                    currentLine = tmp.Left (tmp.Find ("\n"));
                                    startFrom = 0;
                                    while (true)
                                    {
                                        startFrom = currentLine.Find ("==", startFrom);
                                        if (startFrom >= 0)
                                        {
                                            newParamDetailPos.Add (startFrom);
                                            startFrom++;
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }

                                    for (int param = 0; param < numParams; param++)
                                    {
                                        for (int curValue = 0; curValue < newParamDetailPos.GetSize (); curValue++)
                                        {
                                            tmp = currentLine.Right (currentLine.GetLength () - newParamDetailPos.GetAt (curValue) - 2);
                                            std::string tmpValue = tmp.Left (tmp.Find (';')).GetBuffer (1);

                                            switch (curValue)
                                            {
                                            case 0:
                                                plan->steps[step].stepParams[param].position = static_cast <TA_Base_Core::ParameterNumber> (atol ((tmpValue.c_str ())));
                                                break;
                                            case 1:
                                                plan->steps[step].stepParams[param].value = CORBA::string_dup (tmpValue.c_str ());
                                                break;
                                            default:
                                                break;
                                            }
                                        }
                                        if (param != numParams - 1)
                                        {
                                            curLine++;
                                            tmp = fileBuffer.Right (fileBuffer.GetLength () - newLineIdxArray.GetAt (curLine) - 1);
                                            currentLine = tmp.Left (tmp.Find ("\n"));
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // update the DB with new plan details
                    categoryNode->importPlan (plan);

					// CL 15087 (3) ++
					// sending audit message for import plan action
					{
						// get plan path
						//15087 shengguang++
						std::ostringstream strOut;
						std::ostringstream planName;
						strOut <<"Plan ";
						//++shengguang
						CategoryNode* parentNode = dynamic_cast<CategoryNode*>(categoryNode->getParentNode());
						std::vector<std::string> vecPlanPath;
						while (parentNode)
						{
							vecPlanPath.push_back(parentNode->getNodeName());
							parentNode = dynamic_cast<CategoryNode*>(parentNode->getParentNode());
						}
						for( int i = vecPlanPath.size() - 1; i >= 0; i--)
						{
							//strOut << vecPlanPath[i] << "/";
							//15087 shengguang++
							planName << vecPlanPath[i] << "/";
						}
						vecPlanPath.clear();
						planName << categoryNode->getNodeName() << "/" 
							<< plan->name.in();
						//15087 shengguang++
						strOut<<planName.str()<<" is successfully "<<"Import at ";
						
						SYSTEMTIME datatime; 
						GetLocalTime(&datatime);
						strOut<<datatime.wDay<<"-";
						strOut<<datatime.wMonth<<"-";
						strOut<<datatime.wYear<<" ";
						strOut<<datatime.wHour<<":";
						strOut<<datatime.wMinute<<":";
						strOut<<datatime.wSecond;
						//++shengguang
						// create descriptionParamenters
						TA_Base_Core::NameValuePair nvp0("Value", "Successful" );
						TA_Base_Core::NameValuePair nvp1("AuditMessage",strOut.str() );
						std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
						TA_Base_Core::DescriptionParameters dp;
						dp.push_back(&nvp0);
						dp.push_back(&nvp1);
						TA_Base_Core::AuditMessageSender *messageSender = 
							TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
							TA_Base_Core::PlanAgentAudit::Context );
						
						//TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
						unsigned long entityKey = planManagerEntityData->getKey();
						messageSender->sendAuditMessage(TA_Base_Core::PlanAgentAudit::PlanImportExport,
							entityKey, dp, "", session, "", "", "");
						delete messageSender;
					}
					// ++ CL 15087 (3)

				}
				catch (TA_Base_Core::TransactiveException&)
				{
					// CL 15087 (3) ++
					{
					std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
					std::ostringstream strOut;
					strOut << "Imported Pos:";
					CategoryNode* parentNode = dynamic_cast<CategoryNode*>(categoryNode->getParentNode());
					std::vector<std::string> vecPlanPath;
					while (parentNode)
					{
						vecPlanPath.push_back(parentNode->getNodeName());
						parentNode = dynamic_cast<CategoryNode*>(parentNode->getParentNode());
					}
					for( int i = vecPlanPath.size() - 1; i >= 0; i--)
					{
						strOut << vecPlanPath[i] << "/";
					}
					vecPlanPath.clear();
					strOut << categoryNode->getNodeName() << " File:" 
						<< impFilePath.GetBuffer(1) << " Fail";
					
					TA_Base_Core::NameValuePair nvp0("Value", "Fail" );
					TA_Base_Core::NameValuePair nvp1("AuditMessage",strOut.str() );
					TA_Base_Core::DescriptionParameters dp;
					dp.push_back(&nvp0);
					dp.push_back(&nvp1);
					TA_Base_Core::AuditMessageSender *messageSender = 
						TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
						TA_Base_Core::PlanAgentAudit::Context );
					
					//TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
					unsigned long entityKey = planManagerEntityData->getKey();
					messageSender->sendAuditMessage(TA_Base_Core::PlanAgentAudit::PlanImportExport,
						entityKey, dp, "", session, "", "", "");
					delete messageSender;
					}
					// ++ CL 15087 (3)
					
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << std::string (impFilePath.GetBuffer (1));
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_210120);
                }
            }while (pos != NULL);
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnExportPlan ()
{
    FUNCTION_ENTRY( "OnExportPlan" );

    LPCTSTR defFileExt = ".pln";
    // TD17888 marc_bugfix
    DWORD dwFlags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    LPCTSTR fileFilter = "Plan Files (*.pln)|*.pln|";
    // CFileDialog expFileDlg (FALSE, defFileExt, NULL, dwFlags, fileFilter, NULL);
    
    CString m_DefaultExportDirectory;
    TCHAR szMaxPath[MAX_PATH];


    // TD17888 marc_bugfix
    SHGetSpecialFolderPath(NULL, szMaxPath, CSIDL_PERSONAL, FALSE);

    TA_Base_Core::IEntityData* planManagerEntityData =
        TA_Base_Core::EntityAccessFactory::getInstance().getEntity("PlanManager");

    TA_Base_Core::PlanManagerEntityData *planEntityData = dynamic_cast<TA_Base_Core::PlanManagerEntityData *>(planManagerEntityData);

    m_DefaultExportDirectory = planEntityData->getExportDirectory().c_str();


    TreeNodeList selectedNodes = GetSelectedNodes();
    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        // DTL-1006
        // Configure expFileDlg
        TA_Base_Bus::FileDialogArialDisableFolders expFileDlg (FALSE, defFileExt, NULL, dwFlags, fileFilter, NULL);

        if (m_DefaultExportDirectory.IsEmpty())
        {
            expFileDlg.m_ofn.lpstrInitialDir = szMaxPath;
        }
        else
        {
            CString fileName = "Untitled";
            CFile outFile;

            if ( 0 == outFile.Open(m_DefaultExportDirectory + fileName, CFile::modeCreate | CFile::modeReadWrite) )
            {
                expFileDlg.m_ofn.lpstrInitialDir = szMaxPath;
            }
            else
            {
                // remove the file if it is created
                if (outFile != NULL)
                {
                    outFile.Close();
                    CFile::Remove(m_DefaultExportDirectory + fileName);
                }
                expFileDlg.m_ofn.lpstrInitialDir = m_DefaultExportDirectory;

            }

        }

        expFileDlg.m_ofn.lpstrTitle = "Export Plan As";
        // DTL-1006

        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        TA_ASSERT(planNode != NULL, "Not a plan node");

        try
        {  //for each selected plan (node) ask user which file to save to
            strcpy (expFileDlg.m_ofn.lpstrFile, planNode->getNodeName ().c_str ());

            if ( expFileDlg.DoModal() == IDOK )
            {
                CString exportFilePath = expFileDlg.GetPathName ();
                TA_Base_Core::AuxPlanDetail_var   plan = planNode->exportPlan ();
                CWaitCursor waitCursor;
                // save details of interest to a .pln file
                CFile outFile;

                if ( outFile.Open(exportFilePath, CFile::modeCreate | CFile::modeWrite) )
                {

                    // Variables used repeatedly to write each piece of information to the file.
                    CString textToWrite;
                    std::string headerToWrite;

                    // Output the date as a heading
                    COleDateTime currTime = COleDateTime::GetCurrentTime();

                    // TD12474 ++
                    // headerToWrite = currTime.Format("%Y %B %d %H:%M");

                    TA_Base_Core::DateFormat dateFormat;
                    std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormatNoSec1b);

                    headerToWrite = currTime.Format(dateFormatStr.c_str());
                    // ++ TD12474

                    headerToWrite += "\n";
                    outFile.Write(headerToWrite.c_str(),headerToWrite.size());

                    // output plan details
                    textToWrite = "Plan_Details\n";
                    outFile.Write(textToWrite.GetBuffer (1),textToWrite.GetLength ());
                    textToWrite = "name==";
                    textToWrite += plan->name;
                    // before writing data
                    // remove any new-line
                    // chars from it
                    // as they break the import algorithm
                    ReplaceNLChars (textToWrite);
                    textToWrite += ";";
                    textToWrite += "description==";
                    textToWrite += plan->description;
                    ReplaceNLChars (textToWrite);
                    textToWrite += ";";
                    textToWrite += "\n";
                    outFile.Write(textToWrite.GetBuffer (1),textToWrite.GetLength ());

                    // steps details
                    textToWrite = "Steps";
                    textToWrite += "\n";
                    textToWrite += TA_Base_Core::DataConversion::toString( plan->steps.length() ).c_str();
                    textToWrite += "\n";
                    outFile.Write(textToWrite.GetBuffer (1),textToWrite.GetLength ());
                    for (unsigned int i = 0; i < plan->steps.length (); i++)
                    {
                        textToWrite = "name==";
                        textToWrite += plan->steps[i].name;
                        ReplaceNLChars (textToWrite);
                        textToWrite += ";";
                        textToWrite += "type==";
                        textToWrite += TA_Base_Core::DataConversion::toString( static_cast<long>(plan->steps[i].type) ).c_str();
                        textToWrite += ";";
                        textToWrite += "description==";
                        textToWrite += plan->steps[i].description;
                        ReplaceNLChars (textToWrite);
                        textToWrite += ";";
                        textToWrite += "position==";
                        textToWrite += TA_Base_Core::DataConversion::toString( plan->steps[i].position ).c_str();
                        textToWrite += ";";
                        textToWrite += "skip==";
                        if (plan->steps[i].skip)
                        {
                            textToWrite += "1";
                        }
                        else
                        {
                            textToWrite += "0";
                        }
                        textToWrite += ";";
                        textToWrite += "skippable==";
                        if (plan->steps[i].skippable)
                        {
                            textToWrite += "1";
                        }
                        else
                        {
                            textToWrite += "0";
                        }
                        textToWrite += ";";
                        textToWrite += "ignoreFailure==";
                        if (plan->steps[i].ignoreFailure)
                        {
                            textToWrite += "1";
                        }
                        else
                        {
                            textToWrite += "0";
                        }
                        textToWrite += ";";
                        textToWrite += "nowait==";
                        if (plan->steps[i].nowait)
                        {
                            textToWrite += "1";
                        }
                        else
                        {
                            textToWrite += "0";
                        }
                        textToWrite += ";";
                        textToWrite += "delay==";
                        textToWrite += TA_Base_Core::DataConversion::toString( plan->steps[i].delay ).c_str();
                        textToWrite += ";";
                        textToWrite += "\n";
                        outFile.Write(textToWrite.GetBuffer (1),textToWrite.GetLength ());

                        // step parameters (some steps may not have any)
                        if (plan->steps [i].stepParams.length () > 0)
                        {
                            textToWrite = "Step_Parameters";
                            textToWrite += "\n";
                            textToWrite += TA_Base_Core::DataConversion::toString( plan->steps[i].stepParams.length() ).c_str();
                            textToWrite += "\n";
                            outFile.Write(textToWrite.GetBuffer (1),textToWrite.GetLength ());
                            for (unsigned int j = 0; j < plan->steps [i].stepParams.length (); j++)
                            {
                                textToWrite  = "position==";
                                textToWrite += TA_Base_Core::DataConversion::toString( plan->steps[i].stepParams[j].position ).c_str();
                                textToWrite += ";";
                                textToWrite += "value==";
                                textToWrite += plan->steps[i].stepParams[j].value;
                                ReplaceNLChars (textToWrite);
                                textToWrite += ";";
                                textToWrite += "\n";
                                outFile.Write(textToWrite.GetBuffer (1),textToWrite.GetLength ());
                            }
                        }
                        else
                        {
                            textToWrite = "Step_Parameters";
                            textToWrite += "\n";
                            textToWrite += ("0");
                            textToWrite += "\n";
                            outFile.Write(textToWrite.GetBuffer (1),textToWrite.GetLength ());
                        }

                    }

                    outFile.Flush();
                    outFile.Close();
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << std::string (exportFilePath.GetBuffer (1));
                    UINT selectedButton = userMsg.showMsgBox(IDS_UI_210060);

					// CL 15087 (3) ++
					{
						// get plan path
						//15087 shengguang++
						std::ostringstream strOut;
						std::ostringstream planName;
						strOut <<"Plan ";
						//++shengguang
						CategoryNode* parentNode = dynamic_cast<CategoryNode*>(planNode->getParentNode());
						std::vector<std::string> vecPlanPath;
						while (parentNode)
						{
							vecPlanPath.push_back(parentNode->getNodeName());
							parentNode = dynamic_cast<CategoryNode*>(parentNode->getParentNode());
						}
						for( int i = vecPlanPath.size() - 1; i >= 0; i--)
						{
							//strOut << vecPlanPath[i] << "/";
							//15087 shengguang++
							planName << vecPlanPath[i] << "/";
						}
						vecPlanPath.clear();
						planName << planNode->getNodeName();
						//15087 shengguang++
						strOut<<planName.str()<<" is successfully "<<"Export at ";

						SYSTEMTIME datatime; 
						GetLocalTime(&datatime);
						strOut<<datatime.wDay<<"-";
						strOut<<datatime.wMonth<<"-";
						strOut<<datatime.wYear<<" ";
						strOut<<datatime.wHour<<":";
						strOut<<datatime.wMinute<<":";
						strOut<<datatime.wSecond;
						//++shengguang
						std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
						TA_Base_Core::DescriptionParameters dp;

						TA_Base_Core::NameValuePair nvp0("Value", "Successful" );
						TA_Base_Core::NameValuePair nvp1("AuditMessage",strOut.str() );
						dp.push_back(&nvp0);
						dp.push_back(&nvp1);
						TA_Base_Core::AuditMessageSender *messageSender = 
							TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
							TA_Base_Core::PlanAgentAudit::Context );
						
						//TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
						unsigned long entityKey = planManagerEntityData->getKey();
						messageSender->sendAuditMessage(TA_Base_Core::PlanAgentAudit::PlanImportExport,
							entityKey, dp, "", session, "", "", "");
						delete messageSender;
					}
					// ++ CL 15087 (3)
				}
				else
				{
					TA_Base_Bus::TransActiveMessage userMsg;
					userMsg << planNode->getNodeName ();
					userMsg.showMsgBox(IDS_UE_210117);
					// CL 15087 (3) ++
					{
						// get plan path
						//15087 shengguang++
						std::ostringstream strOut;
						std::ostringstream planName;
						strOut <<"Plan ";
						//++shengguang
						CategoryNode* parentNode = dynamic_cast<CategoryNode*>(planNode->getParentNode());
						std::vector<std::string> vecPlanPath;
						while (parentNode)
						{
							vecPlanPath.push_back(parentNode->getNodeName());
							parentNode = dynamic_cast<CategoryNode*>(parentNode->getParentNode());
						}
						for( int i = vecPlanPath.size() - 1; i >= 0; i--)
						{
							//strOut << vecPlanPath[i] << "/";
							//15087 shengguang++
							planName << vecPlanPath[i] << "/";
						}
						vecPlanPath.clear();
						planName << planNode->getNodeName() 
							<< " Fail";
						//15087 shengguang++
						strOut<<planName.str()<<" is successfully "<<"Export at ";
						
						SYSTEMTIME datatime; 
						GetLocalTime(&datatime);
						strOut<<datatime.wDay<<"-";
						strOut<<datatime.wMonth<<"-";
						strOut<<datatime.wYear<<" ";
						strOut<<datatime.wHour<<":";
						strOut<<datatime.wMinute<<":";
						strOut<<datatime.wSecond;
						//++shengguang
						std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
						TA_Base_Core::DescriptionParameters dp;
						TA_Base_Core::NameValuePair nvp0("Value", "Fail" );
						TA_Base_Core::NameValuePair nvp1("AuditMessage",strOut.str() );
						dp.push_back(&nvp0);
						dp.push_back(&nvp1);
						TA_Base_Core::AuditMessageSender *messageSender = 
							TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
							TA_Base_Core::PlanAgentAudit::Context );
						
						//TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
						unsigned long entityKey = planManagerEntityData->getKey();
						messageSender->sendAuditMessage(TA_Base_Core::PlanAgentAudit::PlanImportExport,
							entityKey, dp, "", session, "", "", "");
						delete messageSender;
					}
					// ++ CL 15087 (3)
					return;
				}
			}
		}
		catch (...)
		{
			// CL 15087 (3) ++
			// get plan path
			std::ostringstream strOut;
			strOut << "Exported ";
			CategoryNode* parentNode = dynamic_cast<CategoryNode*>(planNode->getParentNode());
			std::vector<std::string> vecPlanPath;
			while (parentNode)
			{
				vecPlanPath.push_back(parentNode->getNodeName());
				parentNode = dynamic_cast<CategoryNode*>(parentNode->getParentNode());
			}
			for( int i = vecPlanPath.size() - 1; i >= 0; i--)
			{
				strOut << vecPlanPath[i] << "/";
			}
			vecPlanPath.clear();
			strOut << planNode->getNodeName() 
			<< " Fail";

			std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			TA_Base_Core::DescriptionParameters dp;
			TA_Base_Core::NameValuePair nvp0("Value", "Fail" );
			TA_Base_Core::NameValuePair nvp1("AuditMessage",strOut.str() );
			dp.push_back(&nvp0);
			dp.push_back(&nvp1);
			TA_Base_Core::AuditMessageSender *messageSender = 
				TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
				TA_Base_Core::PlanAgentAudit::Context );

			//TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			unsigned long entityKey = planManagerEntityData->getKey();
			messageSender->sendAuditMessage(TA_Base_Core::PlanAgentAudit::PlanImportExport,
				entityKey, dp, "", session, "", "", "");
			delete messageSender;
			// ++ CL 15087 (3)

            TA_Base_Bus::TransActiveMessage userMsg;
            CString applicationName;
            userMsg << planNode->getNodeName ();
            userMsg.showMsgBox(IDS_UE_210118);
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnEditPlan()
{
    FUNCTION_ENTRY( "OnEditPlan" );

    TreeNodeList selectedNodes = GetSelectedNodes();

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        TA_ASSERT(planNode != NULL, "Not a plan node");

        try
        {
            CWaitCursor waitCursor;

            planNode->editPlan();
        }
        catch (...)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            CString applicationName;
            applicationName.LoadString(IDS_PLAN_EDITOR);
            userMsg << applicationName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnControlPlan()
{
    FUNCTION_ENTRY( "OnControlPlan" );

    TreeNodeList selectedNodes = GetSelectedNodes();

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        TA_ASSERT(planNode != NULL, "Not a plan node");

        try
        {
            CWaitCursor waitCursor;

            planNode->controlPlan();
        }
        catch (...)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            CString applicationName;
            applicationName.LoadString(IDS_PLAN_EDITOR);
            userMsg << applicationName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnApprovePlan()
{
    FUNCTION_ENTRY( "OnApprovePlan" );

    CWaitCursor waitCursor;
    TreeNodeList selectedNodes = GetSelectedNodes();

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        TA_ASSERT(planNode != NULL, "Not a plan node");

        try
        {
            planNode->approvePlan(true);
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            MessageBox::error(IDS_APPROVE_PLAN, planNode->getNodeName().c_str(), ex);
            waitCursor.Restore();
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnUnapprovePlan()
{
    FUNCTION_ENTRY( "OnUnapprovePlan" );

    CWaitCursor waitCursor;
    TreeNodeList selectedNodes = GetSelectedNodes();

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        TA_ASSERT(planNode != NULL, "Not a plan node");

        try
        {
            planNode->approvePlan(false);
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            MessageBox::error(IDS_UNAPPROVE_PLAN, planNode->getNodeName().c_str(), ex);
            waitCursor.Restore();
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnSchedulePlan()
{
    FUNCTION_ENTRY( "OnSchedulePlan" );

    CWaitCursor waitCursor;
    TreeNodeList selectedNodes = GetSelectedNodes();

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        TA_ASSERT(planNode != NULL, "Not a plan node");

        try
        {
            planNode->schedulePlan();
        }
        catch (...)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            CString applicationName;
            applicationName.LoadString(IDS_SCHEDULING_MANAGER);
            userMsg << applicationName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnNewPlan()
{
    FUNCTION_ENTRY( "OnNewPlan" );

    CategoryNode* categoryNode = dynamic_cast<CategoryNode*>(GetCurrentNode());

    // PW#2773: If current node is a plan, then use its parent category for inserting the new plan
    if (!categoryNode)
    {
        categoryNode = dynamic_cast<CategoryNode *>(GetCurrentNode()->getParentNode());
        TA_ASSERT(categoryNode != NULL, "Parent node is NULL");
    }

    try
    {
        CWaitCursor waitCursor;
        categoryNode->newPlan();
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_CREATE_NEW_PLAN, ex);
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnDeletePlan()
{
    FUNCTION_ENTRY( "OnDeletePlan" );

    TreeNodeList selectedNodes = GetSelectedNodes();
    TA_ASSERT(selectedNodes.size() > 0, "No plans are selected");

    TA_Base_Bus::TransActiveMessage userMsg;
    CString actionName;
    actionName.LoadString((selectedNodes.size() == 1) ? IDS_DELETE_SINGLE_PLAN : IDS_DELETE_MULTIPLE_PLANS);
    userMsg << actionName;
    UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);

    // Check if the user really wishes to do this
    if (selectedButton == IDYES)
    {
        CWaitCursor waitCursor;

        for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
        {
            PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
            TA_ASSERT(planNode != NULL, "Not a plan node");

            try
            {
                planNode->deletePlan();

                // TD4895: Check if this plan has been copied first.
                for (TreeNodeList::iterator copyIter = m_copyNodes.begin(); copyIter != m_copyNodes.end(); copyIter++)
                {
                    if (*copyIter == *iter)
                    {
                        // Plan has been copied before. It cannot be pasted anymore.
                        m_copyNodes.erase(copyIter);

                        if (m_copyNodes.empty())
                        {
                            // Can no longer hit paste.
                            m_copyNodeAction = CNA_NO_ACTION;
                        }

                        break;
                    }
                }
            }
            catch (TA_Base_Core::TransactiveException  &ex)
            {
                MessageBox::error(IDS_DELETE_PLAN, planNode->getNodeName().c_str(), ex);
                waitCursor.Restore();
            }
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnCopyPlan()
{
    FUNCTION_ENTRY( "OnCopyPlan" );

    // Remember the selected plan(s) ready for pasting
    TreeNodeList selectedNodes = GetSelectedNodes();
    m_copyNodes.clear();

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        TA_ASSERT(planNode != NULL, "Not a plan node");

        m_copyNodes.push_back(planNode);
        m_copyNodeAction = CNA_COPY_PLAN;
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnCutPlan()
{
    FUNCTION_ENTRY( "OnCutPlan" );

    // Remember the selected plan(s) ready for pasting
    TreeNodeList selectedNodes = GetSelectedNodes();
    m_copyNodes.clear();

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
         PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
         TA_ASSERT(planNode != NULL, "Not a plan node");

         m_copyNodes.push_back(planNode);
         m_copyNodeAction = CNA_CUT_PLAN;

         // as per TD 11721
         // this is done here rather then in PlanAgentAcess
         // because the movePlan method there can be
         // invoked 2 times when moving into a category
         // which already has a plan with the same name
//         std::string planName = planNode->getNodeName ();
//
//         TA_Base_Bus::TransActiveMessage userMsg;
//         std::string description = "being moved";
//         userMsg << planName;
//         userMsg << description;
//         if (userMsg.showMsgBox(IDS_UW_210031) == IDYES)
//         {
//            //TD17493++
//            //std::string planPath = PlanAgentAccess::getInstance().getNodePath (planNode->getNodeId ());
//            std::string planPath ="";
//            try
//            {
//                planPath = PlanAgentAccess::getInstance().getNodePath (planNode->getNodeId());
//            }
//            catch (...)
//            {
//                planPath = planName;
//            }
//            //++TD17493
//             PlanAgentAccess::getInstance().generateReport (planPath);
//         }
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnPastePlan()
{
    FUNCTION_ENTRY( "OnPastePlan" );

    if (m_copyNodeAction != CNA_COPY_PLAN && m_copyNodeAction != CNA_CUT_PLAN)
    {
        // Invalid action for plan paste.
        FUNCTION_EXIT;
        return;
    }

    if (m_copyNodes.empty())
    {
        // No plans to paste - just return.
        FUNCTION_EXIT;
        return;
    }

    CategoryNode* categoryNode = dynamic_cast<CategoryNode*>(GetCurrentNode());

    // PW#2773: If current node is a plan, then use its parent category for pasting the plan(s)
    if (!categoryNode)
    {
        categoryNode = dynamic_cast<CategoryNode *>(GetCurrentNode()->getParentNode());
        TA_ASSERT(categoryNode != NULL, "Parent node is NULL");
    }

    CWaitCursor waitCursor;
    UINT overwritePlanResponse = IDNO;

    // Copy or move the plans as required. Where a name clash with an existing plan occurs,
    // allow the operator to overwite that plan.
    for (TreeNodeList::const_iterator iter = m_copyNodes.begin(); iter != m_copyNodes.end();)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        TA_ASSERT(planNode != NULL, "Not a plan node");

        try
        {
            waitCursor.Restore();
            bool overwritePlan = (overwritePlanResponse == IDYES || overwritePlanResponse == IDYESTOALL);

            if (m_copyNodeAction == CNA_COPY_PLAN)
            {
                categoryNode->copyPlan(*planNode, overwritePlan);
            }
            else
            {
                categoryNode->movePlan(*planNode, overwritePlan);
            }
        }
        catch (const TA_Base_Core::PlanAgentException& ex)
        {
            switch (ex.getErrorType())
            {
                case TA_Base_Core::PAE_CANNOT_OVERWRITE_EDITED_PLAN:
                {
                    // If the operator had previously said "no to all" when asked for
                    // confirmation to replace an existing plan, it makes no sense to
                    // show him warnings about subsequent name clashes.
                    if (overwritePlanResponse != IDNOTOALL)
                    {
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << planNode->getNodeName();
                        userMsg.showMsgBox(IDS_UE_210094);
                    }

                    break;
                }

                case TA_Base_Core::PAE_CANNOT_OVERWRITE_APPROVED_PLAN:
                {
                    if (overwritePlanResponse != IDNOTOALL)
                    {
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << planNode->getNodeName();
                        userMsg.showMsgBox(IDS_UE_210095);
                    }

                    break;
                }

                case TA_Base_Core::PAE_CANNOT_OVERWRITE_CATEGORY:
                {
                    if (overwritePlanResponse != IDNOTOALL)
                    {
                        CString action;
                        AfxFormatString1(action, m_copyNodeAction == CNA_COPY_PLAN? IDS_COPY_PLAN : IDS_MOVE_PLAN, planNode->getNodeName().c_str());

                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << action;
                        userMsg.showMsgBox(IDS_UE_210096);
                    }

                    break;
                }

                case TA_Base_Core::PAE_NAME_NOT_UNIQUE:
                {
                    if (overwritePlanResponse != IDYESTOALL && overwritePlanResponse != IDNOTOALL)
                    {
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << planNode->getNodeName();

                        UINT mbStyle = (m_copyNodes.size() == 1? MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2
                                                               : MB_ICONWARNING | MB_YESNO | MB_YESTOALL | MB_NOTOALL | MB_DEFBUTTON3);

                        overwritePlanResponse = XMessageBox(AfxGetMainWnd()->GetSafeHwnd(), userMsg.constructMessage(IDS_UW_210020), NULL, mbStyle);
                    }

                    if (overwritePlanResponse == IDYES || overwritePlanResponse == IDYESTOALL)
                    {
                        continue;
                    }

                    break;
                }

                default:
                    MessageBox::error(m_copyNodeAction == CNA_COPY_PLAN ? IDS_COPY_PLAN : IDS_MOVE_PLAN, planNode->getNodeName().c_str(), ex);
            }
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            MessageBox::error(m_copyNodeAction == CNA_COPY_PLAN? IDS_COPY_PLAN : IDS_MOVE_PLAN, planNode->getNodeName().c_str(), ex);
        }

        if (overwritePlanResponse == IDYES)
        {
            overwritePlanResponse = IDNO;
        }

        iter++;
    }

    if (m_copyNodeAction == CNA_CUT_PLAN)
    {
        // A plan cut is followed by a single paste. The plan(s) cannot be moved more than once.
        m_copyNodeAction = CNA_NO_ACTION;
        m_copyNodes.clear();
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnNewCategory()
{
    FUNCTION_ENTRY( "OnNewCategory" );

    CategoryNode* categoryNode = dynamic_cast<CategoryNode *>(GetCurrentNode());

    // PW#2773: If current node is a plan, then use its parent category for inserting the new category
    if (!categoryNode)
    {
        categoryNode = dynamic_cast<CategoryNode *>(GetCurrentNode()->getParentNode());
        TA_ASSERT(categoryNode != NULL, "Parent node is NULL");
    }

    try
    {
        CWaitCursor waitCursor;

        categoryNode->newCategory();
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_CREATE_CATEGORY, ex);
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnDeleteCategory()
{
    FUNCTION_ENTRY( "OnDeleteCategory" );

    TA_Base_Bus::TransActiveMessage userMsg;
    CString actionName;
    actionName.LoadString(IDS_DELETE_CATEGORY);
    userMsg << actionName;
    UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);

    // Check if the user really wishes to do this
    if (selectedButton == IDYES)
    {
        CategoryNode* categoryNode = dynamic_cast<CategoryNode *>(GetCurrentNode());
        TA_ASSERT(categoryNode != NULL, "Not a category node");

        try
        {
            CWaitCursor waitCursor;

            categoryNode->deleteCategory();
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            MessageBox::error(IDS_DELETE_CATEGORY, ex);
        }
    }

    FUNCTION_EXIT;
}


// Del accelerator handler
void CPlanTreeDoc::OnDeleteCategoryOrPlan()
{
    FUNCTION_ENTRY( "OnDeleteCategoryOrPlan" );

    if (ArePlanNodesSelected())
    {
        OnDeletePlan();
    }
    else
    {
        OnDeleteCategory();
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::OnEditStep()
{
    FUNCTION_ENTRY( "OnEditStep" );

    try
    {
        // Can't edit the step from the Plan Manager main window, so view it instead in read-only mode.
        PlanStep *planStep = GetCurrentStep();
        if (planStep)
        {
            planStep->viewStep();
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_VIEW_PLAN_STEP, ex);
    }
    catch (...)
    {
        MessageBox::error(IDS_VIEW_PLAN_STEP);
    }

    FUNCTION_EXIT;
}


const std::string &CPlanTreeDoc::GetRootNodeLabel()
{
    FUNCTION_ENTRY( "GetRootNodeLabel" );

    static const std::string ROOT_NODE_LABEL = "";

    FUNCTION_EXIT;
    return ROOT_NODE_LABEL;
}


void CPlanTreeDoc::AddToCategoryViewNodes(TreeNode *treeNode)
{
    FUNCTION_ENTRY( "AddToCategoryViewNodes" );

    TreeNodeMap::iterator iter = m_categoryViewNodeMap.find(treeNode->getNodeId());

    if (iter == m_categoryViewNodeMap.end())
    {
        m_categoryViewNodeMap.insert(std::make_pair(treeNode->getNodeId(), treeNode));
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::RemoveFromCategoryViewNodes(TreeNode *treeNode)
{
    FUNCTION_ENTRY( "RemoveFromCategoryViewNodes" );

    TreeNodeMap::iterator iter = m_categoryViewNodeMap.find(treeNode->getNodeId());

    if (iter != m_categoryViewNodeMap.end())
    {
        m_categoryViewNodeMap.erase(iter);
    }

    FUNCTION_EXIT;
}


void CPlanTreeDoc::PurgeCategoryViewNodes()
{
    FUNCTION_ENTRY( "PurgeCategoryViewNodes" );

    m_categoryViewNodeMap.clear();

    FUNCTION_EXIT;
}


bool CPlanTreeDoc::ArePlanNodesSelected()
{
    FUNCTION_ENTRY( "ArePlanNodesSelected" );

    TreeNodeList selectedNodes = GetSelectedNodes();

    if (selectedNodes.empty())
    {
        FUNCTION_EXIT;
        return false;
    }

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        if (!planNode)
        {
            FUNCTION_EXIT;
            return false;
        }
    }

    FUNCTION_EXIT;
    return true;
}


TreeNodeList CPlanTreeDoc::GetSelectedNodes()
{
    FUNCTION_ENTRY( "GetSelectedNodes" );

    // If there are nodes selected in the category node view return those, otherwise return only the node
    // that's currently selected in the plan tree.
    TreeNodeList selectedNodes;

    if (m_categoryViewNodeMap.empty())
    {
        selectedNodes.push_back(GetCurrentNode());
    }
    else
    {
        selectedNodes.reserve(m_categoryViewNodeMap.size());

        for (TreeNodeMap::iterator iter = m_categoryViewNodeMap.begin(); iter != m_categoryViewNodeMap.end(); iter++)
        {
            selectedNodes.push_back(iter->second);
        }
    }

    FUNCTION_EXIT;
    return selectedNodes;
}


// ITreeNodeFilter interface
bool CPlanTreeDoc::isRelevant(TreeNode & /*treeNode*/)
{
    FUNCTION_ENTRY( "isRelevant" );
    FUNCTION_EXIT;
    return false;
}


void CPlanTreeDoc::ReplaceNLChars (CString& value)
{
    FUNCTION_ENTRY( "ReplaceNLChars" );

    value.Replace ("\n", " ");
    value.Replace ("\r", " ");

    FUNCTION_EXIT;
}

void CPlanTreeDoc::OnMakePlanVisible()
{
    FUNCTION_ENTRY( "OnMakePlanVisible" );

    CWaitCursor waitCursor;
    TreeNodeList selectedNodes = GetSelectedNodes();

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        if (planNode == NULL)
        {
            continue;
        }

        try
        {
            planNode->setVisible(TRUE);
        }
        catch (...)
        {
            waitCursor.Restore();
        }
    }

    FUNCTION_EXIT;
}

void CPlanTreeDoc::OnMakePlanInvisible()
{
    FUNCTION_ENTRY( "OnMakePlanInvisible" );

    CWaitCursor waitCursor;
    TreeNodeList selectedNodes = GetSelectedNodes();

    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        PlanNode* planNode = dynamic_cast<PlanNode *>(*iter);
        if ( NULL == planNode )
        {
            continue;
        }

        try
        {
            planNode->setVisible(FALSE);
        }
        catch (...)
        {
            waitCursor.Restore();
        }
    }

    FUNCTION_EXIT;
}

