/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is basically an edit control but it can be set whether the text is valid or not. If the text is
  * invalid it will be shown in red.
  */


#if !defined(AFX_TEXTCOLOUREDIT_H__F291D285_943B_44CA_A6CD_E66C34BD64E0__INCLUDED_)
#define AFX_TEXTCOLOUREDIT_H__F291D285_943B_44CA_A6CD_E66C34BD64E0__INCLUDED_

#include "../stdafx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
    class AFX_EXT_CLASS TextColourEdit : public CEdit
    {
    public:

        enum TextType
        {
            VALID,
            INVALID
        };


        /**
         * Constructor
         *
         * 4669FAT TD#782: These controls need to grey out when set to read only
         *
         * @param greyOutWhenReadOnly (optional) default behaviour matches original
         *           implementation of TextColourEdit.  Set to true to have the edit
         *          control go grey when it is in a read only state (standard CEdit behaviour)
         */
	    TextColourEdit(bool greyoutWhenReadOnly = false);


        /**
         * Destructor
         */
        virtual ~TextColourEdit();


        /**
         * setTextType
         *
         * This sets the colour that the text should be displayed in
         */
        void setTextType(TextType type)
        {
            m_textType = type;
            Invalidate(FALSE);
        }


        /**
         * isTextValid
         *
         * @return This returns true if the text contained in the edit box is valid and false if it is
         *         invalid
         */
        bool isTextValid() const
        {
            return (m_textType == VALID);
        }


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(TextColourEdit)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(TextColourEdit)
        afx_msg HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor );
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()

    private:
        bool m_greyOutWhenReadOnly;
        TextType m_textType;        // This indicates whether we have valid or invalid text so the colour can
                                    // be determined.

        CBrush m_backgroundBrush;           // These are the background brushes to use when painting the control.
        CBrush m_disabledBackgroundBrush;   // One is for enabled windows and the other for disabled.
    };
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTCOLOUREDIT_H__F291D285_943B_44CA_A6CD_E66C34BD64E0__INCLUDED_)
