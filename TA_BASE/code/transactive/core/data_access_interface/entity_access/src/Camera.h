#if !defined( Camera_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define Camera_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Camera.h $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Camera provides access to Camera entity data and a specific framework
  * for Camera entity parameters. It inherits from EntityData.
  *
  * When adding new camera features, code needs to be added everywhere
  * the phrase "ADD NEW FEATURES HERE" appears.
  */


#include "core/data_access_interface/entity_access/src/VideoInput.h"

namespace TA_Base_Core
{
    class Camera : public VideoInput
    {
    public:
        /**
          * Camera
          *
          * Constructs a new Camera object that contains only the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        Camera();

        /**
          * Camera
          *
          * Constructs a new Camera object with the specified key.
          *
          * @param   unsigned long key 
          *          The key to this Camera in the database.
          */

        Camera( unsigned long key );

        /**
          * ~Camera
          *
          * Standard destructor.
          */

        virtual ~Camera() {};

        ///////////////////////////////////////////////////////////////////
        // 
        // Supported Features
        //
        ///////////////////////////////////////////////////////////////////

        /**
          * isMovementSupported
          *
          * Returns a bool indicating if this Camera supports Movement.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the MovementSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the MovementSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isMovementSupported();

        /**
          * setMovementSupported
          *
          * Stores if this Camera supports Movement.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isMovementSupported    
          *             True  => The Camera supports Movement. 
          *                      Sets the MovementSupported parameter to 1.
          *             False => Sets the MovementSupported parameter to 0.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setMovementSupported( bool isMovementSupported );

        /**
          * isProportionalMovementSupported
          *
          * Returns a bool indicating if this Camera supports Proportional Movement.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the ProportionalMovementSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the ProportionalMovementSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isProportionalMovementSupported();

        /**
          * setProportionalMovementSupported
          *
          * Stores if this Camera supports Proportional Movement.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isProportionalMovementSupported
          *             True  => The Camera supports Proportional Movement. 
          *                      Sets the ProportionalMovmentSupported parameter to 1.
          *             False => Sets the ProportionalMovmentSupported parameter to 0.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          */

        void setProportionalMovementSupported( bool isProportionalMovementSupported );

