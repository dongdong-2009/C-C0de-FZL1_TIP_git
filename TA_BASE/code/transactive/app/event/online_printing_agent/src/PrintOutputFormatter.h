#if !defined(PRINTOUTPUTFORMATTER_H_INCLUDED)
#define PRINTOUTPUTFORMATTER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintOutputFormatter.h $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Produces a string representation of a printable data element (e.g. event)
  * formatted according to the configured column specification.
  *
  */

namespace TA_Base_App
{
    class PrintOutputFormatter
    {
    // Operations
    public:
        /**
         * formatData
         *
         * Formats the specified printable data element.
         */
        static std::string formatData(const ColumnList& columns, const PrintableDataElement& printableDataElement);

        /**
         * formatData
         *
         * Formats the specified list of printable data elements.
         */
        static std::string formatData(const ColumnList& columns, const PrintableDataElementList& printableDataElements);

        /**
         * parseColumnSpec
         *
         * Parses a printable column specification as defined in agent configuration and returns as a data structure.
         * Throws an OnlinePrintingAgentException if the column specification is not of the correct format.
         */
        static ColumnList parseColumnSpec(const std::string& columnSpec);

    private:
        typedef std::vector<std::string> StringList;

        /**
         * splitIntoLines
         *
         * Breaks up the specified text into lines of length <= width. Returns the lines as a vector of strings.
         * The text is broken on word boundaries. This function does not handle words longer than width.
         */         
        static StringList splitIntoLines(const std::string& text, unsigned short width);

        /**
         * getColumnSpecField
         *
         * Returns the field in the column specification string at the given position. The field position
         * is incremented to point to the next field, or set to npos if there are no more fields.
         */  
        static std::string getColumnSpecField(const std::string& columnSpec, std::string::size_type& fieldPos);
    };
}

#endif // !defined(PRINTOUTPUTFORMATTER_H_INCLUDED)
