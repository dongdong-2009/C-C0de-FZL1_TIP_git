#if !defined( BVSSTAGE_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define BVSSTAGE_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/BVSStage.h $
  * @author Robert Young
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  */

#include "core/data_access_interface/entity_access/src/VideoInput.h"
#include <vector>

namespace TA_Base_Core
{
    class BVSStage : public VideoInput
    {
    public:
        /**
          * BVSStage
          *
          * Constructs a new BVSStage object that contains only the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        BVSStage();

        /**
          * BVSStage
          *
          * Constructs a new BVSStage object with the specified key.
          *
          * @param   unsigned long key 
          *          The key to this BVSStage in the database.
          */

        BVSStage( unsigned long key );

        /**
          * ~BVSStage
          *
          * Standard destructor.
          */

        virtual ~BVSStage() {};
      
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
          * Returns a pointer to a new BVSStage object that has been initialised with the 
          * specified key.
          *
          * @param  unsigned long key 
          *         The key with which to construct the BVSStage object.
          */

        static IEntityData* clone(unsigned long key);

		unsigned long getEndLocation();

		unsigned long getStartLocation();

		std::vector<unsigned long> getLocationsForStage();

		bool isLocationPartOfStage(unsigned long locationKey);

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
        // Disable copy constructor and assignment operator
        //

        BVSStage( const BVSStage& theBVSStage );
        BVSStage& operator=( const BVSStage& theBVSStage );

        //
        // The supported features.
        //

		static const std::string START_LOCATION;
		static const std::string END_LOCATION;

		unsigned long m_startLocation;
		unsigned long m_endLocation;
		std::vector<unsigned long> m_locations;

    }; // BVSStage

    
    typedef boost::shared_ptr<BVSStage> BVSStagePtr;
} // TA_Base_Core

#endif // !defined(BVSSTAGE_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