        /**
          * isAutoFocusSupported
          *
          * Returns a bool indicating if this Camera supports AutoFocus.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the AutoFocusSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the AutoFocusSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isAutoFocusSupported();

        /**
          * setAutoFocusSupported
          *
          * Stores if this Camera supports AutoFocus.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isAutoFocusSupported
          *             True  => The Camera supports AutoFocus. 
          *                      Sets the AutoFocusSupported parameter to 1.
          *             False => Sets the AutoFocusSupported parameter to 0.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setAutoFocusSupported( bool isAutoFocusSupported );

        /**
          * isManualFocusSupported
          *
          * Returns a bool indicating if this Camera supports 
          * manual Focus adjustment.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the ManualFocusSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the ManualFocusSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isManualFocusSupported();

        /**
          * setManualFocusSupported
          *
          * Stores if this Camera supports manual Focus adjustment.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isManualFocusSupported
          *             True  => The Camera supports manual Focus adjustment. 
          *                      Sets the ManualFocusSupported parameter to 1.
          *             False => Sets the ManualFocusSupported parameter to 0.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setManualFocusSupported( bool isManualFocusSupported );

        /**
          * isAutoIrisSupported
          *
          * Returns a bool indicating if this Camera supports AutoIris.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the AutoIrisSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the AutoIrisSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isAutoIrisSupported();

        /**
          * setAutoIrisSupported
          *
          * Stores if this Camera supports AutoIris.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isAutoIrisSupported
          *             True  => The Camera supports AutoIris. 
          *                      Sets the AutoIrisSupported parameter to 1.
          *             False => Sets the AutoIrisSupported parameter to 0.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setAutoIrisSupported( bool isAutoIrisSupported );

        /**
          * isManualIrisSupported
          *
          * Returns a bool indicating if this Camera supports 
          * manual Iris adjustment.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool
          *             True if the ManualIrisSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the ManualIrisSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isManualIrisSupported();

        /**
          * setManualIrisSupported
          *
          * Stores if this Camera supports manual Iris adjustment.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isManualIrisSupported
          *             True  => The Camera supports manual Iris adjustment. 
          *                      Sets the ManualIrisSupported parameter to 1.
          *             False => Sets the ManualIrisSupported parameter to 0.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setManualIrisSupported( bool isManualIrisSupported );

        /**
          * isIrisPeakWhiteInversionSupported
          *
          * Returns a bool indicating if this Camera supports 
          * iris peak white inversion.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool
          *             True if the IrisPeakWhiteInversionSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the IrisPeakWhiteInversionSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isIrisPeakWhiteInversionSupported();

        /**
          * setIrisPeakWhiteInversionSupported
          *
          * Stores if this Camera supports iris peak white inversion.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isIrisPeakWhiteInversionSupported
          *             True  => The Camera supports iris peak white inversion. 
          *                      Sets the IrisPeakWhiteInversionSupported parameter to 1.
          *             False => Sets the IrisPeakWhiteInversionSupported parameter to 0.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setIrisPeakWhiteInversionSupported( bool isIrisPeakWhiteInversionSupported );

        /**
          * isWashSupported
          *
          * Returns a bool indicating if this Camera supports Washing.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the WashSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the WashSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isWashSupported();

        /**
          * setWashSupported
          *
          * Stores if this Camera supports Washing.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isWashSupported
          *             True  => The Camera supports Washing.
          *                      Sets the WashSupported parameter to 1.
          *             False => Sets the WashSupported parameter to 0.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setWashSupported( bool isWashSupported );

        /**
          * isWipeSupported
          *
          * Returns a bool indicating if this Camera supports Wiping.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the WipeSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the WipeSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isWipeSupported();

        /**
          * setWipeSupported
          *
          * Stores if this Camera supports Wiping.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isWipeSupported
          *             True  => The Camera supports Wiping.
          *                      Sets the WipeSupported parameter to 1.
          *             False => Sets the WipeSupported parameter to 0.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setWipeSupported( bool isWipeSupported );

        /**
          * arePresetsSupported
          *
          * Returns a bool indicating if this Camera supports presets.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the PresetsSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the PresetsSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool arePresetsSupported();

        /**
          * setPresetsSupported
          *
          * Stores if this Camera supports Presets.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool arePresetsSupported
          *             True  => The Camera supports Presets.
          *                      Sets the PresetsSupported parameter to 1.
          *             False => Sets the PresetsSupported parameter to 0.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setPresetsSupported( bool arePresetsSupported );

        // ADD NEW FEATURES HERE.

        // TODO: The following items should be added for backwards compatibility to Adelaide/Perth.
        //
        // isGuardTourSupported
        // isQuadViewSupported
        // isAdpro
      
        ///////////////////////////////////////////////////////////////////////
        //
        // IEntityData methods
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
          * Returns a pointer to a new Camera object that has been initialised with the 
          * specified key.
          *
          * @param  unsigned long key 
          *         The key with which to construct the Camera object.
          */

        static IEntityData* clone(unsigned long key);

    protected:

        ///////////////////////////////////////////////////////////////////////
        //
        // VideoInput Methods
        //
        ///////////////////////////////////////////////////////////////////////
       


    private:

        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

        // 
        // Supported Features
        //

        static const std::string MOVEMENT;
        static const std::string PROPORTIONAL_MOVEMENT;
        static const std::string AUTO_FOCUS;
        static const std::string MANUAL_FOCUS;
        static const std::string AUTO_IRIS;
        static const std::string MANUAL_IRIS;
        static const std::string IRIS_PEAK_WHITE_INVERSION;
        static const std::string WASH;
        static const std::string WIPE;
        static const std::string PRESETS;
        // ADD NEW FEATURES HERE.

        //
        // Disable copy constructor and assignment operator
        //

        Camera( const Camera& theCamera );
        Camera& operator=( const Camera& theCamera );

        /**
          * initialiseParameters
          *
          * Initialises all bool parameters to false and all string parameters to "".
          */

        void initialiseParameters();

        //
        // The supported features.
        //

        bool m_isMovementSupported;  
        bool m_isProportionalMovementSupported;  
        bool m_isAutoFocusSupported;            
        bool m_isManualFocusSupported;         
        bool m_isAutoIrisSupported;           
        bool m_isManualIrisSupported;
        bool m_isIrisPeakWhiteInversionSupported;
        bool m_isWashSupported;             
        bool m_isWipeSupported;            
        bool m_arePresetsSupported;       
        // ADD NEW FEATURES HERE.

    }; // Camera

    
    typedef boost::shared_ptr<Camera> CameraPtr;
} // TA_Base_Core

#endif // !defined(Camera_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
