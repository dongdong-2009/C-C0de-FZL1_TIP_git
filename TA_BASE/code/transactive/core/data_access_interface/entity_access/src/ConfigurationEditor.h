/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ConfigurationEditor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ControlStation holds the data specific to a ControlStation entity from the
  * database and allows read-only access to that data.
  *
  */

#if !defined(ConfigurationEditor_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
#define ConfigurationEditor_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class ConfigurationEditor : public EntityData
    {
    public:

        /**
         * ControlStation (constructor)
         *
         * Constructs a new ControlStation object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        ConfigurationEditor() {};


        /**
         * ControlStation (constructor)
         *
         * Constructs a new ControlStation object with the specified key.
         *
         * @param key The key to this ControlStation in the database.
         */
        ConfigurationEditor(unsigned long key);


        /**
         * Destructor
         */
        virtual ~ConfigurationEditor();


        /**
         * isConfigurationEditorAlreadyRunning
         *
         * This method checks if there is a Configuration Editor already connected to the schema
         * this Configuration Editor is connected to
         * 
         * @param string& - This will be populated with the name of the workstation already running the
         *                  Configuration Editor. This is only populated if the return value is true.
         *
         * @return bool - True if a Configuration Editor is already running. False if not.
         */
        bool isConfigurationEditorAlreadyRunning(std::string& workstationName);


        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
		static std::string getStaticType();

        /**
         * clone
         *
         * Returns a pointer to a new ControlStation object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the ControlStation object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

		/**
         * getShowRegion
         *
         * Returns true if the region field should be shown, otherwise it returns
         * false.  This functions retrives the ShowRegion flag from the database.
		 *
         * @return Boolean flag to indicate if the region data should be shown.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        static bool getShowRegion();

        /**
         * getEventLevels
         *
         * Returns the number of event leves in the system.
         *
         * @return unsigned int The number of event levels.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        static unsigned int getEventLevels();

    private:
        // Private as they are not used.
        ConfigurationEditor( const ConfigurationEditor& theControlStation);            
		ConfigurationEditor& operator=(const ConfigurationEditor &);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

		//TD10758 marc_bugfix
       /**
         * checkConfigurationEditorTimeStamp
         *
         * Returns true if previous timestamp and current timestamp is below a specified range
         *
         * @return bool The result of timestamp checking
         *
         */
		bool checkConfigurationEditorTimeStamp();

    };
    
    typedef boost::shared_ptr<ConfigurationEditor> ConfigurationEditorPtr;
} //close namespace TA_Base_Core

#endif // !defined(ConfigurationEditor_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
