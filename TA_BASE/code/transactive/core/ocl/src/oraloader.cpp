/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\oraloader.cpp
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:20 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2002 Core Lab. All right reserved.
//  Dataset
//  Created:            30.06.98
//  Last modified:      09.04.02
//////////////////////////////////////////////////

#include "ocl.conf"
#include "oraloader.h"

#ifdef OCL_OCI81

#include <stdio.h>

namespace ocl
{
	
	// Defined in ora.cpp
	void CRDateToOraDate(CRDate date, void* oraDate);
	
	const char SConnectionNotDefined[] = "Connection is not defined";
	const char STableNameNotDefined[]  = "Table name must be defined";
	const char SCanNotCreateColumn[]   = "Table contains columns that are not supported by OraLoader";
	const char SUnknownDataType[]      = "DataType is not supported by OraLoader";
	const char SLoaderError[]          = "Loader error: row %d, col %d. ";
	const char SInvalidColumnNumber[]  = "Invalid column number";
	const char SLoadedDataToLarge[]    = "Loaded data to large";
	const char SCallBeginLoad[]        = "You must first call beginLoad function";
	const char SCannotConvert[]        = "Can not convert to column data type";
	
	const int LOADER_BUFFER_SIZE = 0x10000 * 2;
	
	/// LoaderColumn ///
	
	LoaderColumn::LoaderColumn() {
		mDataType   = dtUnknown;
		mSize       = 0;
		mPrecision  = 0;
		mScale      = 0;
		mPosInTable = -1;
		mOffset     = 0;
	}
	
	LoaderColumn::LoaderColumn(const CRString& name, OraDataType dataType,
		int size, int precision, int scale, const CRString& dateFormat)
	{
		mName       = name;
		mDataType   = dataType;
		mSize       = size;
		mPrecision  = precision;
		mScale      = scale;
		mDateFormat = dateFormat;
		mPosInTable = -1;
		mOffset     = 0;
	}
	
	CRString LoaderColumn::name() const {
		return mName;
	}
	
	void LoaderColumn::setName(const CRString& name) {
		mName = name;
	}
	
	OraDataType LoaderColumn::dataType() const {
		return mDataType;
	}
	
	void LoaderColumn::setDataType(OraDataType dataType) {
		mDataType = dataType;
	}
	
	int LoaderColumn::size() const {
		return mSize;
	}
	
	void LoaderColumn::setSize(int size) {
		mSize = size;
	}
	
	int LoaderColumn::precision() const {
		return mPrecision;
	}
	
	void LoaderColumn::setPrecision(int precision) {
		mPrecision = precision;
	}
	
	int LoaderColumn::scale() const {
		return mScale;
	}
	
	void LoaderColumn::setScale(int scale) {
		mScale = scale;
	}
	
	CRString LoaderColumn::dateFormat() const {
		return mDateFormat;
	}
	
	void LoaderColumn::setDateFormat(const CRString& dateFormat) {
		mDateFormat = dateFormat;
	}
	
	/// OraLoader ///
	
	OraLoader::OraLoader() {
		_init();
	}
	
	OraLoader::OraLoader(OraConnection& connection, const CRString& tableName) {
		
		_init();
		
		mConnection = &connection;
		mTableName  = tableName;
	}
	
	OraLoader::~OraLoader() {
		try {
			finishLoad();
		}
		catch (...) {
			// Supress all exceptions
		}
		mColumns.clear();
	}
	
	void OraLoader::_init() {
		
		hDirPathCtx      = 0;
		hColumnArray     = 0;
		hStream          = 0;
		mConnection      = 0;
		mRowSize         = 0;
		mCurrentRow      = 0;
		mBeginToLoadRow  = 0;
		mEndToLoadRow    = 0;
		mMaxRowsInBuffer = 0;
		mBuffer          = 0;
		mColumnError     = -1;
		mRowError        = -1;
		mBreakOnError    = true;
		mLoaderState     = finished;
		isColumnsAutoCreated = false;
	}
	
	OraConnection* OraLoader::connection() {
		
		return mConnection;
	}
	
	void OraLoader::setConnection(OraConnection* connection) {
		mConnection = connection;
	}
	
	CRString OraLoader::tableName() {
		
		return mTableName;
	}
	
