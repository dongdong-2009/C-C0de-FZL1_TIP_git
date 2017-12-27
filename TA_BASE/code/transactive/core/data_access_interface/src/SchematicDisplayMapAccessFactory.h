#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <string>
#include <vector>

namespace TA_Base_Core
{
	class ISchematicDisplayMaps;

	class SchematicDisplayMapAccessFactory
	{
	public:

		virtual ~SchematicDisplayMapAccessFactory(void);

		static SchematicDisplayMapAccessFactory& getInstance();
		
		std::string getSchematicDisplay(unsigned long locationKey, unsigned long subSystemKey);

		std::string getSchematicDisplay(unsigned long locationKey, unsigned long subSystemKey, long subTypeKey);
	
		int getSchematicDisplayAndLabel( unsigned long locationKey, unsigned long subSystemKey, 
			std::vector<ISchematicDisplayMaps*>& rVecOutput);

	private:
		SchematicDisplayMapAccessFactory();
		static SchematicDisplayMapAccessFactory* m_instance;
		static NonReEntrantThreadLockable m_singletonLock;
	};
}
