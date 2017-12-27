/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/CallBannerPage.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifndef CallBannerPage_ENTITY_DATA_H
#define CallBannerPage_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class CallBannerPage : public EntityData
    {
    public:
        /**
         * implement methods as required by the interfaces from
         * which this class inherits
         */
		CallBannerPage();
        CallBannerPage( unsigned long key );
        virtual ~CallBannerPage();
        virtual void invalidate();
        virtual std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);
    
        /**
         * getCarOnePecOneCamera
         *
         * Gets the name of the tab that should be shown on startup
         */
        int getCarOnePecOneCamera();

        /**
         * getCarOnePecTwoCamera
         *
         * Gets the name of the tab that should be shown on startup
         */
		int getCarOnePecTwoCamera();

        /**
         * getCarTwoPecOneCamera
         *
         * Gets the name of the tab that should be shown on startup
         */
        int getCarTwoPecOneCamera();

        /**
         * getCarTwoPecTwoCamera
         *
         * Gets the name of the tab that should be shown on startup
         */
		int getCarTwoPecTwoCamera();

        /**
         * getCarThreePecOneCamera
         *
         * Gets the name of the tab that should be shown on startup
         */
        int getCarThreePecOneCamera();

        /**
         * getCarThreePecTwoCamera
         *
         * Gets the name of the tab that should be shown on startup
         */
		int getCarThreePecTwoCamera();

    protected:

    private:
        //
        // Disable copy constructor and assignment operator
        //

        CallBannerPage( const CallBannerPage& theCallBannerPage );
        CallBannerPage& operator=( const CallBannerPage& theCallBannerPage );
        
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string CAR_ONE_PEC_ONE_CAMERA;
        static const std::string CAR_ONE_PEC_TWO_CAMERA;
        static const std::string CAR_TWO_PEC_ONE_CAMERA;
        static const std::string CAR_TWO_PEC_TWO_CAMERA;
        static const std::string CAR_THREE_PEC_ONE_CAMERA;
        static const std::string CAR_THREE_PEC_TWO_CAMERA;

        bool m_isValidData;
       
        int m_carOnePecOneCamera;
		int m_carOnePecTwoCamera;
		int m_carTwoPecOneCamera;
		int m_carTwoPecTwoCamera;
		int m_carThreePecOneCamera;
		int m_carThreePecTwoCamera;

   	};
    
    typedef boost::shared_ptr<CallBannerPage> CallBannerPagePtr;
} //close namespace TA_Base_Core 

#endif // CallBannerPage_ENTITY_DATA_H

