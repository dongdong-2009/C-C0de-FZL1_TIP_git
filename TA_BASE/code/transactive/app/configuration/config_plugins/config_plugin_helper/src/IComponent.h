/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/IComponent.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This defines the parts of the component that should be common to all DLL's
  */

#if !defined(IComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_)
#define IComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_

namespace TA_Base_App
{
    class IDatabaseAccessor;
    class MessageSender;
    class IProgressStatusBar;

    class IComponent
    {
    public:
    
        virtual ~IComponent(){ };


        /**
         * getDatabaseAccessor
         *
         * This returns a reference to an IDatabaseAccessor. This object creates this and it will
         * be used by all other objects requiring database access.
         *
         * @return IDatabaseAccessor* - The object used to access the database.
         */
        virtual IDatabaseAccessor* getDatabaseAccessor() =0;


        /**
         * getMessageSender
         *
         * This returns a reference to a MessageSender. This object creates this and it will be used
         * by all other objects needing to send audit and online updates.
         *
         * @return MessageSender& - The object to use to send messages.
         */
        virtual MessageSender& getMessageSender() =0;


        /**
         * getStatusBar
         *
         * This returns a pointer to the status bar. Other objects will need this to display their
         * progress.
         *
         * @return IProgressStatusBar - The status bar object to use to show progress
         */
        virtual IProgressStatusBar* getStatusBar() const =0;

    };
}

#endif // !defined(IComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_)
