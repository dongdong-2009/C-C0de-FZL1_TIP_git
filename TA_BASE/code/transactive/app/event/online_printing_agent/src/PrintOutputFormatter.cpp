/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintOutputFormatter.cpp $
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

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;


std::string PrintOutputFormatter::formatData(const ColumnList& columns, const PrintableDataElement& printableDataElement)
{
    FUNCTION_ENTRY("formatData(const Columns& columns, const PrintableDataElement& printableDataElement)");

    // Text output for a single printable data element can span multiple columns and multiple rows.
    // Use a matrix to represent this.
    TA_Base_Core::Matrix<int, int, std::string> outputMatrix;
    int columnCount(columns.size());

    for (int col(0); col < columnCount; col++)
    {
        outputMatrix.addColumn(col);

        PrintableDataElement::const_iterator pdeIt(printableDataElement.find(columns[col].name));
        std::string columnText(pdeIt == printableDataElement.end()? "" : pdeIt->second);
        StringList columnLines(splitIntoLines(columnText, columns[col].width)); 
        int lineCount(columnLines.size());

        for (int ln(0); ln < lineCount; ln++)
        {
            outputMatrix.addRow(ln);
            outputMatrix.setElement(ln, col, columnLines[ln]);
        }
    }

    // Convert the output matrix into a string.
    int lineCount(outputMatrix.getRowIds().size());
    std::ostringstream formattedText;
    formattedText.fill(' '); // pad fields with blanks

    for (int ln(0); ln < lineCount; ln++)
    {
        for (int col(0); col < columnCount; col++)
        {
            formattedText.flags(columns[col].justification==LEFT?std::ios_base::left:std::ios_base::right);
            // Each column must be formatted to the required width and terminated with the appropriate separator.
            formattedText << std::setw(columns[col].width) << outputMatrix.getElement(ln, col) << columns[col].separator;
        }

        if (ln < lineCount - 1)
        {
            formattedText << std::endl;
        }
    }

    FUNCTION_EXIT;
    return formattedText.str();
}


std::string PrintOutputFormatter::formatData(const ColumnList& columns, const PrintableDataElementList& printableDataElements)
{
//    FUNCTION_ENTRY("formatData(const Columns& columns, const PrintableDataElementList& printableDataElements)");

    std::ostringstream formattedText;

    for (PrintableDataElementList::const_iterator pdeIt(printableDataElements.begin()); pdeIt != printableDataElements.end(); pdeIt++)
    {
        formattedText << formatData(columns, *pdeIt) << std::endl;
    }

//    FUNCTION_EXIT;
    return formattedText.str();
}


ColumnList PrintOutputFormatter::parseColumnSpec(const std::string& columnSpec)
{
    FUNCTION_ENTRY("parseColumnSpec(const std::string& columnSpec)");

    // Columns are specified in the following format:
    // <col_name1>:<width>:<justification>:<separator>:<col_name2>:<width>:<justification>:<separator>: ... :<col_nameN>:<width><justification>:[:<separator>]

    ColumnList columns;
    std::string::size_type fieldPos(0);

    do
    {
        std::string name(getColumnSpecField(columnSpec, fieldPos));
        std::string widthStr(getColumnSpecField(columnSpec, fieldPos));
        std::string justification(getColumnSpecField(columnSpec, fieldPos));
        std::string separator(getColumnSpecField(columnSpec, fieldPos));
        int width(::atoi(widthStr.c_str()));

        if (name.empty() ||
            widthStr.empty() ||
            width <= 0 ||
            justification.empty() && (justification!="left" && justification!="right"))
        {
            std::ostringstream msg; 
            msg << "Invalid column specification '" << columnSpec << "'.";

            TA_THROW(OnlinePrintingAgentException(msg.str()));
        }

        Column column;
        column.name = name;
        column.width = static_cast<unsigned short>(width);
        column.separator = separator;
        column.justification = (justification=="left")?LEFT:RIGHT;

        columns.push_back(column);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Extracted column name [%s], width [%d], separator [%s]",
                    column.name.c_str(), column.width, column.separator.c_str());

    } while (fieldPos != std::string::npos);   

    FUNCTION_EXIT;
    return columns;
}


PrintOutputFormatter::StringList PrintOutputFormatter::splitIntoLines(const std::string& text, unsigned short width)
{
//    FUNCTION_ENTRY("splitIntoLines(const std::string& text, unsigned short width)");

    std::istringstream input(text);
    StringList output;
    std::string word(""), line("");
    
    // Break the string into words and then concatenate into lines to fit the width limit.
    // Where a single word exceeds the width, split the word across lines.
    while (input >> word)
    {     
        std::string::size_type lineLength(line.length()); // allow for a white space 
        
        if (lineLength > 0 && lineLength < width)
        {
            lineLength++; // allow for a white space to follow the existing line content
        }
        
        if (word.length() > width)
        {
            // Determine the sublength of the word that will make up the remainder
            // of the line. Where the line is already full, the whole of next line
            // is allocated.
            std::string::size_type utilisedLength(width - lineLength % width);
            std::string::size_type leftoverLength(word.length() - utilisedLength);
             
            // Get the usable portion of the word.
            word = word.substr(0, utilisedLength);  
            
            // Rewind the input stream to point at the cutoff point.
            input.clear();
            input.seekg(-static_cast<std::istringstream::off_type>(leftoverLength), std::ios_base::cur);
        }
        
        if (lineLength + word.length() > width)
        {   
            output.push_back(line);
            // TD14045
            //line.clear();
            line = "";
        }

        line += (line.empty()? "" : " ") + word;
    }
    
    if (!line.empty())
    {
        output.push_back(line);
    }
    
//    FUNCTION_EXIT;
    return output;
}


std::string PrintOutputFormatter::getColumnSpecField(const std::string& columnSpec, std::string::size_type& fieldPos)
{
//    FUNCTION_ENTRY("getColumnSpecField(const std::string& columnSpec, std::string::size_type& fieldPos)");

    if (fieldPos < 0 || fieldPos > columnSpec.length())
    {
//        FUNCTION_EXIT;
        return "";
    }

    static const std::string FIELD_DELIMITER(":");
    static const std::string ESCAPED_FIELD_DELIMITER("\\" + FIELD_DELIMITER);

    // Determine the position of the next delimiter following the starting field position.
    // Ignore all delimiters that have been escaped, as these form part of the field.
    std::string::size_type startPos(fieldPos);
    std::string::size_type endPos(std::string::npos);

    while ((endPos = columnSpec.find(FIELD_DELIMITER, startPos)) != std::string::npos)
    {
        if (endPos != 0 && columnSpec[endPos - 1] != ESCAPED_FIELD_DELIMITER[0])
        {
            break;
        }

        startPos = endPos + 1;
    }

    // Extract the string between the start and end positions.
    std::string field("");

    if (endPos != std::string::npos)
    {
        field = columnSpec.substr(fieldPos, endPos - fieldPos);
        fieldPos = endPos + 1;
    }
    else
    {
        field = columnSpec.substr(fieldPos, endPos);
        fieldPos = endPos;
    }

    // De-escape the field delimiter if necessary.
    std::string::size_type escapePos(std::string::npos);

    while ((escapePos = field.find(ESCAPED_FIELD_DELIMITER)) != std::string::npos)
    {
        field.erase(escapePos, 1);
    }

//    FUNCTION_EXIT;
    return field;
}
