/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigGui.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * ConfigGui is an implementation of IConfigGui. It holds the data specific to a gui entry
  * in the database, and allows read and write access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ConfigGui.h"
#include "core/data_access_interface/src/GuiHelper.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/ControlStation.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
   const std::string ConfigGui::SHORTNAME = "Short Name";
   const std::string ConfigGui::FULLNAME = "Full Name";
   const std::string ConfigGui::EXECUTABLE = "Executable";
   const std::string ConfigGui::HELPFILE= "Help";
   const std::string ConfigGui::IS_ICON_DISPLAYED = "Display Icon";
   const std::string ConfigGui::ARE_INSTANCES_LIMITED = "Limit Instances";
   const std::string ConfigGui::SHOULD_PASS_PARAMS = "Should Pass Parameters";
   const std::string ConfigGui::SHOULD_TERMINATE_ON_LOGOUT = "Should Terminate on Logout";
   const std::string ConfigGui::SHOULD_REPOSITION = "Should Reposition on Launch";

   const std::string ConfigGui::ENTITYNAME = "--entity-name=";

   const std::string ConfigGui::NO = "No";
   const std::string ConfigGui::YES = "Yes";

   const unsigned long ConfigGui::MAXIMUM_RESERVED_KEY = 99;

    const int ConfigGui::MAX_ENTITY_NAME_LENGTH = 80;
    const int ConfigGui::MAX_EXECUTABLE_LENGTH  = 1024;


   long ConfigGui::s_nextAvailableIdentifier = 0;

   ConfigGui::ConfigGui(const std::string& type)
        : m_guiHelper( new GuiHelper() ),
          m_relatedEntity(NULL),
          m_uniqueIdentifier( ++s_nextAvailableIdentifier ),
		  m_noType(type.empty())
   {
       if (!m_noType)
       {
           m_relatedEntity = EntityAccessFactory::getInstance().createEntity(type);

           // Now we try to retrieve a Control Station entity to set as the parent of this entity. If we cannot
           // find one then the parent will have to stay as 0
           try
           {
               std::vector<IEntityData*> matchingEntities = EntityAccessFactory::getInstance().getEntitiesOfType(ControlStation::getStaticType());
               if (!matchingEntities.empty())
               {
                   ControlStation* cs = dynamic_cast<ControlStation*>(matchingEntities[0]);
                   if (cs != NULL)
                   {
                        m_relatedEntity->setParent( cs->getKey() );
                   }
               }
           }
           catch( ... )
           {
               LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not retrieve a Control Station entity to use as the parent of our new Gui entity");
           }
       }
   }

        
    ConfigGui::ConfigGui(const unsigned long key)
        : m_guiHelper( new GuiHelper(key) ),
          m_relatedEntity(NULL),
          m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
		// TD #2899
		// Since the 'type' data that applications were created with is not persisted (?),
		// we'll check the executable string for an occurrence of ENTITYNAME. if it was not
		// found, then we can assume the type is "None"
		m_noType = (m_guiHelper->getExecutable().find(ENTITYNAME) == std::string::npos);
    }

	ConfigGui::ConfigGui(const unsigned long row, TA_Base_Core::IData& data)
		: 
		m_guiHelper( new GuiHelper(row, data) ),
		m_relatedEntity(NULL),
		m_uniqueIdentifier( ++s_nextAvailableIdentifier )
	{
		m_noType = (m_guiHelper->getExecutable().find(ENTITYNAME) == std::string::npos);
	}

    ConfigGui::ConfigGui( const ConfigGui& theGui)
        : m_guiHelper( new GuiHelper(*(theGui.m_guiHelper)) ),
          m_relatedEntity(NULL),
          m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
		// TD #2899
		// Since the 'type' data that applications were created with is not persisted (?),
		// we'll check the executable string for an occurrence of ENTITYNAME. if it was not
		// found, then we can assume the type is "None"
		m_noType = (m_guiHelper->getExecutable().find(ENTITYNAME) == std::string::npos);

        if (theGui.m_relatedEntity != NULL)
        {
            m_relatedEntity = EntityAccessFactory::getInstance().copyEntity( theGui.m_relatedEntity );
        }
    }

        
    ConfigGui::~ConfigGui()
    {
        try
        {
            if (m_guiHelper != NULL)
            {
                delete m_guiHelper;
                m_guiHelper=NULL;
            }
            if (m_relatedEntity != NULL)
            {
                delete m_relatedEntity;
                m_relatedEntity = NULL;
            }
        }
        catch( ... )
        {
        }
    }


    unsigned long ConfigGui::getKey()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->getKey();
    }


    std::string ConfigGui::getName()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->getName();
    }


    std::string ConfigGui::getFullName()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->getFullName();
    }


    std::string ConfigGui::getExecutable()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->getExecutable();
    }


    std::string ConfigGui::getHelpFilePath()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->getHelpFilePath();
    }

    
    bool ConfigGui::isIconDisplayed()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->isIconDisplayed();
    }


    bool ConfigGui::areInstancesLimited()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->areInstancesLimited();
    }


    bool ConfigGui::shouldPassParameters()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->shouldPassParameters();
    }


    bool ConfigGui::shouldReposition()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->shouldReposition();
    }


    bool ConfigGui::shouldTerminateOnLogout()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->shouldTerminateOnLogout();
    }


    time_t ConfigGui::getDateCreated()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->getDateCreated();
    }


    time_t ConfigGui::getDateModified()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->getDateModified();
    }


    ItemChanges ConfigGui::getAllItemChanges()
    {
        /*
        FUNCTION_ENTRY("getAllItemChanges");

        ItemChanges changes = m_guiChanges;

        // We must get the changes to the entity as well
        if (m_relatedEntity == NULL)
        {
            // First ensure that it is loaded
            getRelatedEntity();
        }

        if (m_relatedEntity != NULL)
        {
            ItemChanges entityChanges = m_relatedEntity->getAllItemChanges();

            std::copy(entityChanges.begin(), entityChanges.end(), std::inserter(changes, changes.begin()));
        }

        FUNCTION_EXIT;
        return changes;
        */
        return m_guiChanges;
    }


    bool ConfigGui::hasChanged()
    {
        // If the location changes has something in it then changes have been made.
        if (!m_guiChanges.empty())
        {
            return true;
        }

        // Otherwise we must check if the entity had had any changes before returning
        if(m_relatedEntity != NULL)
        {
            return m_relatedEntity->hasChanged();
        }
        // If the entity has not been loaded then it hasn't been changed.

        // If we get to here then there are no changes
        return false;
    }


    
    IConfigEntity* ConfigGui::getRelatedEntity()
    {
        FUNCTION_ENTRY("getRelatedEntity");

        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");
        
		if(m_noType)
		{
			return NULL;
		}
		else
		{
			std::string entityName = getEntityNameFromExecutable(m_guiHelper->getExecutable());

			if ( (m_relatedEntity == NULL) && (!entityName.empty()) )
			{
				// Ok let's load that entity
				m_relatedEntity = dynamic_cast<IConfigEntity*>(EntityAccessFactory::getInstance().getEntity( entityName, true ));
			}

			return m_relatedEntity;
		}

        FUNCTION_EXIT;
    }


    void ConfigGui::setName(const std::string& name)
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        updateGuiChanges(SHORTNAME,m_guiHelper->getName(), name);

        m_guiHelper->setName(name);
    }


    void ConfigGui::setFullName(const std::string& fullName)
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        updateGuiChanges(FULLNAME,m_guiHelper->getFullName(), fullName);

        m_guiHelper->setFullName(fullName);
    }


    void ConfigGui::setExecutable(const std::string executable)
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

		std::string executableCopy = executable;

		if(m_noType)
		{
			// TD #2899
			// Check if the ENTITYNAME string exists in the executable string.
			// It shouldn't be there, so if it does we'll remove it
			std::string::size_type startPos = executable.find(ENTITYNAME);
			
			// it was found..
			if (startPos != std::string::npos)
			{
				std::string::size_type endPos = executable.find(" ", startPos);
				if (endPos == std::string::npos)
				{
					endPos = executable.length();
				}

				// we want to erase the preceding " " character too (if there is one)
				startPos = (startPos > 0 ? startPos - 1 : 0);
				executableCopy.replace(startPos, endPos - startPos, "");
			}
		}
		else
		{
			std::string oldEntityName = getEntityNameFromExecutable( m_guiHelper->getExecutable() );
			std::string newEntityName = getEntityNameFromExecutable( executable );

			// If the entity names has changed
			if(0 != oldEntityName.compare(newEntityName) )
			{
				// We should set the entity name in the related entity
				if (m_relatedEntity == NULL)
				{
					// First ensure that it is loaded
					getRelatedEntity();
				}

				if (m_relatedEntity != NULL)
				{
					if( newEntityName.length() > MAX_ENTITY_NAME_LENGTH )
					{
						// We must reduce the length of the entity name and update the executable accordingly.
						newEntityName = newEntityName.substr(0, MAX_ENTITY_NAME_LENGTH);

						std::string::size_type startPos = executable.find(ENTITYNAME);
						startPos = startPos + ENTITYNAME.size();
						std::string::size_type endPos = executable.find(" ", startPos);
						if (endPos == std::string::npos)
						{
							endPos = executable.length();
						}

						executableCopy.replace(startPos, endPos - startPos, newEntityName);
					}

					if (executableCopy.length() > MAX_EXECUTABLE_LENGTH)
					{
						// We must reduce the length of the executable so we do this by reducing the length
						// of the entity name and update the executable accordingly.
						int amountOver = executableCopy.length() - MAX_EXECUTABLE_LENGTH ;
						newEntityName = newEntityName.substr(0, newEntityName.length() - amountOver);

						std::string::size_type startPos = executable.find(ENTITYNAME);
						startPos = startPos + ENTITYNAME.size();
						std::string::size_type endPos = executable.find(" ", startPos);
						if (endPos == std::string::npos)
						{
							endPos = executable.length();
						}

						executableCopy.replace(startPos, endPos - startPos, newEntityName);
					}

					m_relatedEntity->setName( newEntityName );
				}
			}
		}

		updateGuiChanges(EXECUTABLE,m_guiHelper->getExecutable(), executableCopy);

		m_guiHelper->setExecutable(executableCopy);
    }


    void ConfigGui::setHelpFilePath(const std::string& helpFileName)
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        updateGuiChanges(HELPFILE,m_guiHelper->getHelpFilePath(), helpFileName);

        m_guiHelper->setHelpFilePath(helpFileName);
    }


    void ConfigGui::setIsIconDisplayed(bool isIconDisplayed)
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        std::string newValue = NO;
        std::string oldValue = NO;
        if (isIconDisplayed)
        {
            newValue = YES;
        }
        if (m_guiHelper->isIconDisplayed())
        {
            oldValue = YES;
        }

        updateGuiChanges(IS_ICON_DISPLAYED, oldValue, newValue);

        m_guiHelper->setIsIconDisplayed(isIconDisplayed);
    }


    void ConfigGui::setAreInstancesLimited(bool areInstancesLimited)
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        std::string newValue = NO;
        std::string oldValue = NO;
        if (areInstancesLimited)
        {
            newValue = YES;
        }
        if (m_guiHelper->areInstancesLimited())
        {
            oldValue = YES;
        }

        updateGuiChanges(ARE_INSTANCES_LIMITED, oldValue, newValue);

        m_guiHelper->setAreInstancesLimited(areInstancesLimited);
    }


    void ConfigGui::setShouldPassParameters(bool shouldPassParams)
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        std::string newValue = NO;
        std::string oldValue = NO;
        if (shouldPassParams)
        {
            newValue = YES;
        }
        if (m_guiHelper->shouldPassParameters())
        {
            oldValue = YES;
        }

        updateGuiChanges(SHOULD_PASS_PARAMS, oldValue, newValue);

        m_guiHelper->setShouldPassParameters(shouldPassParams);
    }


    void ConfigGui::setShouldReposition(bool shouldReposition)
    {
        TA_ASSERT(m_guiHelper != NULL, "The ConfigGuiHelper pointer is null.");

        std::string newValue = NO;
        std::string oldValue = NO;
        if ( shouldReposition )
        {
            newValue = YES;
        }
        if ( m_guiHelper->shouldReposition() )
        {
            oldValue = YES;
        }

        updateGuiChanges( SHOULD_REPOSITION, oldValue, newValue );

        m_guiHelper->setShouldReposition( shouldReposition );
    }


    void ConfigGui::setShouldTerminateOnLogout(bool shouldTerminateOnLogout)
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        std::string newValue = NO;
        std::string oldValue = NO;
        if (shouldTerminateOnLogout)
        {
            newValue = YES;
        }
        if (m_guiHelper->shouldTerminateOnLogout())
        {
            oldValue = YES;
        }

        updateGuiChanges(SHOULD_TERMINATE_ON_LOGOUT, oldValue, newValue);

        m_guiHelper->setShouldTerminateOnLogout(shouldTerminateOnLogout);
    }


    bool ConfigGui::isNew()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        return m_guiHelper->isNew();
    }

    
    void ConfigGui::invalidate()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        m_guiHelper->invalidate();

        if (m_relatedEntity != NULL)
        {
            m_relatedEntity->invalidate();
        }

        m_guiChanges.clear();
    }


    void ConfigGui::applyChanges()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        // Flag whether this is a new insert or not - if it is, and something goes wrong during the second write
        // attempt, then we need to delete the first one...

        bool wasNew = m_guiHelper->isNew();

        m_guiHelper->applyChanges();

        try 
        {
            if (m_relatedEntity != NULL)
            {
                m_relatedEntity->applyChanges();
            }
        }
        catch (DatabaseException&)
        {
            // If there are any problems writing the entity to the database, and this was a new entity, then
            // need to delete the config entity as well...
            if (wasNew)
            {
                m_guiHelper->deleteThisGui();
                m_guiHelper->resetToNew();
            }

            // Then on throw the exception to be caught at a higher level.
            throw;
        }
        catch (DataException&)
        {
            // If there are any problems writing the entity to the database, and this was a new entity, then
            // need to delete the config entity as well...
            if (wasNew)
            {
                m_guiHelper->deleteThisGui();
                m_guiHelper->resetToNew();
            }

            // Then on throw the exception to be caught at a higher level.
            throw;
        }
        catch (DataConfigurationException&)
        {
            // If there are any problems writing the entity to the database, and this was a new entity, then
            // need to delete the config entity as well...
            if (wasNew)
            {
                m_guiHelper->deleteThisGui();
                m_guiHelper->resetToNew();
            }

            // Then on throw the exception to be caught at a higher level.
            throw;
        }
        catch (...)
        {
            // If there are any problems writing the entity to the database, and this was a new entity, then
            // need to delete the config entity as well...
            if (wasNew)
            {
                m_guiHelper->deleteThisGui();
                m_guiHelper->resetToNew();
            }

            // Then on throw the exception to be caught at a higher level.
            throw;
        }

        m_guiChanges.clear();
    }

        
    void ConfigGui::deleteThisGui()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        if (m_relatedEntity == NULL)
        {
            // If the entity has not yet been loaded this will ensure it is loaded
            getRelatedEntity();
        }

        if (m_relatedEntity != NULL)
        {
            m_relatedEntity->deleteThisEntity();
            delete m_relatedEntity;
            m_relatedEntity = NULL;
        }

        m_guiHelper->deleteThisGui();

        delete m_guiHelper;
        m_guiHelper = NULL;
    }


    bool ConfigGui::canDelete()
    {
        TA_ASSERT(m_guiHelper != NULL,"The ConfigGuiHelper pointer is null.");

        // Only keys greater than the maximum reserved key can be deleted by the user as they are 
        if (m_guiHelper->getKey() <= MAXIMUM_RESERVED_KEY)
        {
            return false;
        }
        return true;
    }


    void ConfigGui::updateGuiChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateLocationChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_guiChanges.find(name);

        if (matching != m_guiChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_guiChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_guiChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


    std::string ConfigGui::getEntityNameFromExecutable(const std::string& executable)
    {
        std::string::size_type pos = executable.find(ENTITYNAME);

		if (pos == std::string::npos)
        {
            // There is no entity name so return an empty string
            return "";
        }

        // Find where the entity name parameter ends. It will be either at the next space or at the end of the
        // string
        std::string::size_type endPos = executable.find(" ",pos);
		if (endPos == std::string::npos)
        {
            // We're at the end
            endPos = executable.size();
        }

        int startPos = pos + ENTITYNAME.size();

        return executable.substr( startPos, endPos - startPos );
    }


} // closes TA_Base_Core
