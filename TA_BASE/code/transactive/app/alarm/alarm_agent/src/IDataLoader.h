/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/IDataLoader.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This interface allows the DatabaseLoader to make callbacks
  * to implementing classes to instruct them to attempt to load
  * data from the database or to request it from their controlling
  * agent.
  */
#if !defined(IDATALOADER)
#define IDATALOADER

namespace TA_Base_App
{

    class IDataLoader
    {
    public:

        /**
          * Constructor
          */
        IDataLoader(){}

        /** 
          * Virtual Destructor
          */
        virtual ~IDataLoader(){}

        /** 
          * loadFromDatabase
          *
          * This method will be called by the DatabaseLoader to instruct the
          * IDataLoader to load its required data from the database.
          */
        virtual void loadFromDatabase() = 0;

        /** 
          * requestFromControllingAgent
          *
          * This method will be called by the DatabaseLoader to instruct the
          * IDataLoader to request its required data from the agent running in
          * control mode.
          */
        virtual void requestFromControllingAgent() = 0;

        /**
          * hasDataLoaded
          *
          * @return TRUE if data has been successfully loaded, FALSE otherwise
          */
        virtual bool hasDataLoaded() = 0;

    private:
        
        // Disable constructors and assignment operator
        IDataLoader( const IDataLoader& theIDataLoader);
        IDataLoader& operator=(const IDataLoader&);
    };
}

#endif // !defined(IDATALOADER)
