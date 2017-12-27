/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/BannerFrameworkException.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This file outlines a simple exception class for use with
 * the BannerFramework. It is
 * derived from TransactiveException.
 * This exception class is used when errors are encountered 
 * within the framework that are necessary to pass up to the banner applicaiton.
 */

#if !defined(BANNERFRAMEWORKEXCEPTION_H)
#define BANNERFRAMEWORKEXCEPTION_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class BannerFrameworkException : public TransactiveException  
	{
	public:
 
        enum EFailType
        {
            GENERAL_ERROR,  // A generic error
            UNKNOWN_PAGE    // showPage() was called on a page that has not been added 
                            // to the framework
        };

	 /**
	   * BannerFrameworkException (Constructor)
	   * 
	   * This constructs an BannerFrameworkException.
	   *
	   * @param char* - This contains as much information as possible about the error that occurred. This can be any
	   *                additional information about the error.
	   *
	   */
		BannerFrameworkException(const char *msg) throw();

	 /**
	   * BannerFrameworkException (Constructor)
	   * 
	   * This constructs an BannerFrameworkException.
	   *
       * @param EFailType - This indicates what sort of failure occurred. This is used to specialise the exception if necessary.
	   * @param char* - This contains as much information as possible about the error that occurred. This can be any
	   *                additional information about the error.
	   *
	   */
		BannerFrameworkException(const EFailType failType, const char *msg) throw();

		virtual ~BannerFrameworkException() throw();

      /**
       * getFailType
       *
       * @return EFailType - The type of failure that occured. This is for use with
       *                     functions such as logging in where it can fail because of an error
       *                     or it can fail because the password etc was not correct or the user
       *                     does not have access rights.
       */
        EFailType getFailType() const
        {
            return m_failType;
        };

    private:
        EFailType m_failType;
	};
} // Closes TA_Base_Core

#endif // !defined(BANNERFRAMEWORKEXCEPTION_H)

