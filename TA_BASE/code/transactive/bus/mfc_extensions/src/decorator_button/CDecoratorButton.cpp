/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/CDecoratorButton.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * MFC extension of CButton. This class will support a decorator pattern.
 */

#include "bus/mfc_extensions/src/decorator_button/CDecoratorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDecoratorButton


    CDecoratorButton::CDecoratorButton() : m_pDecoratorButtonBehaviourStrategy(NULL), m_isChecked(false)
    {
	    m_pFinalDecorator = this;
	    m_pConcreteDecoratorButton = this;
    }

    CDecoratorButton::~CDecoratorButton()
    {
    }


    BEGIN_MESSAGE_MAP(CDecoratorButton, CButton)
	    //{{AFX_MSG_MAP(CDecoratorButton)
	    ON_WM_LBUTTONUP()
	    ON_WM_RBUTTONUP()
	    ON_WM_DROPFILES()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CDecoratorButton message handlers

    void CDecoratorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
    {
        // create off screen DC to prevent flickering when we draw
        DRAWITEMSTRUCT tempDrawItem;
        CopyMemory(&tempDrawItem, lpDrawItemStruct, sizeof(DRAWITEMSTRUCT));
    
        CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
	    CDC tempDC;
	    tempDC.CreateCompatibleDC(pDC);
        CBitmap tempBitmap;
        tempBitmap.CreateCompatibleBitmap(pDC, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom);
	    CBitmap* pbmpOld = tempDC.SelectObject( &tempBitmap );

        tempDrawItem.hDC = tempDC.m_hDC;

        // set the check state
        if (m_isChecked)
        {
            tempDrawItem.itemState |= ODS_CHECKED;
        }
        else
        {
            tempDrawItem.itemState &= ~ODS_CHECKED;
        }

        // draw on the off screen DC
	    m_pFinalDecorator->Draw(&tempDrawItem);

        // blit back to the window DC
        pDC->BitBlt(0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom,
            &tempDC, 0, 0, SRCCOPY);

        tempDC.SelectObject(pbmpOld);
    }

    void CDecoratorButton::OnLButtonUp(UINT nFlags, CPoint point) 
    {
        if (m_pDecoratorButtonBehaviourStrategy != NULL)
        {
    	    m_pDecoratorButtonBehaviourStrategy->leftClickBehaviour();
        }
	    
	    CButton::OnLButtonUp(nFlags, point);
    }

    void CDecoratorButton::OnRButtonUp(UINT nFlags, CPoint point) 
    {
        if (m_pDecoratorButtonBehaviourStrategy != NULL)
        {
    	    m_pDecoratorButtonBehaviourStrategy->rightClickBehaviour();
        }
	    
	    CButton::OnRButtonUp(nFlags, point);
    }

    void CDecoratorButton::OnDropFiles(HDROP hDropInfo) 
    {
        if (m_pDecoratorButtonBehaviourStrategy != NULL)
        {
    	    m_pDecoratorButtonBehaviourStrategy->dropBehaviour(hDropInfo);
        }
    
	    CButton::OnDropFiles(hDropInfo);
    }

    void CDecoratorButton::PreSubclassWindow() 
    {
      // Make the button owner-drawn
	    ModifyStyle (0, BS_OWNERDRAW, 0);
	    CButton::PreSubclassWindow();
    }

    void CDecoratorButton::Draw(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
	    CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
	    CRect rect = lpDrawItemStruct->rcItem;
	    UINT state = lpDrawItemStruct->itemState;

	    if (state & ODS_SELECTED)
	    {
			pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
		}
		else if  (isChecked()) // Fixed TD#14281
		{
			CBrush lBrush(RGB(238, 238, 238));
			pDC->FillRect(&rect, &lBrush);

			pDC->DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);	    
		}
	    else
	    {
		    pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH);
	    }
    }

    void CDecoratorButton::setFinalDecorator(TA_Base_Bus::AbstractDecoratorButton* pFinalDec)
    {
	    m_pFinalDecorator = pFinalDec;
    }

    void CDecoratorButton::setButtonBehaviorStrategy(TA_Base_Bus::IDecoratorButtonBehaviourStrategy* pStrategy)
    {
        m_pDecoratorButtonBehaviourStrategy = pStrategy;
    }

    void CDecoratorButton::setCheck(bool checkIt)
    {
        if (checkIt != m_isChecked)
        {
    	    m_isChecked = checkIt;
            Invalidate(FALSE);
        }
    }

    bool CDecoratorButton::isChecked()
    {
	    return m_isChecked;
    }
