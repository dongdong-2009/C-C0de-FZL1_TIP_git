//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/YSplitterWnd.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Vertically movable splitter window. Automatically resizes controls below and above.

#if !defined(AFX_YSPLITTERWND_H__8E3E5264_02A9_11D2_BF99_000021000B7C__INCLUDED_)
#define AFX_YSPLITTERWND_H__8E3E5264_02A9_11D2_BF99_000021000B7C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CySplitterWnd window

    class CySplitterWnd : public CWnd
    {
    // Construction
    public:
        CySplitterWnd();

    // Attributes
    public:

    // Operations
    public:
        BOOL BindWithControl(CWnd *parent, DWORD ctrlId);
        void Unbind(void);

        void SetMinHeight(int above, int below);
        void SetSplitRatio(double splitRatio);

        BOOL AttachAbove(DWORD ctrlId, int offset, BOOL fixedHeight = FALSE);
        BOOL AttachBelow(DWORD ctrlId, int offset, BOOL fixedHeight = FALSE);
        BOOL DetachAll(void);
        void RecalcLayout(void);

    protected:
        BOOL GetMouseClipRect(LPRECT rectClip, CPoint point);

    // Attributes
    protected:
        CWnd *m_Parent;

        typedef struct
        {
            DWORD m_ctrlId;
            int   m_offset;
            int   m_height;
            BOOL  m_fixedHeight;
        } ResizableControl;

        CArray<ResizableControl, ResizableControl> m_aboveControls, m_belowControls;

        int m_highestCtrl, m_lowestCtrl;
        int m_minAbove, m_minBelow;

        double m_splitRatio;

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CySplitterWnd)
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CySplitterWnd();

        // Generated message map functions
    protected:
        // {{AFX_MSG(CySplitterWnd)
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Developer Studio will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_YSPLITTERWND_H__8E3E5264_02A9_11D2_BF99_000021000B7C__INCLUDED_)
