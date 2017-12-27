/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/ICommand.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if !defined( ICOMMAND_H )
#define ICOMMAND_H

#include "core/utilities/src/RandomNumberGenerator.h"
#include "core/threads/src/Thread.h"
#include <vector>

namespace TA_Base_Core
{
    // The ICommand framework allows commands to be executed by calling the execute()
    // method.  If a command executed by an ICommand implementation requires arguments, 
    // these are typically provided to the command object prior to calling execute.

    // SYNCHRONOUS or ASYNCHRONOUS - execute() can return when it has finished
    // executing the command, or spawn a worker thread that executes the command 
    // asynchronously and return immediately.

    // REENTRANT or NON-REENTRANT - execute() can be called multiple times simultaneously
    // from the same thread (if it is asynchronous) or a different thread.  The implementation
    // can handle this in different ways:
    // - use a lock so that only one thread at a time can enter execute(). 
    //   cancelExecutionAndWait() will then only cancel the execution of the thread
    //   currently holding the lock.
    // - allow multiple threads to enter execute().
    //   The command must either not maintain any state, or must maintain one copy of
    //   it's state for each thread.
    //   cancelExecutionAndWait() will need to cancel the execution of all threads
    //   currently in execute().
    // - restart the execution each time execute() is called.

    // Alternatively, a non-reentrant implementation only allows one call to execute() 
    // at a time.  This can be enforced by:
    // - asserting that execute() is not reentrant
    // - using a lock

    struct ICommand
    {
        // Instructs the command object to execute the command.
        virtual void execute() = 0;

        // Instructs the command object to stop executing, regardless of whether it has
        // finished.  The implementation of cancelExecutionAndWait() must wait for the command
        // to stop executing before returning.
        virtual void cancelExecutionAndWait() = 0;
    };

    // Executes a synchronous command asynchronously by calling command->execute() from 
    // a worker thread.  
    // ASYNCHRONOUS
    // NON-REENTRANT (asserted)
    class AsynchronousCommand : public ICommand, public Thread
    {
    public:
        AsynchronousCommand(ICommand* command);
        ~AsynchronousCommand();

        // Implementation of ICommand
        virtual void execute();
        virtual void cancelExecutionAndWait();

	    // Implementation of Thread
	    virtual void run();			
	    virtual void terminate();

    private:
        ICommand* m_command;

        volatile bool m_executing;
    };

    typedef std::vector<ICommand*> CommandVec_t;

    struct ICommandVecIterator
    {
        // TODO LPP: It seems messy that the iterator itself knows it's own bounds.
        virtual void setBounds(const CommandVec_t::iterator& begin, const CommandVec_t::iterator& end) = 0;

        // Returns an iterator to the next element in the CommandVec_t sequence.
        // If the end of the sequence is reached, an iterator to end() should be
        // returned.  The following call to next() will return a valid iterator,
        // probably to the start of the sequence.
        // Note that the number of elements that next() iterates through
        // doesn't have to be the same as the number of elements in the sequence.
        virtual CommandVec_t::iterator next() = 0;
    };

    // Iterates through all elements of the vector in order.  Returns to the 
    // beginning of the sequence after end() is reached. 
    // Eg. (0, 1, ..., n-1, end, 0, 1, ...)
    class SequentialIterator : public ICommandVecIterator
    {
    public:
        SequentialIterator();

        // Implementation of ICommandVecIterator
        virtual void setBounds(const CommandVec_t::iterator& begin, const CommandVec_t::iterator& end);
        virtual CommandVec_t::iterator next();

    private:
        CommandVec_t::iterator m_begin;
        CommandVec_t::iterator m_end;
        CommandVec_t::iterator m_iter;
    };

    // Iterates through all elements of the vector, returning end() each
    // time the iterator is advanced, and also when the end of the 
    // underlying sequence is reached. 
    // Eg. (0, end, 1, end, ..., n-1, end, end, end, 0, end, ...)
    class SequentialOneShotIterator : public ICommandVecIterator
    {
    public:
        SequentialOneShotIterator();

