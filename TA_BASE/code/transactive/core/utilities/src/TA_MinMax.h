/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File$
  * @author:   Adam Radics
  * @version:  $Revision: 1.1.2.1 $
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author: adamr $
  *
  * TA_min and TA_max macros
  * Because the boost and default ones conflict
  */
#if !defined(TA_MINMAX)
#define TA_MINMAX

// do it for different case, to make it nice

#define ta_min TA_MIN
#define ta_max TA_MAX

#define TA_min TA_MIN
#define TA_max TA_MAX

// the actual min and max macros

#define TA_MIN(arg1, arg2) ( (arg1) < (arg2) ? (arg1) : (arg2) )

#define TA_MAX(arg1, arg2) ( (arg1) > (arg2) ? (arg1) : (arg2) )


#endif // !defined(TA_MINMAX)
