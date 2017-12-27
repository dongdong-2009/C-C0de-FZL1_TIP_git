#if !defined(IItemLoader_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_)
#define IItemLoader_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_

/**
  * IItemLoader.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/IItemLoader.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a contract between a object that stores items and an actual Item Loader.
  * The storage class will call methods on this interface to retrieve items from the database.
  *
  */

namespace TA_Base_App
{
    //
    // IItemLoader
    //
    class IItemLoader
    {

    public:

        /**
          * ~IItemLoader
          * 
          * Destructor.
          */
        virtual ~IItemLoader() {}

        /**
          * retrieveFromDatabase
          * 
          * This method executes the sql query to retrieve the items from the database.
          */
        virtual void retrieveFromDatabase() =0;
    };
}

#endif // !defined(IItemLoader_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_)
