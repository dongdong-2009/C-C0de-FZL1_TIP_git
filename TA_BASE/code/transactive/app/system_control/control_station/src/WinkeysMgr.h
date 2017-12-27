/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/WinkeysMgr.h $
  * @author:  Jiang Haipeng
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description> based on the configration from database, this class will manage the keys status and their functions.
  *
  */

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINKEYSMGR_H__F6E45A0D_F48F_4B86_9AD9_7E26D86137A7__INCLUDED_)
#define AFX_WINKEYSMGR_H__F6E45A0D_F48F_4B86_9AD9_7E26D86137A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "stdafx.h"

class WinkeysMgr  
{
public:
	static WinkeysMgr * instance();
    /** 
      * onRuntimeParams
      *
      * description, if user change the configurations, the state of the current CAD keys will be updated.
      *
      */
	void onRuntimeParams();
    /** 
      * unLockCAD
      *
      * description, if the CAD keys are locked, then to unlock them
      *
      */
	void unLockCAD();
    /** 
      * lockCAD
      *
      * description, if the database variable disable the CAD keys, then to lock them
      *
      */
	void lockCAD();
private:
	static WinkeysMgr * m_self;
	BOOL m_bDisableCAD;       // true: enable the ctrl+alt+detele key sequence, otherwise disable it.
private:
	WinkeysMgr();
	virtual ~WinkeysMgr();
};

#endif // !defined(AFX_WINKEYSMGR_H__F6E45A0D_F48F_4B86_9AD9_7E26D86137A7__INCLUDED_)
