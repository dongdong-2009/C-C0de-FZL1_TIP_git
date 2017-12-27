
#ifndef __C_ORACLE_CONNECTION_HH__
#define __C_ORACLE_CONNECTION_HH__

#ifdef WIN32
#pragma warning(disable: 4290) // C++ exception specification ignored
#endif // WIN32


#include <vector>
#include <set>
#include "IDbConnection.h"
#include "ocilib.h"
#include "ocilib_internal.h"
#include "AQCommonData.h"

NS_BEGIN(TA_AQ_Lib)

//namespace TA_Base_Core
//{
class IQueryAdapter;
class CMessage;

class COracleConnection : public IDbConnection
{
private:
	typedef struct st_ColumnNameType_opt
	{
		std::string      m_strColumnName;            
		int              m_nColumnType;
		int              m_nColumnSubType;
		int              m_nColumnSize;
	}ColumnNameType, *PColumnNameType;
	
	typedef  std::map<std::string, PColumnNameType>			   mapColumNTConT;
	typedef  std::map<std::string, PColumnNameType>::iterator  mapColumNTConIterT;
	typedef  std::map<std::string, PColumnNameType>::value_type  mapColumNTConVT;


	typedef		std::vector<std::string>              vecStrConT;
	typedef		std::vector<std::string>::iterator    vecStrConIterT;

	typedef		std::set<std::string>				  setStrConT;
	typedef		std::set<std::string>::iterator		  setStrConIterT;

public:

	COracleConnection(const std::string& strDbName, 
					  const std::string& strUserName,
					  const std::string& strPwd, 
					  const std::string& strHost = "");

	
	~COracleConnection();
	
	bool isAnyQueryActive();	

	void open();

	bool isOpen();
	
	void close();

	void closeAll();

	void setAutoCommit(bool bAutoCommit);
	 
	bool getAutoCommit(); 
	
	void rollback();
	
	void commit();
	
	void exec( const std::string& statement, 
				IQueryAdapter*& pIQueryAdapter, 
				bool isQuery = false, 
				int prefetch = 10);
	
	void fetchBLOB(const std::string &strSQL, 
					const std::string &colomnName, 
					DataClobArray& data );
	
	void updateBLOB(
		const std::string &strTableName, 
		const std::string &colomnName,
		const std::vector<unsigned char>& data, 
		const std::string &strWhereClause);

	void cleanQuery( IQueryAdapter*& pQuery );	
	
public:	

	
	
	/*
	deqMsg() return value: 0 or -1
	*/
	int    deqMsg(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg);	
	bool   listen(const std::string& strQName, u_int  nSubID);
	bool   listen(const std::string& strQName, const std::string& strSubName);
	int    aqCommit(const std::string& strQName, int nSubID, const std::string& strConsumer, const nPairT& rSeqIDPair);
	int    aqCommit(const std::string& strQName, const std::string& strMsgID);

	/*
	*enqMsg Return Value:
	*Return	0: enqMsg Ok
	*Return -1: enqMsg have Error
	*Return ER_ORACLE_EnqueueFailed: oracle queue has been stopped to prevent any further enqueueing. 
	*Return	Err_EnqReqACkCode_PLTypeInvalid, Err_EnqReqACkCode_OCILibErr, Err_EnqReqACkCode_DataSrcErr: Create EnqObject Error
	*EnqMsg handle is NULL, nErrorCode value: Err_EnqReqACkCode_PLTypeInvalid
	*EnqMsg's object handle is NULL, nErrorCode value:Err_EnqReqACkCode_OCILibErr
	*msg data have error, nErrorCode value: Err_EnqReqACkCode_DataSrcErr
	*/  	
	int    enqMsg(CMessage* pEnqMsg, PMAQ_ENQ_OPT pEnqOpt);
	int	   enqMsg( std::list<CMessage*>& EnqMsgList, PMAQ_ENQ_OPT pEnqOpt );
private:
	int _GetDbConnectionStr(const std::string& strDbName, const std::string& strUserName, 
		const std::string& strPwd, const std::string& strHost, std::string &strDbConnectionStr);
	int _CreateConnection();
	int _GetOracleErrMsg(std::string &strError, int &nError);
	int _LOGOracleErrMsg( std::string strLogInfo, int nWriteLog = 0);



