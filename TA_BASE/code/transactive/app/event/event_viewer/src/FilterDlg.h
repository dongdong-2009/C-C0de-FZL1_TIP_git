/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/src/FilterDlg.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2013/01/24 17:57:59 $
 * Last modified by:  $Author: CM $
 * 
 * FilterDlg is an intermediate sub-class of CDialog. It is implemented by the filter dialogs used in the main applicaiton,
 * which allows a single interface to different types of dialogs.
 */

#if !defined(FILTER_DLG_H)
#define FILTER_DLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class FilterDlg : public CDialog
{
public:

    /**
     * constructor
     *
     * This constructor is here to allow FilterDlg to be a true sub-class of CDialog. All overriden methods are in the child 
     * classes of this class.
     */
    FilterDlg(UINT nIDTemplate,CWnd* pParent): CDialog(nIDTemplate,pParent)
    {};

    /**
     * updateFilter
     *
     * Notifies the filter dialog that the filter object has been changed. When this method is called, 
     * the filter object is re-read and the filter dialog updated to reflect its contents.
     */
    virtual void updateFilter() =0;
};


#endif // !defined(FILTER_DLG_H)