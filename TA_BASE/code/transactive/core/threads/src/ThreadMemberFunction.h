#ifndef THREAD_MEMBER_FUNCTION_H_
#define THREAD_MEMBER_FUNCTION_H_

#include "core/threads/src/Thread.h"

namespace TA_Base_Core
{
	//limin++, failover
	template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
	{
    public:
        
        typedef void ( T::*MemberFunction )();
        typedef std::vector< MemberFunction > MemberFunctionList;
        
        static ThreadedMemberFunction& alloc( T& o )
        {
            instance()->clear();
            instance() = new ThreadedMemberFunction( o );
            
            return *instance();
        }
        
        void add( MemberFunction mf )
        {
            m_members.push_back( mf );
        }
        
        virtual void run()
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member functions to call", m_members.size() );
            
            for ( typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "calling [%d] member function", std::distance( m_members.begin(), mfIter ) );
                    
                    ( m_object.*( *mfIter ) )();
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
                }
            }
            
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction" );
        }
        
        virtual void terminate(){}
        
    private:
        
        ThreadedMemberFunction( T& o )
            : m_object( o )
        {
        }
        
        ~ThreadedMemberFunction()
        {
            terminateAndWait();
        }
        
        static ThreadedMemberFunction*& instance()
        {
            static ThreadedMemberFunction* m_instance = NULL;
            return m_instance;
        }
        
        static void clear()
        {
            delete instance();
            instance() = NULL;
        }
        
        T& m_object;
        MemberFunctionList m_members;
	};
}
#endif