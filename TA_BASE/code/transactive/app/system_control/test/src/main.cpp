#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif


#include <string>
#include <iostream>
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;


struct RunParamTest : public Thread, public RunParamUser
{
    RunParamTest()
    {
        RunParams::getInstance().registerRunParamUser( this, "Hello" );
        m_inside.set_parent( this );
        start();
    }

    ~RunParamTest()
    {
        RunParams::getInstance().deregisterRunParamUser( this );
        terminateAndWait();
    }

    virtual void run()
    {
        for ( size_t i = 0; i < 1; ++i )
        {
            RunParams::getInstance().set( "Hello", "World" );
            sleep( 2000 );
        }

        sleep( 4000 );
    }

    virtual void onRunParamChange(const std::string& name, const std::string& value)
    {
        std::cout << "RunParamTest::onRunParamChange - " << name << " = " << value << std::endl;

        m_inside.inside_call();

        RunParams::getInstance().deregisterRunParamUser( this );
        RunParams::getInstance().registerRunParamUser( this, "Hello" );

        RunParams::getInstance().get( "Good" );
        RunParams::getInstance().set( "Good", "Morning" );

        std::cout << "sleeping 5 seconds begin.." << std::endl;
        sleep( 5000 );
        std::cout << "sleeping 5 seconds end.." << std::endl;
    }

    virtual void terminate() {}

public:

    struct InsideTest : public RunParamUser
    {
        InsideTest()
            : m_parent( NULL )
        {
            RunParams::getInstance().registerRunParamUser( this, "Good" );
        }

        ~InsideTest()
        {
            RunParams::getInstance().deregisterRunParamUser( this );
        }

        void set_parent(RunParamTest* parent)
        {
            m_parent = parent;
        }

        virtual void onRunParamChange(const std::string& name, const std::string& value)
        {
            std::cout << "InsideTest::onRunParamChange - " << name << " = " << value << std::endl;
        }

        void inside_call()
        {
            RunParams::getInstance().get( "Good" );
            RunParams::getInstance().set( "Good", "Morning" );

            RunParams::getInstance().deregisterRunParamUser( this );
            RunParams::getInstance().registerRunParamUser( this, "Good" );

            RunParams::getInstance().deregisterRunParamUser( m_parent );
            RunParams::getInstance().registerRunParamUser( m_parent, "Hello" );
        }

        RunParamTest* m_parent;
    };

private:

    InsideTest m_inside;
};


struct RunParamTestRemove : public Thread
{
    RunParamTestRemove( RunParamTest* test )
        : m_test( test )
    {
        start();
    }

    ~RunParamTestRemove()
    {
        terminateAndWait();
    }

    virtual void run()
    {
        RunParams::getInstance().deregisterRunParamUser( m_test );
    }

    virtual void terminate() {}

private:

    RunParamTest* m_test;
};



int main(int argc, char* argv[])
{
    DebugUtil::getInstance().setLevel( DebugUtil::DebugDebug );

    RunParams::getInstance();
    //TA_Base_Core::Thread::sleep( 1000 );

    RunParamTest test0;
    TA_Base_Core::Thread::sleep( 500 );
    RunParamTestRemove remove( &test0 );
    //RunParamTest test1;
    //RunParamTest test2;
    //RunParamTest test3;
    //RunParamTest test4;
    //RunParamTest test5;

    TA_Base_Core::Thread::sleep( 2000 );
    return 0;
}
