/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/print_list_ctrl/CellColourMap.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * As both the EventListComponent and the AlarmListControl are virtual lists cell colour is
  * implemented by handing the NM_CUSTOMDRAW message. As a result, the list control itself does
  * not have any knowledge of what colours will be used until the drawing actually takes place.
  * In order to allow the colour information to be accurately presented when printing a method
  * was required to map from list control cells to foreground and background colours.
  *
  * This class maps individual cells to the colours and brushes required to set cell text and
  * background colours.
  */

#pragma warning (disable:4786)

#include "bus/mfc_extensions/src/print_list_ctrl/CellColourMap.h"

namespace TA_Base_Bus
{
    CellColourMap::CellColourMap( unsigned int numColumns, unsigned int numRows, COLORREF defaultColour,
                                  COLORREF defaultBkColour )
        : m_numColumns(numColumns), m_numRows(numRows), m_cellColours(NULL), m_defaultColour(defaultColour),
          m_cellBkBrushes(NULL), m_defaultBkBrush(NULL)
    {
        // Create the default brush.
        m_defaultBkBrush = new CBrush();
        m_defaultBkBrush->CreateSolidBrush( defaultBkColour );

        // Add it to the map.
        m_colourToBrush[defaultBkColour] = m_defaultBkBrush;

        // Initialise the cell foreground colour array and cell background brush array.
        m_cellColours = new COLORREF*[m_numColumns];
        m_cellBkBrushes = new CBrush**[m_numColumns];
        for ( unsigned int column=0; column<m_numColumns; column++ )
        {
            m_cellColours[column] = new COLORREF[m_numRows];
            m_cellBkBrushes[column] = new CBrush*[m_numRows];
            for ( unsigned int row=0; row<m_numRows; row++ )
            {
                m_cellColours[column][row] = m_defaultColour;
                m_cellBkBrushes[column][row] = m_defaultBkBrush;
            }
        }
    }

    CellColourMap::~CellColourMap()
    {
        // Free up any memory allocated during the constructor.
        for ( unsigned int x=0; x<m_numColumns; x++ )
        {
            delete [] m_cellColours[x];
            delete [] m_cellBkBrushes[x];
        }
        delete [] m_cellColours;
        delete [] m_cellBkBrushes;
        m_cellColours = NULL;
        m_cellBkBrushes = NULL;

        // Free up any brushes that are currently allocated.
        ColourBrushMap::iterator it;
        for ( it=m_colourToBrush.begin(); it!=m_colourToBrush.end(); it++ )
        {
            delete it->second;
        }
        m_colourToBrush.clear();
    }

    void CellColourMap::setCellColour( unsigned int column, unsigned int row, COLORREF cellColour )
    {
        // Store the colour.
        m_cellColours[column][row] = cellColour;
    }

    void CellColourMap::setCellBkColour( unsigned int column, unsigned int row, COLORREF cellBkColour )
    {
        CBrush* brushToUse = NULL;

        // See if we already have a brush for this colour.
        ColourBrushMap::iterator it = m_colourToBrush.find( cellBkColour );
        if ( it == m_colourToBrush.end() )
        {
            // If we don't already have a brush for this colour...

            // Create the brush.
            brushToUse = new CBrush();
            brushToUse->CreateSolidBrush( cellBkColour );

            // Add it to the map.
            m_colourToBrush[cellBkColour] = brushToUse;
        }
        else
        {
            // Otherwise just use the existing brush.
            brushToUse = it->second;
        }

        // Store the required brush.
        m_cellBkBrushes[column][row] = brushToUse;
    }

    void CellColourMap::setRowColour( unsigned int row, COLORREF rowColour )
    {
        // Store the colour for each cell in the row.
        for ( unsigned int i=0; i<m_numColumns; i++ )
        {
            m_cellColours[i][row] = rowColour;
        }
    }

    void CellColourMap::setRowBkColour( unsigned int row, COLORREF rowBkColour )
    {
        CBrush* brushToUse = NULL;

        // See if we already have a brush for this colour.
        ColourBrushMap::iterator it = m_colourToBrush.find( rowBkColour );
        if ( it == m_colourToBrush.end() )
        {
            // If we don't already have a brush for this colour...

            // Create the brush.
            brushToUse = new CBrush();
            brushToUse->CreateSolidBrush( rowBkColour );

            // Add it to the map.
            m_colourToBrush[rowBkColour] = brushToUse;
        }
        else
        {
            // Otherwise just use the existing brush.
            brushToUse = it->second;
        }

        // Store the required brush for all columns in the row.
        for ( unsigned int i=0; i<m_numColumns; i++ )
        {
            m_cellBkBrushes[i][row] = brushToUse;
        }
    }

    COLORREF CellColourMap::getCellColour( unsigned int column, unsigned int row )
    {
        // Return the cell colour.
        return m_cellColours[column][row];
    }

    CBrush* CellColourMap::getCellBkBrush( unsigned int column, unsigned int row )
    {
        // Return the cell brush.
        return m_cellBkBrushes[column][row];
    }

    COLORREF CellColourMap::getDefaultColour()
    {
        // Return default colour.
        return m_defaultColour;
    }

    CBrush* CellColourMap::getDefaultBkBrush()
    {
        // Return default brush.
        return m_defaultBkBrush;
    }
}; // TA_Base_Bus