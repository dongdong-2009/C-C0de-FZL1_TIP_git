// SharedAllocator.h: interface for the SharedAllocator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREDALLOCATOR_H__64EB52ED_7EDA_4738_AB94_A495407CC046__INCLUDED_)
#define AFX_SHAREDALLOCATOR_H__64EB52ED_7EDA_4738_AB94_A495407CC046__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef _SIZT size_type;

template<typename T, typename Memory>
class SharedAllocator:public std::allocator<T>
{
public:
	template<typename U>
	struct rebind
	{
		typedef SharedAllocator<U, Memory> other;
	};
	SharedAllocator(){};
	template<typename U>
		SharedAllocator(const SharedAllocator<U, Memory>&){};
	pointer allocate(size_type numObiects, const void *localityHint = 0)
	{
		return static_cast<pointer>(Memory::alloc(numObiects * sizeof(T)));
	}
	void deallocate(pointer ptrToMemory, size_type numObjects)
	{
		Memory::dealloc(ptrToMemory);
	}
};

#endif // !defined(AFX_SHAREDALLOCATOR_H__64EB52ED_7EDA_4738_AB94_A495407CC046__INCLUDED_)
