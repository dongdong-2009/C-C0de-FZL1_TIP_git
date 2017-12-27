/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File$
  * @author:	Darren Sampson
  * @version:	$Revision$
  *
  * Last modification:	$DateTime$
  * Last modified by:	$Author$
  *
  * Description:
  *
  */

#if !defined(AFX_OPCTAGMANAGER_H__C949C3DC_2E82_4F56_BA5F_BEF280D11B39__INCLUDED_)
#define AFX_OPCTAGMANAGER_H__C949C3DC_2E82_4F56_BA5F_BEF280D11B39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <vector>
#include "app/scada/OpcBridgeLib/src/OpcTag.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_App
{
	class OpcTagManager  
	{
	public:
		OpcTagManager();
		virtual ~OpcTagManager();

        /**
		  * getOpcTag
		  *
		  * gets an OPC Tag from the allTags list, or creates it
		  *
		  * @param	tagName		The name of the tag to get
		  */
		//boost::shared_ptr<OpcTag> getOpcTag(const std::string& tagName, _variant_t & initValue, WORD & initQuality);

		//boost::shared_ptr<OpcTag> getOpcTag(const std::string& tagName);

		OpcTag* getOpcTag(const std::string& tagName, _variant_t & initValue, WORD & initQuality);
		OpcTag* getOpcTag(const std::string& tagName);

		/**
		  * deleteOpcTag
		  *
		  * deletes the OPC Tag and removes it from the allTags list
		  *
		  * @param	tag		The tag to delete
		  */
		void deleteOpcTag(const std::string& tagName);

		//
		// getTagLocation
		//
		std::string getTagLocation( const std::string & tagName ) const;
	
		//
		// getTagObjectName
		//
		std::string getTagObjectName( const std::string & tagName ) const;

		//
		// getTagAttribute
		//
		std::string getTagAttribute( const std::string & tagName ) const;

        void createStatisticalTag( const std::string & tagName );
        void updateStatisticalTag( const std::string & tagName, _variant_t value );
		bool isUpdateTagPeriodically() { return m_bUpdatePeriodically; };
	protected:

		//boost::shared_ptr<OpcTag> findTag( const std::string& tagName );
		OpcTag* findTag( const std::string& tagName );

	private:
		bool														m_bUpdatePeriodically;
		typedef std::map<std::string, OpcTag*>	OpcTagMap;
		OpcTagMap													m_opcTagMap;
        OpcTagMap													m_opcStatisticalTagMap;
		TA_Base_Core::ReadWriteThreadLockable						m_opcTagMapLock;
	};
}

#endif // !defined(AFX_OPCTAGMANAGER_H__C949C3DC_2E82_4F56_BA5F_BEF280D11B39__INCLUDED_)
