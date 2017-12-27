/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/corba/src/VisitType.cpp $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */

#include <iomanip.h>

#include <stdlib.h>
#include <ctype.h>
#include <list>
#include <algorithm>
#include <orb.hh>
#include <DynamicAny.hh>

// TODO LPP: Make file and dsp need to be updated.


// The following is based on show_TC() from Advanced Corba Programming with C++, 
// Henning & Vinoski, Chapter 16.

// TODO LPP: We could probably use a visitor-style interface for navigation of Corba types.
// Then the client can perform any action, including dumping type information to a 
// string, when a type node is visited.

namespace TA_Base_Core
{
    // Indent to the specified level.
    void indent(std::ostream& stm, int indent_lvl)
    {
        const int INDENT = 4;

        for (int i = 0; i < INDENT * indent_lvl; i++)
        {
            stm << ' ';
        }
    }

    void writeValue(std::ostream& stm, const CORBA::Any * ap)
    {
        CORBA::TypeCode_var tc = ap->type();
        if (tc->kind() == CORBA::tk_octet) 
        {
            stm << "default:" << endl;
        } 
        else 
        {
            stm << "case ";
            switch (tc->kind()) 
            {
            case CORBA::tk_short:
            {
                CORBA::Short s;
                *ap >>= s;
                stm << s;
                break;
            }
            case CORBA::tk_long:
            {
                CORBA::Long l;
                *ap >>= l;
                stm << l;
                break;
            }
            case CORBA::tk_ushort:
            {
                CORBA::UShort us;
                *ap >>= us;
                stm << us;
                break;
            }
            case CORBA::tk_ulong:
            {
                CORBA::ULong ul;
                *ap >>= ul;
                stm << ul;
                break;
            }
            case CORBA::tk_boolean:
            {
                CORBA::Boolean b;
                *ap >>= CORBA::Any::to_boolean(b);
                stm << (b ? "TRUE" : "FALSE");
                break;
            }
            case CORBA::tk_char:
            {
                CORBA::Char c;
                *ap >>= CORBA::Any::to_char(c);
                if (isalnum(c)) 
                {
                    stm << "'" << c << "'";
                } 
                else 
                {
                    stm << "'\\" << setw(3) << setfill('0')
                         << oct << (unsigned)c << "'";
                }
                break;
            }
            case CORBA::tk_longlong:
            {
                CORBA::LongLong ll;
                *ap >>= ll;
                stm << ll;
                break;
            }
            case CORBA::tk_ulonglong:
            {
                CORBA::ULongLong ull;
                *ap >>= ull;
                stm << ull;
                break;
            }
            case CORBA::tk_wchar:
            {
                CORBA::WChar wc;
                *ap >>= CORBA::Any::to_wchar(wc);
                stm << "'" << wc << "'";
                break;
            }
            case CORBA::tk_enum:
            {
                DynamicAny::DynAny_var da = daf->create_dyn_any(*ap);
                DynamicAny::DynEnum_var de = DynamicAny::DynEnum::_narrow(da);
                CORBA::String_var s = de->get_as_string();
                stm << s;
                da->destroy();
                break;
            }
            default:
            {
                // Union discriminator can't be anything else
                abort();
            }
            }

            stm << ":" << endl;
        }
    }


    // Predicate object for find_if algorithm. Returns true
    // if a type code in a container is equal to the type
    // code passed to the constructor.
    struct EqualTypeCodes 
    {
        EqualTypeCodes(CORBA::TypeCode_ptr tc) : 
            m_tc(tc) 
        {
        }

        bool operator()(CORBA::TypeCode_ptr rhs) const 
        {
            return m_tc->equal(rhs);
        }

        CORBA::TypeCode_ptr m_tc;
    };


    CORBA::ORB_var                  orb; // Initialized from main()ya
    DynamicAny::DynAnyFactory_var   daf; // Initialized from main()


    // Show the contents of a type code. 'tcp' is the type code to
    // show, 'tlist' is the list of type codes seen so far,
    // 'indent_lvl' is the current nesting level. 'tlist' is used
    // to prevent getting trapped in an infinite loop for recursive
    // structures and unions.
    void WriteTCToStream(
        std::ostream& stm,
        CORBA::TypeCode_ptr tcp,
        list<CORBA::TypeCode_var>& tlist,
        int indent_lvl)
    {
        static const char* const kind_name[] = 
        {
            "tk_null", "void", "short", "long",
            "unsigned short", "unsigned long", "float",
            "double", "boolean", "char", "octet", "any",
            "CORBA::TypeCode", "CORBA::Principal",
            "interface", "struct", "union", "enum",
            "string", "sequence", "array", "typedef",
            "exception", "long long", "unsigned long long",
            "long double", "wchar", "wstring", "fixed"
        };

        indent(stm, indent_lvl);
    
        stm << kind_name[tcp->kind()];     // Print the TCKind value.

        // Print name and repository ID for those type codes
        // that have these parameters.
        switch (tcp->kind()) 
        {
        case CORBA::tk_objref:
        case CORBA::tk_struct:
        case CORBA::tk_union:
        case CORBA::tk_except:
        case CORBA::tk_enum:
        case CORBA::tk_alias:
        {
            stm << " " << tcp->name()
                << " (" << tcp->id() << "):" << endl;
        }
        default:
            ;       // Do nothing
        }

        // For type codes that have other parameters,
        // show the remaining parameters.
        switch (tcp->kind()) 
        {
        default:                    // No other params to print
            stm << endl;
            break;

        // For fixed types, show digits and scale.
        case CORBA::tk_fixed:
            stm << "<" << tcp->fixed_digits() << ","
                 << tcp->fixed_scale() << ">" << endl;
            break;

        // For enumerations, show the enumerators.
        case CORBA::tk_enum:
        {
            indent(indent_lvl + 1);
            for (CORBA::ULong i = 0; i < tcp->member_count(); i++) 
            {
                stm << tcp->member_name(i);
                if (i < tcp->member_count() - 1)
                {
                    stm << ", ";
                }
            }
            stm << endl;

            break;
        }

        // For strings, show the bound (if any).
        case CORBA::tk_string:
        case CORBA::tk_wstring:
        {
            CORBA::ULong l = tcp->length();
            if (l != 0)
                stm << "<" << l << ">";
            stm << endl;

            break;
        }

        // For sequences, show the bound (if any) and
        // the element type.
        case CORBA::tk_sequence:
        {
            CORBA::ULong l = tcp->length();
            if (l != 0)
                stm << "<" << l << ">";
            stm << ":" << endl;
            CORBA::TypeCode_var etype = tcp->content_type();
            WriteTCToStream(etype, tlist, indent_lvl + 1);

            break;
        }

        // For arrays, show the dimension and element type.
        case CORBA::tk_array:
        {
            CORBA::ULong l = tcp->length();
            stm << "[" << l << "]:" << endl;
            CORBA::TypeCode_var etype = tcp->content_type();
            WriteTCToStream(etype, tlist, indent_lvl + 1);

            break;
        }

        // For typedefs, show the type of the aliased type.
        case CORBA::tk_alias:
        {
            CORBA::TypeCode_var atype = tcp->content_type();
            WriteTCToStream(atype, tlist, indent_lvl + 1);

            break;
        }

        // For structures and exceptions, show the
        // names and types of each member.
        case CORBA::tk_struct:
        case CORBA::tk_except:
        {
            // Avoid a recursive loop by checking whether we
            // have shown this type code before.
            list<CORBA::TypeCode_var>::iterator where;
            where = find_if(
                tlist.begin(),
                tlist.end(),
                EqualTypeCodes(tcp) );

            // If we have not seen this type code before, add it
            // to the list of type codes processed so far and
            // decode the member type codes.
            if (where == tlist.end()) 
            {
                tlist.push_back(CORBA::TypeCode::_duplicate(tcp));
                for (CORBA::ULong i = 0; i < tcp->member_count(); i++) 
                {
                    stm << tcp->member_name(i) << ":" << endl;
                    indent(indent_lvl + 1);
                    CORBA::TypeCode_var mt = tcp->member_type(i);
                    WriteTCToStream(mt, tlist, indent_lvl + 2);
                }
            } 
            else 
            {
                stm << " " << tcp->name()
                     << " (" << tcp->id() << ")" << endl;
            }

            break;
        }

        // For unions, show the discriminator type.
        // Then, for each member, show the case label,
        // member name, and member type. To show the case
        // label, we use the show_label() helper function.
        case CORBA::tk_union:
        {
            // Avoid getting trapped in a recursive loop.
            list<CORBA::TypeCode_var>::iterator where;
            where = find_if(
                tlist.begin(),
                tlist.end(),
                EqualTypeCodes(tcp) );

            // Show the members only if we haven't shown this type
            // code before.
            if (where == tlist.end()) 
            {
                tlist.push_back(CORBA::TypeCode::_duplicate(tcp));
                indent(indent_lvl + 1);

                // Show discriminator type.
                stm << "Discriminator type:" << endl;
                CORBA::TypeCode_var dt;
                dt = tcp->discriminator_type();
                WriteTCToStream(dt, tlist, indent_lvl + 2);

                // Show case label, member name, and
                // member type for each member.
                for (CORBA::ULong i = 0; i < tcp->member_count(); i++) 
                {
                    CORBA::Any_var label = tcp->member_label(i);
                    indent(indent_lvl + 1);
                    show_label(label);
                    indent(indent_lvl + 2);
                    stm << tcp->member_name(i) << ":" << endl;
                    CORBA::TypeCode_var mt = tcp->member_type(i);
                    WriteTCToStream(mt, tlist, indent_lvl + 3);
                }
            } 
            else 
            {
                stm << " " << tcp->name()
                     << " (" << tcp->id() << ")" << endl;
            }
    
            break;
        }

        }
    }


    std::string gTypeCodeToStr(CORBA::TypeCode_ptr tcp)
    {
        list<CORBA::TypeCode_var> tclist;
        std::ostringstream stm;
        
        WriteTCToStream(stm, tcp, tclist, 0);

        return stm.str();
    }


}   // namespace TA_Base_Core