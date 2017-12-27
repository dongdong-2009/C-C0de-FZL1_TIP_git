/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/PollListener.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is used by ManagedProcess to respond to polls.
  * It declares a virtual method, poll(), that should just
  * return if the process is functioning correctly, and should
  * throw a PollException if a problem is detected. The
  * implementation of poll() in this class just returns, but this
  * class can be subclassed (and poll() overridden) to allow
  * a process to perform regular health checks while executing.
  *
  */

#if !defined(AFX_POLLLISTENER_H__FE39922F_FB47_444B_98D3_50CC106DF45C__INCLUDED_)
#define AFX_POLLLISTENER_H__FE39922F_FB47_444B_98D3_50CC106DF45C__INCLUDED_

namespace TA_Base_Core
{

    class PollListener  
    {

    public:

        /**
          * Constructor
          */
	    PollListener();

        /**
          * poll
          *
          * Simply returns if the process is functioning as expected, and throws
          * a PollException if a problem is detected within the process. This
          * implementation simply returns everytime, but processes can inherit
          * from this class and override this method to perform checks specific
          * to the process.
          */
        virtual void poll();
    
        /**
          * Destructor
          */
        virtual ~PollListener();

    private:

        // Disable the copy constructor and assignment operator
        PollListener(const PollListener& thePollListener);
        PollListener& operator=(const PollListener&);

    };

} // namespace TA_Base_Core

#endif // !defined(AFX_POLLLISTENER_H__FE39922F_FB47_444B_98D3_50CC106DF45C__INCLUDED_)
