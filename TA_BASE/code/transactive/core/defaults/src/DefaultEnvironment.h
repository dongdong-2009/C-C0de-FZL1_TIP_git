#if !defined(DefaultEnvironment_H)
#define DefaultEnvironment_H
/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  Adam Radics
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 * This is used for common #pragma statements etc
 * It provides one single place to disable all the meaningless VC++6 warnings
 * without having pragmas included everywhere.
 *
 * Also common enviromental #defines can be done here.
 *
 * Note this is a top level include. When the code is re-organised this should
 * be put at the top level, rather than in a component or layer.
 */


// This is specific for Visual C++ 6
// 1300 is the version number of the compiler, not VC itself.
// VC6 has compiler version 12.00, VC7 has 13.00, VC7.1 has 13.10)

#if defined(_MSC_VER) && _MSC_VER < 1300

// see http://msdn.microsoft.com/en-us/library/aa249862%28VS.60%29.aspx 
// for warning descriptions

// disable these warnings
// 4503: decorated name length exceeded, name was truncated
// 4786: identifier was truncated to '255' characters in the debug information

#pragma warning(disable: 4503 4786 )

// do not disable these warnings:
// 4018: signed/unsigned mismatch
//       Reason: This calls for a static_cast, it will be picked up on gcc also

// 4101: unreferenced local variable
//       Reason: Remove your unused locals!

// 4146: unary minus operator applied to unsigned type, result still unsigned
//       Reason: this calls for a static_cast, it will be picked up on gcc also

// 4250: 'class1' : inherits 'class2::member' via dominance
//       Reason: This should be checked when it happens, and disabled locally if ok

// 4284: return type for 'operation' is 'type' (ie; not a UDT or reference to a UDT.  Will produce errors if applied using infix notation)
//       Reason: Check this, it is mostly related to templates and can indicate an error. Disable locally if ok

// 4290: C++ exception specification ignored
//       Reason: There should be no use of exception specifications in C++

// 4355: 'this' used in base member initializer list
//       Reason: This can be disabled in the CPP file on a case by case basis. Any occurrence of this should be checked to ensure it is ok

// 4715: not all control paths return a value
//       Reason: This is something that might be a valid warning. Locally disable in the cpp file if necessary

// 4723: potential divide by 0
//       Reason: This is an actual warning, dont disable it.

#endif


#endif // !defined(DefaultEnvironment_H)