	void OraLoader::setTableName(const CRString& tableName) {
		
		if (mTableName != tableName) {
			mTableName = tableName;
			clearColumns();
		}
	}
	
	bool OraLoader::breakOnError() const {
		return mBreakOnError;
	}
	
	void OraLoader::setBreakOnError(bool value) {
		mBreakOnError = value;
	}
	
	void OraLoader::addColumn(LoaderColumn& column) {
		
		if (mColumns.count() && isColumnsAutoCreated) {
			isColumnsAutoCreated = false;
			clearColumns();
		}
		
		mColumns.add(&column);
	}
	
	void OraLoader::addColumn(const CRString& name, OraDataType dataType,
		int size, int precision, int scale, const CRString& dateFormat /*= ""*/)
	{
		if (mColumns.count() && isColumnsAutoCreated) {
			isColumnsAutoCreated = false;
			clearColumns();
		}
		
		LoaderColumn* column = new LoaderColumn(name, dataType, size, precision, scale, dateFormat);
		mColumns.add(column);
	}
	
	int OraLoader::columnCount() {
		
		return mColumns.count();
	}
	
	void OraLoader::getColumn(int index, LoaderColumn& column) {
		
		column = mColumns.item(index);
	}
	
	void OraLoader::getColumn(const CRString name, LoaderColumn& column) {
		
		column = mColumns.item(name);
	}
	
	void OraLoader::clearColumns() {
		mColumns.clear();
		
		if (mBuffer) {
			// Anti gcc war
			delete (char*)mBuffer;
			mBuffer = 0;
		}
		
		isColumnsAutoCreated = false;
	}
	
	void OraLoader::createColumns() {
		
		checkSession();	
		OraEnvironment::checkOCI81();
		
		if (mTableName.empty())
			throw OraException(STableNameNotDefined);
		
		clearColumns();
		
		OraQuery ds(*mConnection);
		ds.setSQL(CRString("SELECT * FROM " + mTableName).c_str());
		
		ds.initFields();
		
		for (int i = 0; i < ds.fieldCount(); i++) {
			
			LoaderColumn* column = new LoaderColumn;
			
			column->setName(ds.field(i).name());
			column->setDataType(ds.field(i).dataType());
			column->mPosInTable = i;
			
			switch (column->dataType()) {
			case dtString:
			case dtChar:
				column->setSize(ds.field(i).size());
				break;
				
			case dtDouble:
				column->setSize(ds.field(i).size());
				column->setPrecision(ds.field(i).precision());
				column->setScale(ds.field(i).scale());
				break;
				
			case dtDate:
				column->setSize(ds.field(i).size());
				column->setDateFormat("dd.mm.yyyy");
				break;
				
			case dtInteger:
			case dtUInteger:
				column->setSize(ds.field(i).size());
				break;
				
			default:
				clearColumns();
				throw OraException(SCanNotCreateColumn);
			}
			
			mColumns.add(column);
		}
		
		isColumnsAutoCreated = true;
	}
	
