/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\crdb.h
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:21 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright ?1999-2002 Core Lab. All right reserved.
//  Dataset
//  Last modified:      29.01.01
/////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __DATASET_H__
#define __DATASET_H__

#include "crdef.h"

//////////////////  Data types  /////////////////

namespace ocl {
	
	typedef word CRDataType;
	
	const CRDataType dtUnknown     = 0;
	const CRDataType dtString      = 1;
	const CRDataType dtFixedString = 2;
	const CRDataType dtInt32       = 3;
	const CRDataType dtInt64       = 4;
	const CRDataType dtInt         = dtInt32;
	const CRDataType dtInteger     = dtInt;
	const CRDataType dtUInteger    = 12;
	const CRDataType dtFloat       = 5;
	const CRDataType dtDouble      = 7;
	const CRDataType dtDate        = 6;
	const CRDataType dtBlob        = 10;
	const CRDataType dtClob        = 11;
	const CRDataType dtBinary      = 12;
	const CRDataType dtBFloat      = 13;
	const CRDataType dtBDouble     = 14;
	
	class CRDataset;
	class CRLob;
	
	////////////////////  CCRValue  //////////////////
	
	class CRValue {
	public:
		
		enum ValueType {
			vtNone,
				vtInt,
				vtFloat,
				vtDouble,
				vtString,
				vtDate
		};
		
		CRValue();
		CRValue(int value);
		CRValue(float value);
		CRValue(double value);
		CRValue(const CRString& value);
		CRValue(const char* value);
		CRValue(const CRDate& value);
		CRValue(const CRValue& value);
		~CRValue();
		
		ValueType type() const;
		
		bool isNull() const;
		void setNull();
		
		int getInt() const;
		float getFloat() const;
		double getDouble() const;
		CRString getString() const;
		CRDate getDate() const;
		
		bool operator==(const CRValue& value) const;
		
		const CRValue& operator=(int value);
		const CRValue& operator=(float value);
		const CRValue& operator=(double value);
		const CRValue& operator=(const CRString& value);
		const CRValue& operator=(const char* value);
		const CRValue& operator=(const CRDate& value);
		const CRValue& operator=(const CRValue& value);
		
	private:
		void clear();  
		
		union {
			int mInt;
			float mFloat;
			double mDouble;
			CRDate* mDate;
			CRString* mString;
		};
		
		ValueType valueType;
	};
	
	///////////////////  CCRField  ///////////////////
	
	const int bufSize = 30;
	
	class CRField {
	public:
		CRField();
		CRField(const CRString& name, CRDataType dataType,
			int size, bool required = false, int fieldNo = 0);
		virtual ~CRField();
		
		CRString name() const;
		CRDataType dataType() const;
		CRDataType subDataType() const;
		bool required() const;
		int fieldNo() const;
		virtual int size();  
		int precision() const;
		int scale() const;
		
		bool isNull();
		void setNull();
		
		CRDate getDate();
		void setDate(const CRDate& value);
		
		double getDouble();
		void setDouble(double value);
		
		float getFloat();
		void setFloat(float value);
		
		int getInt();
		void setInt(int value);
		
		virtual const char* getChars();
		virtual char* getChars(char* result);
		virtual char* getChars(char* result, int length);
		virtual CRString getString();
		
		void setChars(const char* value);
		void setString(const CRString& value);
		
		unsigned char* getBinary(unsigned char* result);
		void setBinary(unsigned char* value, int length);
		
		virtual bool isLob() const;
		
		virtual CRLob& getLob();
		virtual void setLob(const CRLob& value);
		
		void assign(const CRDate& value) { setDate(value); };
		void assign(int value) { setInt(value); };
		void assign(double value) { setDouble(value); };
		void assign(const char* value) { setChars(value); };
		void assign(const CRString& value) { setString(value); };
		void assign(const CRField& value);
		
	protected:
		CRString mName;
		CRString mActualName;
		CRDataType mDataType;
		bool mRequired;
		bool mFixed;
		int mFieldNo;
		int mLength;
		int mPrecision;
		int mScale;
		int mDataSize;   // size of memory for storage data
		int mDataOffset; // offset 
		char mBuf[bufSize];
		word mSubDataType;
		
		CRDataset* mDataset;
		
		void _init();
		
		virtual bool isComplex() const;
		virtual bool hasParent() const;
		
		bool getData(void* buffer);
		void setData(void* buffer);
		
		friend class CRDataset;
		friend class CRNamedList<CRField>;
	};
	
	//////////////////  CRFields  ///////////////////
	
	typedef CRNamedList <CRField> CRFields;
	
