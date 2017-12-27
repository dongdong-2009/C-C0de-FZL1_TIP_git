/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/CtaScadaExceptions.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if !defined(CtaScadaInvalidConfigurationException_54AC14B5_DF5A_4116_AF54_34BD657D7018__INCLUDED_)
#define CtaScadaInvalidConfigurationException_54AC14B5_DF5A_4116_AF54_34BD657D7018__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"


namespace TA_Base_Core
{

    /**
     * @version 1.0
     * @created 04-Mar-2005 10:31:10 AM
     */
    class CtaScadaInvalidConfigurationException : public TransactiveException 
    {

    public:
	    
        CtaScadaInvalidConfigurationException() throw()
        : TransactiveException("Unspecified inconsistency detected in the datapoint configuration")
        { }
	    
        CtaScadaInvalidConfigurationException(const char* msg) throw()
        : TransactiveException(msg)
        { }
	    
        virtual ~CtaScadaInvalidConfigurationException() throw()
        { }

    };

    class CtaScadaWriteException : public TransactiveException 
    {

    public:
	    
        CtaScadaWriteException() throw()
        : TransactiveException("Unspecified datapoint write failure")
        { }
	    
        CtaScadaWriteException(const char* msg) throw()
        : TransactiveException(msg)
        { }
	    
        virtual ~CtaScadaWriteException() throw()
        { }

    };

}
#endif // !defined(CtaScadaInvalidConfigurationException_54AC14B5_DF5A_4116_AF54_34BD657D7018__INCLUDED_)