	void OraLoader::beginLoad() {
		
		OraEnvironment::checkOCI81();
		
		if (mLoaderState == prepared)
			return;
		
		checkSession();
		
		if (mTableName.empty())
			throw OraException(STableNameNotDefined);
		
		if (mColumns.empty())
			createColumns();
		
		if (!hDirPathCtx) {
			// if OCIThreaded = true then 'ORA-24330 OCI internal error'
			sword res = OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void**)&hDirPathCtx, OCI_HTYPE_DIRPATH_CTX, 0, 0);
			assert(res == OCI_SUCCESS);
		}
		
		CRString::size_type pos = mTableName.find('.');
		CRString tName, schName;
		
		if (pos != CRString::npos) {
			schName = mTableName.substr(0, pos);
			tName = mTableName.substr(pos + 1);
		}
		else {
			schName = mConnection->username();
			tName = mTableName;
		}
		
		check(OCIAttrSet(hDirPathCtx, OCI_HTYPE_DIRPATH_CTX, (void*)schName.c_str(),
			schName.length(), OCI_ATTR_SCHEMA_NAME, OraEnvironment::getOCIError()));
		
		check(OCIAttrSet(hDirPathCtx, OCI_HTYPE_DIRPATH_CTX, (void*)tName.c_str(),
			tName.length(), OCI_ATTR_NAME, OraEnvironment::getOCIError()));
		
		ub2 columnNumber = mColumns.size();
		check(OCIAttrSet(hDirPathCtx, OCI_HTYPE_DIRPATH_CTX, &columnNumber,
			0, OCI_ATTR_NUM_COLS, OraEnvironment::getOCIError()));
		
		OCIParam* hColumnList;
		check(OCIAttrGet(hDirPathCtx, OCI_HTYPE_DIRPATH_CTX, &hColumnList, 0,
			OCI_ATTR_LIST_COLUMNS, OraEnvironment::getOCIError()));
		
		ub1 paramType;
		check(OCIAttrGet(hColumnList, OCI_DTYPE_PARAM, &paramType, 0,
			OCI_ATTR_PTYPE, OraEnvironment::getOCIError()));
		
		assert(paramType == OCI_PTYPE_LIST);
		
		for (unsigned i = 0; i < mColumns.size(); i++) {
			
			OCIParam* hColumn;
			check(OCIParamGet(hColumnList, OCI_DTYPE_PARAM, OraEnvironment::getOCIError(), (void**)&hColumn, i + 1));
			
			check(OCIAttrSet(hColumn, OCI_DTYPE_PARAM, (void*)mColumns[i].name().c_str(),
				mColumns[i].name().length(), OCI_ATTR_NAME, OraEnvironment::getOCIError()));
			
			word dataType;
			switch (mColumns[i].dataType()) {
			case dtString:
				dataType = SQLT_CHR;
				break;
			case dtChar:
				dataType = SQLT_AFC;
				break;
			case dtInteger:
				dataType = SQLT_INT;
				break;
			case dtUInteger:
				dataType = SQLT_UIN;
				break;
			case dtDouble:
				dataType = SQLT_FLT;
				break;
			case dtDate:
				dataType = SQLT_DAT;
				break;
			default:
				throw OraException(SUnknownDataType);
			}
			
			check(OCIAttrSet(hColumn, OCI_DTYPE_PARAM, &dataType,
				0, OCI_ATTR_DATA_TYPE, OraEnvironment::getOCIError()));
			
			ub4 columnSize = mColumns[i].size();
			check(OCIAttrSet(hColumn, OCI_DTYPE_PARAM, &columnSize,
				0, OCI_ATTR_DATA_SIZE, OraEnvironment::getOCIError()));
			
			ub1 columnPrecision = mColumns[i].precision();
			if (mColumns[i].precision() > 0)
				check(OCIAttrSet(hColumn, OCI_DTYPE_PARAM, &columnPrecision,
				0, OCI_ATTR_PRECISION, OraEnvironment::getOCIError()));
			
			if (mColumns[i].scale() > 0) {
				
				sb1 columnScale = mColumns[i].scale();
				check(OCIAttrSet(hColumn, OCI_DTYPE_PARAM, &columnScale,
					0, OCI_ATTR_SCALE, OraEnvironment::getOCIError()));
				
				//Set char set for float fields
				word cs = 1; // US7ASCII
				check(OCIAttrSet(hColumn, OCI_DTYPE_PARAM, &cs, 0, OCI_ATTR_CHARSET_ID, OraEnvironment::getOCIError()));
			}
			
			// date format
			if (!mColumns[i].dateFormat().empty())
				check(OCIAttrSet(hColumn, OCI_DTYPE_PARAM, (void*)mColumns[i].dateFormat().c_str(),
				mColumns[i].dateFormat().length(), OCI_ATTR_DATEFORMAT, OraEnvironment::getOCIError()));
			
			check(OCIDescriptorFree(hColumn, OCI_DTYPE_PARAM));
			
			
			mColumns[i].mOffset = mRowSize;
			mRowSize += mColumns[i].size();
		}
		
		check(OCIAttrSet(hDirPathCtx, OCI_HTYPE_DIRPATH_CTX, (void*)&LOADER_BUFFER_SIZE,
			0, OCI_ATTR_BUF_SIZE, OraEnvironment::getOCIError()));
		
		try {
			// Access violation on Oracle 9.0.1 client
			check(OCIDirPathPrepare(hDirPathCtx, mConnection->getOCISvcCtx(), OraEnvironment::getOCIError()));
		}
		catch (...) {
			check(OCIHandleFree(hDirPathCtx, OCI_HTYPE_DIRPATH_CTX));
			hDirPathCtx = 0;
			throw;
		}
		
		if (!hColumnArray)
			check(OCIHandleAlloc(hDirPathCtx, (void**)&hColumnArray, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, 0, 0));
		
		if (!hStream)
			check(OCIHandleAlloc(hDirPathCtx, (void**)&hStream, OCI_HTYPE_DIRPATH_STREAM, 0, 0));
		
		check(OCIAttrGet(hColumnArray, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &mMaxRowsInBuffer, 0,
			OCI_ATTR_NUM_ROWS, OraEnvironment::getOCIError()));
		
		size_t uSize = mMaxRowsInBuffer * mRowSize;
		mBuffer = new char[uSize];
		memset(mBuffer, 0, uSize);
		
		mLoaderState = prepared;
}