	//////////////////  CCRDataset  //////////////////
	
	enum CRDatasetState {dsInactive,dsBrowse,dsInsert,dsEdit};
	
	class CRDataset {
	public:
		CRDataset();
		virtual ~CRDataset();
		
		void open();
		void close();
		void refresh();
		
		bool isActive() const;
		
		// Fields
		virtual void initFields();
		
		int fieldCount() const;
		CRFields& fields();
		
		CRField& field(int index) const;
		CRField& field(const CRString& name) const;
		CRField* findField(int index) const;
		CRField* findField(const CRString& name) const;
		
		// Navigation
		virtual bool isEOF() const;
		virtual bool isBOF() const;
		
		void first();
		void last();
		void next();
		void prev();
		virtual int move(long distance);
		virtual bool moveTo(long recNo);
		
		// Bookmarks
		virtual void getBookmark(void** bookmark);
		virtual bool moveToBookmark(void* bookmark);
		void freeBookmark(void* bookmark);
		
		// Misc
		long recordCount() const;
		virtual long recordNo() const;
		
		CRField& operator[](int index) const { return field(index); };
		CRField& operator[](const CRString& name) const { return field(name); };
		
	protected:
		CRFields mFields;
		
		int mDataSize;       // size of data
		int mIndicatorsSize; // size of indicators
		int mRecordSize;     // FDataSize + TIndicatorSize
		long mRecordCount;
		int mComplexFieldCount;
		
		bool mBOF;
		bool mEOF;
		CRDatasetState mState;
		
		char* activeBuffer;
		char* tempBuffer;
		
		virtual void internalOpen();
		virtual void internalClose();
		
		virtual void internalRefresh();
		
		// Items / data
		virtual void initData();
		virtual void freeData();
		virtual void prepareData();
		
		// Fields
		virtual void internalInitFields();
		
		bool getField(CRField* field, void* recBuf, void* dest);
		bool getFieldBuf(CRField* field, void* recBuf, void** dest);
		void putField(CRField* field, void* recBuf, void* source);
		
		virtual bool getNull(CRField* field, void* recBuf);
		virtual void setNull(CRField* field, void* recBuf, bool value);
		
		virtual void getDateFromBuf(void* buf, void* date);
		virtual void putDateToBuf(void* buf, void* date);
		
		bool getFieldData(CRField* field, void* dest);
		bool getFieldDataBuf(CRField* field, void** fieldBuf);
		void putFieldData(CRField* field, void* source);
		
		bool hasComplexFields();
		virtual void createComplexFields(void* recBuf, bool withBlob = true);
		virtual void freeComplexFields(void* recBuf, bool withBlob = true);
		
		// Records / buffers
		char* allocRecBuf();
		void freeRecBuf(void* recBuf);
		
		void allocBuffers();
		void freeBuffers();
		virtual bool validBuffer();
		
		void initRecord(void* recBuf);
		
		virtual void getRecord(void* recBuf);
		virtual void getNextRecord(void* recBuf);
		virtual void getPrevRecord(void* recBuf);
		virtual void putRecord(void* recBuf);
		
		virtual void insertRecord(void* recBuf);
		virtual void appendRecord(void* recBuf);
		void appendBlankRecord();
		virtual void updateRecord(void* recBuf);
		virtual void deleteRecord();
		
		void omitRecords();
		
		// Navigation
		virtual void internalFirst();
		virtual void internalLast();
		
		friend class CRField;
};


/////////////////  Blockman  ///////////////////

const int btSign = 0xDD;   // DEBUG
const int flUsed = 0xEE;
const int flFree = 0xDD;

const int defaultPieceSize = 64*1024 - 22;

struct BlockHeader {
	word itemCount;
	word usedItems;
	BlockHeader* prev;
	BlockHeader* next;
	byte test;       // DEBUG
};

enum ItemStatus { isUnmodified, isUpdated, isAppended, isDeleted };
enum UpdateRecAction { urFail, urAbort, urSkip, urRetry, urApplied, urNone };

struct ItemHeader {
	BlockHeader* block;
	ItemHeader* prev;
	ItemHeader* next;
	ItemHeader* rollback;
	ItemStatus status;
	UpdateRecAction updateResult;
	long order;
	byte flag;
};

class Blockman {
public:
	Blockman();
	~Blockman();
	
	BlockHeader* allocBlock(int itemCount);
	void freeBlock(BlockHeader* block);
	
	void addFreeBlock();
	void freeAllBlock();
	
	ItemHeader* allocItem();
	void freeItem(ItemHeader* item);
	
