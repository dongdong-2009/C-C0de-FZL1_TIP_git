#if !defined(ArchiveInventoryAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
#define ArchiveInventoryAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveInventoryAccessFactory.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveInventoryAccessFactory is a singleton that is used to generate and retrieve 
  * IArchiveTables and IArchiveInventory objects.
  */

#include <string>

namespace TA_Base_Core
{
    class IArchiveInventory;    // Forward declaration.

    class ArchiveInventoryAccessFactory
    {
    public:

        /**
         * ~ArchiveInventoryAccessFactory
         *
         * Standard destructor.
         */

        virtual ~ArchiveInventoryAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      ArchiveInventoryAccessFactory&
         *              A reference to an instance of a ArchiveInventoryAccessFactory object.
         */

        static ArchiveInventoryAccessFactory& getInstance()
        {
            static ArchiveInventoryAccessFactory fact;
            return fact;
        }

        /**
         * getArchiveInventory
         *
         * Retrieves the archives that have been restored. 
         *
         * @return      IArchiveInventory*
         *              A pointer to an object conforming to the IArchiveInventory interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         */

        IArchiveInventory* getArchiveInventory();

    private:
        //
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        //
        ArchiveInventoryAccessFactory() {};
        ArchiveInventoryAccessFactory& operator=(const ArchiveInventoryAccessFactory &) {};
        ArchiveInventoryAccessFactory( const ArchiveInventoryAccessFactory& ) {};
    };

} // end namespace TA_Base_Core

#endif // !defined(ArchiveInventoryAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
