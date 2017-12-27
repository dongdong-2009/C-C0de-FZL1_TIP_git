#if !defined( Sequence_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define Sequence_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Sequence.h $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Sequence is a concrete implementation of ISequence, which is in turn an implementation of
  * IEntityData. It provides access to Sequence entity data and a specific framework
  * for Sequence entity parameters.
  */


#include "core/data_access_interface/entity_access/src/VideoInput.h"

namespace TA_Base_Core
{
    class Sequence : public VideoInput
    {
    public:
        /**
          * Sequence
          *
          * Constructs a new Sequence object that contains only the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        Sequence();

        /**
          * Sequence
          *
          * Constructs a new Sequence object with the specified key.
          *
          * @param   unsigned long key 
          *          The key to this Sequence in the database.
          */

        Sequence( unsigned long key );

        /**
          * ~Sequence
          *
          * Standard destructor.
          */

        virtual ~Sequence() {};

        ///////////////////////////////////////////////////////////////////
        // 
        // Supported Features
        //
        ///////////////////////////////////////////////////////////////////

        /**
          * isReadOnly
          *
          * Returns a bool indicating if this Sequence is read only or configurable.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the ReadOnly parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the ReadOnly parameter value 
          *             cannot be converted to a bool.
          */

        bool isReadOnly();

        /**
          * setReadOnly
          *
          * Stores if this Sequence is read only or configurable.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isReadOnly    
          *             True  => The Sequence is read only. 
          *                      Sets the ReadOnly parameter to 1.
          *             False => Sets the ReadOnly parameter to 0.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parameter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setReadOnly( bool isReadOnly );

        /**
          * getDwellTimeSecs
          *
          * Returns the dwell time in seconds for this Sequence.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     unsigned short    
          *             The dwell time in seconds.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the DwellTimeSecs parameter value 
          *             cannot be converted to an unsigned short.
          */

        unsigned short getDwellTimeSecs();

        /**
          * setDwellTimeSecs
          *
          * Stores the dwell time in seconds for this Sequence.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool dwellTimeSecs
          *             The dwell time in seconds.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          */

        void setDwellTimeSecs( unsigned short dwellTimeSecs );


		// TD16691
        /**
          * getSequenceDescription
          *
          * Returns the description for this Sequence.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     std::string    
          *             The description.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the DwellTimeSecs parameter value 
          *             cannot be converted to an unsigned short.
          */

		std::string getSequenceDescription();

        /**
          * setSequenceDescription
          *
          * Stores the description for this Sequence.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      std::string sequenceDescription
          *             The description.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          */

        void setSequenceDescription( const std::string& sequenceDescription );
		// TD16691

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
          * Returns a pointer to a new Sequence object that has been initialised with the 
          * specified key.
          *
          * @param  unsigned long key 
          *         The key with which to construct the Sequence object.
          */

        static IEntityData* clone( unsigned long key );

    private:

        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

        // 
        // Entity parameters
        //

        static const std::string READ_ONLY;

        // 
        // Entity parameters TD16691
        //

        static const std::string SEQUENCE_DESCRIPTION;

        //
        // Disable copy constructor and assignment operator
        //

        Sequence( const Sequence& theSequence );
        Sequence& operator=( const Sequence& theSequence );

    }; // Sequence

    
    typedef boost::shared_ptr<Sequence> SequencePtr;
} // TA_Base_Core

#endif // !defined(Sequence_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
