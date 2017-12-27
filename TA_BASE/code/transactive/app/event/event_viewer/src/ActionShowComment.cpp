/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/ActionShowComment.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class holds the comment dialog that allows the user to view comments. This is a
  * modified version of the dialog used in the Alarm Manager.
  */


#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/ActionIds.h"
#include "app/event/event_viewer/src/ActionShowComment.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Ctor
ActionShowComment::ActionShowComment(CWnd* pParent /*=NULL*/)
	: CDialog(ActionShowComment::IDD, pParent)
{
	//{{AFX_DATA_INIT(ActionShowComment)
	m_comment = _T("");
	//}}AFX_DATA_INIT
}


// Dtor
ActionShowComment::~ActionShowComment()
{
}


void ActionShowComment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ActionShowComment)
	DDX_Text(pDX, IDC_EDIT_ALARM_COMMENT, m_comment);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ActionShowComment, CDialog)
	//{{AFX_MSG_MAP(ActionShowComment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

unsigned int ActionShowComment::getActionId()
{
    return ACTION_SHOW_COMMENT;
}

void ActionShowComment::executeAction( Filter* filter )
{
    // Get the index of the first selected item.
    const std::vector<unsigned long>& selectedItems = filter->getSelectedItems();

    // Make sure an item is selected... this should always be true if double clicking!
    if ( 0 < selectedItems.size() )
    {
        // Set the item information.
        m_comment = filter->getItemData( selectedItems[0], CombinedEventFilter::COLUMN_COMMENT, false ).c_str();
    }

    // Show the message box only if there is a comment to show.
    if ( FALSE == m_comment.IsEmpty() )
    {
        DoModal();
    }
}

/////////////////////////////////////////////////////////////////////////////
// ActionShowComment message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL ActionShowComment::OnInitDialog() 
{
	FUNCTION_ENTRY("OnInitDialog");
	CDialog::OnInitDialog();

    // Make sure the current value of m_comment is applied to the edit control, not
    // the other way around.
    UpdateData(FALSE);

	FUNCTION_EXIT;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
