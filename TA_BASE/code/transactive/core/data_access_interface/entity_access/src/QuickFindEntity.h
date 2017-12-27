/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/QuickFindEntity.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * QuickFindEntity is an implementation of IQuickFindEntity. It holds the data specific to an entity that
  * is a device. This device allows quick launch of its controller, associated display and/or associated
  * camera image from the Control Station.
  */

#if !defined(QuickFindEntity_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define QuickFindEntity_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/entity_access/src/IQuickFindEntity.h"

namespace TA_Base_Core
{
    class QuickFindEntity : public IQuickFindEntity
    {

    public:

        /**
         * Constructor
         *
         * Construct an QuickFindEntity class based around a key.
         *
         * @param key The key of this entity in the database
         * @param name The name of this entity in the database
         * @param description The description of this entity in the database
         * @param guiApp The key of the related GUI application for this entity.
         */
        QuickFindEntity(const unsigned long key, const std::string& name,
                        const std::string& description, const unsigned long guiApp);

        /**
         * Destructor
         */
        virtual ~QuickFindEntity();

        /**
         * getQuickFindEntityId
         *
         * Returns the key for this entity
         *
         * @return The key for this entity as an unsigned long.
         */
        virtual unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of the entity
         *
         * @return The name of the entity as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getName();


        /**
         * getDescription
         *
         * Returns the description of the entity
         *
         * @return The description of the entity as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getDescription();

        
        /**
         * getGuiApplicationKey
         *
         * Returns the key of the control GUI for this entity.
         *
         * @return An unsigned long which is a key for a GUIApplication entry. This will return
         *         0 if no control GUI is configured for this entity type.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual unsigned long getGuiApplicationKey();

        
        /**
         * getAlarmDisplay
         *
         * Returns the alarm display linked to the entity
         *
         * @return The name and path of the GraphWorX display. An empty string will be returned
         *         if there is no associated display.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getAlarmDisplay();

        
        /**
         * getCameraAssociation
         *
         * Returns the name of the camera associated with this entity
         *
         * @return The name of the camera associated with this entity. This will be an empty
         *         string if there is no associated camera.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getCameraAssociation();

        
        /**
         * getCameraPresetAssociation
         *
         * Returns the preset of the camera associated with this entity. An entity will only have
         * a preset association if it has a camera association. An entity can have a camera association
         * but no preset association.
         *
         * @return The number of the camera preset. 0 if no preset is configured.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual unsigned long getCameraPresetAssociation();

        
        /**
         * invalidate
         *
         * Make the data contained by this QuickFindEntity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
        // Copy constructor and assignment operator are not used so they are made private
        QuickFindEntity( const QuickFindEntity& theQuickFindEntity);            
		QuickFindEntity& operator=(const QuickFindEntity &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        void reload();

        static const std::string CAMERA_ASSOCIATION;
        static const std::string CAMERA_PRESET_ASSOCIATION;
        static const std::string ALARM_DISPLAY;

        unsigned long m_key;
        std::string   m_name;
        std::string   m_description;
        unsigned long m_guiApplicationKey;
        std::string   m_alarmDisplay;
        std::string   m_cameraAssociation;
        unsigned long m_cameraPresetAssociation;

        bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // !defined(QuickFindEntity_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
