 /*
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/PlanDAI.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This implements the IPlan interface and allows the user read-only access to a single plan
  */


#if !defined(Plan_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
#define Plan_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_

#include "core/data_access_interface/plans/src/IPlan.h"

namespace TA_Base_Core
{
    class PlanDAI : public IPlan
    {

    public:

        /**
         * Constructor
         *
         * @param const std::string& - The full path of this plan in the format "[/<category1>/<categoryN>]/<planName>"
         */
        PlanDAI(const std::string& planPath);
        /**
         * Constructor
         *
         * @param const unsigned long - The node id of this plan"
         */
        PlanDAI(const unsigned long ulNodeId);


        /**
         * Destructor
         */
        virtual ~PlanDAI();

        
        /**
         * getName
         *
         * This returns the name of the plan.
         *
         * @return string - This is the name of the plan extracted from the plan's full path
         *
         * @exception DataException - This is thrown if the path is empty or if it denotes a category
         *                            rather than a plan.
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the component data was
         *                                        invalid in some way.
         */
        virtual std::string getName();


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
        virtual std::string getPath();


        /**
         * invalidate
         *
         * Make the data contained by this Location as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        PlanDAI& operator=(const PlanDAI &) { };
        PlanDAI( const PlanDAI& ) {};

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... 
         * or set... method is called and the data state is not valid.
         *
         * pre: Either - this plan was initially loaded from the database
         */
        void reload();

		/** 
		  * refreshNodeId()
		  *
		  * Load from the database and store internally the node ID corresponding to the plan's path.
          *
          * DataException is thrown if no records or multiple records are found.
          * DatabaseException is thrown if errors occur while accessing the database.
          *
		  */
        void refreshNodeId();

		/** 
		  * refreshPath()
		  *
		  * Load from the database and store internally the path corresponding to the plan id.
          *
          * DataException is thrown if no records or multiple records are found.
          * DatabaseException is thrown if errors occur while accessing the database.
          *
		  */
        void refreshPath();

		/** 
		  * validateNodeType()
		  *
		  * Ensure the specified path represents a plan node.
          *
          * DataException is thrown if no records or multiple records are found, or the node is a category.
          * DatabaseException is thrown if errors occur while accessing the database.
          *
		  */
        void validateNodeType();

 
    private:

        std::string m_path;      
        unsigned long m_nodeId;

        bool m_isValidData;
    };
} //end namespace TA_Base_Core

#endif // !defined(Component_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
