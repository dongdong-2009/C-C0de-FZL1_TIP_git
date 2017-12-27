/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/message_box/XMessageBox.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 16:08:36 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
// XMessageBox.h
//
// This software is released into the public domain.  
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XMESSAGEBOX_H
#define XMESSAGEBOX_H

#define MB_DEFBUTTON5		0x00000400L
#define MB_DEFBUTTON6		0x00000500L

#define MB_CONTINUEABORT	0x00000008L     // adds two buttons, "Continue"  and "Abort"
#define MB_DONOTASKAGAIN	0x01000000L     // add checkbox "Do not ask me again"
#define MB_DONOTTELLAGAIN	0x02000000L     // add checkbox "Do not tell me again"
#define MB_YESTOALL			0x04000000L     // must be used with either MB_YESNO or MB_YESNOCANCEL
#define MB_NOTOALL			0x08000000L     // must be used with either MB_YESNO or MB_YESNOCANCEL
#define MB_NORESOURCE		0x10000000L		// do not try to load button strings from resources
#define MB_NOSOUND			0x80000000L     // do not play sound when mb is displayed

#define IDCONTINUE			10
#define IDYESTOALL			11
#define IDNOTOALL			12


AFX_EXT_CLASS int XMessageBox(HWND hwnd, 
                              LPCTSTR lpszMessage,
                              LPCTSTR lpszCaption = NULL, 
                              UINT uStyle = MB_OK|MB_ICONEXCLAMATION,
                              UINT uHelpId = 0);

#endif //XMESSAGEBOX_H