        // Implementation of ICommandVecIterator
        virtual void setBounds(const CommandVec_t::iterator& begin, const CommandVec_t::iterator& end);
        virtual CommandVec_t::iterator next();

    private:
        CommandVec_t::iterator m_begin;
        CommandVec_t::iterator m_end;
        CommandVec_t::iterator m_iter;
        
        // Toggles to indicate whether the following call to next() should return end.
        bool m_returnEnd;
    };

    // Selects an element at random (using a uniform distribution), returning
    // end() each time the iterator is advanced.  Note that this sequence
    // appears to be of infinite length.
    // Eg. (3, end, 1, end, ..., 6, end, 3, end, ...)
    class RandomOneShotIterator : public ICommandVecIterator
    {
    public:
        // (m_begin - m_end) must equal likelihoods.size()
        // TODO LPP: Having to pass a vector of probabilities into the constructor is
        // inflexible.  It makes changing the size of the vector difficult later.
        RandomOneShotIterator(std::vector<int> likelihoods);

        // Implementation of ICommandVecIterator
        virtual void setBounds(const CommandVec_t::iterator& begin, const CommandVec_t::iterator& end);
        virtual CommandVec_t::iterator next();

    private:
        CommandVec_t::iterator m_begin;
        CommandVec_t::iterator m_end;

        DiscreteProbabilityDistribution m_dist;
    };

    // Calls execute() synchronously on the child commands (possibly a subset), 
    // in the order determined by the ICommandVecIterator.
    // SYNCHRONOUS
    // NON-REENTRANT (asserted)
    class CompositeCommand : public ICommand
    {
    public:
        CompositeCommand(/*takes*/ ICommandVecIterator* iter);

        // addChildCommand() must not be called if child has already been added.
        // removeChildCommand() must not be called if child has already been removed.
        void addChildCommand(ICommand* child);
        void removeChildCommand(ICommand* child);

        // Implementation of ICommand
        virtual void execute();
        virtual void cancelExecutionAndWait();

    private:
        CommandVec_t m_children;
        std::auto_ptr<ICommandVecIterator> m_iter;

        volatile bool m_executing;
        volatile bool m_cancelExecution;
    };


    // Pauses the thread calling execute() for a random period.  The delay is a 
    // uniformly distributed random number in the range [minDelayMs, maxDelayMs),
    // generated each time execute() is called.
    // SYNCHRONOUS
    // REENTRANT (different threads)
    class RandomDelayCommand : public ICommand
    {
    public:
        RandomDelayCommand(unsigned int minDelayMs, unsigned int maxDelayMs); 

        // Implementation of ICommand
        virtual void execute();
        virtual void cancelExecutionAndWait();

    private:
        unsigned int m_minDelayMs;
        unsigned int m_maxDelayMs;
    };


    // Pauses the thread calling execute() for a fixed period.
    // SYNCHRONOUS
    // REENTRANT (different threads)
    class DelayCommand : public ICommand
    {
    public:
        DelayCommand(unsigned int delayMs) :
            m_delayMs(delayMs) {}

        // Implementation of ICommand
        virtual void execute();
        virtual void cancelExecutionAndWait() {}

    private:
        unsigned int m_delayMs;
    };


    // Executes a command n times (or indefinately if n == 0).
    // SYNCHRONOUS
    // NON-REENTRANT (asserted)
    class RepeatingCommand : public ICommand
    {
    public:
        RepeatingCommand(ICommand* command, unsigned int n); 

        // Implementation of ICommand
        virtual void execute();
        virtual void cancelExecutionAndWait();

    private:
        ICommand* m_command;
        unsigned int m_n;

        volatile bool m_executing;
        volatile bool m_cancelExecution;
    };


};  // namespace TA_Base_Core


#endif   // ICOMMAND_H
