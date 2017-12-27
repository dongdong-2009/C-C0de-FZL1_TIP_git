/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IQuickFindEntity.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IQuickFindentity is an interface to an entity which has quick find capabilities.
  */

#if !defined(IQuickFindEntity_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IQuickFindEntity_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

namespace TA_Base_Core
{

    class IQuickFindEntity
    {

    public:
        virtual ~IQuickFindEntity() { };

        /**
         * getSessionId
         *
         * Returns the key for this entity
         *
         * @return The key for this entity as an unsigned long.
         */
        virtual unsigned long getKey() =0;

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
        virtual std::string getName() =0;


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
        virtual std::string getDescription() =0;

        
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
        virtual unsigned long getGuiApplicationKey() =0;

        
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
        virtual std::string getAlarmDisplay() =0;

        
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
        virtual std::string getCameraAssociation() =0;

        
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
        virtual unsigned long getCameraPresetAssociation() =0;

        
        /**
         * invalidate
         *
         * Make the data contained by this Session as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };
} //close namespace TA_Base_Core

#endif // !defined(IQuickFindEntity_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
