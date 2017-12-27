/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\oraloader.h
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:22 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @ 1999-2002 Core Lab. All right reserved.
//  Dataset
//  Created:            30.06.98
//  Last modified:      09.04.02
//////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __ORALOADER_H__
#define __ORALOADER_H__

#include "crdef.h"
#include "ora.h"

#ifdef OCL_OCI81

namespace ocl
{
	
	class LoaderColumn {
	public:
		
		LoaderColumn();
		LoaderColumn(const CRString& name, OraDataType dataType,
			int size, int precision, int scale = 0, const CRString& dateFormat = "");
		
		CRString name() const;
		void setName(const CRString& name);
		
		OraDataType dataType() const;
		void setDataType(OraDataType dataType);
		
		// Return size of column in bytes.
		// For strings without null-terminator
		int size() const;
		void setSize(int size);
		
		int precision() const;
		void setPrecision(int precision);
		
		int scale() const;
		void setScale(int scale);
		
		CRString dateFormat() const;
		void setDateFormat(const CRString& dateFormat);
		
	private:
		CRString mName;
		OraDataType mDataType;
		int mSize;
		int mPrecision;
		int mScale;
		CRString mDateFormat;
		
		// Zero-based position within table
		int mPosInTable;
		
		// Offset within row in buffer
		int mOffset;
		
		friend class OraLoader;
		friend class CRNamedList<LoaderColumn>;
	};
	
	
	class OraLoader {
	public:
		
		OraLoader();
		OraLoader(OraConnection& connection, const CRString& tableName);
		
		~OraLoader();
		
		OraConnection* connection();
		void setConnection(OraConnection* connection);
		
		CRString tableName();
		void setTableName(const CRString& tableName);
		
		// If true throw exception on illegal row
		// and stop loading remaining rows
		bool breakOnError() const;
		void setBreakOnError(bool value);
		
		void addColumn(LoaderColumn& column);
		void addColumn(const CRString& name, OraDataType dataType,
			int size, int precision, int scale = 0, const CRString& dateFormat = "");
		
		int columnCount();
		
		void getColumn(int index, LoaderColumn& column);
		void getColumn(const CRString name, LoaderColumn& column);
		
		void createColumns();
		void clearColumns();
		
		// Prepare for loading
		void beginLoad();
		
		// Clear loader structures
		void finishLoad();
		
		void putNextRow();
		
		// Return column and row
		// in wich error was occured
		int columnError() const;
		int rowError() const;
		
		void putColumnData(int columnIndex, const CRString& data);
		void putColumnData(const CRString& columnName, const CRString& data);
		
		void putColumnData(int columnIndex, const char* data);
		void putColumnData(const CRString& columnName, const char* data);  
		
		void putColumnData(int columnIndex, const CRDate& data);
		void putColumnData(const CRString& columnName, const CRDate& data);
		
		void putColumnData(int columnIndex, int data);
		void putColumnData(const CRString& columnName, int data);
		
		void putColumnData(int columnIndex, double data);
		void putColumnData(const CRString& columnName, double data);
		
		void putColumnData(int columnIndex, const CRValue& data);
		void putColumnData(const CRString& columnName, const CRValue& data);
		
		void putColumnNull(int columnIndex);
		
	private:
		
		// Disable coping
		OraLoader(const OraLoader&) {}
		
		// Called by constructors
		void _init();
		
		// Throw exception if session is not connected
		void checkSession();
		
		// Do actual load to DB when buffer is full.
		// Return true on success
		void loadBuffer();
		
		// Reset OCI structures and buffer data
		void reset();
		
		// Write data to the buffer
		void putColumnData(int columnIndex, void* data, int dataLength);
		
		bool mBreakOnError;
		
		enum State {
			prepared,  // beginLoad was called
				finished   // finishLoad was called
		};
		
		// State of the OraLoader object
		State mLoaderState;
		
		OCIDirPathCtx* hDirPathCtx;
		OCIDirPathColArray* hColumnArray;
		OCIDirPathStream* hStream;
		
		// Connection that used for loading
		OraConnection* mConnection;
		
		// Loaded table name
		CRString mTableName;
		
		// Array of columns in loaded table
		CRNamedList<LoaderColumn> mColumns;
		
		// Is columns created with createColumns() function
		bool isColumnsAutoCreated;
		
		// Size of row in bytes
		int mRowSize;
		
		// Position in the buffer where next row will be placed
		int mCurrentRow;
		
		// Row from which loading begin
		int mBeginToLoadRow;
		
		// End row for loading
		int mEndToLoadRow;
		
		// Number of rows that buffer can contain
		int mMaxRowsInBuffer;
		
		// Column and row in wich error occured
		int mColumnError;
		int mRowError;
		
		// Buffer that contains loader data
		void* mBuffer;
};

} // namespace ocl

#endif // OCL_OCI81

#endif //__ORALOADER_H__
