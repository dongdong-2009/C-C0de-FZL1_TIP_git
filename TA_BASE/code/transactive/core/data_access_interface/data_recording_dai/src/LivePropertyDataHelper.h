#ifndef LIVE_PROPERTY_DATA_HELPER_H
#define LIVE_PROPERTY_DATA_HELPER_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/LivePropertyDataHelper.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include <map>

namespace TA_Base_Core
{
    class LivePropertyDataHelper
    {

    public:

        /**
         * String constants
         *
         */
        static const std::string    TABLE_NAME;

        static const std::string    FIELD_TYPEKEY;
        static const std::string    FIELD_CATEGORY;
        static const std::string    FIELD_NAME;
        static const std::string    FIELD_IDENTIFIER;
        static const std::string    FIELD_TABLE;
        static const std::string    FIELD_COLUMN;
        static const std::string    FIELD_COLUMNTYPE;
        static const std::string    FIELD_ENGMAX;
        static const std::string    FIELD_ENGMIN;
        static const std::string    FIELD_STATUS;
        static const std::string    FIELD_INTERVALBETWEENWRITES;
        static const std::string    FIELD_MINSUMMARYINTERVAL;


        /**
         * Constructor
         *
         */
        LivePropertyDataHelper();


        /**
         * Constructor
         *
         */
        LivePropertyDataHelper( unsigned long typeKey, const std::string& name );


        /**
         * Destructor
         *
         */
        ~LivePropertyDataHelper();


        /**
         * getTypeKey
         *
         * @return  the type key for this Live Property
         *
         */
        unsigned long getTypeKey();


        /**
         * getCategory
         *
         * @return  the category for this Live Property
         *
         */
        std::string getCategory();


        /**
         * getName
         *
         * @return  the name for this Live Property
         *
         */
        std::string getName();


        /**
         * getIdentifier
         *
         * @return  the identifier for this Live Property
         *
         */
        std::string getIdentifier();

        /**
         * getTable
         *
         * @return  the table name for this Live Property
         *
         */
        std::string getTable();


        /**
         * getColumn
         *
         * @return  the column for this Live Property
         *
         */
        std::string getColumn();


        /**
         * getColumnType
         *
         * @return  the column type for this Live Property
         *
         */
        std::string getColumnType();


        /**
         * getEngineeringMax
         *
         * @return  the engineering maximum for this Live Property
         *
         */
        double getEngineeringMax();


        /**
         * getEngineeringMin
         *
         * @return  the engineering minimum for this Live Property
         *
         */
        double getEngineeringMin();


        /**
         * getStatus
         *
         * @return  the status for this Live Property
         *
         */
        std::string getStatus();


        /**
         * getIntervalBetweenWrites
         *
         * @return  the interval between writes for this Live Property
         *
         */
        unsigned long getIntervalBetweenWrites();


        /**
         * getMinSummaryInterval
         *
         * @return  the minimum summary interval for this Live Property
         *
         */
        unsigned long getMinSummaryInterval();

        
        /**
         * setTypeKey
         *
         * @param typeKey  the type key for this Live Property
         *
         */
        void setTypeKey( unsigned long typeKey );


        /**
         * setCategory
         *
         * @param category  the category for this Live Property
         *
         */
        void setCategory( const std::string& category );
        
         
        /**
         * setName
         *
         * @param name  the name for this Live Property
         *
         */
        void setName( const std::string& name );
        
         
        /**
         * setIdentifier
         *
         * @param id  the identifier for this Live Property
         *
         */
        void setIdentifier( const std::string& id );
        
         
        /**
         * setTable
         *
         * @param tableName  the table name for this Live Property
         *
         */
        void setTable( const std::string& tableName );
        
         
        /**
         * setColumn
         *
         * @param columnName  the column for this Live Property
         *
         */
        void setColumn( const std::string& columnName );
        
         
        /**
         * setColumnType
         *
         * @param columnType  the column type for this Live Property
         *
         */
        void setColumnType( const std::string& columnType );
        
         
        /**
         * setEngineeringMax
         *
         * @param engMax  the engineering maximum for this Live Property
         *
         */
        void setEngineeringMax( double engMax );
        
         
        /**
         * setEngineeringMin
         *
         * @param engMin  the engineering minimum for this Live Property
         *
         */
        void setEngineeringMin( double engMin );
        

        /**
         * setStatus
         *
         * @param status  the status for this Live Property
         *
         */
        void setStatus( const std::string& status );
        

        /**
         * setIntervalBetweenWrites
         *
         * @param interval  the interval between writes for this Live Property
         *
         */
        void setIntervalBetweenWrites( unsigned long interval );
        

        /**
         * setMinSummaryInterval
         *
         * @param interval  the minimum summary interval for this Live Property
         *
         */
        void setMinSummaryInterval( unsigned long interval );

        
        /**
         * invalidate
         *
         * Marks the current data values as being invalid and requiring
         * a refresh from the database
         *
         */
        void invalidate();


        /**
         * applyChanges
         *
         * Writes pending changes to the database
         *
         */
        void applyChanges();


        /**
         * remove
         *
         * Deletes this Live Property from the database
         *
         */
        void remove();


    private:

        /**
         * testForReload
         *
         * Tests whether or not a refresh of the values is required
         *
         */
        inline void testForReload();


        /**
         * reload
         *
         * Reads complete configuration from database and refreshes values
         *
         */
        void reload();


		/**
		*bulkLoading
		*
		*/
		typedef struct _LIVE_PROPERTY
		{
			unsigned long       typeKey;
			std::string         category;
			std::string         name;
			std::string         identifier;
			std::string         table;
			std::string         column;
			std::string         columnType;
			double              engineeringMax;
			double              engineeringMin;
			std::string         status;
			unsigned long       intervalBetweenWrites;
			unsigned long       minSummaryInterval;
		
		}_LIVE_PROPERTY;

		typedef boost::shared_ptr<_LIVE_PROPERTY> LivePropertyPtr;
		static	std::map<std::string, LivePropertyPtr>	s_mapProperty;	
		static	bool	s_Initialize;
		void	bulkLoadTotalTable();
        bool                m_isValid;
        bool                m_isNew;
        unsigned long       m_typeKey;
        std::string         m_category;
        std::string         m_name;
        std::string         m_identifier;
        std::string         m_table;
        std::string         m_column;
        std::string         m_columnType;
        double              m_engineeringMax;
        double              m_engineeringMin;
        std::string         m_status;
        unsigned long       m_intervalBetweenWrites;
        unsigned long       m_minSummaryInterval;


    };

}

#endif // LIVE_PROPERTY_DATA_HELPER_H
