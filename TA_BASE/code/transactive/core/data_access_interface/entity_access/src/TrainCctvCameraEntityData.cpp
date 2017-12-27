/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TrainCctvCameraEntityData.cpp $
  * @author:  Darren Sampson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * TrainCctvCameraEntityData provide read-only access to AtsAgent entity data.
  */


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/TrainCctvCameraEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"

namespace TA_Base_Core
{
    const std::string TrainCctvCameraEntityData::ENTITY_TYPE                = "TrainCctvCamera";
    const std::string TrainCctvCameraEntityData::CAMERA_ALARM_EXPRESSION    = "CameraAlarmExpression";
	const std::string TrainCctvCameraEntityData::PHYSICAL_TRAIN_NUMBER      = "PhysicalTrainNumber";
	const std::string TrainCctvCameraEntityData::CAMERA_NUMBERS             = "CameraNumbers";
	const std::string TrainCctvCameraEntityData::ASSET_NAME                 = "AssetName";

    TrainCctvCameraEntityData::TrainCctvCameraEntityData()
    {}


    TrainCctvCameraEntityData::TrainCctvCameraEntityData( unsigned long key ) :
		EntityData( key, getStaticType() )
	{
    }


    TrainCctvCameraEntityData::~TrainCctvCameraEntityData()
    {
    }


    std::string TrainCctvCameraEntityData::getType()
	{
		return getStaticType();
    }


	std::string TrainCctvCameraEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    void TrainCctvCameraEntityData::invalidate()
    {
        getHelper()->invalidate();
    }


    IEntityData* TrainCctvCameraEntityData::clone(unsigned long key)
    {
        return new TrainCctvCameraEntityData(key);
    }


	std::string TrainCctvCameraEntityData::getCameraAlarmExpression()
	{
		// Get the parameters for this entity
		return getHelper()->getParameter(CAMERA_ALARM_EXPRESSION);
	}


	unsigned long TrainCctvCameraEntityData::getPhysicalTrainNumber()
	{
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(PHYSICAL_TRAIN_NUMBER);
		unsigned long physicalTrainNumber = 0;
		if (!value.empty())
		{
            physicalTrainNumber = EntityHelper::getUnsignedLongData(value);
		}

        return physicalTrainNumber;
	}


	std::vector<unsigned long> TrainCctvCameraEntityData::getCameraNumbers()
	{
		std::string cameraNumbers = getHelper()->getParameter(CAMERA_NUMBERS);
		return getCameraNumbersFromString(cameraNumbers);
	}


	std::string TrainCctvCameraEntityData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}


	std::vector<unsigned long> TrainCctvCameraEntityData::getCameraNumbersFromString( const std::string& cameraNumbersString)
	{
        std::vector<unsigned long> cameras;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( "," );
        tokenizer tokens(cameraNumbersString, sep);
        
        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
			unsigned long number = atoi((*tok_iter).c_str());
			if(number!=0)
			{
				cameras.push_back(number);
			}
			else
			{
	            std::ostringstream message;
				message << "Invalid Camera List for trainCameraEntity: '" << getKey() << "'";
				const char* msg = message.str().c_str();

				TA_THROW( DataException(msg, DataException::NO_VALUE, "Camera Numbers") );
			}
        }

        // if parts is empty, then this should return the entire string
        if (cameras.size() == 0)
        {
			unsigned long number = atoi(cameraNumbersString.c_str());
			if(number!=0)
			{
				cameras.push_back(number);
			}
			else
			{
	            std::ostringstream message;
				message << "Invalid Camera List for trainCameraEntity: '" << getKey() << "'";
				const char* msg = message.str().c_str();

				TA_THROW( DataException(msg, DataException::NO_VALUE, "Camera Numbers") );
			}
        }

        return cameras;
	}

}

