#ifndef _CCOMMAND_H
#define _CCOMMAND_H
#include "Btype.h"
#include<queue>
//using namespace std;

class CCommand
{
public:
	enum
	{
	
		SELECT = 0,
		EXECUTE = 1,
		CANCEL =2,
		
		CALL_DIR = 1,
		CALL_FILE = 2,
		CALL_SECTION = 3,
		SELECT_FILE = 4,
		SELECT_SECTION = 5,

		ACK_FILE = 1,
		NACK_FILE = 2,
		ACK_SECTION = 3,
		NACK_SECTION = 4	
	};
	CCommand();
	~CCommand();
	CCommand(const CCommand&cmd);

	CCommand& operator=(const CCommand &cmd);

	int create_Call_Value_Area_No(int rtuAddr, b_ui8 protectDevNo);////
	int create_Call_Value(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo);
	int create_Modify_Value_Area_No(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo,b_ui8 seType);
	int  create_Modify_Value(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo, b_ui8 seType,float *fBuf=0, int count=0,b_ui8 beginNo=0, b_ui8 endFlag=1);
	void create_Call_All_Data(b_ui16 iRtuAddr,b_ui8 bCot , b_ui8 bQoi);
	void create_Time_Syn();
	void create_Call_Kwh(b_ui16 iRtuAddr,b_ui8 bCot , b_ui8 bRqt,b_ui8 bFrz);

	int create_File_Call(int rtuAddr,int infoAddr,b_ui8 cmdType,b_ui16 fileName,b_ui8 secName);
	int create_File_Affirm(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 secName,b_ui8 affirmFlag);
//	void create_File_Affirm();

	void create_SingleCommand(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bScs,b_ui8 bSeType,b_bool bTimeTag,b_ui8 QU);
	void create_DoubleCommand(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_ui8 bSeType,b_bool bTimeTag,b_ui8 QU);
	void create_StepCommand(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_ui8 bSeType,b_bool bTimeTag,b_ui8 QU);
	void create_SetPointNva(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_ui8 bSeType,b_bool tmflag,b_ui8 bQl);
	void create_SetPointSVA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_ui8 bSeType,b_bool tmflag,b_ui8 bQl);
	void create_SetPointSFA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_ui8 bSeType,b_bool tmflag,b_ui8 bQl);
	void create_Bitstirng32(b_ui16 iRtuAddr,b_ui8 bCot,b_ui32 iInfoAddr,b_ui32 bitstring32,b_bool tmflag);
	
//	void create_SV(int iRtuAddr,int iInfoAddr,int iValue,unsigned char bCmdType,unsigned char bSeType,bool tmFlag=0);
//	void create_SV(int iRtuAddr,int iInfoAddr,float fVal,unsigned char bCmdType,unsigned char bSeType,bool tmFlag=0);
//	void create_SV(int iRtuAddr,int iInfoAddr,b_ui8* bVal,unsigned char bCmdType,unsigned char bSeType,bool tmFlag=0);
	void create_read_command(int iRtuAddr,int iInfoAddr);
	void create_reset_process(b_ui16 iRtuAddr,b_ui8 bQrp);
	void create_test_with_time(int rtuAddr,b_ui32 iInfoAddr,b_ui16 bTcs);
	void create_parameter_activation(b_ui16 iRtuAddr,b_ui8 bCot,b_ui32 iInfoAddr,b_ui8 bQpa);
	void create_parameter_measured_value(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8* bVal, b_ui8 bQpm,b_ui8 bCmdType);
	int getTi();
private:
	void SyncTimeCoding(b_ui8 *const buf);
	b_ui16 host2protocol16(b_ui16 src);
	b_ui32 host2protocol32(b_ui32 src);
	int checkHostSerial();
public:	
//private:
	unsigned char _bTi;
	int _iRtuAddr;
	int _iInfoAddr;
	unsigned char _bCmdType;
	unsigned char _bSeType;
	bool tmFlag;
	int _iValue;
	float*_fBuf;
	int _fSize;
	b_ui8 *_arg;
	int _argSize;
};

class CCommandManager
{
public:
	CCommandManager();
	~CCommandManager();
	bool getNextCommand( CCommand& cmd);
	bool getCurrentCommand( CCommand& cmd);
	void    putCommandToQueue(const CCommand &cmd);
	std::queue<CCommand> m_cmdQueue;
	CCommand        *m_currentCommand;
};

#endif
