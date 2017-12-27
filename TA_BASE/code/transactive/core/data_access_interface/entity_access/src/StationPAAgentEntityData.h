/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/StationPAAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * StationPAAgentEntityData implements the interface IStationPAAgentEntityData to provide read-only access
  * to StationPAAgent entity data.
  */

#ifndef StationPAAgent_ENTITY_DATA_H
#define StationPAAgent_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IStationPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/AbstractPAAgentEntityData.h"

namespace TA_Base_Core
{
    class StationPAAgentEntityData: public virtual AbstractPAAgentEntityData, public virtual IStationPAAgentEntityData
    {
	public:

		StationPAAgentEntityData();
        StationPAAgentEntityData ( unsigned long key );

        virtual ~StationPAAgentEntityData();


        //
        // operations required by AbstractPAAgentEntityData
        //

    public:

        static IEntityData* clone ( unsigned long key )
        {
            return new StationPAAgentEntityData ( key );
        }

    protected:

        virtual bool reloadSpecificParameter( IData& data );

		//
		// operations required by IEntityData
		//

    public:

        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();

        static std::string getStaticType();

    protected:

        // Any new parameter members.

    private:

        StationPAAgentEntityData ( const StationPAAgentEntityData & obj ){};
		StationPAAgentEntityData & operator= ( const StationPAAgentEntityData &){};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
    };
    
    typedef boost::shared_ptr<StationPAAgentEntityData> StationPAAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif
