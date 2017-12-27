/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TrainCctvCameraEntityData.h $
  * @author Ross Tucker
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef TRAIN_CCTV_CAMERA_ENTITY_DATA
#define TRAIN_CCTV_CAMERA_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "boost/tokenizer.hpp"

namespace TA_Base_Core
{
    class TrainCctvCameraEntityData : public EntityData
    {

    public:
        TrainCctvCameraEntityData();
        TrainCctvCameraEntityData( unsigned long key );
        virtual ~TrainCctvCameraEntityData();
        virtual void invalidate();
        virtual std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);

		std::string getCameraAlarmExpression();
		unsigned long getPhysicalTrainNumber();
		std::vector<unsigned long> getCameraNumbers();
		std::string getAssetName();

    private:
		std::vector<unsigned long> getCameraNumbersFromString( const std::string& cameraNumbersString);

    private:

        TrainCctvCameraEntityData ( const TrainCctvCameraEntityData & obj ){};
		TrainCctvCameraEntityData & operator= ( const TrainCctvCameraEntityData &){};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string CAMERA_ALARM_EXPRESSION;
	    static const std::string PHYSICAL_TRAIN_NUMBER;
	    static const std::string CAMERA_NUMBERS;
	    static const std::string ASSET_NAME;
    };
    
    typedef boost::shared_ptr<TrainCctvCameraEntityData> TrainCctvCameraEntityDataPtr;
}

#endif // ATS_AGENT_ENTITY_DATA
