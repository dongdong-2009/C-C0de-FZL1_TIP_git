/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/PrimitiveWrapper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PrimitiveWrapper is a template class that wraps primitive types, adding to them a 'set' state
  * This removes to need to set the primitive to a magic value flagging an unset state (which is
  * typically the value 0 or -1).
  * If you get linker errors, look to head of PrimitiveWrapper source file and add in template declarations which
  * will force the compiler to compile relevant template specializations into the obj file.
  */

#if !defined(PrimitiveWrapper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PrimitiveWrapper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include "core/exceptions/src/ValueNotSetException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    template <class Type> class PrimitiveWrapper 
    {
        public:
    
            /** 
             * PrimitiveWrapper
             *
             * This constructor creates a wrapper for a primitive that is initially unset
             *
             */
            PrimitiveWrapper() :
              // m_value(0),  // Commented out to allow classes objects to be used (not just primitive types)
              m_valueSet(false)
            {}
    
            /** 
             * PrimitiveWrapper
             *
             * This constructor creates a wrapper for a primitive that is initially set
             * to the value passed in
             *
             * @param startingValue the value this container will be set to      
             *
             */
            PrimitiveWrapper(const Type& startingValue) :
              // m_value(0),  // Commented out to allow classes objects to be used (not just primitive types)
              m_valueSet(false)
            {
                setValue(startingValue);           
            }
    		
            /** 
             * PrimitiveWrapper
             *
             * Copy constructor
             *
             * @param thePrimitiveWrapper copy source     
             *
             */
            PrimitiveWrapper( const PrimitiveWrapper<Type>& thePrimitiveWrapper) :
              // m_value(0),  // Commented out to allow classes objects to be used (not just primitive types)
              m_valueSet(false)
    
            {
                *this = thePrimitiveWrapper;                
            }
        
            /** 
             * =
             *
             * Assignment operator
             *
             * @param thePrimitiveWrapper copy source     
             *
             */
    		PrimitiveWrapper<Type>& operator=(const PrimitiveWrapper<Type>& thePrimitiveWrapper)
            {            
                m_value = thePrimitiveWrapper.m_value;
                m_valueSet = thePrimitiveWrapper.m_valueSet;
                return *this;
            }
    
            virtual ~PrimitiveWrapper() {}

            /**
             * setValue
             * 
             * Updates the value of this container
             *
             * @param value the value this container will be set to
             *
             */
            void setValue(const Type& value)
            {
                m_value = value;
                m_valueSet = true;
            }
    
            /**
             * hasBeenSet
             * 
             * Returns the 'set' status of this container
             *
             * @return true if internal value set, false otherwise
             *
             */
            bool hasBeenSet() const
            {
                return m_valueSet;
            }
    
            /**
             * resetValue
             * 
             * Resets the 'set' state of this container, subsequent calls to getValue
             * will throw an exception if the value is not set
             */
            void resetValue()
            {
                m_valueSet = false;
            }
    
            /**
             * getValue
             *
             * Retrieves the value set for this container, provided the value has been set
             * 
             * @return the value currently set for this container
             *
             * @throws ValueNotSetException 
             *      - If the value has not been internally set, either through constructor or setValue
             *      - If resetValue was called, and value not set again with setValue
            */
            const Type& getValue() const;

            /**
             * hasBeenSetToValue
             *
             * @return false if no value set for this wrapper, or the value set doesn't
             *              correspond to the input test value
             *
             * @param value the value to compare with
             *
             */
            bool hasBeenSetToValue(const Type& value) const
            {
                if (hasBeenSet())
                {
                    return value == m_value;
                }
                else
                {
                    return false;
                }
            }

        private:

            // Internal value contained by this wrapper
            Type m_value;
    
            // the 'set' status of m_value, false if currently not set
            bool m_valueSet;
    };

    
    /////////////////////////////////
    //// Inline function definitions
    /////////////////////////////////
    
    template <class Type>
    const Type& PrimitiveWrapper<Type>::getValue() const
    { 
        if ( !m_valueSet ) 
        {                
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }
        return m_value; 
    }
}    

#endif // !defined(PrimitiveWrapper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
