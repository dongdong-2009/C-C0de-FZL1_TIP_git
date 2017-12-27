/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/system_controller/src/SubSystemControllerAccessFactory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * SubSystemControllerAccessFactory is a singleton that is
  * used to retrieve ISystemControllerData objects from
  * the database.
  *
  */

#ifndef SubSystemControllerAccessFactory_H
#define SubSystemControllerAccessFactory_H

#include <string>
#include <vector>

namespace TA_Base_App
{
    class SubSystemControllerAccessFactory
    {

    public:
        virtual ~SubSystemControllerAccessFactory() {};

		/**
		  * getInstance
		  *
		  * Creates and returns an instance of this object.
		  *
		  * @return A reference to an instance of an SubSystemControllerAccessFactory object.
		  */
		static SubSystemControllerAccessFactory& getInstance();

        /** 
          * getAllSystemControllerNames
          *
          * Returns all the System Controllers
          *
          * param string vector - A vector of system controller names
          *
          * @exception DatabaseException  Problem communicating with the database.
          * @exception DataException Problem converting the data retrieved
          */
        void getAllSystemControllerNames( std::vector<std::string>& vecHostNames );

    private:
        SubSystemControllerAccessFactory() {};
		SubSystemControllerAccessFactory(const SubSystemControllerAccessFactory&);
		SubSystemControllerAccessFactory& operator=(const SubSystemControllerAccessFactory &);

        static SubSystemControllerAccessFactory* m_instance;
    };

} // namespace TA_Base_Core

#endif // SubSystemControllerAccessFactory_H