	int _CheckDBConnection();
	int _CheckEnqMsgInputParam(CMessage* pEnqMsg);
	int _GetEnqQName(CMessage* pEnqMsg, std::string& strQueueName);
	int _GetStrEnqAgentArray(CMessage* pEnqMsg, std::string& strEnqAgentArray);
	int _GetLstEnqAgentArray(const std::string& strRecipientLst, vecStrConT& lstEnqAgentArray);
	int _CreateEnqAgentArray(const vecStrConT& lstEnqAgentArray);
	int _FreeEnqAgentArray();
	int _GetAQDataTypeNameByAQQName(const std::string& strQueueName, std::string& strAQDataTypeName);
	int _GetLstAQDataTypeInfoColumn(const std::string& strAQDataTypeName, mapColumNTConT& mapAQDataTypeInfoColumn);
	int _GetAgtNameFromNameAddress(const std::string& strNameAddress, std::string& strAgtName);

	int _CreateEnqHandle(const std::string& strQueueName, OCI_TypeInfo* pAQDataTypeInfo);
	int	_FreeEnqHandle();
	int	_FreeEnqObjetHandle();


	/*
	*_CreateEnqObjetHandle Return Value:
	*Return	0: Create EnqObject Ok
	*Return	Err_EnqReqACkCode_PLTypeInvalid, Err_EnqReqACkCode_OCILibErr, Err_EnqReqACkCode_DataSrcErr: Create EnqObject Error
	*EnqMsg handle is NULL, nErrorCode value: Err_EnqReqACkCode_PLTypeInvalid
	*EnqMsg's object handle is NULL, nErrorCode value:Err_EnqReqACkCode_OCILibErr
	*msg data have error, nErrorCode value: Err_EnqReqACkCode_DataSrcErr
	*/  
	int _CreateEnqObjetHandle(CMessage* pEnqMsg);

	//deq
private:
	std::string _OCIMsgGetIDEx(OCI_Msg *msg, unsigned int *len);

	int _CheckDeqMsgInputParam( const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg);
	int _GetDeqQueueName(const std::string& strQNameSrc, std::string& strQNameGet);
	int _GetDeqBaseInfo(PMAQ_DEQ_OPT pDeqMsg, int& nDeqMode, int& nDeqNav, int& nDeqFetchSize, std::string& strDeqClientAgentName);
	int _GetDeqAgentArrayInfo(PMAQ_DEQ_OPT pDeqMsg);

	int _CreateDeqAgentArray();
	int _FreeDeqAgentArray();
	int _CreateDeqHandle(const std::string& strQueueName, OCI_TypeInfo* pAQDataTypeInfo);
	int _FreeDeqHandle();
	
private:	
	int _EnqInit( CMessage* pEnqMsg );
	int _EnqUnInit();
	bool _CheckEnqParamSame(CMessage* pEnqMsg);
	//use Ocilib fun to enq msg
	int  _OcilibEnqMsg(CMessage* pEnqMsg);
	//use sql to call pack enq msg
	int  _SqlEnqMsg(CMessage* pEnqMsg);
	//use prepare one sql to call pack enq msg
	int _SqlEnqMsgPrepare( CMessage* pEnqMsg );
	//use prepare Array sql to call pack enq msg
	int _SqlEnqMsgPrepareArray(std::list<CMessage*>& EnqMsgList);
	//init Memort for   prepare Array sql
	int _PrepareBatchInsertionMemory(int nBatchSize);
	
	
	

	int _DeqInit(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg);
	int _DeqUnInit();
	bool _CheckDeqParamSame(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg);
	int _GetDeqPayload(const std::string& strColumnName, const std::string& strColumnValue, std::string& strPayload);
	int _OracleAQUnInit();
	int _GetSetDeqModeNav(PMAQ_DEQ_OPT pDeqMsg);
	int _GetMsgIDSetFromList( LstMsgConT& lstMsgSrc, setStrConT& setMsgIdDest );

	

