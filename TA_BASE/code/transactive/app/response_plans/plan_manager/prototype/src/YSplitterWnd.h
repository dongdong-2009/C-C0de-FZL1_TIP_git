#if !defined(AFX_YSPLITTERWND_H__8E3E5264_02A9_11D2_BF99_000021000B7C__INCLUDED_)
#define AFX_YSPLITTERWND_H__8E3E5264_02A9_11D2_BF99_000021000B7C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// YSplitterWnd.h : header file
//

#define WM_SPLITTER_MOVED   (WM_USER+5000)


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

    BOOL AttachAbove(DWORD ctrlId, int offset);
    BOOL AttachBelow(DWORD ctrlId, int offset);
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
    } ResizableControl;

    CArray<ResizableControl, ResizableControl> m_aboveControls, m_belowControls;

    int m_highestCtrl, m_lowestCtrl;
    int m_minAbove, m_minBelow;

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

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YSPLITTERWND_H__8E3E5264_02A9_11D2_BF99_000021000B7C__INCLUDED_)
