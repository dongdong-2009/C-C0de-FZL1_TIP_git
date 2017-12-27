/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/RecordableItem.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include "core/data_access_interface/data_recording_dai/src/RecordableItem.h"
#include "core/data_access_interface/data_recording_dai/src/RecordableItemAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    RecordableItem::RecordableItem( unsigned long id )
    : m_id( id ), m_typeKey( 0 ), m_enabled( false ), m_isValid( false )
    {
        // Nothing else
    }


    RecordableItem::RecordableItem( unsigned long id, unsigned long type, bool recordingEnabled )
    : m_id( id ), m_typeKey( type ), m_enabled( recordingEnabled ), m_isValid( true )
    {
        // Nothing else
    }


    RecordableItem::~RecordableItem()
    {
        // Nothing to do
    }

    
    unsigned long RecordableItem::getTypeKey()
    {
        if ( ! m_isValid )
        {
            reload();
        }
        return m_typeKey;
    }


    unsigned long RecordableItem::getId()
    {
        if ( ! m_isValid )
        {
            reload();
        }
        return m_id;
    }


    bool RecordableItem::isRecordingEnabled()
    {
        if ( ! m_isValid )
        {
            reload();
        }
        return m_enabled;
    }


    void RecordableItem::invalidate()
    {
        FUNCTION_ENTRY( "invalidate" );

        m_isValid = false;
    }


    void RecordableItem::reload()
    {
        FUNCTION_ENTRY( "reload" );

        IRecordableItemPtr copyOfThis = RecordableItemAccessFactory::getInstance().getRecordableItem( m_id );
        
        m_typeKey = copyOfThis->getTypeKey();
        m_enabled = copyOfThis->isRecordingEnabled();
        m_isValid = true;
    }
}

