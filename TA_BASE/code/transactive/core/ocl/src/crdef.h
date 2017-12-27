/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\crdef.h
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:22 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

/////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @ 1999-2002 Core Lab. All right reserved.
//  OCL definitions
//  Last modified:      28.03.00
/////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __OCLDEF_H__
#define __OCLDEF_H__

#if !defined(OCL_MFC) && !defined(OCL_VCL)
#define OCL_SCL
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define OCL_WIN32    //define OCL for MS Windows
#  undef boolean       //OCI boolean conflict with windows boolean
#  define NOCRYPT
#  include <windows.h>
#  define boolean int  //restore boolean definition as declared in OCI
#elif defined(linux) || defined(__linux) || defined(__linux__)
#  define OCL_LINUX
#elif defined( SOLARIS ) //to support solaris, haipeng modified
#  define OCL_SOLARIS
#else 
#  error OCL do not support this platform
#endif

#if (defined(OCL_MFC) || defined(OCL_VCL)) && !defined(OCL_WIN32)
#error MFC and VCL are supported on WIN32 platform only
#endif

// #define OCL_DEPRECATED // mark deprecated code

#include <string>
#include <vector>
#include <assert.h>
#include <string.h>

#ifdef OCL_SCL
#include <exception>
#endif

#ifdef OCL_MFC
#include <afx.h>
#endif

#ifdef OCL_VCL
#include <vcl.h>
#pragma hdrstop
#endif

#ifdef OCL_LINUX
#include <pthread.h>
#endif

#ifdef OCL_SOLARIS //to support solaris, haipeng modified
#include <pthread.h>
#endif


namespace ocl {
	
	typedef unsigned short word;
	typedef unsigned char byte;
	
	using std::string;
	
	class CRString : public string {
	public:
		CRString() {}
		
		CRString(const char* str) : string(str) {}
		
		CRString(const string& str) : string(str) {}
		
#ifdef _MFC_VER
		CRString(const CString& str) : string((LPCTSTR)str) {}
#endif
		
		CRString& toUpper();
		CRString& toLower();
		
		CRString& trim();
		CRString& trimLeft();
		CRString& trimRight();
		
		operator const char*() const {
			return this->c_str();
		}
		
		char operator[](int index) const {
			return string::operator[](index);
		}
		
#ifdef _MFC_VER
		operator CString() const {
			return this->c_str();
		}
#endif
	};
	
	/////////  Standart C++ library support  ////////
	
#ifdef OCL_SCL
	
	class CRException : public std::exception {
	public:
		CRException(const CRString& msg);
		~CRException() throw();
		
		CRString message();
		void setMessage(const CRString& msg);
		
		virtual const char* what() const throw();
		
	protected:
		CRString mMsg;
	};
	
	char* strupr(char* s);
	int stricmp(const char* string1, const char* string2);
	int strnicmp(const char *string1, const char *string2, size_t count);
	
#endif
	
	////////////////  MFC support  //////////////////
	
#ifdef OCL_MFC
	
	class CRException : public CException {
	public:
		CRException(const CRString& msg);
		
		CRString message();
		void setMessage(const CRString& msg);
		
		virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
			PUINT pnHelpContext = NULL);
		
	protected:
		CRString mMsg;
		
	};
	
#endif
	
	//////////////////  VCL support  ////////////////
	
#ifdef OCL_VCL
	
	class CRException : public Exception {
	public:
		CRException(const CRString& msg);
		
		CRString& message();
		void setMessage(const CRString& msg);
	};
	
#endif
	
	////////////////////////  CRDate  ///////////////////////
	
	class CRDate {
	public:
		CRDate();
		CRDate(unsigned year, unsigned month, unsigned day, unsigned hour = 0,
			unsigned minute = 0, unsigned second = 0/*, unsigned Millesecond = 0*/);
		
		unsigned year() const { return mYear; };
		void setYear(unsigned year);
		unsigned month() const { return mMonth; };
		void setMonth(unsigned month);
		unsigned day() const { return mDay; };
		void setDay(unsigned day);
		unsigned hour() const { return mHour; };
		void setHour(unsigned hour);
		unsigned minute() const { return mMinute; };
		void setMinute(unsigned minute);
		unsigned second() const { return mSecond; };
		void setSecond(unsigned second);
		unsigned dayOfWeek();
		
		bool operator==(const CRDate& value) const;
		
		CRString format(const char* format = NULL) const;
		void now();
		
		static CRString dateFormat();
		static void setDateFormat(const CRString& value);
		
	private:
		
		static CRString mDateFormat;
		
		word mYear;
		byte mMonth;
		byte mDay;
		byte mHour;
		byte mMinute;
		byte mSecond;
		byte mMillesecond;
	};
	
	////////////////////////  CRMutex  ///////////////////////
	
#if defined(OCL_WIN32)
	typedef CRITICAL_SECTION CRMutexType;
#elif defined(OCL_LINUX)
	typedef pthread_mutex_t  CRMutexType;
#elif defined(OCL_SOLARIS) //to support solaris, haipeng modified
	typedef pthread_mutex_t  CRMutexType;
