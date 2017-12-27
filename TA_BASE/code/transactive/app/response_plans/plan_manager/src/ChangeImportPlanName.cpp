/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ChangeImportPlanName.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ChangeImportPlanName.cpp : implementation file
//

#include "stdafx.h"
#include "planmanager.h"
#include "ChangeImportPlanName.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 azenitha

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CChangeImportPlanName dialog


CChangeImportPlanName::CChangeImportPlanName(CategoryNode& categoryNode, CString promptMessage, CWnd* pParent /*=NULL*/)
    : CDialog(CChangeImportPlanName::IDD, pParent),
    m_categoryNode (categoryNode)
{
    FUNCTION_ENTRY( "CChangeImportPlanName" );

    // {{AFX_DATA_INIT(CChangeImportPlanName)
    m_PromptMessageTxt = promptMessage.GetBuffer (1);
    m_NewPlanNameTxt = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CChangeImportPlanName::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CChangeImportPlanName)
    DDX_Control(pDX, IDC_PROMPT_EDIT, m_PromptMessageEdit);
    DDX_Control(pDX, IDC_PLAN_NAME_EDIT, m_NewPlanNameEdit);
    DDX_Text(pDX, IDC_PROMPT_EDIT, m_PromptMessageTxt);
    DDX_Text(pDX, IDC_PLAN_NAME_EDIT, m_NewPlanNameTxt);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CChangeImportPlanName, CDialog)
    // {{AFX_MSG_MAP(CChangeImportPlanName)
        // NOTE: the ClassWizard will add message map macros here
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeImportPlanName message handlers
CString CChangeImportPlanName::GetNewPlanName ()
{
    FUNCTION_ENTRY( "GetNewPlanName" );
    FUNCTION_EXIT;
    return m_NewPlanNameTxt;
}


void CChangeImportPlanName::OnOK ()
{
    FUNCTION_ENTRY( "OnOK" );

    CString newName;
    m_NewPlanNameEdit.GetWindowText (newName);
    if (newName.IsEmpty () || PlanNameExists (newName))
    {
        // TD14164 azenitha++
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_210032);
        /*AfxMessageBox ("Enter different name", MB_OK);*/
        // TD14164 ++azenitha
    }
    else
    {
        CDialog::OnOK ();
    }

    FUNCTION_EXIT;
}


bool CChangeImportPlanName::PlanNameExists (CString newName)
{
    FUNCTION_ENTRY( "PlanNameExists" );

    TreeNodeList childNodes;
    m_categoryNode.getChildren(childNodes);
    for (TreeNodeList::iterator iter = childNodes.begin(); iter != childNodes.end(); iter++)
    {
        TreeNode *childNode = (TreeNode *) *iter;
        if (newName.Compare (childNode->getNodeName ().c_str ()) == 0)
        {
            FUNCTION_EXIT;
            return true;
        }

    }

    FUNCTION_EXIT;
    return false;
}


