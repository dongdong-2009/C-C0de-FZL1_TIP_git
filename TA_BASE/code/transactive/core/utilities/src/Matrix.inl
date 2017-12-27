/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/Matrix.inl $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is a container class that enables storage of items of the specified type in 
  * the form of a 2-dimensional matrix.
  * Individual items (matrix elements) are identified by their row ID and column ID.
  * The rows and columns of the matrix are not sorted in any particular way.
  *
  * For non-basic RowId/ColumnId types, copy constructor and assignment operator should 
  * be provided, along with the <, == and > comparison operators.
  *
  * For non-basic ElementType, copy constructor and assignment operator must be provided.
  *
  * The functionality offered by this class is nowhere near as comprehensive as that of, 
  * say, Boost's multi array, but it does enable the user to specify row/column index
  * types, plus it allows specific rows/columns to be removed as well as added.
  *
  * This implementation focuses on storage and element access efficiency, at the expense 
  * of resizing performance.
  *
  */

#include <sstream>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/MatrixException.h"
#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_Base_Core
{
    template <typename RowId, typename ColumnId, typename ElementType>
    Matrix<RowId, ColumnId, ElementType>::Matrix()
    {
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    Matrix<RowId, ColumnId, ElementType>::~Matrix()
    {
        ThreadGuard matrixGuard(*this);

        m_rowIdSet.clear();
        m_columnIdSet.clear();
        m_elementMap.clear();
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    void Matrix<RowId, ColumnId, ElementType>::addRow(RowId rowId)
    {
        ThreadGuard matrixGuard(*this);

        // If the row already exists do nothing.
        if (rowExists(rowId))
        {
            return;
        }

        // Create as many default elements for the new row as there are columns.
        typename ColumnIdSet::const_iterator colIter;
        for (colIter = m_columnIdSet.begin(); colIter != m_columnIdSet.end(); colIter++)
        {
            typename ElementMap::iterator elementIter(m_elementMap.find(ElementId(rowId, *colIter)));
            TA_ASSERT(elementIter == m_elementMap.end(), "Row being added has existing elements")

            m_elementMap.insert(std::make_pair(ElementId(rowId, *colIter), ElementType()));
        }

        // Store the new row ID.
        m_rowIdSet.insert(rowId);
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    void Matrix<RowId, ColumnId, ElementType>::addColumn(ColumnId columnId)
    {
        ThreadGuard matrixGuard(*this);

        // If the column already exists do nothing.
        if (columnExists(columnId))
        {
            return;
        }

        // Create as many default elements for the new column as there are rows.
        typename RowIdSet::const_iterator rowIter;
        for (rowIter = m_rowIdSet.begin(); rowIter != m_rowIdSet.end(); rowIter++)
        {
            typename ElementMap::iterator elementIter(m_elementMap.find(ElementId(*rowIter, columnId)));
            TA_ASSERT(elementIter == m_elementMap.end(), "Column being added has existing elements")

            m_elementMap.insert(std::make_pair(ElementId(*rowIter, columnId), ElementType()));
        }

        // Store the new column ID.
        m_columnIdSet.insert(columnId);
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    void Matrix<RowId, ColumnId, ElementType>::removeRow(RowId rowId)
    {
        ThreadGuard matrixGuard(*this);

        // If the row does not exist do nothing.
        if (!rowExists(rowId))
        {
            return;
        }

        // Remove all elements associated with this row across all columns.
        typename ColumnIdSet::const_iterator colIter;
        for (colIter = m_columnIdSet.begin(); colIter != m_columnIdSet.end(); colIter++)
        {
            typename ElementMap::iterator elementIter(m_elementMap.find(ElementId(rowId, *colIter)));
            TA_ASSERT(elementIter != m_elementMap.end(), "Row being removed has missing elements")
            
            m_elementMap.erase(elementIter);
        }

        // Remove the specified row ID.
        m_rowIdSet.erase(rowId);
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    void Matrix<RowId, ColumnId, ElementType>::removeColumn(ColumnId columnId)
    {
        ThreadGuard matrixGuard(*this);

        // If the column does not exist do nothing.
        if (!columnExists(columnId))
        {
            return;
        }

        // Remove all elements associated with this column across all rows.
        typename RowIdSet::const_iterator rowIter;
        for (rowIter = m_rowIdSet.begin(); rowIter != m_rowIdSet.end(); rowIter++)
        {
            typename ElementMap::iterator elementIter(m_elementMap.find(ElementId(*rowIter, columnId)));
            TA_ASSERT(elementIter != m_elementMap.end(), "Column being removed has missing elements")
            
            m_elementMap.erase(elementIter);
        }

        // Remove the specified column ID.
        m_columnIdSet.erase(columnId);
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    std::vector<RowId> Matrix<RowId, ColumnId, ElementType>::getRowIds()
    {
        ThreadGuard matrixGuard(*this);

        std::vector<RowId> rowIds;
        rowIds.reserve(m_rowIdSet.size());

        typename RowIdSet::const_iterator rowIter;
        for (rowIter = m_rowIdSet.begin(); rowIter != m_rowIdSet.end(); rowIter++)
        {
            rowIds.push_back(*rowIter);
        }

        return rowIds;
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    std::vector<ColumnId> Matrix<RowId, ColumnId, ElementType>::getColumnIds()
    {
        ThreadGuard matrixGuard(*this);

        std::vector<ColumnId> columnIds;
        columnIds.reserve(m_columnIdSet.size());

        typename ColumnIdSet::const_iterator colIter;
        for (colIter = m_columnIdSet.begin(); colIter != m_columnIdSet.end(); colIter++)
        {
            columnIds.push_back(*colIter);
        }
           
        return columnIds;
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    const ElementType Matrix<RowId, ColumnId, ElementType>::getElement(RowId rowId, ColumnId columnId)
    {
        ThreadGuard matrixGuard(*this);

        typename ElementMap::const_iterator elementIter(m_elementMap.find(ElementId(rowId, columnId)));

        // Return the element if located.
        if (elementIter != m_elementMap.end())
        {
            return elementIter->second;
        }

        // The element could not be found - raise an exception
        std::ostringstream msg;
        msg << "Invalid element coordinates (row ID: " << rowId << ", column ID: " << columnId << ")";

        TA_THROW(MatrixException(msg.str()));
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    void Matrix<RowId, ColumnId, ElementType>::setElement(RowId rowId, ColumnId columnId, const ElementType& element)
    {
        ThreadGuard matrixGuard(*this);

        typename ElementMap::iterator elementIter(m_elementMap.find(ElementId(rowId, columnId)));

        // Update the element if located.
        if (elementIter != m_elementMap.end())
        {
            elementIter->second = element;

            return;
        }

        // The element could not be found - raise an exception
        std::ostringstream msg;
        msg << "Invalid element coordinates (row ID: " << rowId << ", column ID: " << columnId << ")";

        TA_THROW(MatrixException(msg.str()));
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    bool Matrix<RowId, ColumnId, ElementType>::rowExists(RowId rowId)
    {
        return (m_rowIdSet.find(rowId) != m_rowIdSet.end());
    }


    template <typename RowId, typename ColumnId, typename ElementType>
    bool Matrix<RowId, ColumnId, ElementType>::columnExists(ColumnId columnId)
    {
        return (m_columnIdSet.find(columnId) != m_columnIdSet.end());
    }
}