#else
#error Class CRMutex is not yet supported in this environment
#endif
    
	class CRMutex {
	public:
		//lock - initial state
		CRMutex(/*bool lock = false*/);
		~CRMutex();
		
		void lock();
		void unlock();
		
	private:
		CRMutexType mutex;
		//bool isLocked;
	};
	
	inline CRMutex::CRMutex(/*bool lock*/ /*=false*/) {
		//isLocked = lock;
		
#if defined(OCL_WIN32)
		InitializeCriticalSection(&mutex);
		
// 		if (isLocked)
// 			EnterCriticalSection(&mutex);
#elif defined(OCL_LINUX) || defined(OCL_SOLARIS) //to support solaris, haipeng modified
		pthread_mutex_init(&mutex, NULL);
		
// 		if (isLocked)
// 			pthread_mutex_lock(&mutex);
#endif
	}
	
	inline CRMutex::~CRMutex() {
		//unlock();
		
#if defined(OCL_WIN32)
		DeleteCriticalSection(&mutex);
#elif defined(OCL_LINUX) || defined(OCL_SOLARIS) //to support solaris, haipeng modified
		pthread_mutex_destroy(&mutex);  
#endif
	}
	
	inline void CRMutex::lock() {
		//if (!isLocked) {
			
#if defined(OCL_WIN32)
			EnterCriticalSection(&mutex);
#elif defined(OCL_LINUX) || defined(OCL_SOLARIS) //to support solaris, haipeng modified
			pthread_mutex_lock(&mutex);
#endif
			
			//isLocked = true;
		//}
	}
	
	inline void CRMutex::unlock() {
		//if (isLocked) {
			
#if defined(OCL_WIN32)
			LeaveCriticalSection(&mutex);
#elif defined(OCL_LINUX) || defined(OCL_SOLARIS) //to support solaris, haipeng modified
			pthread_mutex_unlock(&mutex);
#endif  
			
			//isLocked = false;
		//}
	}

    class CRThreadGuard
    {
    public:
        CRThreadGuard( CRMutex& refMutex );
        virtual ~CRThreadGuard();
    protected:
        CRThreadGuard();
    private:
        CRMutex& mMutex;
    };

    inline CRThreadGuard::CRThreadGuard(ocl::CRMutex &refMutex) : 
    mMutex( refMutex )
    {
        mMutex.lock();
    }

    inline CRThreadGuard::~CRThreadGuard()
    {
        mMutex.unlock();
    }
	
	void throwError(const char* format, ...);
	
	////////////////////////  CCRList  ////////////////////////
	
	template <class T>
		class CRList : public std::vector<T*> {
#if __BORLANDC__ != 0x0550
		// this construct needs in order to resolve GCC version 3.4 and above errors
		// BorlandC5.0 does not support such syntax that is why we use define here
	public:
		typedef std::vector<T*> STD_VECTOR; // it helps to remove Visual C++ error message 
		using STD_VECTOR::begin;
		using STD_VECTOR::size;
		using STD_VECTOR::end;
#endif
	public:
		~CRList();
		
		void add(T* item) { push_back(item); };
		void addUnique(T* item) { remove(item); add(item); }
		void insert(int index, T* item);
		void remove(int index) { erase(begin() + index); };
		void remove(T* item);
		void clear();
		int count() const { return size(); };
		T& item(int index) const;
		T* findItem(int index) const;
		
		T& operator[](int index) const { return item(index); };
	};
	
	template <class T>
		CRList<T>::~CRList() {
		
		clear();
	}
	
	template <class T>
		void CRList<T>::remove(T* item) {
		
		for (typename std::vector<T*>::size_type i = 0; i < size(); i++)
			if (item == &operator[](i)) erase(begin() + i);
	}
	
	template <class T>
		void CRList<T>::clear() {
		erase(begin(), end());
	}
	
	template <class T>
		T& CRList<T>::item(int index) const {
		
		if (index >= (int)size())
			throwError("Index %u is not bound", index);
		
		return **(begin() + index);
	}
	
	template <class T>
		T* CRList<T>::findItem(int index) const {
		
		if (index < (int)size())
			return *(begin() + index);
		else
			return NULL;
	}
	
	////////////////////////  CCRNamedList  ///////////////////////
	
	template <class T>
		class CRNamedList : public CRList<T> {
#if __BORLANDC__ != 0x0550
public:
	using CRList<T>::begin;
	using CRList<T>::end;
	using CRList<T>::count;
#endif
public:
	T& item(int index) const { return CRList<T>::item(index); };
	T& item(const CRString& name) const;
	T& item(const char* name) const;
	T* findItem(int index) const { return CRList<T>::findItem(index); };
	T* findItem(const CRString& name) const;
	T* findItem(const char* name) const;
	
	void clear();
	
	T& operator[](int index) const { return item(index); };
	T& operator[](const char* name) const { return item(name); };
	};
	
	template <class T>
		T* CRNamedList<T>::findItem(const CRString& name) const {
		
		return findItem(name.c_str());
	}
	
	template <class T>
		T* CRNamedList<T>::findItem(const char* name) const {
		
		if (name)
			for (int i= 0; i < count(); i++)
				if (!ocl::stricmp((*(begin() + i))->mName.c_str(), name))
					return *(begin() + i);
				
				return NULL;
	}
	
	template <class T>
		T& CRNamedList<T>::item(const CRString& name) const {
		
		return item(name.c_str());
	}
	
	template <class T>
		T& CRNamedList<T>::item(const char* name) const {
		T* res;
		
		res = findItem(name);
		if (!res)
			throwError("Item %s not found", name);
		
		return *res;
	}
	
	template <class T>
		void CRNamedList<T>::clear() {
		
		for(int i = 0; i < count(); i++)
			delete *(begin() + i);
		
		erase(begin(), end());
	}
	
#ifdef OCL_DEPRECATED
	typedef CRString CCRString;
	typedef CRException CCRException;
	typedef CRDate CCRDate;
#endif
	
} // namespace ocl

#endif
