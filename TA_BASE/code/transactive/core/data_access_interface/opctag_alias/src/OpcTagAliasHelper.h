/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/OpcTagAliasHelper.h $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * OpcTagAliasHelper is an object that is held by all type-specific action group objects, as well as by
  * the ConfigOpcTagAlias. It contains all data common across action groups, and manipulation
  * methods for the data. It helps avoid re-writing code for each action group object.
  *
  */
  

#if !defined(OpcTagAliasHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
#define OpcTagAliasHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_


#include <string>
#include <iterator>


namespace TA_Base_Core
{

    class OpcTagAliasHelper
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty ConfigOpcTagAlias class ready to be populated.
         */
        OpcTagAliasHelper( );


        /**
         * Constructor
         *
         * Construct an OpcTagAlias class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not successful
         *
         * @param key The key of this OpcTagAlias in the database
         * @param name The name of this OpcTagAlias in the database
         */
        OpcTagAliasHelper( const unsigned long entitykey, const std::string& name);

        

        /**
         * Constructor
         *
         * Construct an OpcTagAlias class all the necessary fields, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not successful
         *
         * @param key The key of this OpcTagAlias in the database
         * @param name The name of this OpcTagAlias in the database
         */
        OpcTagAliasHelper( const unsigned long key, const unsigned long entitykey, const std::string& name , const std::string attribute);
        /**
         * Constructor
         *
         * Construct a OpcTagAliasHelper class based around an existing OpcTagAlias. This will
		 * copy all the parameters of the existing OpcTagAlias
         *
         * @param theOpcTagAlias The OpcTagAlias to copy
         */
		OpcTagAliasHelper( const OpcTagAliasHelper& theOpcTagAlias);            	

        
        /**
         * Destructor
         */
        virtual ~OpcTagAliasHelper();


        /**
         * isNew
         *
         * This returns whether this is a new OpcTagAlias or not
         *
         * @return bool - True if this is a new entity
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the key for this OpcTagAlias.
         *
         * @return The key for this OpcTagAlias as an unsigned long.
         */
        virtual unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this OpcTagAlias. If this is the first time data for this OpcTagAlias
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getName();
		
	    /**
	      * long getEntitykey
	      * 
	      * Returns the entity key for this OpcTagAlias
	      * 
	      * @return virtual unsigned 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getEntitykey();

        /**
	      * getAttributeName
	      * 
	      * Returns the the attribute
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getAttributeName();

        /**
         * getDateCreated
         *
         * Returns the date created for this OpcTagAlias
         *
         * @return The date created for this OpcTagAlias as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this OpcTagAlias.
         *
         * @return The date modified for this OpcTagAlias as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified();


        /**
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name);


        /**
         * setEntitykey
         *
         * Sets the entitykey
         *
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual void setEntitykey(unsigned long entitykey);
        
        /**
	      * setAttribute
	      * 
	      * Returns the the attribute
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setAttributeName(const std::string& attribute);

        /**
         * invalidate
         *
         * Make the data contained by this OpcTagAlias as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();
    

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the OpcTagAlias object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This entity has not been deleted
         */
        virtual void applyChanges();


        /**
         * deleteThisOpcTagAlias
         *
         * Removes this OpcTagAlias from the database. 
         * The calling application MUST then delete this OpcTagAlias object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this OpcTagAlias was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This OpcTagAlias has not been deleted
         */
        virtual void deleteThisOpcTagAlias();


    private:
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... 
         * or set... method is called and the data state is not valid.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        void reload();


        /**
         * modifyExisting
         *
         * This will update an existing OpcTagAlias in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the OpcTagAlias object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExisting();

        
        /**
         * addNew
         *
         * This will add a new OpcTagAlias in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the OpcTagAlias object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNew();

	    /**
	      * getPrimarykeyByEntitykeyAndAlias
	      * 
	      * get the primary key and attribute by the entitykey and alias
	      * 
	      * @return void 
	      * @param : const unsigned long entitykey
	      * @param : const std::string& name
	      * 
	      * @exception <exceptions> Optional
	      */
        void getPrimarykeyAndAttributeByEntitykeyAndAlias(const unsigned long entitykey, const std::string& name);


	    /**
	      * getNextSequenceNumber
	      * 
	      * get next sequence number
	      * 
	      * @return unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
        unsigned long getNextSequenceNumber();

        // Operator = is not used so this method is hidden.
        OpcTagAliasHelper& operator=(const OpcTagAliasHelper &);


        unsigned long m_key;
        std::string m_name;
        unsigned long m_entitykey; //the entitykey should uniquely identify the data
        std::string m_attribute;
        time_t m_dateCreated;
        time_t m_dateModified;

        bool m_isNew;
        bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // !defined(OpcTagAliasHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