	void getData(ItemHeader* item, void* desc);
	void putData(ItemHeader* item, void* source);
	
	BlockHeader* firstBlock();
	ItemHeader* firstFree();
	void setFirstFree(ItemHeader* firstFree);
	void setDataSize(int dataSize);
	
protected:
	BlockHeader* mFirstBlock;
	ItemHeader* mFirstFree;
	word mDataSize;
	word mDefaultItemCount;
};

////////////////  CCRMemDataset  /////////////////

enum ReorderOption {roInsert,roDelete,roFull};

class CRMemDataset : public CRDataset {
public:
	CRMemDataset();
	virtual ~CRMemDataset();
	
	// Navigation
	virtual bool isEOF() const;
	virtual bool isBOF() const;
	virtual bool moveTo(long recNo);
	
	// Bookmarks
	virtual void getBookmark(void** bookmark);
	virtual bool moveToBookmark(void* bookmark);
	
	// Misc
	virtual long recordNo() const;
	
protected:
	Blockman mBlockman;
	ItemHeader* mFirstItem;
	ItemHeader* mLastItem;
	ItemHeader* mCurrentItem;
	long mRecordNoOffset;
	
	// Items / data
	ItemHeader* insertItem();
	ItemHeader* appendItem();
	void deleteItem(ItemHeader* item);
	
	virtual void initData();
	virtual void freeData();
	virtual void prepareData();
	
	void reorderItems(ItemHeader* item, ReorderOption reorderOption);
	
	// Records / buffers
	virtual bool validBuffer();
	
	virtual void getRecord(void* recBuf);
	virtual void getNextRecord(void* recBuf);
	virtual void getPrevRecord(void* recBuf);
	virtual void putRecord(void* recBuf);
	
	virtual void insertRecord(void* recBuf);
	virtual void appendRecord(void* recBuf);
	virtual void updateRecord(void* recBuf);
	virtual void deleteRecord();
	
	bool omitRecord(ItemHeader* item);
	
	// Navigation
	virtual void internalFirst();
	virtual void internalLast();
	
private:
	long mRefreshIteration;
};

////////////////  CSharedObject  ////////////////

class CRSharedObject {
public:
	CRSharedObject();
	
	int addRef();
	void release();
	
	int refCount() { return mRefCount; }
	
	void checkValid();
	
protected:
	virtual ~CRSharedObject() {}
	
private:
	int mRefCount;
};

///////////////////  CCRLob  ////////////////////

struct PieceHeader {
	CRLob* blob;
	long size;
	long used;  // offest 8 uses GetUsedPtr
	PieceHeader* prev;
	PieceHeader* next;
	byte test;       // DEBUG
	
	static PieceHeader* nextPiece(PieceHeader* piece);
	static void* pieceData(PieceHeader* piece);
	static void* pieceUsedPtr(PieceHeader* piece);
};

class CRLob : public CRSharedObject {
protected:
	virtual ~CRLob();
public:
	long read(long position, long count, void* dest);
	void write_blob(long position, long count, void* source);
	void clear();
	void compress();
	
	// Stream/File
	void loadFromFile(const CRString& fileName);
	void saveToFile(const CRString& fileName);

	char* getChars(char* result);
	CRString getString();
	void setChars(const char* value);
	void setString(const CRString& value);

    // TD18882 fix blob data partially saved into database
    void setChars(const char* value, long len);

	void assign(CRLob& source);
	
	long size() const;
	
	PieceHeader* firstPiece() const;
	
protected:
	CRLob();  
	
	void checkValid();   // DEBUG
	void checkCached();
	
	// Cached
	void enableCache();
	void commit();
	void cancel();  
	
	void saveToRollback();
	
	// Pieces
	PieceHeader* allocPiece(long size);
	void reallocPiece(PieceHeader** piece, long size);
	void freePiece(PieceHeader* piece);
	void appendPiece(PieceHeader* piece);
	void deletePiece(PieceHeader* piece);
	void compressPiece(PieceHeader** piece);
	
	long defPieceSize() const;
	
	long mDefPieceSize;
	byte test;   // DEBUG
	PieceHeader* mFirstPiece;
	bool mCached;
	CRLob* mRollback;
	
	friend class CRDataset;
};

void DBError(const char* msg);

#ifdef OCL_DEPRECATED
typedef CRException CCRDBException;
typedef CRValue CCRValue;
typedef CRField CCRField;
typedef CRFields CCRFields;  
typedef CRDataset CCRDataset;
typedef CRMemDataset CCRMemDataset;
typedef CRSharedObject CCRSharedObject;
typedef CRLob CCRLob;
#endif

} // namespace ocl

#endif
