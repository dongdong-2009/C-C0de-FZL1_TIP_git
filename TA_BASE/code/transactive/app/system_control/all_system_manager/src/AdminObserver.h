/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/AdminObserver.h $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * AdminObserver is an abstract class. Each object that needs to register 
  * with the System Manager as an observer must inherit from this class and 
  * implement the OnNewData method.
  */

#if !defined(AFX_ADMINOBSERVER_H__679184C1_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)
#define AFX_ADMINOBSERVER_H__679184C1_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    class AdminManager;

    class AdminObserver  
    {
    public:

        /**
          * Constructor
          */
        AdminObserver();

        /**
          * Destructor
          */
        virtual ~AdminObserver();

        /**
          * onNewData
          * 
          * This method must be overloaded. It is called by the AdminManager if
          * the observer has registered when new data is available.
          */
        virtual void onNewData( AdminManager* ) = 0;

    protected:

        /**
          * registerObserver
          * 
          * This method calls the AdminManager's RegisterObserver function
          * with a this pointer
          */
        void registerObserver();

        /**
          * unregisterObserver
          * 
          * This method calls the AdminManager's UnregisterObserver function
          * with a this pointer
          */
        void unregisterObserver();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ADMINOBSERVER_H__679184C1_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)
