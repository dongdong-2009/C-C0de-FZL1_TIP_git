/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\crdb.cpp
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:18 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/


//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2002 Core Lab. All right reserved.
//  Dataset
//  Created:            30.06.98
//  Last modified:      30.03.01
//////////////////////////////////////////////////

#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ocl.conf"
#include "crdb.h"

#ifdef WIN32
#define ECVT _ecvt
#else
#define ECVT ecvt
#endif      

namespace ocl {

const char SCannotConvert[]         = "Cannot convert to %s";
const char SNoEditMode[]            = "No edit mode";
const char SObjectNotExist[]        = "Object not exist: %s";
const char SInvalidSharedObject[]   = "Invalid SharedObject";
const char SCannotConvertToString[] = "Can not convert to STRING";
const char SCannotConvertToDate[]   = "Can not convert to DATE";
const char SCannotConvertToInt[]    = "Can not convert to INT";
const char SCannotConvertToFloat[]  = "Can not convert to FLOAT";
const char SCannotConvertToDouble[] = "Can not convert to DOUBLE";
const char SCanotOpenInputFile[]    = "Can not open input file";
const char SCanotOpenOutputFile[]   = "Can not open output file";
const char SBlobNotInCashedMode[]   = "Blob must be in cahed mode";
const char SCachAlreadyEnabled[]    = "Cached have already enabled";
const char SBufferTooLarge[]        = "Buffer too large";

long refreshIteration = 1;

////////////////////////  CRValue  ///////////////////////

CRValue::CRValue() {
  valueType = vtNone;
}

CRValue::CRValue(int value) {

  valueType = vtNone;  
  *this = value;
}

CRValue::CRValue(float value) {

  valueType = vtNone;  
  *this = value;
}

CRValue::CRValue(double value) {

  valueType = vtNone;  
  *this = value;
}

CRValue::CRValue(const CRString& value) {

  valueType = vtNone;  
  *this = value;
}

CRValue::CRValue(const char* value) {

  valueType = vtNone;  
  *this = value;
}

CRValue::CRValue(const CRDate& value) {

  valueType = vtNone;
  *this = value;
}

CRValue::CRValue(const CRValue& value) {

  valueType = vtNone;  
  *this = value;
}

CRValue::~CRValue() {

  clear();
}

CRValue::ValueType CRValue::type() const {

  return valueType;
}

bool CRValue::isNull() const {

  bool res;

  switch (valueType) {

    case vtInt:
      res = mInt == 0;
      break;

    case vtFloat:
      res = mFloat == 0;
      break;

    case vtDouble:
      res = mDouble == 0;
      break;

    case vtString:
      res = *mString == "";
      break;

    case vtDate:
      res = *mDate == CRDate();
      break;

    case vtNone:
      res = true;
      break;
  }

  return res;
}

void CRValue::setNull() {

  switch (valueType) {

    case vtInt:
      mInt = 0;
      break;

    case vtFloat:
      mFloat = 0;
      break;

    case vtDouble:
      mDouble = 0;
      break;

    case vtString:
      *mString = "";
      break;

    case vtDate:
      *mDate = CRDate();
      break;
  }
}

void CRValue::clear() {

  switch (valueType) {

    case vtString:
      delete mString;
      break;

    case vtDate:
      delete mDate;
      break;
      
  }

  valueType = vtNone;
}

int CRValue::getInt() const {

  switch (valueType) {

    case vtInt:
      return mInt;

    case vtDouble:
    case vtFloat:
      return (int)mDouble;

    default:
      throw CRException(SCannotConvertToInt);
  }
}

float CRValue::getFloat() const {

  switch (valueType) {
    case vtInt:
      return (float)mInt;
    case vtDouble:
    case vtFloat:
      return (float)mDouble;
    default:
      throw CRException(SCannotConvertToFloat);
  }
}

double CRValue::getDouble() const {

  switch (valueType) {

    case vtInt:
      return mInt;
    case vtDouble:
    case vtFloat:
      return mDouble;
    default:
      throw CRException(SCannotConvertToDouble);

  }
}

CRString CRValue::getString() const {

  char buf[30];

  switch (valueType) {

    case vtString:
      return *mString;

    case vtDate:
      return mDate->format();

    case vtInt:
      sprintf(buf, "%i", mInt);
      return CRString(buf);

    case vtFloat:
      sprintf(buf, "%f", mFloat);
      return CRString(buf);

    case vtDouble:
      sprintf(buf, "%f", mDouble);
      return CRString(buf);

    default:
      throw CRException(SCannotConvertToString);
  }
}

CRDate CRValue::getDate() const {

  switch (valueType) {

    case vtDate:
      return *mDate;

    default:
      throw CRException(SCannotConvertToDate);
      
  }
}

bool CRValue::operator==(const CRValue& value) const {
  bool res;

  if (valueType != value.valueType)
    return false;

  switch (valueType) {

    case vtInt:
      res = mInt == value.mInt;
      break;

    case vtFloat:
      res = mFloat == value.mFloat;
      break;

    case vtDouble:
      res = mDouble == value.mDouble;
      break;

    case vtString:
      res = *mString == *value.mString;
      break;

    case vtDate:
      res = *mDate == *value.mDate;
      break;
  }

  return res;  
}

const CRValue& CRValue::operator=(int value) {

  clear();
  
  if (valueType != vtInt)
    valueType = vtInt;

  mInt = value;

  return *this;
}

const CRValue& CRValue::operator=(float value) {

  clear();
  
  if (valueType != vtFloat) 
    valueType = vtFloat;

  mFloat = value;

  return *this;
}

const CRValue& CRValue::operator=(double value) {

  clear();
  
  if (valueType != vtDouble)
    valueType = vtDouble;

  mDouble = value;

  return *this;
}

const CRValue& CRValue::operator=(const CRString& value) {

  if (valueType != vtString) {
    clear();
    valueType = vtString;
    mString = new CRString();
  }

  *mString = value;

  return *this;  
}

const CRValue& CRValue::operator=(const char* value) {
  
  if (valueType != vtString) {
    clear();
    valueType = vtString;
    mString = new CRString();
  }

  *mString = value;

  return *this;  
}

const CRValue& CRValue::operator=(const CRDate& value) {

  if (valueType != vtDate) {
    clear();
    valueType = vtDate;
    mDate = new CRDate();
  }

  *mDate = value;
  
  return *this;
}

const CRValue& CRValue::operator=(const CRValue& value) {
  
  if (this == &value)
    return *this;

  switch (value.valueType) {

    case vtInt:
      *this = value.mInt;
      break;

    case vtFloat:
      *this = value.mFloat;
      break;

    case vtDouble:
      *this = value.mDouble;
      break;

    case vtString:
      *this = *value.mString;
      break;

    case vtDate:
      *this = *value.mDate;
      break;

    case vtNone:
      clear();
      break;
  }

  return *this; 
}

//////////////////  CRField  /////////////////////

CRField::CRField() {
  _init();
}

CRField::CRField(const CRString& name, CRDataType dataType, int size, bool required, int fieldNo) {

  _init();

  mName = name;
  mDataType = dataType;
  mDataSize = size;
  mRequired = required;
  mFieldNo = fieldNo;
}

void CRField::_init() {
  mDataType    = dtUnknown;
  mRequired    = false;
  mFixed       = false;
  mFieldNo     = 0;
  mLength      = 0;
  mPrecision   = 0;
  mScale       = 0;
  mDataSize    = 0;  // size of memory for storage data
  mDataOffset  = 0;  // offset 
  mBuf[0]      = 0;
  mSubDataType = 0;
  mDataset     = 0;
}

CRField::~CRField() {

}

CRString CRField::name() const {

  return mName;
}

CRDataType CRField::dataType() const {

  return mDataType;
}

CRDataType CRField::subDataType() const {

  return mSubDataType;
}

int CRField::size() {

  switch (mDataType) {
    case dtString:
    case dtFixedString:
      return mDataSize - 1; // exclude terminator
    case dtClob: case dtBlob:
      return 0;
    default:
      return mDataSize;
  }
}

bool CRField::required() const {

  return mRequired;
}

int CRField::fieldNo() const {

  return mFieldNo;
}

int CRField::precision() const {

  return mPrecision;
}

int CRField::scale() const {

  return mScale;
}

bool CRField::isComplex() const {

  return (mDataType == dtBlob || mDataType == dtClob);
}

bool CRField::hasParent() const {

  return false;
}

bool CRField::isNull() {

  return !mDataset->getFieldData(this, 0);
}

void CRField::setNull() {

  mDataset->putFieldData(this, 0);
}

bool CRField::getData(void* buffer) {

  return mDataset->getFieldData(this, buffer);
}

void CRField::setData(void* buffer) {

  mDataset->putFieldData(this, buffer);
}

CRDate CRField::getDate() {
  CRDate result;

  if (isNull())
    return CRDate();

  switch (mDataType) {
    case dtDate:
      getData(&result);
      return result;
    default:
      throwError(SCannotConvert, "Date");
      return CRDate();
  }
}

void CRField::setDate(const CRDate& value) {

  switch (mDataType) {
    case dtDate:
      setData((void*)&value);
      break;
    default:
      throwError(SCannotConvert, "Date");
  }
}

double CRField::getDouble() {
  double result;

  if (isNull())
    return 0;

  switch (mDataType) {
    case dtFloat:
    case dtDouble:
      getData(&result);
      return result;
    case dtInt:
      return (double)getInt();
    default:
      throwError(SCannotConvert, "double");
      return 0;
  }
}

void CRField::setDouble(double value) {

  switch (mDataType) {
    case dtDouble:
      setData(&value);
      break;
    default:
      throwError(SCannotConvert, "double");
  }
}

float CRField::getFloat() {
  
  float result = 0;

  switch (mDataType) {
    case dtFloat:
      getData(&result);
      break;
    default:
      throwError(SCannotConvert, "float");
  }

  return result;
}

void CRField::setFloat(float value) {

  switch (mDataType) {
    case dtFloat:
      setData(&value);
      break;
    case dtDouble:
      setDouble(value);
      break;
    default:
      throwError(SCannotConvert, "float");
  }
}

int CRField::getInt() {
  int result;

  if (isNull())
    return 0;

  switch (mDataType) {
    case dtInteger:
      getData(&result);
      return result;
    case dtFloat:
      return (int)getFloat();
    case dtDouble:
      return (int)getDouble();
    default:
      throwError(SCannotConvert, "int");
      return 0;
  }
}

void CRField::setInt(int value) {

  switch (mDataType) {
    case dtInteger:    
      setData(&value);
      break;
    default:
      throwError(SCannotConvert, "int");
  }
}

char* CRField::getChars(char* result) {
   
  if (!result)
    return 0;

  if (isNull()) {
    result[0] = 0;
    return result;
  }

  if (result == mBuf)
    return getChars(mBuf, sizeof(mBuf));

  switch (mDataType) {
    case dtString:
    case dtFixedString:
      strcpy(result, getChars());
      break;
    case dtDate:
      strcpy(result, getDate().format().c_str());
      break;
    case dtDouble:
      char formatStr[8];
      
      if (mScale != 0)
        sprintf(formatStr, "%%.%if", mScale);
      else {
        if (mPrecision == 0)
          sprintf(formatStr, "%%.%ig", 38);
        else
          sprintf(formatStr, "%%.%if", 0);
      }

      sprintf(result, formatStr, getDouble());
      break;
    case dtInteger:
      sprintf(result, "%i", getInt());
      break;
    case dtBlob: case dtClob:
      getLob().getChars(result);
      break;
    default:
      throwError(SCannotConvert);
  }
  return result;
}

const char* CRField::getChars() {
  char* result;

  if (isNull())
    return 0;

  switch (mDataType) {
    case dtString:
    case dtFixedString:
      mDataset->getFieldBuf(this, mDataset->activeBuffer, (void**)&result);
      return result;

    case dtBlob: 
    case dtClob:
      return "";
    default:
      // copy to internal bufer
      return getChars(mBuf, sizeof(mBuf));
  }
}

//  copyies data into client buffer
char* CRField::getChars(char* result, int length)
{
  if (!result)
    return 0;

  if (isNull()) {
    result[0] = 0;
    return result;
  }

  char *pos;
  int maxLen = length - 1;
  switch (mDataType) {
    case dtString:
    case dtFixedString:
      mDataset->getFieldBuf(this, mDataset->activeBuffer, (void**)&pos);
      strncpy(result, pos, length);
      break;

    case dtDate:
      strncpy(result, getDate().format().c_str(), length);
      break;

    case dtFloat:
    case dtDouble:
      char *digit;
      int dec, sign;

      //  check buffer size
      assert( length > 1 );

      if (mScale != 0)
        digit = ECVT( getDouble(), mPrecision, &dec, &sign );
      else {
        if (mPrecision == 0)
          digit = ECVT( getDouble(), 38, &dec, &sign );
        else
          digit = ECVT( getDouble(), 0, &dec, &sign );
      }

      pos = result;
      if ('\0' != *digit){
        if (0 != sign)
          *pos++ = '-';

        if (dec <= 0 && pos - result < maxLen){
          *pos++ = '0';
          if (pos - result < maxLen)
            *pos++ = '.';
          if (abs(dec) > maxLen - (pos - result)){
            if (pos - result < maxLen)
              *pos++ = '0';
            *pos = '\0';
            break;
          }
        }

        while((dec < 0 || digit!= '\0')&& pos - result < maxLen){
          if (dec < 0){
            *pos++ = '0';
            dec++;
          }
          else if (dec > 0){
            dec--;
            *pos++ = *digit++;
            if ( 0 == dec && digit!= '\0' && pos - result < maxLen)
              *pos++ = '.';
          }
          else
            *pos++ = *digit++;
        }
      }
/*
        if (pos - result + dec < length){
          memcpy(pos, digit, dec);
          pos += dec;
          if (pos - result + 1 < length){
            *pos++ = '.';
            sign  = min(ln - dec - 1, length - (pos - result) - 1); 
            if (sign > 0){ 
              memcpy(pos, digit + dec, sign);
              pos += sign;
            }
          }
        }
        else
          memcpy(result, digit, length - (pos - result) - 1);

*/
      *pos = '\0';
      break;

    case dtInteger:
      char buf[70];
#ifdef WIN32
      itoa(getInt(), buf, 10);
#else
      sprintf(buf, "%d", getInt());
#endif      
      strncpy(result, buf, length);
      break;

    case dtBlob: 
    case dtClob:
      getLob().getChars(result);
      break;

    default:
      throwError(SCannotConvert);
  }

  result[maxLen] = '\0';
  return result;
}

CRString CRField::getString() {

  if (isNull())
    return "";

  switch (mDataType) {
    case dtString:
    case dtFixedString:
      return CRString(getChars());

    case dtBlob: case dtClob:
      return getLob().getString();

    default:
      getChars(mBuf);
      return CRString(mBuf);
  }
}

void CRField::setString(const CRString& value) {
  setChars(value.c_str());
}

void CRField::setChars(const char* value) {

  switch (mDataType) {
    case dtString:
    case dtFixedString:
      setData(&value);
      break;
    default:
      throwError(SCannotConvert, "String");
  }
}

unsigned char* CRField::getBinary(unsigned char* result) {

  if (mDataType != dtBinary)
    throwError(SCannotConvert, "RAW");

  getData(result);

  return result;
}

void CRField::setBinary(unsigned char* value, int length) {

  if (mDataType != dtBinary)
    throwError(SCannotConvert, "RAW");

  if (length > mDataSize)
    throwError(SBufferTooLarge);

  setData(&value);
}

bool CRField::isLob() const {
  return mDataType == dtClob || mDataType == dtBlob;
}

CRLob& CRField::getLob() {
  CRLob* result;

  if (!mDataset->validBuffer())
    throwError(SObjectNotExist);

  switch (mDataType) {
    case dtBlob: case dtClob:
      getData(&result);
      break;
    default:
      throwError(SCannotConvert, "Blob");
  }

  return *result;
}

void CRField::setLob(const CRLob& value) {

  switch (mDataType) {
    case dtBlob: case dtClob:
      setData((void*)&value);
    default:
      throwError(SCannotConvert, "Blob");
  }
}

//////////////////  CRDataset  //////////////////

struct TCRBookmark {
  long refreshIteration;
  ItemHeader* item;
  long order;
};

CRDataset::CRDataset() {

  mDataSize = 0;
  mRecordSize = 0;
  mIndicatorsSize = 0;
  mState = dsInactive;
  activeBuffer = 0;
  tempBuffer = 0;

  initData();
}

CRDataset::~CRDataset() {
  close();
}

// Items / data

void CRDataset::initData() {

  mBOF = true;
  mEOF = true;
}

void CRDataset::freeData() {

  initData();
}

void CRDataset::prepareData() {
}

// Open / close

void CRDataset::internalOpen() {
}

void CRDataset::open() {

  if (isActive())
    return;

  initData();

  try {
    internalOpen();
    prepareData();
    allocBuffers();
    getNextRecord(activeBuffer);
  }
  catch (...) {
    freeBuffers();
    freeData();
    internalClose();
    
    throw;
  };
  
  mState = dsBrowse;
}

void CRDataset::internalClose() {
}

void CRDataset::close() {

  if (isActive()) {
    try {
      freeBuffers();
      internalClose();
      
    //finally
      freeData();
      mState = dsInactive;
    }
    catch (...) {
      internalClose();
      freeData();
      mState = dsInactive;
      throw;
    }
  }
}

void CRDataset::internalRefresh() {
}

void CRDataset::refresh() {

  internalRefresh();
  getNextRecord(activeBuffer);
} 

bool CRDataset::isActive() const {

  return mState != dsInactive;
}

// Fields

void CRDataset::internalInitFields() {

  mIndicatorsSize = mFields.count()*sizeof(short);
}

void CRDataset::initFields() {

  mFields.clear();

  internalInitFields();

  mDataSize = 0;

  for (int i = 0; i < mFields.count(); i++ ) {
    mFields[i].mDataOffset = mDataSize;
    mFields[i].mDataset = this;
    mDataSize += mFields[i].mDataSize;
  }
  mDataSize += mDataSize % 2;     // align to word for sun

  mRecordSize = mDataSize + mIndicatorsSize;

  mRecordSize += mRecordSize % 4; // align to dword for sun
}

bool CRDataset::getField(CRField* field, void* recBuf, void* dest) {
  bool isBlank;

  assert(field);

  isBlank = getNull(field, recBuf);

  if (dest == 0 ||  isBlank && !field->isComplex())//
    return !isBlank;

  switch (field->mDataType) {
    case dtDate:
      getDateFromBuf((char*)recBuf + field->mDataOffset, dest);
      break;
  default:
    memcpy(dest, ((char*)recBuf + field->mDataOffset), field->mDataSize);
  }

  return !isBlank;
}

bool CRDataset::getFieldBuf(CRField* field, void* recBuf, void** dest) {
  bool isBlank;

  assert(field);

  isBlank = getNull(field, recBuf);

  if (dest == 0 || isBlank && !field->isComplex())
    *dest = 0;
  else
    *dest = (char*)recBuf + field->mDataOffset;

  return !isBlank;
}

void CRDataset::putField(CRField* field, void* recBuf, void* source) {

  if (!source) {
    setNull(field, recBuf, true);
    return;
  }

  switch (field->mDataType) {
    case dtDate:
      putDateToBuf((char*)recBuf + field->mDataOffset, source);
      break;
  default:
    memcpy((char*)recBuf + field->mDataOffset, source, field->mDataSize);
  }
}

void CRDataset::getDateFromBuf(void* buf, void* date) {
}

void CRDataset::putDateToBuf(void* buf, void* date) {
}

bool CRDataset::getNull(CRField* field, void* recBuf) {

#ifdef OCL_TRIAL
  #include "_trial.cpp"
#endif

  return *(short*)((char*)recBuf + mDataSize + (field->mFieldNo - 1)*sizeof(short)) == -1;
}

void CRDataset::setNull(CRField* field, void* recBuf, bool value) {
  short flag;

  if (value)
    flag = -1;
  else
    flag = 0;

  *(short*)((char*)recBuf + mDataSize + (field->mFieldNo - 1)*sizeof(short)) = flag;
}

bool CRDataset::getFieldData(CRField* field, void* dest) {

  if (validBuffer())
    return getField(field, activeBuffer, dest);
  else
    return false;
}

bool CRDataset::getFieldDataBuf(CRField* field, void** fieldBuf) {

  if (validBuffer())
    return getFieldBuf(field, activeBuffer, fieldBuf);
  else {
    *fieldBuf = 0;
    return false;
  }
}

void CRDataset::putFieldData(CRField* field, void* source) {

  if (mState == dsInsert || mState == dsEdit)
    putField(field, activeBuffer, source);
  else
    throwError(SNoEditMode);
}

int CRDataset::fieldCount() const {

  return mFields.count();
}

CRFields& CRDataset::fields() {

  return mFields;
}

CRField& CRDataset::field(int index) const {

  return mFields.item(index);
}

CRField& CRDataset::field(const CRString& name) const {

  return mFields.item(name.c_str());
}

CRField* CRDataset::findField(int index) const {

  return mFields.findItem(index);
}

CRField* CRDataset::findField(const CRString& name) const {

  return mFields.findItem(name.c_str());
}

bool CRDataset::hasComplexFields() {

  bool res = false;

  for (int i= 0; i < mFields.count(); i++)
    if (mFields[i].isComplex())
      res = true;
    
  return res;
}

void CRDataset::createComplexFields(void* recBuf, bool withBlob) {
  CRLob* blob;

  for (int i = 0; i < fieldCount(); i++)
    if ((mFields[i].dataType() == dtBlob || mFields[i].dataType() == dtClob) &&
      withBlob)
    {
      blob = new CRLob();
      *(CRLob**)((char*)recBuf + mFields[i].mDataOffset) = blob;
    }
}

void CRDataset::freeComplexFields(void* recBuf, bool withBlob) {
  CRLob* blob;

  for (int i = 0; i < fieldCount(); i++)
    if ((mFields[i].dataType() == dtBlob || mFields[i].dataType() == dtClob) &&
      withBlob)
    {
      blob = *(CRLob**)((char*)recBuf + mFields[i].mDataOffset);
      delete blob;
    }
}

// Records / buffers

char* CRDataset::allocRecBuf() {

  return (char*)malloc(mRecordSize);
}

void CRDataset::freeRecBuf(void* recBuf) {

  free(recBuf);
}

void CRDataset::allocBuffers() {

  activeBuffer = allocRecBuf();
  tempBuffer = allocRecBuf();
}

void CRDataset::freeBuffers() {

  if (activeBuffer) {
    freeRecBuf(activeBuffer);
    activeBuffer = 0;
  }
  if (tempBuffer) {
    freeRecBuf(tempBuffer);
    tempBuffer = 0;
  }
}

bool CRDataset::validBuffer() {

  return false;
}

void CRDataset::initRecord(void* recBuf) {

  for (int i = 0; i < fieldCount(); i++)
    setNull(&mFields[i], recBuf, true);
}

void CRDataset::getRecord(void* recBuf) {
}


void CRDataset::getNextRecord(void* recBuf) {
}


void CRDataset::getPrevRecord(void* recBuf) {
}


void CRDataset::putRecord(void* recBuf) {
}


void CRDataset::appendRecord(void* recBuf) {
}

void CRDataset::appendBlankRecord() {
  void* recBuf;

  recBuf = allocRecBuf();
  try {
    initRecord(recBuf);
    appendRecord(recBuf);
  // finally
    freeRecBuf(recBuf);
  }
  catch (...) {
    freeRecBuf(recBuf);
    throw;
  }
}

void CRDataset::insertRecord(void* recBuf) {
}

void CRDataset::updateRecord(void* recBuf) {
}

void CRDataset::deleteRecord() {
}

void CRDataset::omitRecords() {

}

// Navigation

bool CRDataset::isEOF() const {

  return mEOF;
}

bool CRDataset::isBOF() const {

  return mBOF;
}

void CRDataset::internalFirst() {
}

void CRDataset::first() {

  internalFirst();

  getRecord(activeBuffer);
}

void CRDataset::internalLast() {
}

void CRDataset::last() {

  internalLast();

  getRecord(activeBuffer);
}

void CRDataset::next() {

  getNextRecord(activeBuffer);
}

void CRDataset::prev() {

  getPrevRecord(activeBuffer);
}

int CRDataset::move(long distance) {

  int i = 1;

  if (distance > 0) {
    for (i; i <= distance; i++)
      getNextRecord(activeBuffer);
  }
  else {
    for (i; i <= abs(distance); i++)
      getPrevRecord(activeBuffer);
  }

  return i;
}

bool CRDataset::moveTo(long recNo) {

  return false;
}

// Bookmarks

void CRDataset::getBookmark(void** bookmark) {

  if (bookmark) {
    if (!*bookmark) {
      *bookmark = malloc(sizeof(TCRBookmark));
    }
    ((TCRBookmark*)*bookmark)->order = recordNo();
  }
}

bool CRDataset::moveToBookmark(void* bookmark) {

  if (bookmark &&
    ((TCRBookmark*)bookmark)->order > 0 &&
    ((TCRBookmark*)bookmark)->order <= recordCount())
  {
    return moveTo(((TCRBookmark*)bookmark)->order);
  }
  else
    return false;
}

void CRDataset::freeBookmark(void* bookmark) {

  if (bookmark)
    free(bookmark);
}

// Misc

long CRDataset::recordCount() const {

  return mRecordCount;
}

long CRDataset::recordNo() const {

  return -1;
}

////////////////  CRMemDataset  /////////////////

CRMemDataset::CRMemDataset() : CRDataset() {
    
  initData();
}

CRMemDataset::~CRMemDataset() {
}

// Items / data

ItemHeader* CRMemDataset::insertItem() {
  ItemHeader* item;

  if (isEOF())
    return appendItem();

  if (isBOF())
    mCurrentItem = mFirstItem;

  item = mBlockman.allocItem();

  item->next = mCurrentItem;

  if (mCurrentItem) {
    item->prev = mCurrentItem->prev;
    if (mCurrentItem->prev)
      mCurrentItem->prev->next = item;
    mCurrentItem->prev = item;
  }
  else
    item->prev = 0;

  if (mFirstItem == mCurrentItem)
    mFirstItem = item;

  if (mLastItem)
    mLastItem = item;

  return item;
}

ItemHeader* CRMemDataset::appendItem() {
  ItemHeader* item;

  item = mBlockman.allocItem();

  if (mFirstItem) {
    mFirstItem = item;
    item->order = 1;
  }
  else {
    mLastItem->next = item;
    item->order = mLastItem->order + 1;
  }

  item->prev = mLastItem;
  item->next = 0;
  mLastItem = item;

  return item;
}

void CRMemDataset::deleteItem(ItemHeader* item) {

  if (item) {
    if (item == mFirstItem) {
      if (item == mLastItem) {
        mCurrentItem = 0;
        mFirstItem = 0;
        mLastItem = 0;
        mBOF = true;
        mEOF = true;
      }
      else {
        mFirstItem = item->next;
        mFirstItem->prev = 0;
        if (item == mCurrentItem)
          mCurrentItem = mFirstItem;
      }
    }
    else
      if (item == mLastItem) {
        mLastItem = item->prev;
        mLastItem->next = 0;
        if (item == mCurrentItem)
          mCurrentItem = mLastItem;
      }
      else{
        if (item == mCurrentItem)
          mCurrentItem = item->next;

        item->prev->next = item->next;
        item->next->prev = item->prev;
      }

    if (hasComplexFields())
      freeComplexFields((char*)item + sizeof(ItemHeader));

    mBlockman.freeItem(item);
  }
}

void CRMemDataset::initData() {

  mFirstItem = 0;
  mLastItem = 0;
  mCurrentItem = 0;

  mBOF = true;
  mEOF = true;
  mRecordCount = 0;
  mRecordNoOffset = 0;

  mRefreshIteration = refreshIteration++;
}

void CRMemDataset::freeData() {
  ItemHeader* item;

  if (hasComplexFields()) {
  // Free complex fields
    item = mFirstItem;
    while (item) {
      freeComplexFields((char*)item + sizeof(ItemHeader));
      item = item->next;
    }
  }

  mBlockman.freeAllBlock();

  initData();
}

void CRMemDataset::prepareData() {

  mBlockman.setDataSize(mRecordSize);
}

void CRMemDataset::reorderItems(ItemHeader* item, ReorderOption reorderOption) {

}

// Records / buffers

bool CRMemDataset::validBuffer() {

  return mCurrentItem != 0;
}

void CRMemDataset::getRecord(void* recBuf) {

  if (mCurrentItem)
    mBlockman.getData(mCurrentItem, recBuf);
}

void CRMemDataset::getPrevRecord(void* recBuf) {

  if (!isBOF()) {
    if (mCurrentItem) {
      mCurrentItem = mCurrentItem->prev;
      mEOF = false;
    }

    if (!mCurrentItem) {
      mCurrentItem = mFirstItem;
      mBOF = true;
    }

    //OmitRecords();

    getRecord(recBuf);
  }
}

void CRMemDataset::getNextRecord(void* recBuf) {

  if (!isEOF()) {
    if (isBOF()) {
      mBOF = false;
      mCurrentItem = mFirstItem;
    }
    else
      if (mCurrentItem->next)
        mCurrentItem = mCurrentItem->next;
      else
        mEOF = true;

    omitRecords();
    if (!mCurrentItem)
      mEOF = true;
    else
      getRecord(recBuf);
  }
}

void CRMemDataset::putRecord(void* recBuf) {

  mBlockman.putData(mCurrentItem, recBuf);
}

void CRMemDataset::insertRecord(void* recBuf) {

  mCurrentItem = insertItem();
  putRecord(recBuf);
  mRecordCount++;
}

void CRMemDataset::appendRecord(void* recBuf) {

  last();
  insertRecord(recBuf);
}

void CRMemDataset::updateRecord(void* recBuf) {

  putRecord(recBuf);
}

void CRMemDataset::deleteRecord() {

  deleteItem(mCurrentItem);
}

bool CRMemDataset::omitRecord(ItemHeader* item) {

  return false;
}

// Navigation

bool CRMemDataset::isBOF() const {

#ifdef MOCL
  bool res = mBOF;
  __asm { mov EAX, 0 } // Avoid bug with bool marshaling
  return res;
#else
  return mBOF;
#endif
}

bool CRMemDataset::isEOF() const {

#ifdef MOCL
  bool res = mEOF;
  __asm { mov EAX, 0 } // Avoid bug with bool marshaling
  return res;
#else
  return mEOF;
#endif
}

void CRMemDataset::internalFirst() {

  mCurrentItem = mFirstItem;
  mBOF = true;
  if (mLastItem)
    mEOF = false;
}

void CRMemDataset::internalLast() {

  mCurrentItem = mLastItem;
  mEOF = true;
  if (mFirstItem)
    mBOF = false;
}

bool CRMemDataset::moveTo(long recNo) {
  ItemHeader* item;
  ItemHeader* currItem;
  int forwardDir;

  if (mFirstItem && recNo > 0) {
    if (mCurrentItem)
      currItem = mCurrentItem;
    else
      currItem = mFirstItem;

    if (recNo < abs(mLastItem->order + mRecordNoOffset - recNo) &&
      recNo < abs(currItem->order + mRecordNoOffset - recNo))
    {
    // from first
      item = mFirstItem;
      forwardDir = true;
    }
    else
      if (abs(mLastItem->order + mRecordNoOffset - recNo) <
        abs(currItem->order + mRecordNoOffset - recNo))
      {
      // from
        item = mLastItem;
        forwardDir = mLastItem->order + mRecordNoOffset < recNo;
      }
      else {
      // from current
        item = currItem;
        forwardDir = currItem->order + mRecordNoOffset < recNo;
      }

    while (item && item->order + mRecordNoOffset != recNo)
      if (forwardDir)
        item = item->next;
      else
        item = item->prev;

    if (item) {
      mCurrentItem = item;
      getRecord(activeBuffer);
      return true;
    }
  }
  return false;
}

// Bookmarks

void CRMemDataset::getBookmark(void** bookmark) {

  if (bookmark) {
    if (!*bookmark) {
      *bookmark = malloc(sizeof(TCRBookmark));
    }
    ((TCRBookmark*)*bookmark)->refreshIteration = mRefreshIteration;
    ((TCRBookmark*)*bookmark)->item = mCurrentItem;

    if (mCurrentItem)
      ((TCRBookmark*)*bookmark)->order = mCurrentItem->order + mRecordNoOffset;
    else
      ((TCRBookmark*)*bookmark)->order = -1;
  }
}

bool CRMemDataset::moveToBookmark(void* bookmark) {
  ItemHeader* oldCurrentItem;

  if (bookmark) {
    if (((TCRBookmark*)bookmark)->refreshIteration == mRefreshIteration &&
      ((TCRBookmark*)bookmark)->item)
    {
      oldCurrentItem = mCurrentItem;
      try {  // for freed item
        mCurrentItem = ((TCRBookmark*)bookmark)->item;
        if (mCurrentItem->flag == flUsed) {
          mBOF = false; mEOF = false;
          getRecord(activeBuffer);          
          return true;
        }
        else
          mCurrentItem = oldCurrentItem;
      }
      catch(...) {
        mCurrentItem = oldCurrentItem;
      }
    }

  // set by order
    return CRDataset::moveToBookmark(bookmark);
  }
  return false;
}

// Misc

long CRMemDataset::recordNo() const {

  if (mCurrentItem)
    return mCurrentItem->order + mRecordNoOffset;
  else
    return 0;
}

/////////////////  Blockman  ///////////////////

Blockman::Blockman() {

  mDataSize = 0;
  mFirstFree = 0;
  mFirstBlock = 0;
  mDefaultItemCount = 1;  //10; // WAR
}

Blockman::~Blockman() {

  freeAllBlock();
}

BlockHeader* Blockman::allocBlock(int itemCount) {
  BlockHeader* block;
  int blockSize;

  blockSize = sizeof(BlockHeader) + itemCount*(sizeof(ItemHeader) + mDataSize);

  block = (BlockHeader*)malloc(blockSize);

  block->itemCount = (word)itemCount;
  block->usedItems = (word)itemCount;

  block->next = mFirstBlock;
  block->prev = 0;

  block->test = btSign;         // DEBUG

  if(mFirstBlock)
    mFirstBlock->prev = block;

  mFirstBlock = block;

  return block;
}

void Blockman::freeBlock(BlockHeader* block) {

  if (block == mFirstBlock) {
    mFirstBlock = block->next;
    if (mFirstBlock)
      mFirstBlock->prev = 0;
  }
  else
    block->prev->next = block->next;

  assert(block->test == btSign);

  free(block);
}

void Blockman::freeAllBlock() {

  while (mFirstBlock)
    freeBlock(mFirstBlock);

  mFirstFree = 0;
}

void Blockman::addFreeBlock() {
  BlockHeader* block;
  ItemHeader* item;
  word i;

  block = allocBlock(mDefaultItemCount);

  item = (ItemHeader*)((char*)block + sizeof(BlockHeader));

  for (i = 1; i <= mDefaultItemCount; i++) {
    item->prev = 0;
    item->next = mFirstFree;
    item->block = block;
    item->flag = flFree;

    if (mFirstFree)
      mFirstFree->prev = item;
    mFirstFree = item;

    item = (ItemHeader*)((char*)item + sizeof(ItemHeader) + mDataSize);
  }
  block->usedItems = 0;
}

ItemHeader* Blockman::allocItem() {
  ItemHeader* item;

  if (!mFirstFree)
    addFreeBlock();

  item = mFirstFree;

  assert(item->flag == flFree);
  item->flag = flUsed;

  mFirstFree = mFirstFree->next;
  if (mFirstFree)
    mFirstFree->prev = 0;

  item->order = 0;

  item->block->usedItems++;

  return item;
}

void Blockman::freeItem(ItemHeader* item) {
  ItemHeader* free;

  assert(item->flag == flUsed);

  item->flag = flFree;

  if (item->block->usedItems == 1) {
  // Procesing Free List
    free = (ItemHeader*)((char*)item->block + sizeof(BlockHeader));
    for (int i = 1; i <= item->block->itemCount; i++) {
      if (free != item) {
        assert(free->flag == flFree);   // DEBUG

        if (free == mFirstFree) {
          mFirstFree = free->next;
          if (mFirstFree != 0)
            mFirstFree->prev = 0;
        }
        else {
          free->prev->next = free->next;
          if (free->next != 0)
            free->next->prev = free->prev;
        }
      }
      free = (ItemHeader*)((char*)free + sizeof(ItemHeader) + mDataSize);
    }

    freeBlock(item->block);
  }
  else {
    item->prev = 0;
    item->next = mFirstFree;
    if (mFirstFree != 0)
      mFirstFree->prev = item;
    mFirstFree = item;
    item->block->usedItems--;
  }
}

void Blockman::getData(ItemHeader *item, void* desc) {

  memcpy(desc, (char*)item + sizeof(ItemHeader), mDataSize);
}

void Blockman::putData(ItemHeader *item, void* source) {

  memcpy((char*)item + sizeof(ItemHeader), source, mDataSize);
}

BlockHeader* Blockman::firstBlock() {

  return mFirstBlock;
}

ItemHeader* Blockman::firstFree() {

  return mFirstFree;
}

void Blockman::setFirstFree(ItemHeader* firstFree) {

  mFirstFree = firstFree;
}

void Blockman::setDataSize(int dataSize) {

  mDataSize = (word)dataSize;
}

//////////////////////  CRSharedObject  ///////////////////////

CRSharedObject::CRSharedObject() {

  mRefCount = 1;
}

int CRSharedObject::addRef() {

  return mRefCount++;
}

void CRSharedObject::release() {

  if (mRefCount == 1)
    delete this;
  else
    mRefCount--;
}

void CRSharedObject::checkValid() {
  if (!mRefCount)
    throwError(SInvalidSharedObject);
}

//////////////////  CRLob  /////////////////////

CRLob::CRLob() :
  CRSharedObject()
{

  mCached = false;
  mFirstPiece = 0;
  mDefPieceSize = defaultPieceSize;
  mRollback = 0;
  test = btSign;                    // DEBUG
}

CRLob::~CRLob() {

  checkValid();   // DEBUG
  test = 0;    // DEBUG

  mCached = false;
  clear();

  if (mRollback != 0)
    delete mRollback;
}

void CRLob::checkValid() {   // DEBUG

  assert(test == btSign);
}

void CRLob::clear() {
  PieceHeader* piece;
  PieceHeader* freePiece;

  if (mCached && mRollback == 0)
    saveToRollback();

  piece = mFirstPiece;
  while (piece) {
    freePiece = piece;
    piece = piece->next;
    free(freePiece);
	freePiece = NULL;
  }
  mFirstPiece = 0;
}

// Pieces

PieceHeader* CRLob::allocPiece(long size) {
  PieceHeader* piece;

  piece = (PieceHeader*)malloc(sizeof(PieceHeader) + size);
  piece->blob = 0;
  piece->size = size;
  piece->used = 0;
  piece->prev = 0;
  piece->next = 0;
  return piece;
}

void CRLob::reallocPiece(PieceHeader** piece, long size) {

  if (!size) {
    freePiece(*piece);
    *piece = 0;
  }
  else {
    *piece = (PieceHeader*)realloc(*piece, sizeof(PieceHeader) + size);

    (*piece)->size = size;
    if ((*piece)->used > size)
      (*piece)->used = size;
    if ((*piece)->blob) {
      if ((*piece)->prev)
        (*piece)->prev->next = *piece;
      else
        mFirstPiece = *piece;

      if ((*piece)->next)
        (*piece)->next->prev = *piece;
    }
  }
}

void CRLob::freePiece(PieceHeader* piece) {

  if (piece->blob)
    deletePiece(piece);

  free(piece);
}

void CRLob::appendPiece(PieceHeader* piece) {
  PieceHeader* last;

  if (!piece)
    return;

  piece->blob = this;
  piece->next = 0;
  if (!mFirstPiece) {
    piece->prev = 0;
    mFirstPiece = piece;
  }
  else {
    last = mFirstPiece;
    while (last->next)
      last = last->next;
    last->next = piece;
    piece->prev = last;
  }
}

void CRLob::deletePiece(PieceHeader* piece) {

  if (piece->blob != this)
    assert(false);

  if (mFirstPiece == piece) {
    mFirstPiece = piece->next;
    if (mFirstPiece)
      mFirstPiece->prev = 0;
  }
  else
    piece->prev->next = piece->next;

  piece->blob = 0;
}

void CRLob::compressPiece(PieceHeader** piece) {

  if (((*piece)->used) < (*piece)->size)
    reallocPiece(piece, (*piece)->used);
}

PieceHeader* CRLob::firstPiece() const {

  return mFirstPiece;
}

long CRLob::read(long position, long count, void* dest) {
  PieceHeader* piece;
  long pos;       // shift from Blob
  long shift;     // for read, in Piece
  long readCount; // all
  long moveSize;  // in Piece

  checkValid();   // DEBUG

  if (!count)
    count = size();

  if (mFirstPiece == 0 || position > size())
    return 0;

  piece = mFirstPiece;
  readCount = 0;
  pos = 0;
  while (piece && pos < position + count) {
    if (pos + piece->used > position) {
      if (position > pos)
        shift = position - pos;
      else
        shift = 0;

      if (pos + piece->used > position + count)
        moveSize = (position + count) - (pos + shift);
      else
        moveSize = piece->used - shift;

      memcpy((char*)dest + readCount, (char*)piece + sizeof(PieceHeader) + shift,
        moveSize);
      readCount += moveSize;
    }
    pos += piece->used;
    piece = piece->next;
  }
  return readCount;
}

void CRLob::write_blob(long position, long count, void* source) {
  PieceHeader* piece;
  long pos;        // shift from Blob
  long shift;      // for read, in Piece
  long writeCount; // all
  long moveSize;   // in Piece

  checkValid();   // DEBUG

  if (mCached && !mRollback)
    saveToRollback();

  if (position > size())
    position = size();

  piece = mFirstPiece;
  writeCount = 0;
  pos = 0;
  while (pos < position + count) {
    if (!piece) {
      if (count > mDefPieceSize)
        piece = allocPiece(mDefPieceSize);
      else
        piece = allocPiece(count);
      appendPiece(piece);
    };

    if (pos + piece->size > position) {
      if (position > pos)
        shift = position - pos;
      else
        shift = 0;

      if (pos + piece->size > position + count)
        moveSize = (position + count) - (pos + shift);
      else
        moveSize = piece->size - shift;

      memcpy((char*)piece + sizeof(PieceHeader) + shift, (char*)source + writeCount,
        moveSize);
      writeCount += moveSize;

      if (shift > piece->used)
        assert(false);
      if (shift + moveSize > piece->used)
        piece->used = shift + moveSize;
    }
    pos += piece->used;
    piece = piece->next;
  }
}

void CRLob::compress() {
  PieceHeader* piece;
  PieceHeader* nextPiece;

  piece = mFirstPiece;
  while (piece) {
    nextPiece = piece->next;
    compressPiece(&piece);
    piece = nextPiece;
  }
}

// Stream/File

void CRLob::loadFromFile(const CRString& fileName) {

  unsigned long remainder, bufLen;
  PieceHeader  *piece;

  FILE* in = fopen(fileName.c_str(), "rb");

  if (!in)
    throwError(SCanotOpenInputFile);

  //get file size
  fseek(in, 0, SEEK_END);
  remainder = ftell(in);

  //restore position
  fseek(in, 0, SEEK_SET);

  //while end of file
  while (remainder > 0) {

    if (remainder > (unsigned long)mDefPieceSize)
      bufLen = mDefPieceSize;
    else
      bufLen = remainder;

    piece = allocPiece(bufLen);

    //read piece from file
    fread((void*)((char*)piece + sizeof(PieceHeader)), sizeof(char), bufLen, in);

    piece->used = bufLen;

    appendPiece(piece);

    remainder -= bufLen;
  }

  fclose(in);
}

void CRLob::saveToFile(const CRString& fileName) {

  PieceHeader *piece;
  unsigned long bufLen;

  //open for write and append
  FILE* out = fopen(fileName.c_str(), "wb");

  if (!out)
    throwError(SCanotOpenOutputFile);

  piece = mFirstPiece;

  while (piece) {
    bufLen = piece->used;
    fwrite((char*)((char*)piece + sizeof(PieceHeader)), sizeof(char), bufLen, out);
    piece = piece->next;
  }

  fclose(out);
}

char* CRLob::getChars(char* result) {
  
  if (result) {
    read(0, 0, result);
    result[size()] = 0; // null terminator
  }

  return result;
}

CRString CRLob::getString() {
  PieceHeader* piece;
  long pos;       // shift from Blob
  long moveSize;  // in Piece
  CRString str;

  checkValid();   // DEBUG

  if (mFirstPiece == 0)
    return "";

  long blobSize = size();

  piece = mFirstPiece;
  pos = 0;
  while (piece && pos < 0 + blobSize) {
    if (pos + piece->used > 0) {
      if (pos + piece->used > blobSize)
        moveSize = blobSize - pos;
      else
        moveSize = piece->used;

      str.append((char*)piece + sizeof(PieceHeader), moveSize);
    }
    pos += piece->used;
    piece = piece->next;
  }
  return str;
}

void CRLob::setChars(const char* value) {
  
  clear();
  write_blob(0, strlen(value) + 1, (void*)value);
}

void CRLob::setString(const CRString& value) {
  
  setChars(value.c_str());
}

// TD18882
void CRLob::setChars(const char* value, long len) {
  
  clear();
  write_blob(0, len, (void*)value);
}

void CRLob::assign(CRLob& source) {
  const int bufSize = 65536;
  char buf[bufSize];
  long pos;
  long size;

  clear();

  pos = 0;
  do {
    size = source.read(pos, bufSize, buf);
    if (size > 0) {
      write_blob(pos, size, buf);
      pos += size;
    }
  }
  while (size > 0);
}

// Cached

void CRLob::checkCached() {

  if (!mCached)
    throwError(SBlobNotInCashedMode);
}

void CRLob::saveToRollback() {

  checkCached();

  mRollback = new CRLob();

  mRollback->mFirstPiece = mFirstPiece;

  mFirstPiece = 0;
}

void CRLob::enableCache() {

  if (mCached)
    throwError(SCachAlreadyEnabled);

  mCached = true;
}

void CRLob::commit() {

  checkCached();

  if (mRollback) {
    delete mRollback;
    mRollback = 0;
  }

  mCached = false;
}

void CRLob::cancel() {
  PieceHeader* piece;

  checkCached();

  if (mRollback) {
    piece = mRollback->mFirstPiece;
    mRollback->mFirstPiece = mFirstPiece;
    mFirstPiece = piece;

    delete mRollback;
    mRollback = 0;
  }

  mCached = false;
}

long CRLob::size() const {
  PieceHeader* piece;
  long size;

  size = 0;
  piece = mFirstPiece;
  while (piece) {
    size += piece->used;
    piece = piece->next;
  }
  return size;
}

long CRLob::defPieceSize() const {

  return mDefPieceSize;
}

PieceHeader* PieceHeader::nextPiece(PieceHeader* piece) {
  if (piece != 0)
    return piece->next;
  else
    return 0;
}

void* PieceHeader::pieceData(PieceHeader* piece)
{
  if (piece != 0)
    return (char*)piece + sizeof(PieceHeader);
  else
    return 0;
}

void* PieceHeader::pieceUsedPtr(PieceHeader* piece)
{
  if (piece != 0)
    return &piece->used;
  else
    return 0;
}

void DBError(const char* msg) {
  throw CRException(msg);
}

} // namespace ocl
