/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DbStatus.h $
 * @author:  R.Stagg
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class maintains a list of 'known' databases.
 * It tracks the state of this databases to provide a quick reference of which
 * db's are avalable
 */
#if !defined(DbStatus_169C2248_E75E_4945_ACA4_F3BDC501E5A3__INCLUDED_)
#define DbStatus_169C2248_E75E_4945_ACA4_F3BDC501E5A3__INCLUDED_

#include <string>
#include <map>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Core
{
    enum EDbState
    {
        Pending,
        Online,
        Offline
    };

    class DbStatus : public RunParamUser
    {

    public:
	    virtual ~DbStatus();

    public:
	    /**
	     * Retuns true if the requested database is not marked offline.
	     * @param databaseId    an identifier used to query the cache to see if the db is
	     * 'up'
	     * 
	     */
	    bool isAvailable(const std::string& databaseId);
	    /**
	     * Returns the only copy of this class.
	     */
	    static DbStatus& getInstance();
		static void removeInstance();

        /**
         * Callback for when a runparam changes
         */
        void onRunParamChange(const std::string& name, const std::string& value);


    private:
	    /**
	     * Adds a new database to the list of monitored db's.
	     * @param dbId
	     * @param dbState    The current state
	     * 
	     */
	    void addDbState(const std::string& dbId, const EDbState dbState);
	    /**
	     * Gets the current state of a known database.
	     * If the database is not known an exception is thrown.
	     * @param dbId    an identifier used to query the cache
	     * 
	     */
	    EDbState getDbState(const std::string& dbId);
	    /**
	     * Updates the state of a known database.
	     * @param dbId    An identifier used to query the cache
	     * @param dbState
	     * 
	     */
        void updateDbState(std::string dbName, const EDbState dbState);
	    /**
         * Private constructor/copy/assignment
         */
        DbStatus();
	    DbStatus(const DbStatus& theDbStatus);

    private:
        typedef std::map<std::string, EDbState> DbList;
        DbList m_dbList;
        WriteReadThreadLockable m_listLock;
        static DbStatus* m_instance;
        static NonReEntrantThreadLockable m_instanceLock;

    };
} //namespace
#endif // !defined(DbStatus_169C2248_E75E_4945_ACA4_F3BDC501E5A3__INCLUDED_)