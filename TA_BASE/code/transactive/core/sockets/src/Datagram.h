/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/Datagram.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// Datagram.h
//
// a base class for datagrams as used by IDatagramSocket.h
//
// all a datagram has to be able to do at this level is to
// present itself as a simple byte buffer as this maps in the
// most efficient way to how OS datagram functions work.  A more
// considered design that hides this crude presentation more
// effectively using containers, iterators and such wouldn't
// work as raw char *'s would still have to be constructed or cast
// for the OS functions.  So leave it crude (but simple and efficient)
// for now...
//
// The datagrams are fixed size.  There really is no point reallocating
// these sorts of things.
//
// Instead of sub-classing this, consider decorators or some other
// lightweight compositional design for interpreting different
// payload types.  See ICMPMessageCodec.

#ifndef DATAGRAM_H
#define DATAGRAM_H

namespace TA_Base_Core
{
	class Datagram
	{
	public:
		Datagram();
		Datagram(const Datagram &copy);
		virtual ~Datagram();

		// it is safe to modify buf up to it's size as returned by the int.
		// What this means is that if you want to use an instance of
		// Datagram to be filled with data of an indeterminate (but known
		// maximum) size then you firstly do something like:
		//
		//   myDatagram.SetBuffer(0, maxSize > Datagram::MaxDatagramSize ? Datagram::MaxDatagramSize : maxSize);
		//
		// which ensures it's big enough. Now you can use the returned
		// buf with GetBuffer() safely.  If you then want to make the Datagram
		// fit the actual number of bytes then do:
		//
		//   myDatagram.SetBuffer(0, bufsize);
		virtual unsigned int GetBuffer(char** buf);

		// if buf == 0, then only the size is set - no data copy is invoked.
		// if numBytes > MaxDatagramSize then the extra bytes will be
		// ignored.
		virtual void SetBuffer(const char* buf, unsigned int numBytes);

		// MaxDatagramSize may not correspond to the system MTU.  Instead, you
		// should be aware of the system MTU and use that for capping
		// the size of the datagram together with MaxDatagramSize
		enum Limits
		{
			MaxDatagramSize = 1500
		};

	private:
		char m_buffer[MaxDatagramSize];
		unsigned int m_bufferSize;
	};
}

#endif