	/*
	return value will be -1 or 0 or ER_ORACLE_AQ_DEQUEUE_TIMEOUT or ER_ORACLE_DequeueFailed 
	-1: means deq have error!
	0 :means deq function is ok! but there have no data for deq
	ER_ORACLE_AQ_DEQUEUE_TIMEOUT: means deq function is ok! but there have no data for deq
	ER_ORACLE_DequeueFailed; oracle deq closed
	*/
	int _DeqMsgBrowser( const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg );
	/*	
	return value will be -1 or 0 or ER_ORACLE_AQ_DEQUEUE_TIMEOUT or ER_ORACLE_DequeueFailed 
	-1: means deq have error!
	0 :means deq function is ok!
	ER_ORACLE_AQ_DEQUEUE_TIMEOUT: means deq function is ok! but there have no data for deq
	ER_ORACLE_DequeueFailed; oracle deq closed
	*/
	int _DeqOneMagBrowser(setStrConT& setDeqGetMsgId, LstMsgConT& rLstMsg, int &nOneRowDataLen);

	
	/*
	return value will be -1 or 0 or Err_DEQACkCode_NOTFIND or ER_ORACLE_AQ_DEQUEUE_TIMEOUT 
	-1: means deq have error!
	0 :deq remove data is ok!
	ER_ORACLE_AQ_DEQUEUE_TIMEOUT: means deq function is ok! but there have no data for deq
	Err_DEQACkCode_NOTFIND: means deq function is ok! but there deq get one msg is not in remove list
	*/
	int _DeqMsgRemove( const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg );

	/*	
	return value will be -1 or 0 or Err_DEQACkCode_DEQNODATA or ER_ORACLE_AQ_DEQUEUE_TIMEOUT 
	-1: means deq have error!
	0 :means deq function is ok!
	ER_ORACLE_AQ_DEQUEUE_TIMEOUT: means deq function is ok! but there have no data for deq
	Err_DEQACkCode_NOTFIND: means deq function is ok! but there deq get one msg is not in remove list
	*/ 
	int _DeqOneMagRemove(setStrConT& setDeqGetMsgId, LstMsgConT& rLstMsg, int &nOneRowDataLen);
	
	
	int _FreeMapColumNameType(mapColumNTConT& mapColumNameType);
	int _DBCommit();
	//enq deq use
private:
	OCI_Connection*				m_connection;
	std::string					m_strDbConStr;
	
	std::string                 m_strAQDataTypeName;
	OCI_TypeInfo*               m_pAQDataTypeInfo;
	mapColumNTConT              m_mapAQDataTypeInfoColumn;
	int                         m_nInfoGetType;//OCI_TIF_TYPE
	int                         m_nPayLoadType;//MsgPayloadType
	int                         m_nLstAQDataTypeInfoColumCount;

	int                         m_nLogNmberCount;
	bool                        m_bIsConnected;
	

	
//enq use	
private:
	bool						m_bEnqSet;
	bool						m_bEnqInit;
	
    std::string                 m_strEnqQueueName;
    
    std::string                 m_strEnqAgentArray;
    vecStrConT					m_lstEnqAgentArray;
    OCI_Agent**                 m_ppEnqAgentArray;
	int                         m_nEnqAgentArraySize;
										     
	OCI_Enqueue*				m_pEnqEnqueueHandle;
	OCI_Msg*					m_pEngMsgHandle;
	OCI_Object*					m_pEnqObjectHandle;


//deq   use
private:
	bool						m_bDeqSet;
	bool						m_bDeqInit;

	OCI_Dequeue*				m_pDeqDequeue;
	std::string                 m_strDeqQueueName;

	std::string                 m_strDeqClientAgentName;
	int							m_nDeqMode; 
	int						    m_nDeqNav;
	int							m_nDeqFetchSize; //200kb  // the row of message for MySQL, the size of message for Oracle
	std::string					m_strDeqMsgID;

	std::string                 m_strDeqAgentArray;
	vecStrConT					m_lstDeqAgentArray;
	OCI_Agent**                 m_ppDeqAgentArray;
	int                         m_nDeqAgentArraySize;
	int                         m_nCloseSet;

	
private:
	char						(*m_ppBuffMessageQualifierValue)[101];
	char						(*m_ppBuffAuditDataOracleValue)[4001];
	char						(*m_ppBuffAuditDataMySQLValue)[4001];
	bool                        m_bBufferInitd;
	int                         m_nBatchSize;

private:
	char*                       m_pBufferSQL;
	int                         m_nBufferSQLSize;

private:
	OCI_Statement*				m_pStatement;


};

//}

NS_END(TA_AQ_Lib)


#endif // __C_ORACLE_CONNECTION_HH__



