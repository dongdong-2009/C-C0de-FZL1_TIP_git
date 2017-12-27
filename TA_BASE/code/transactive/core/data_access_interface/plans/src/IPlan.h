 /*
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/IPlan.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This interface contains the database retrieval and write methods for retrieving plan information
  */

#if !defined(IPlan_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)
#define IPlan_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_

#include <string>

namespace TA_Base_Core
{
    class IPlan
    {

    public:

        virtual ~IPlan() { };

       
        /**
         * getName
         *
         * This returns the name of the plan.
         *
         * @return string - This is the name of the plan
         *
         * @exception DataException - This is thrown if the path is empty or if it denotes a category
         *                            rather than a plan.
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the component data was
         *                                        invalid in some way.
         */
        virtual std::string getName() =0;


        /**
         * getPath
         *
         * This returns the full path of the plan.
         *
         * @return string - Plan path in the format /<category1>/<categoryN>/<planName>
         *
         * @exception DataException - This is thrown if the path is empty or if it denotes a category
         *                            rather than a plan.
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the component data was
         *                                        invalid in some way.
         */
        virtual std::string getPath() =0;

    };
} //end namespace TA_Base_Core

#endif // !defined(IPlan_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)
