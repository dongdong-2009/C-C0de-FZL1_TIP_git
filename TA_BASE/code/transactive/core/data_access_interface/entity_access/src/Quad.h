#if !defined( Quad_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define Quad_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Quad.h $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Quad provides access to Quad entity data and a specific framework
  * for Quad entity parameters. It inherits from EntityData.
  */


#include "core/data_access_interface/entity_access/src/VideoInput.h"

namespace TA_Base_Core
{
    class Quad : public VideoInput
    {
    public:
        /**
          * Quad
          *
          * Constructs a new Quad object that contains only the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        Quad();

        /**
          * Quad
          *
          * Constructs a new Quad object with the specified key.
          *
          * @param   unsigned long key 
          *          The key to this Quad in the database.
          */

        Quad( unsigned long key );

        /**
          * ~Quad
          *
          * Standard destructor.
          */

        virtual ~Quad() {};
      
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
          * Returns a pointer to a new Quad object that has been initialised with the 
          * specified key.
          *
          * @param  unsigned long key 
          *         The key with which to construct the Quad object.
          */

        static IEntityData* clone(unsigned long key);

		virtual std::string getIncludedCameras();
    private:

        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

        // 
        // Supported Features
        //

       /* static const std::string TOP_LEFT_VIDEO_INPUT_KEY;
        static const std::string TOP_RIGHT_VIDEO_INPUT_KEY;
        static const std::string BOTTOM_LEFT_VIDEO_INPUT_KEY;
        static const std::string BOTTOM_RIGHT_VIDEO_INPUT_KEY;*/

        //
        // Disable copy constructor and assignment operator
        //

        Quad( const Quad& theQuad );
        Quad& operator=( const Quad& theQuad );

        //
        // The supported features.
        //

    }; // Quad

    
    typedef boost::shared_ptr<Quad> QuadPtr;
} // TA_Base_Core

#endif // !defined(Quad_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
