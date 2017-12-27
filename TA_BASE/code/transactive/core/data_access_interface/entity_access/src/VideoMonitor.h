#if !defined( VideoMonitor_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define VideoMonitor_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoMonitor.h $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * VideoMonitor provides access to VideoMonitor entity data and a specific framework
  * for VideoMonitor entity parameters. 
  * Inherits from the VideoOutput class.
  */

#include "core/data_access_interface/entity_access/src/VideoOutput.h"
#define VM_ALARM_TYPE_NONE			 0
#define VM_ALARM_TYPE_VSS_TRIGERING  1
#define VM_ALARM_TYPE_TVSS_TRIGERING 2

namespace TA_Base_Core
{
    class VideoMonitor : public VideoOutput
    {
    public:
        /**
          * VideoMonitor
          *
          * Constructs a new VideoMonitor object that contains the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        VideoMonitor();

        /**
          * VideoMonitor
          *
          * Constructs a new VideoMonitor object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this VideoMonitor in the database.
          */

        VideoMonitor( unsigned long key );

        /**
          * ~VideoMonitor
          *
          * Standard destructor.
          */

        virtual ~VideoMonitor() {};

        /**
          * getSize
          *
          * Gets the size of the monitor.
          *
          * @return     std::string
          *             The VideoMonitor size.
          *             This will return "Small" if there has been an error 
          *             setting the size or reloading the size from the database.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the Size parameter value 
          *             cannot be converted to a string.
          */

        std::string getSize();

        /**
          * setSize
          *
          * Sets the size of the monitor.  The size will be set to "Small"
          * if the specified value is invalid.
          *
          * Precondition: size is SMALL, MEDIUM or LARGE.
          *
          * @param      const std::string& size
          *             The VideoMonitor size.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setSize( const std::string& size );
        
        /** 
          * getConsoleKey
          *
          * Gets the key of the Console to which this VideoMonitor is attached 
          * (0 if this is not a console monitor).
          *
          * @return - the key of the Console to which this VideoMonitor is attached 
          * (0 if this is not a console monitor)
          *
          * @exception  DatabaseException 
          *             Thrown if there is a problem:
          *             1. Establishing a connection with the database, or 
          *             2. While retrieving data.
          * @exception  DataException 
          *             Thrown if:
          *             1. The data cannot be converted to the required format, or
          *             2. If the wrong amount of data is retrieved, or 
          *             3. If the entity key used to idenify this object is invalid.
          */

        unsigned long getConsoleKey();
		
		unsigned int getDisplayOrder();

		bool isAlarmMonitor();

		bool isOverallMonitor();

		unsigned int getAlarmType();
        ///////////////////////////////////////////////////////////////////////
        //
        // IEntityData Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * getType
          *
          * Returns the type of this entity. This method must be callable when the object
          * is constructed using the default constructor. The entity type string should be
          * stored as a static const variable in the concrete class. The entity type should
          * not be loaded from the database (note that this method defines no exceptions).
          *
          * @return      std::string
          *              The entity type of this entity.
          */

	    virtual std::string getType();

	    static std::string getStaticType();

        /**
          * clone
          *
          * Returns a new instance of this entity. This method is used to create a concrete
          * entity based upon type in the EntityAccessFactory.
          *
          * @return     IEntityData*
          *             A new instance of this entity.  It is the caller's responsibility
          *             to clean up the object.
          *
          * @param      unsigned long key 
          *             The pkey of this entity in the database.
          */ 

        static IEntityData* clone( unsigned long key );

    private:
        //
        // Disable copy constructor and assignment operator
        //

        VideoMonitor( const VideoMonitor& theVideoMonitor );
        VideoMonitor& operator=( const VideoMonitor& theVideoMonitor );

        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

        //
        // The entity parameter names.
        //

        static const std::string SIZE;
        static const std::string CONSOLE;
		static const std::string DISPLAYORDER;
		static const std::string ALARMMONITOR;
		static const std::string ISOVERALLMONITOR;

        //
        // The monitor sizes.
        //

        static const std::string SMALL;
        static const std::string MEDIUM;
        static const std::string LARGE;

    }; // VideoMonitor

    
    typedef boost::shared_ptr<VideoMonitor> VideoMonitorPtr;
} // TA_Base_Core

#endif // !defined(VideoMonitor_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
