/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Sequence.cpp $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Sequence is a concrete implementation of ISequence, which is in turn an implementation of
  * IEntityData. It provides access to Sequence entity data and a specific framework
  * for Sequence entity parameters.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    // 
    // The entity type is fixed for all instances of this class.
    // 

    const std::string Sequence::ENTITY_TYPE = "Sequence";

    // 
    // Entity parameters
    //
    
    const std::string Sequence::READ_ONLY       = "ReadOnly";

	//
	// Entity parameters TD16691
	const std::string Sequence::SEQUENCE_DESCRIPTION	= "SequenceDescription";


    //
    // Sequence
    //
    Sequence::Sequence()
        : VideoInput( ENTITY_TYPE )
    {
    }


    //
    // Sequence
    //
    Sequence::Sequence( unsigned long key ) 
        : VideoInput( key, ENTITY_TYPE )
    {
    }


    ///////////////////////////////////////////////////////////////////
    // 
    // Supported Features
    //
    ///////////////////////////////////////////////////////////////////


    //
    // isReadOnly
    //
    bool Sequence::isReadOnly()
    {
		std::string value = getHelper()->getParameter(READ_ONLY);
		bool isReadOnly = false; 
		if (!value.empty())
		{
			isReadOnly = EntityHelper::getBooleanData(value);
		}

		return isReadOnly;
    }


    //
    // setReadOnly
    //
    void Sequence::setReadOnly( bool isReadOnly )
    {
        setParameter( READ_ONLY, isReadOnly);
    }

	// TD16691
	std::string Sequence::getSequenceDescription()
	{
		return getHelper()->getParameter(SEQUENCE_DESCRIPTION);
	}

	void Sequence::setSequenceDescription( const std::string& sequenceDescription )
	{
		setParameter(SEQUENCE_DESCRIPTION, sequenceDescription);
	}
	// TD16691

    //
    // getDwellTimeSecs
    //
    /*unsigned short Sequence::getDwellTimeSecs()
    {
        validateData();
        return m_dwellTimeSecs;
    }*/


    //
    // setDwellTimeSecs
    //
    /*void Sequence::setDwellTimeSecs( unsigned short dwellTimeSecs )
    {
        setParameter( DWELL_TIME_SECS, dwellTimeSecs, m_dwellTimeSecs );
    }*/


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string Sequence::getType()
    {
        return getStaticType();
    }

    std::string Sequence::getStaticType()
    {
        return ENTITY_TYPE;
    }


    //
    // clone
    //
    IEntityData* Sequence::clone( unsigned long key )
    {
        return new Sequence( key );        
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Protected methods
    //
    ///////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////
    //
    // Private methods
    //
    ///////////////////////////////////////////////////////////////////////


} // TA_Base_Core
