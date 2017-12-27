#if !defined( MESSAGEFILTERS_H )
#define MESSAGEFILTERS_H 

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/MessageFilters.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * The available message filters for installation
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/IMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/utilities/src/TA_String.h"

namespace TA_Base_Core
{
	class CommsMessageFilter : public IMessageFilter
	{
	public:
		
		/**
		* constructor
		*/
		CommsMessageFilter( unsigned long entityKey )
			: m_entityKey( entityKey ) 
        {
            LOG2( SourceInfo, DebugUtil::DebugMsg, 
                "CommsMessageFilter::CommsMessageFilter(): constructing %s entityKey=%d", 
                gPtrToStr(this).c_str(), m_entityKey);
        }
		
		/**
		* destructor
		*/
		virtual ~CommsMessageFilter() {}

        // Implementation of IMessageFilter
		virtual bool match( const CosNotification::StructuredEvent& event ) const
		{
			// extract the specialised message from the Any
			CommsMessageCorbaDef* specialisedMessage = 0;

			if ( ( event.remainder_of_body >>= specialisedMessage) != 0)
			{
				// Check that the entityKey is a match
				if ( specialisedMessage->assocEntityKey == m_entityKey )
				{
					return true;
				}
			}
			else
			{
				LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"CommsMessageFilter::match(): %s unable to extract payload as CommsMessageCorbaDef.",
                    gPtrToStr(this).c_str() );
			}

			// If we get here we have a failure
			return false;
		}

	private:
        // NOT CLONEABLE
		CommsMessageFilter( const CommsMessageFilter& );

		/**
		 * store the entity key for comparison purposes
		 */
		unsigned long m_entityKey;

	};	// CommsMessageFilter


}; // TA_Base_Core

#endif // !defined( MESSAGEFILTERS_H )
