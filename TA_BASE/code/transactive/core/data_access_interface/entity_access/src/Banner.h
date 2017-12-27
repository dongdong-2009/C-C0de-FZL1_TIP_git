/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Banner.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifndef Banner_ENTITY_DATA_H
#define Banner_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class Banner : public EntityData
    {
    public:
        /**
         * implement methods as required by the interfaces from
         * which this class inherits
         */
		Banner();
        Banner( unsigned long key );
        virtual ~Banner();
        virtual void invalidate();
        virtual std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);
    
        /**
         * getStartTab
         *
         * Gets the name of the tab that should be shown on startup
         */
        std::string getStartTab();

        /**
         * getLeftPosition
         *
         * Gets the position of the left most pixels in the banner
         */
        int getLeftPosition();

        /**
         * getTopPosition
         *
         * Gets the position top most pixels in the banner
         */
        int getTopPosition();

        /**
         * getHeight
         *
         * Gets the height of the banner in pixels
         */
        int getHeight();

        /**
         * getWidth
         *
         * Gets the width of the banner in pixels
         */
        int getWidth();

        /**
         * getCallTypeColour
         *
         * Gets the colour of the given call type.
         *
         * @exception DataException  if the call type is not recognised
         *
         */
        unsigned long getCallTypeColour(const std::string& callType);


    protected:

        /**
         *  startsWith
         *
         *  Indicates if containingString starts with prefix
         *  eg
         *  startsWith("hi there", "hi")   => true
         *  startsWith("hi there", "blah") => false
         */
        bool Banner::startsWith(
            const std::string& containingString,
            const std::string& prefix);


    private:
        //
        // Disable copy constructor and assignment operator
        //
        Banner( const Banner& theBanner );
        Banner& operator=( const Banner& theBanner );
        bool m_isValidData;
       
        std::string m_startTab;
        int m_topPosition;
        int m_leftPosition;
        int m_height;
        int m_width;

        // The map of call type names to colours
        //       [   name  ]  [  colour   ]
        std::map<std::string, unsigned long> m_callTypeColourMap;

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string START_TAB;
        static const std::string TOP_POSITION;
        static const std::string LEFT_POSITION;
        static const std::string HEIGHT;
        static const std::string WIDTH;
        static const std::string CALL_TYPE_COLOUR_PREFIX;
    };

    typedef boost::shared_ptr<Banner> BannerPtr;
} //close namespace TA_Base_Core 

#endif // Banner_ENTITY_DATA_H

