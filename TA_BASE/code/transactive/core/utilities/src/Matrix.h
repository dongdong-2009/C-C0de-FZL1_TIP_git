#if !defined(_MATRIX_H__5954866B_94B3_4D73_9FC7_63AACECDE4FE__INCLUDED_)
#define _MATRIX_H__5954866B_94B3_4D73_9FC7_63AACECDE4FE__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/Matrix.h $
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

#include <map>
#include <set>
#include <vector>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_Core
{
    template <typename RowId, typename ColumnId, typename ElementType> 
    class Matrix : virtual public NonReEntrantThreadLockable
    {
    // Operations
    public:
        Matrix();
        virtual ~Matrix();

        /**
          * Adds a new row with the specified ID to the matrix.
          *
          */
        void addRow(RowId rowId);

        /**
          * Adds a new column with the specified ID to the matrix.
          *
          */
        void addColumn(ColumnId columnId);

        /**
          * Removes the row with the specified ID from the matrix.
          * Does nothing if the ID denotes a nonexisting row.
          *
          */
        void removeRow(RowId rowId);

        /**
          * Removes the column with the specified ID from the matrix.
          * Does nothing if the ID denotes a nonexisting column.
          *
          */
        void removeColumn(ColumnId columnId);

        /**
          * Returns the IDs of all rows of the matrix.
          *
          */
        std::vector<RowId> getRowIds();

        /**
          * Returns the IDs of all columns of the matrix.
          *
          */
        std::vector<ColumnId> getColumnIds();

        /**
          * Returns matrix element at the specified row and column.
          * Throws MatrixException if the row ID and/or column ID is not valid.
          *
          */
        // const ElementType& getElement(RowId rowId, ColumnId columnId);
        const ElementType getElement(RowId rowId, ColumnId columnId);

        /**
          * Sets the matrix element at the specified row and column to the specified value.
          * Throws MatrixException if the row ID and/or column ID is not valid.
          *
          */
        void setElement(RowId rowId, ColumnId columnId, const ElementType& element);

    protected:
        bool rowExists(RowId rowId);
        bool columnExists(ColumnId columnId);

    private:
        Matrix(const Matrix<RowId, ColumnId, ElementType>& matrix);
        Matrix<RowId, ColumnId, ElementType>& operator=(const Matrix<RowId, ColumnId, ElementType>& matrix);


    // Types
    private:
        /**
          * Uniquely identifies a single matrix element by its row and column IDs.
          *
          */
        struct ElementId
        {
            ElementId() {}
            ElementId(RowId rowId, ColumnId columnId) : m_rowId(rowId), m_columnId(columnId) {}
            ElementId(const ElementId& elementId) : m_rowId(elementId.m_rowId), m_columnId(elementId.m_columnId) {}

            bool operator<(const ElementId& elementId) const
            {
                if (m_rowId == elementId.m_rowId)
                {
                    return m_columnId < elementId.m_columnId;
                }

                return m_rowId < elementId.m_rowId;
            }

            RowId m_rowId;
            ColumnId m_columnId;
        };
        
        typedef std::map<ElementId, ElementType> ElementMap;
        typedef std::set<RowId> RowIdSet;
        typedef std::set<ColumnId> ColumnIdSet;


    // Attributes
    private:
        ElementMap m_elementMap;      // matrix contents
        RowIdSet m_rowIdSet;          // matrix row IDs
        ColumnIdSet m_columnIdSet;    // matrix column IDs
    };
}


#include "core/utilities/src/Matrix.inl"


#endif // !defined(_MATRIX_H__5954866B_94B3_4D73_9FC7_63AACECDE4FE__INCLUDED_)
