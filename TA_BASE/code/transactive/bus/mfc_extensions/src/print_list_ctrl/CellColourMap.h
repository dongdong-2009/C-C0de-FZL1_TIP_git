/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/print_list_ctrl/CellColourMap.h $
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

#if !defined( _CELL_COLOUR_MAP_H_)
#define _CELL_COLOUR_MAP_H_

#include <afxwin.h>
#include <map>

namespace TA_Base_Bus
{
#ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) CellColourMap
#else
    class AFX_EXT_CLASS CellColourMap
#endif
    {
    public:
        /**
         * Constructor
         *
         * Initialises internal variables.
         */
        CellColourMap( unsigned int numColumns, unsigned int numRows, COLORREF defaultColour, COLORREF defaultBkColour );

        /**
         * Destructor
         *
         * Cleans up any memory allocated during the life of the object.
         */
        virtual ~CellColourMap();

        /**
         * setCellColour
         *
         * Sets the colour of an individual cell.
         *
         * @param column The column the cell is in.
         * @param row The row the cell is in.
         * @param cellColour The colour the cell should be.
         */
        void setCellColour( unsigned int column, unsigned int row, COLORREF cellColour );

        /**
         * setCellBkColour
         *
         * Sets the background colour of an individual cell.
         *
         * @param column The column the cell is in.
         * @param row The row the cell is in.
         * @param cellColour The colour the cell should be.
         */
        void setCellBkColour( unsigned int column, unsigned int row, COLORREF cellBkColour );

        /**
         * setRowColour
         *
         * Sets the colour of an entire row.
         *
         * @param row The row.
         * @param rowColour The colour the row should be.
         */
        void setRowColour( unsigned int row, COLORREF rowColour );

        /**
         * setRowBkColour
         *
         * Sets the background colour of an entire row.
         *
         * @param row The row.
         * @param rowColour The colour the row should be.
         */
        void setRowBkColour( unsigned int row, COLORREF rowBkColour );

        /**
         * getCellColour
         *
         * Returns the colour that should be used to write cell text.
         *
         * @param column The column the cell is in.
         * @param row The row the cell is in.
         *
         * @return COLORREF The colour to write text in.
         */
        COLORREF getCellColour( unsigned int column, unsigned int row );

        /**
         * getCellBkBrush
         *
         * Returns the brush that should be used to fill the cell.
         *
         * @param column The column the cell is in.
         * @param row The row the cell is in.
         *
         * @return CBrush* The brush used to fill the cell.
         */
        CBrush* getCellBkBrush( unsigned int column, unsigned int row );

        /**
         * getDefaultColour
         *
         * Returns the default colour to use when writing text with no
         * colour specified.
         *
         * @return COLORREF The default colour.
         */
        COLORREF getDefaultColour();

        /**
         * getDefaultBkBrush
         *
         * Returns the brush constructed from the default colours.
         *
         * @return CBrush* The default brush.
         */
        CBrush* getDefaultBkBrush();

    private:
        // Various typedefs
        typedef std::map<COLORREF,CBrush*> ColourBrushMap;

        // Dimensions of the 2D array.
        unsigned int m_numColumns;
        unsigned int m_numRows;

        // 2D array for storing cell text colours.
        COLORREF** m_cellColours;

        // This is the default colour that will be used for drawing text.
        COLORREF m_defaultColour;

        // The map that stores COLORREF<->CBrush* pairs.
        ColourBrushMap m_colourToBrush;

        // 2D array for storing brushes for filling cell backgrounds.
        CBrush*** m_cellBkBrushes;

        // This is the default colour that will be used for filling cells.
        CBrush* m_defaultBkBrush;
    };
}; // TA_Base_Bus

#endif // !defined( _CELL_COLOUR_MAP_H_)
