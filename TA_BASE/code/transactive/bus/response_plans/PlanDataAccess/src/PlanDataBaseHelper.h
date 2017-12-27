#ifndef PLANDATABASEHELPER_H_INCLUDED
#define PLANDATABASEHELPER_H_INCLUDED

namespace TA_Base_Core
{
	class IDatabase;
}

namespace TA_Base_Bus
{

class PlanDataBaseHelper  
{
public:
	PlanDataBaseHelper(TA_Base_Core::IDatabase* pIDatabase);
	~PlanDataBaseHelper();

private:
	PlanDataBaseHelper();		
	PlanDataBaseHelper( const PlanDataBaseHelper& );
	PlanDataBaseHelper& operator=( const PlanDataBaseHelper& );

	TA_Base_Core::IDatabase* m_pIDatabase;
};

}


#endif