




#if !defined(__IPRINTABLE_OBJECT_H_INCLUDED__)
#define __IPRINTABLE_OBJECT_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
	class IPrintableObject
	{
	public:
		virtual ~IPrintableObject(){};

		virtual void printObject() = 0 ;
	};

}

#endif // __IPRINTABLE_OBJECT_H_INCLUDED__