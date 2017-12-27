/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/ICommand.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "core/utilities/src/ICommand.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"
#include <algorithm>

namespace TA_Base_Core
{
    AsynchronousCommand::AsynchronousCommand(ICommand* command) :
        m_command(command),
        m_executing(false)
    {
        TA_Assert(m_command != NULL);
    } 

    AsynchronousCommand::~AsynchronousCommand() 
    {
        // TODO LPP: If run() finishes normally, it is arguable that it should not be 
        // necessary to call terminateAndWait().  However, Thread currently requires 
        // terminateAndWait() to be called prior to destructing the object, but we are 
        // not allowed to call it from this thread!  So we require the client to
        // explicitly call cancelExecutionAndWait().
        TA_Assert(!m_executing);
    }

    void AsynchronousCommand::execute()
    {
        // TODO LPP: This pattern isn't actually threadsafe! Multiple threads could assert
        // that m_executing is false, then simultaneously set it.
        TA_Assert(!m_executing);
        m_executing = true;

        start();
    }

    // TODO LPP: Presumably the thread calling cancelExecutionAndWait() must not be the
    // same one calling execute() (eg. say m_command called back on this instance).
    void AsynchronousCommand::cancelExecutionAndWait()
    {
        terminateAndWait();
        m_executing = false;
    }

	void AsynchronousCommand::run()
    {
        TA_Assert(m_command != NULL);

        m_command->execute();

        // TODO LPP: IDEA: Could we start another thread here which calls terminateAndWait() on
        // this object, then we call terminateAndWait() on the thread?  This allows
        // the invariant of Thread to be satisfied (ie. run() is not allowed to call
        // terminateAndWait() directly).
    }

	void AsynchronousCommand::terminate()
    {
        TA_Assert(m_command != NULL);
        m_command->cancelExecutionAndWait();
    }


    // TODO LPP: A better approach would be to pass the begin and end iterators to
    // the constructor, and to write this in terms of a templatised sequential iterator.
    SequentialIterator::SequentialIterator()
    {
    }

    void SequentialIterator::setBounds(const CommandVec_t::iterator& begin, const CommandVec_t::iterator& end)
    {
        m_begin = begin;
        m_end = end;
        
        m_iter = m_begin;    
    }

    CommandVec_t::iterator SequentialIterator::next()
    {
        CommandVec_t::iterator iter(m_iter);
        if (m_iter == m_end)
        {
            m_iter = m_begin;
        }
        else
        {
            ++m_iter;
        }

        return iter;
    }


    SequentialOneShotIterator::SequentialOneShotIterator() :
        m_returnEnd(false)
    {
    }

    void SequentialOneShotIterator::setBounds(const CommandVec_t::iterator& begin, const CommandVec_t::iterator& end)
    {
        m_begin = begin;
        m_end = end;
        
        m_iter = m_begin;    
    }

    CommandVec_t::iterator SequentialOneShotIterator::next()
    {
        if (m_returnEnd)
        {
            m_returnEnd = false;
            return m_end;
        }
        else
        {
            m_returnEnd = true;

            CommandVec_t::iterator iter(m_iter);
            if (m_iter == m_end)
            {
                m_iter = m_begin;
            }
            else
            {
                ++m_iter;
            }

            return iter;
        }
    }


    RandomOneShotIterator::RandomOneShotIterator(std::vector<int> likelihoods)
    {
        for (unsigned int i = 0; i < likelihoods.size(); ++i)
        {
            m_dist.bucket_push_back(likelihoods[i]);
        }
    }

    void RandomOneShotIterator::setBounds(const CommandVec_t::iterator& begin, const CommandVec_t::iterator& end)
    {
        m_begin = begin;
        m_end = end;
    }

    CommandVec_t::iterator RandomOneShotIterator::next()
    {
        TA_Assert((m_end - m_begin) == m_dist.size());

        int index = m_dist.generate();
        TA_Assert(0 <= index && index < (int)m_dist.size());
		LOG2( SourceInfo, DebugUtil::DebugDebug, "RandomOneShotIterator::next(): %s returning child %d", 
            TA_Base_Core::gPtrToStr(this).c_str(), index );

        return m_begin + index;
    }


