/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/ITVSSAgentEntityData.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: grace.koh $
  * 
  * ITVSSAgentEntityData is an interface to a TVSS entity object. It allows the TVSS object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#ifndef ITVSS_ENTITY_DATA_H
#define ITVSS_ENTITY_DATA_H


#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include <string>

namespace TA_Base_Core
{

    class ITVSSEntityData : public virtual IEntityData
    {

    public:
        virtual ~ITVSSEntityData() { };

		/**
         * getPrimaryTCPServers
         *
         * Returns the IP address(es) (or machine name(s)) of the primary radio TCPServer(s).
		 * They are returned in the format <name>,<name> when more than 1 is specified.
		 *                            e.g. 123.234.567.432,123.234.567.433
		 *                                 localhost,NTServer
         *
         * @return    IP address of the primary radio TCPServer(s)
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual std::string getPrimaryTCPServers() = 0;

        /**
         * getSecondaryTCPServers
         *
         * Returns the IP address(es) (or machine name(s)) of the secondary radio TCPServer(s).
		 * They are returned in the format <name>,<name> when more than 1 is specified.
		 *                            e.g. 123.234.567.432,123.234.567.433
		 *                                 localhost,NTServer
         *
         * @return    IP address of the secondary radio TCPServer(s)
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual std::string getSecondaryTCPServers() = 0;


		
        /**
         * getXONXOFFTimeout
         *
         * Returns the time interval to be used for timing out an XON signal from TIMS.
         *
         * @return    Time interval (in seconds) as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getXONXOFFTimeout() = 0;

		virtual std::string getAssetName() = 0;
    };

}//close namespace TA_Base_Core

#endif // !defined(ITVSS_ENTITY_DATA_H)
