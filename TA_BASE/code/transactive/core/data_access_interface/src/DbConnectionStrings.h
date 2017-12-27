/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DbConnectionStrings.h $
 * @author:  R.Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2015/03/13 16:44:36 $
 * Last modified by:  $Author: Ouyang $
 * 
 * This class reads and parses a csv file to get the connection
 * strings for all known data types
 */
#if !defined(DbConnectionStrings_29AA5110_185C_4781_89E1_C0663B2B94FD__INCLUDED_)
#define DbConnectionStrings_29AA5110_185C_4781_89E1_C0663B2B94FD__INCLUDED_

#include <map>
#include <string>
#include <vector>

#include "core/data_access_interface/src/DbStatus.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    typedef std::pair<EDataTypes, EDataActions> DataKey;
    typedef std::pair<std::string, std::string> DataConnection; // database name, connection string
    typedef std::vector<DataConnection> DataConnections;
    typedef std::map<DataKey, DataConnections > DatabaseConnectionMap; 

    class DbConnectionStrings
    {

    public:
        DbConnectionStrings(std::string filename);
	    virtual ~DbConnectionStrings();

		static DbConnectionStrings& getInstance();

    public:
	    /**
	     * Fills in the conection string with the correct string for the given datatype
	     * and action. An exception will be thrown if a string can not be found.
	     * @param dataType    The type of data this connection is for.
	     * @param action    the action to be preformed on this datatype
	     * @param connectionString    The returned connection string goes here.
	     * 
	     */
	    DatabaseConnectionMap& getConnectionMap();

        DataConnections getConnectionList(const EDataTypes, const EDataActions);

        static std::string getDatabaseName(const std::string& connectionString, const std::string& prefix = RPARAM_DBPREFIX);

    private:

		DbConnectionStrings(){}
        void ParseFile();
		void ParseFileEx();
	    DbConnectionStrings(const DbConnectionStrings& theDbConnection);
		DataConnections getConnectionListInternal(const EDataTypes, const EDataActions);
        std::string m_filename;
        DatabaseConnectionMap m_dbMap;

    private: 

		typedef std::map<std::string , DbConnectionStrings*> DbConnStrMap;
		
		DataConnections getConnectionListStatic(const std::string& strFileName,const EDataTypes, const EDataActions);
		DbConnStrMap  m_sMapDbConnStrs;

		static NonReEntrantThreadLockable m_singletonLock;
		static DbConnectionStrings* m_sDbConnStrObj;
		ReEntrantThreadLockable m_getDbNameLock;

    };
} // namespace
#endif // !defined(DbConnectionStrings_29AA5110_185C_4781_89E1_C0663B2B94FD__INCLUDED_)