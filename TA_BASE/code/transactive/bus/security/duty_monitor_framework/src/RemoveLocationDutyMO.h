#pragma once

namespace TA_Base_Bus 
{
	class RegionDutyMatrix;
	class RemoveLocationDutyMO :public ACE_Method_Request 
	{
	public:
		RemoveLocationDutyMO(RegionDutyMatrix& matrix, unsigned long locationKey);
		virtual ~RemoveLocationDutyMO();
		virtual int call();
	private:
		RemoveLocationDutyMO& operator=( const RemoveLocationDutyMO& );
		RemoveLocationDutyMO( const RemoveLocationDutyMO& );
	private:
		RegionDutyMatrix&	matrix_;
		unsigned long		m_locationKey;
	};
}