void OraLoader::loadBuffer()
{
	int convertedRows      = 0,
		loadedRows         = 0,
		convertErrorColumn = 0,
		loadErrorColumn    = 0;
	
	bool isConvertErrorOccured = false,
		isLoadErrorOccured    = false;
	
	sword res;
	
	CRString convertErrorMsg, loadErrorMsg;
	
	if (mBeginToLoadRow == mEndToLoadRow)
		return;
	
	// Convert to Direct Path stream
	res = OCIDirPathColArrayToStream(hColumnArray, hDirPathCtx, hStream,
		OraEnvironment::getOCIError(), mEndToLoadRow, mBeginToLoadRow);
	
	if (res != OCI_SUCCESS) {
		isConvertErrorOccured = true;
		OraEnvironment::getOraError(res, convertErrorMsg);
	}
	
	// Get rows processed number
	check(OCIAttrGet(hColumnArray, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &convertedRows,
		0, OCI_ATTR_ROW_COUNT, OraEnvironment::getOCIError()));
	
	// Column index with error: for error message
	check(OCIAttrGet(hColumnArray, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &convertErrorColumn,
		0, OCI_ATTR_COL_COUNT, OraEnvironment::getOCIError()));
	
	// Load stream to db
	res = OCIDirPathLoadStream(hDirPathCtx, hStream, OraEnvironment::getOCIError());
	
	if (res != OCI_SUCCESS) {
		isLoadErrorOccured = true;
		OraEnvironment::getOraError(res, loadErrorMsg);
	}
	
	// Get rows processed number
	check(OCIAttrGet(hColumnArray, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &loadedRows, 0,
		OCI_ATTR_ROW_COUNT, OraEnvironment::getOCIError()));
	
	// Column index with error: for error message
	check(OCIAttrGet(hColumnArray, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &loadErrorColumn, 0,
		OCI_ATTR_COL_COUNT, OraEnvironment::getOCIError()));
	
	// Next row will be placed with zero buffer position
	mCurrentRow = 0;
	
	// Allways reset stream after loading
	check(OCIDirPathStreamReset(hStream, OraEnvironment::getOCIError()));
	
	if (isLoadErrorOccured) {
		mBeginToLoadRow += loadedRows + 1; // Next loading start after error rows
		if (mBreakOnError) {
			mRowError = mBeginToLoadRow - 1;
			mColumnError = loadErrorColumn;
			char msgBuffer[256];
			sprintf(msgBuffer, SLoaderError, mRowError, mColumnError);
			throw OraException(CRString(msgBuffer) + loadErrorMsg);
		}
		else
			return;
	}
	
	if (isConvertErrorOccured) {
		mBeginToLoadRow += convertedRows + 1; // // Next loading start after error rows
		if (mBreakOnError) {
			mRowError = mBeginToLoadRow - 1;
			mColumnError = convertErrorColumn;
			char msgBuffer[256];
			sprintf(msgBuffer, SLoaderError, mRowError, mColumnError);
			throw OraException(CRString(msgBuffer) + convertErrorMsg);
		}
		else
			return;
	}
	
	// whole buffer was loaded successfull
	reset();
}

