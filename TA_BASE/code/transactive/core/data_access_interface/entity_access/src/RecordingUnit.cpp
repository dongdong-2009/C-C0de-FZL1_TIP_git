/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RecordingUnit.cpp $
  * @author Robert Young 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RecordingUnit is a derivation of EntityData. It provides access to RecordingUnit entity data and a specific framework
  * for RecordingUnit entity parameters.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <sstream>

#include "core/data_access_interface/entity_access/src/RecordingUnit.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    // 
    // The entity type is fixed for all instances of this class.
    // 

    const std::string RecordingUnit::ENTITY_TYPE            = "RecordingUnit";

    //
    // Console param name
    //

    const std::string RecordingUnit::CONSOLE                = "Console";

    // 
    // Supported Features
    //
    
    const std::string RecordingUnit::PLAYBACK               = "PlaybackSupported";
    const std::string RecordingUnit::RECORDING              = "RecordingSupported";
    const std::string RecordingUnit::RECORDING_SPEED        = "RecordingSpeedSupported";
    const std::string RecordingUnit::RECORDING_UNIT_TYPE    = "RecordingUnitType";



    //
    // RecordingUnit
    //
    RecordingUnit::RecordingUnit()
        : VideoSwitchResource( ENTITY_TYPE )
    {
    }


    //
    // RecordingUnit
    //
    RecordingUnit::RecordingUnit( unsigned long key )
        : VideoSwitchResource( key, ENTITY_TYPE )
    {
    }


    ///////////////////////////////////////////////////////////////////
    // 
    // Supported Features
    //
    ///////////////////////////////////////////////////////////////////


    //
    // isPlaybackSupported
    //
    bool RecordingUnit::isPlaybackSupported()
    {
        return (getHelper()->getParameter(PLAYBACK) == PARAMETER_TRUE);
    }


    //
    // isRecordingSupported
    //
    bool RecordingUnit::isRecordingSupported()
    {
        return (getHelper()->getParameter(RECORDING) == PARAMETER_TRUE);
    }


    //
    // isRecordingSpeedSupported
    //
    bool RecordingUnit::isRecordingSpeedSupported()
    {
        return (getHelper()->getParameter(RECORDING_SPEED) == PARAMETER_TRUE);
    }


    //
    // getConsoleKey
    //
    unsigned long RecordingUnit::getConsoleKey()
    {            
		std::string value = getHelper()->getParameter(CONSOLE);
        return EntityHelper::getUnsignedLongData(value);
    }


    //
    // getRecordingUnitType
    //
    RecordingUnit::ERecordingUnitType RecordingUnit::getRecordingUnitType()
    {
		std::string value = getHelper()->getParameter(RECORDING_UNIT_TYPE);
		int type = EntityHelper::getIntegerData(value);
		RecordingUnit::ERecordingUnitType recordingUnitType = DVD;
		switch( type )
		{
		case VCR:
			recordingUnitType = VCR;
			break;
		case DVD:
			recordingUnitType = DVD;
			break;
		}

		return recordingUnitType;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string RecordingUnit::getType()
    {
        return getStaticType();
    }


    //
    // getStaticType
    //
    std::string RecordingUnit::getStaticType()
    {
        return ENTITY_TYPE;
    }


    //
    // clone
    //
    IEntityData* RecordingUnit::clone( unsigned long key )
    {
        return new RecordingUnit( key );        
    }


} // TA_Base_Core