    CompositeCommand::CompositeCommand(/*takes*/ ICommandVecIterator* iter) :
        m_iter(iter),
        m_executing(false),
        m_cancelExecution(false)
    {
        TA_Assert(m_iter.get() != NULL);
    }

    void CompositeCommand::addChildCommand(ICommand* child)
    {
        CommandVec_t::iterator iter = std::find(m_children.begin(), m_children.end(), child);
        TA_Assert(iter == m_children.end());

        m_children.push_back(child);
        m_iter->setBounds(m_children.begin(), m_children.end());
    }

    void CompositeCommand::removeChildCommand(ICommand* child)
    {
        CommandVec_t::iterator iter = std::find(m_children.begin(), m_children.end(), child);
        TA_Assert(iter != m_children.end());

        m_children.erase(iter);
    }

    void CompositeCommand::execute()
    {
        TA_Assert(!m_executing);
        m_executing = true;
        m_cancelExecution = false;

        TA_Assert(m_iter.get() != NULL);
        
        CommandVec_t::iterator iter = m_iter->next();
        while (iter != m_children.end())
        {
            TA_Assert(*iter != NULL);

            if (m_cancelExecution)
            {
                break;
            }

            (*iter)->execute();
            iter = m_iter->next();
        }

        m_executing = false;
    }

    void CompositeCommand::cancelExecutionAndWait()
    {
        m_cancelExecution = true;

        // TODO LPP: We don't really want to wait for each child to finish executing
        // before cancelling the next child.  Perhaps we need to spawn a thread to
        // cancel each child, then wait for all the threads to finish.
        for (CommandVec_t::iterator iter = m_children.begin(); iter != m_children.end(); ++iter)
        {
            (*iter)->cancelExecutionAndWait();
        }
    }


    RandomDelayCommand::RandomDelayCommand(unsigned int minDelayMs, unsigned int maxDelayMs) :
        m_minDelayMs(minDelayMs),
        m_maxDelayMs(maxDelayMs)
    {
    }

    void RandomDelayCommand::execute()
    {
        int delay = generateUniformRandom(m_minDelayMs, m_maxDelayMs);
		LOG2( SourceInfo, DebugUtil::DebugDebug, "RandomDelayCommand::execute(): %s will sleep for %d ms", 
            TA_Base_Core::gPtrToStr(this).c_str(), delay );

        Thread::sleep(delay);
		LOG1( SourceInfo, DebugUtil::DebugDebug, "RandomDelayCommand::execute(): %s finished sleeping",
            TA_Base_Core::gPtrToStr(this).c_str() );
    }

    void RandomDelayCommand::cancelExecutionAndWait()
    {
        // TODO LPP: We should be able to cancel the delay - which means the execute() thread
        // should block on a Condition, rather than just using sleep().
    }


    void DelayCommand::execute()
    {
	    LOG2( SourceInfo, DebugUtil::DebugDebug, "DelayCommand::execute(): %s will sleep for %d ms", 
            gPtrToStr(this).c_str(), m_delayMs );

        Thread::sleep(m_delayMs);

	    LOG1( SourceInfo, DebugUtil::DebugDebug, "DelayCommand::execute(): %s finished sleeping",
            gPtrToStr(this).c_str() );
    }


    RepeatingCommand::RepeatingCommand(ICommand* command, unsigned int n) :
        m_command(command),
        m_n(n),
        m_executing(false),
        m_cancelExecution(false)
    {
        TA_Assert(m_command != NULL);
    } 

    void RepeatingCommand::execute()
    {
        TA_Assert(m_command != NULL);
        TA_Assert(!m_executing);
        m_executing = true;
        m_cancelExecution = false;

        while (!m_cancelExecution)
        {
            m_command->execute();

            if (m_n > 0)
            {
                if (--m_n == 0)
                {
                    break;
                }
            }
            else
            {
                // If m_n == 0, we loop indefinately
            } 
        }

        m_executing = false;
    }

    void RepeatingCommand::cancelExecutionAndWait()
    {
        TA_Assert(m_command != NULL);

        m_cancelExecution = true;
        m_command->cancelExecutionAndWait();
    }

};  // namespace TA_Base_Core