void OraLoader::finishLoad() {
	
	OraEnvironment::checkOCI81();
	
	if (mLoaderState == finished)
		return;
	
	bool oldState = mBreakOnError;
	
	if (mCurrentRow != 0 || mBeginToLoadRow != mEndToLoadRow) {
		if (mCurrentRow != 0)
			mEndToLoadRow = mCurrentRow;
		
		mBreakOnError = true;
		
		while (true) {
			try {
				loadBuffer();
				break;
			}
			catch (...) {
				if (mBeginToLoadRow == mEndToLoadRow)
					break;
			}
		}
	}
	
	mBreakOnError = oldState;
	
	check(OCIDirPathFinish(hDirPathCtx, OraEnvironment::getOCIError()));
	
	check(OCIHandleFree(hColumnArray, OCI_HTYPE_DIRPATH_COLUMN_ARRAY));
	check(OCIHandleFree(hStream, OCI_HTYPE_DIRPATH_STREAM));
	check(OCIHandleFree(hDirPathCtx, OCI_HTYPE_DIRPATH_CTX));
	
	hColumnArray = 0;
	hStream = 0;
	hDirPathCtx = 0;
	
	delete (char*)mBuffer;
	mBuffer = 0;
	
	mColumnError     = -1;
	mRowError        = -1;
	
	mLoaderState = finished;
}

void OraLoader::putNextRow() {
	
	mCurrentRow++;
	
	// If buffer full load it to DB
	if (mCurrentRow >= mMaxRowsInBuffer) {
		mEndToLoadRow = mCurrentRow;
		loadBuffer();
	}
	
	// If waiting to load rows exist load it
	if (mBeginToLoadRow != mEndToLoadRow)
		loadBuffer();
}

int OraLoader::columnError() const {
	return mColumnError;
}

int OraLoader::rowError() const {
	return mRowError;
}

void OraLoader::putColumnData(int columnIndex, const CRString& data) {
	
	if (mLoaderState == finished)
		throw CRException(SCallBeginLoad);
	
	if ((unsigned)columnIndex >= mColumns.size())
		throw CRException(SInvalidColumnNumber);
	
	LoaderColumn column = mColumns[columnIndex];
	
	if (column.dataType() != dtString)
		throw OraException(SCannotConvert);
	
	// If string length less then column data size
	if ((unsigned)column.size() < data.length())
		throw CRException(SLoadedDataToLarge);
	
	// Process actual writing to buffer and OCI call
	putColumnData(columnIndex, (void*)data.c_str(), data.length());
}

void OraLoader::putColumnData(const CRString& columnName, const CRString& data) {
	
	LoaderColumn& column = mColumns.item(columnName);
	putColumnData(column.mPosInTable, data);
}

void OraLoader::putColumnData(int columnIndex, const char* data) {
	
	if (mLoaderState == finished)
		throw CRException(SCallBeginLoad);
	
	if ((unsigned)columnIndex >= mColumns.size())
		throw CRException(SInvalidColumnNumber);
	
	LoaderColumn column = mColumns[columnIndex];
	
	if (column.dataType() != dtString)
		throw OraException(SCannotConvert);
	
	// If string length less then column data size
	if ((unsigned)column.size() < strlen(data))
		throw CRException(SLoadedDataToLarge);
	
	// Process actual writing to buffer and OCI call
	putColumnData(columnIndex, (void*)data, strlen(data));
}

void OraLoader::putColumnData(const CRString& columnName, const char* data) {
	
	LoaderColumn& column = mColumns.item(columnName);
	putColumnData(column.mPosInTable, data);
}

void OraLoader::putColumnData(int columnIndex, const CRDate& data) {
	
	if (mLoaderState == finished)
		throw CRException(SCallBeginLoad);
	
	if ((unsigned)columnIndex >= mColumns.size())
		throw CRException(SInvalidColumnNumber);
	
	if (mColumns[columnIndex].dataType() != dtDate)
		throw OraException(SCannotConvert);
	
	byte oraDate[7];
	CRDateToOraDate(data, oraDate);
	
	// Process actual writing to buffer and OCI call
	putColumnData(columnIndex, oraDate, 7);
}

void OraLoader::putColumnData(const CRString& columnName, const CRDate& data) {
	
	LoaderColumn& column = mColumns.item(columnName);
	putColumnData(column.mPosInTable, data);
}

void OraLoader::putColumnData(int columnIndex, int data) {
	
	if (mLoaderState == finished)
		throw CRException(SCallBeginLoad);
	
	if ((unsigned)columnIndex >= mColumns.size())
		throw CRException(SInvalidColumnNumber);
	
	double dData;
	
	switch (mColumns[columnIndex].dataType()) {
    case dtInt:
		putColumnData(columnIndex, &data, sizeof(int));
		break;
    case dtDouble:
		dData = data;
		putColumnData(columnIndex, &dData, sizeof(double));
		break;
    default:
		throw OraException(SCannotConvert);
	}
}

void OraLoader::putColumnData(const CRString& columnName, int data) {
	
	LoaderColumn& column = mColumns.item(columnName);
	putColumnData(column.mPosInTable, data);
}

void OraLoader::putColumnData(int columnIndex, double data) {
	
	if (mLoaderState == finished)
		throw CRException(SCallBeginLoad);
	
	if ((unsigned)columnIndex >= mColumns.size())
		throw CRException(SInvalidColumnNumber);
	
	int nData;
	
	switch (mColumns[columnIndex].dataType()) {
    case dtInt:
		nData = (int)data;
		putColumnData(columnIndex, &nData, sizeof(int));
		break;
    case dtDouble:
		putColumnData(columnIndex, &data, sizeof(double));
		break;
    default:
		throw OraException(SCannotConvert);
	}
}

void OraLoader::putColumnData(const CRString& columnName, double data) {
	
	LoaderColumn& column = mColumns.item(columnName);
	putColumnData(column.mPosInTable, data);
}

void OraLoader::putColumnData(int columnIndex, const CRValue& data) {
	
	if (mLoaderState == finished)
		throw CRException(SCallBeginLoad);
	
	if ((unsigned)columnIndex >= mColumns.size())
		throw CRException(SInvalidColumnNumber);
	
	switch (data.type()) {
    case CRValue::vtString:
		putColumnData(columnIndex, data.getString());
		break;
		
    case CRValue::vtInt:
		putColumnData(columnIndex, data.getInt());
		break;
		
    case CRValue::vtFloat:
		putColumnData(columnIndex, data.getFloat());
		break;
		
    case CRValue::vtDouble:
		putColumnData(columnIndex, data.getDouble());
		break;
		
    case CRValue::vtDate:
		putColumnData(columnIndex, data.getDate());
		break;
		
    case CRValue::vtNone:
		putColumnNull(columnIndex);
		break;
	}
}

void OraLoader::putColumnData(const CRString& columnName, const CRValue& data) {
	
	LoaderColumn& column = mColumns.item(columnName);
	putColumnData(column.mPosInTable, data);
}

void OraLoader::putColumnNull(int columnIndex) {
	
	if (mLoaderState == finished)
		throw CRException(SCallBeginLoad);
	
	if ((unsigned)columnIndex >= mColumns.size())
		throw CRException(SInvalidColumnNumber);
	
	// Process actual writing to buffer and OCI call
	putColumnData(columnIndex, 0, 0);
}

void OraLoader::putColumnData(int columnIndex, void* data, int dataLength) {
	
	LoaderColumn column = mColumns[columnIndex];
	
	assert(dataLength <= column.size());
	
	// Pointer to the item adress in the buffer
	// for current column index and row
	ub1* itemPtr = (ub1*)mBuffer + (mCurrentRow) * mRowSize + column.mOffset;
	
	// Determine mode for OCIDirPathColArrayEntrySet function
	int flag;
	
	if (data != 0) {
		flag = OCI_DIRPATH_COL_COMPLETE;
		
		// Copy data to loader buffer
		memcpy(itemPtr, data, dataLength);
	}
	else
		flag = OCI_DIRPATH_COL_NULL;
	
	check(OCIDirPathColArrayEntrySet(hColumnArray, OraEnvironment::getOCIError(),
		mCurrentRow, columnIndex, itemPtr, dataLength, flag));
}

void OraLoader::checkSession() {
	
	if (!OraEnvironment::isOCIInited())
		OraEnvironment::initOCI();
	
	if (!mConnection)
		throw OraException(SConnectionNotDefined);
	
	if(!mConnection->isConnected())
		mConnection->open();
}

void OraLoader::reset() {
	
	mCurrentRow = mBeginToLoadRow = mEndToLoadRow = 0;
	
	// prepare array and stream for next loading iteration
	check(OCIDirPathColArrayReset(hColumnArray, OraEnvironment::getOCIError()));
	check(OCIDirPathStreamReset(hStream, OraEnvironment::getOCIError()));  
}

} // namespace ocl

#endif // OCL_OCI81
