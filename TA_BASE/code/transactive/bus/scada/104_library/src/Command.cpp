#include"Command.h"
#include "Iec104Def.h"
#include <time.h>

CCommand::CCommand()
	:_bTi(-1),
	_iRtuAddr(-1),
	_iInfoAddr(-1),
	_bCmdType(-1),
	_bSeType(-1),
	tmFlag(0),
	_iValue(-1),
	_fBuf(NULL),
	_fSize(0),
	_arg (NULL),
	_argSize(0)	
{
}

CCommand::~CCommand()
{
	if(_fBuf)
		delete[] _fBuf;
	if(_arg)
		delete[] _arg;
}
//copy construct 
CCommand::CCommand(const CCommand&cmd)
{
	_bTi = cmd._bTi;
	_iRtuAddr = cmd._iRtuAddr;
	_iInfoAddr =cmd._iInfoAddr;
	_bCmdType = cmd._bCmdType;
	_bSeType = cmd._bSeType;
	tmFlag = cmd.tmFlag;
	_iValue = cmd._iValue;
	int i=0;
	if(cmd._fBuf!=NULL)
	{
	//	float *tmp = _fBuf;
		_fBuf = new float[cmd._fSize];
		_fSize = cmd._fSize;
		for(i=0;i<_fSize;i++)
			_fBuf[i] = cmd._fBuf[i];
	//	if(tmp)
	//		delete[] tmp;
	}
	else
	{
		_fBuf = NULL;
		_fSize = 0;
	}
	if(cmd._arg!=NULL)
	{
	//	unsigned char *tmp = _arg;
		_arg = new b_ui8[cmd._argSize];
		
		for(i=0;i<cmd._argSize;i++)
			_arg[i] = cmd._arg[i];
	//	if(_argSize>0)
	//		delete[] tmp;
		_argSize = cmd._argSize;
	}
	else
	{
		_arg = NULL;
		_argSize = 0;
	}
}

CCommand& CCommand::operator=(const CCommand &cmd)
{
	_bTi = cmd._bTi;
	_iRtuAddr = cmd._iRtuAddr;
	_iInfoAddr =cmd._iInfoAddr;
	_bCmdType = cmd._bCmdType;
	_bSeType = cmd._bSeType;
	tmFlag = cmd.tmFlag;
	_iValue = cmd._iValue;
	int i=0;
	if(cmd._fBuf!=NULL)
	{
		float *tmp = _fBuf;
		_fBuf = new float[cmd._fSize];
		_fSize = cmd._fSize;
		for(i=0;i<_fSize;i++)
			_fBuf[i] = cmd._fBuf[i];
		if(tmp)
			delete[] tmp;
	}
	else
	{
		_fBuf = NULL;
		_fSize = 0;
	}
	if(cmd._arg!=NULL)
	{
		unsigned char *tmp = _arg;
		_arg = new unsigned char[cmd._argSize];
		
		for(i=0;i<cmd._argSize;i++)
			_arg[i] = cmd._arg[i];
		if(_argSize>0)
			delete[] tmp;
		_argSize = cmd._argSize;
	}
	else
	{
		_arg = NULL;
		_argSize = 0;
	}
	return *this;
}
void  CCommand::SyncTimeCoding(b_ui8 *const buf)
{
	time_t ltime;
	//struct tm * nowtime;
	time( &ltime );
	//nowtime = localtime( &ltime );	
	struct tm *tt;
	//tt = gmtime(&ltime);
	tt = localtime(&ltime);
	int iMs = tt->tm_isdst + tt->tm_sec * 1000;
	int  iMs1 = iMs % 60000;
	b_ui8 iMinute = tt->tm_min;	
	b_ui8 iHour = tt->tm_hour %24;
	buf[0] = iMs1 %256;
	buf[1] =  iMs1 /256;
	buf[2] = iMinute &0x3f;
	buf[3] = iHour &0x1f;
	buf[4] = ((tt->tm_wday & 0x07) << 5) | ((tt->tm_mday )& 0x1f);
	buf[5] = (tt->tm_mon +1)& 0x0f;
	buf[6] = tt->tm_year - 100; 
}
void CCommand::create_read_command(int iRtuAddr,int iInfoAddr)
{
	_bTi = IEC104_C_RD_NA;
	_iRtuAddr =iRtuAddr;
	_iInfoAddr = iInfoAddr;
}
void CCommand::create_reset_process(b_ui16 iRtuAddr,b_ui8 bQrp)
{
	_bTi = IEC104_C_RP_NA;
	_iRtuAddr =iRtuAddr;
	if(_argSize>0)
		delete[] _arg;
	_argSize=1;
	_arg = new b_ui8[_argSize];
	_arg[0]=bQrp;
}
void CCommand::create_test_with_time(int rtuAddr,b_ui32 iInfoAddr,b_ui16 bTcs)
{
	_bTi = IEC104_C_TS_TA;
	_iRtuAddr = rtuAddr;
	_iInfoAddr = iInfoAddr;
	if(_argSize>0)
		delete[] _arg;
	_argSize=2;
	_arg = new b_ui8[_argSize];
	memcpy(_arg,&bTcs,2);
}

void CCommand::create_parameter_activation(b_ui16 iRtuAddr,b_ui8 bCot,b_ui32 iInfoAddr,b_ui8 bQpa)
{
	_bTi = IEC104_P_NC_NA;
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;
	if(_argSize>0)
		delete[] _arg;
	_argSize = 2;
	_arg = new b_ui8[2];
	_arg[0] = bCot;
	_arg[1] = bQpa;
}

//103 extensions
int CCommand::create_Call_Value_Area_No(int rtuAddr, b_ui8 protectDevNo)
{
	_bTi = IEC104_E_CD_NA;
	_iRtuAddr = rtuAddr;
	_iInfoAddr = 241*256*256 + 0 + protectDevNo;
	_argSize = 2;
	_arg = new b_ui8[_argSize];
	_arg[0]=0;
	_arg[1] =protectDevNo;
	return 1;
}
int CCommand::create_Modify_Value_Area_No(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo,b_ui8 seType)
{
	_bTi = IEC104_E_MD_NA;
	_iRtuAddr = rtuAddr;
	switch(seType)
	{
		case CCommand::SELECT:
			_iInfoAddr = 249*256*256 + 0 + protectDevNo;
			_bSeType	 = 0;
			break;
		case CCommand::EXECUTE:
			_iInfoAddr = 250*256*256 + 0 + protectDevNo;
			_bSeType = 1;
			break;
		case CCommand::CANCEL:
			_iInfoAddr = 251*256*256 + 0 + protectDevNo;
			_bSeType = 2;
			break;
		default:
			return -1;
	}
	
	_argSize = 3;
	_arg = new b_ui8[_argSize];
	_arg[0]=areaNo;	
	_arg[1] = protectDevNo;
	_arg[2] = 0;
	return 1;
}
int CCommand::create_Call_Value(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo)
{
	_bTi = IEC104_E_CD_NA;
	_iRtuAddr = rtuAddr;
	_iInfoAddr = 241*256*256 + 0 + protectDevNo;
	_argSize = 2;
	_arg = new b_ui8[_argSize];
	_arg[0]=areaNo+1;	
	_arg[1] = protectDevNo;
	return 1;
}
int  CCommand::create_Modify_Value(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo, b_ui8 seType,float *fBuf, int count,b_ui8 beginNo, b_ui8 endFlag)
{
	_bTi = IEC104_E_MD_NA;
	_iRtuAddr = rtuAddr;
	int i=0;
	b_ui8 ngd= 0;
	switch(seType)
	{
		case CCommand::SELECT:
			_iInfoAddr = 249*256*256 + 0 + protectDevNo;
			_fSize = count;
			_fBuf = new float[_fSize];
			for(i=0; i < _fSize; i++)
				_fBuf[i] = fBuf[i];
			_argSize = 4;
			_arg = new b_ui8[_argSize];
			_arg[0] = areaNo;
			_arg[1] = protectDevNo;
			ngd = count;
			if(!endFlag)
				ngd = 0x80 | ngd;
			_arg[2] = ngd;
			_arg[3] = beginNo;
			_bSeType = 0;
			//
			break;
		case CCommand::EXECUTE:
			_iInfoAddr = 250*256*256 + 0 + protectDevNo;
			_argSize = 2;
			_arg = new b_ui8[_argSize];
			_arg[0] = areaNo;
			_arg[1] = protectDevNo;
			_bSeType = 1;
			break;
		case CCommand::CANCEL:
			_iInfoAddr = 251*256*256 + 0 + protectDevNo;
			_argSize = 2;
			_arg = new b_ui8[_argSize];
			_arg[0] = areaNo;
			_arg[1] = protectDevNo;
			_bSeType = 2;
			break;
		default:
			return -1;
	}
	return 1;
	
}


void CCommand::create_Call_All_Data(b_ui16 iRtuAddr,b_ui8 bCot =6, b_ui8 bQoi=20)
{
	_bTi = IEC104_C_IC_NA;
	_iRtuAddr =  iRtuAddr;
	_argSize=2;
	_arg = new b_ui8[_argSize];
	_arg[0] = bCot;
	_arg[1] = bQoi;
}
void CCommand::create_Time_Syn()
{
	_bTi = IEC104_C_CS_NA;
	_iRtuAddr = -1;
	_iInfoAddr = -1;
	_bCmdType = -1;
	_bSeType = -1;
	tmFlag = 0;
	_iValue = -1;
}
void CCommand::create_Call_Kwh(b_ui16 iRtuAddr,b_ui8 bCot, b_ui8 bRqt,b_ui8 bFrz)
{
	_bTi = IEC104_C_CI_NA;
	_iRtuAddr =  iRtuAddr;
	b_ui8 bQcc = bRqt |(bFrz<<6);
	_argSize = 2;
	_arg = new b_ui8[_argSize];
	_arg[0]= bCot;
	_arg[1]=bQcc;
}
void CCommand::create_SingleCommand(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bScs,b_ui8 bSeType,b_bool bTimeTag,b_ui8 QU)
{
	b_ui8 bSco =0,bCot=IEC104_COT_ACT;
	_bTi = 45;
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;
	bSco = (bScs & 0x01) |(QU <<2);
	switch(bSeType)
	{
		case SELECT:
			bSco |= 0x80;
			break;			
		case CANCEL:
			bCot = IEC104_COT_DEACT;
			bSco |= 0x80;
			break;
	}
	_argSize = 2;
	if( bTimeTag==b_true)
	{
		_argSize = 9;
		_bTi = IEC104_C_SC_TA;
	}
	_arg= new b_ui8[_argSize];
	_arg[0]= bCot;
	_arg[1]= bSco;
	if( bTimeTag==b_true)
	{
		SyncTimeCoding(_arg+2);
	}
}
void CCommand::create_DoubleCommand(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_ui8 bSeType,b_bool bTimeTag,b_ui8 QU)
{
	b_ui8 bDco =0,bCot=IEC104_COT_ACT;
	_bTi = IEC104_C_DC_NA;
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;
	bDco = (bDcs & 0x03) |(QU<<2);
	switch(bSeType)
	{
		case SELECT:
			bDco |= 0x80;
			break;			
		case CANCEL:
			bCot = IEC104_COT_DEACT;
			bDco |= 0x80;
			break;
	}
	_argSize = 2;
	if( bTimeTag==b_true)
	{
		_argSize = 9;
		_bTi = IEC104_C_DC_TA;
	}
	_arg= new b_ui8[_argSize];
	_arg[0]= bCot;
	_arg[1]= bDco;
	if( bTimeTag==b_true)
	{
		SyncTimeCoding(_arg+2);
	}
}
void CCommand::create_StepCommand(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs, b_ui8 bSeType,b_bool bTimeTag,b_ui8 QU)
{
	b_ui8 bRco =0,bCot=IEC104_COT_ACT;
	_bTi = IEC104_C_RC_NA;
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;
	bRco = (bRcs & 0x03) |(QU<<2);
	switch(bSeType)
	{
		case SELECT:
			bRco |= 0x80;
			break;			
		case CANCEL:
			bCot = IEC104_COT_DEACT;
			bRco |= 0x80;
			break;
	}
	_argSize = 2;
	if( bTimeTag==b_true)
	{
		_argSize = 9;
		_bTi = IEC104_C_RC_TA;
	}
	_arg= new b_ui8[_argSize];
	_arg[0]= bCot;
	_arg[1]= bRco;
	if( bTimeTag==b_true)
	{
		SyncTimeCoding(_arg+2);
	}
}
void CCommand::create_SetPointNva(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_ui8 bSeType,b_bool tmflag,b_ui8 bQl)
{
	b_ui8 bQos =0,bCot=IEC104_COT_ACT;
	_bTi = IEC104_C_SE_NA;
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;
	bQos = bQl&0x7f;
	switch(bSeType)
	{
		case SELECT:
			bQos |= 0x80;
			break;			
		case CANCEL:
			bCot = IEC104_COT_DEACT;
			bQos |= 0x80;
			break;
	}
	_argSize = 4;
	if( tmflag==b_true)
	{
		_argSize = 11;
		_bTi = IEC104_C_SE_TA;
	}
	_arg= new b_ui8[_argSize];
	_arg[0]= bCot;
	b_ui16 dstNva = host2protocol16(Nva);
	memcpy(&_arg[1],&dstNva,2);
	_arg[3]= bQos;
	if(tmflag==b_true)
	{
		SyncTimeCoding(_arg+4);
	}
}
void CCommand::create_SetPointSVA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_ui8 bSeType,b_bool tmflag,b_ui8 bQl)
{
	b_ui8 bQos =0,bCot=IEC104_COT_ACT;
	_bTi = IEC104_C_SE_NB;
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;
	bQos = bQl&0x7f;
	switch(bSeType)
	{
		case SELECT:
			bQos |= 0x80;
			break;			
		case CANCEL:
			bCot = IEC104_COT_DEACT;
			bQos |= 0x80;
			break;
	}
	_argSize = 4;
	if( tmflag==b_true)
	{
		_argSize = 11;
		_bTi = IEC104_C_SE_TB;
	}
	_arg= new b_ui8[_argSize];
	_arg[0]= bCot;
	b_ui16 dstSva = host2protocol16(Sva);
	memcpy(&_arg[1],&dstSva,2);
	_arg[3]= bQos;
	if( tmflag==b_true)
	{
		SyncTimeCoding(_arg+4);
	}
}
void CCommand::create_SetPointSFA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_ui8 bSeType,b_bool tmflag,b_ui8 bQl)
{
	b_ui8 bQos =0,bCot=IEC104_COT_ACT;
	_bTi = IEC104_C_SE_NC;
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;
	b_ui32 srcSfa;
	bQos = bQl&0x7f;
	switch(bSeType)
	{
		case SELECT:
			bQos |= 0x80;
			break;			
		case CANCEL:
			bCot = IEC104_COT_DEACT;
			bQos |= 0x80;
			break;
	}
	_argSize = 6;
	if( tmflag==b_true)
	{
		_argSize +=7;
		_bTi = IEC104_C_SE_TC;
	}
	_arg= new b_ui8[_argSize];
	_arg[0]= bCot;
	memcpy(&srcSfa,&Sfa,4);
	b_ui32 dstSfa= host2protocol32(srcSfa);
	memcpy(&_arg[1],&dstSfa,4);
	_arg[5]= bQos;
	if(tmflag==b_true)
	{
		SyncTimeCoding(_arg+6);
	}
}

void CCommand::create_Bitstirng32(b_ui16 iRtuAddr,b_ui8 bCot,b_ui32 iInfoAddr,b_ui32 bitstring32,b_bool tmflag)
{
	b_ui32 srcBit32,dstBit32;
	_bTi = IEC104_C_BO_NA;
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;
	_argSize = 5;
	if( tmflag==b_true)
	{
		_argSize +=7;
		_bTi = IEC104_C_BO_TA;
	}
	_arg= new b_ui8[_argSize];
	_arg[0]= bCot;
	memcpy(&srcBit32,&bitstring32,4);
	dstBit32= host2protocol32(srcBit32);
	memcpy(&_arg[1],&dstBit32,4);
	if(tmflag==b_true)
	{
		SyncTimeCoding(_arg+5);
	}	
}
void CCommand::create_parameter_measured_value(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8* bVal, b_ui8 bQpm,b_ui8 bCmdType)
{
	int i=0;
	b_ui16 val16=0;
	b_ui32 val32=0;
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;
	if(_argSize>0)
	{
		delete[] _arg;
		_argSize = 0;
	}
	switch(bCmdType)
	{
		case 0://normalized value
			_bCmdType = 0;
			_bTi = IEC104_P_ME_NA;
			_argSize = 3;
			_arg = new b_ui8[_argSize];
			memcpy(&val16,bVal,2);
			val16 = host2protocol16(val16);
			memcpy(_arg, &val16,2);
			memcpy(_arg+2,&bQpm,1);
			break;
		case 1://scaled value
			_bCmdType = 1;
			_argSize = 3;
			_bTi = IEC104_P_ME_NB;
			_arg = new b_ui8[_argSize];
			memcpy(&val16,bVal,2);
			val16 = host2protocol16(val16);
			memcpy(_arg, &val16,2);
			memcpy(_arg+2,&bQpm,1);
			break;
		case 2://short floating point number
			_bCmdType = 2;
			_argSize = 5;
			_bTi = IEC104_P_ME_NC;
			_arg = new b_ui8[_argSize];
			memcpy(&val32,bVal,4);
			val32=host2protocol32(val32);
			memcpy(_arg, &val32,4);
			memcpy(_arg+4,&bQpm,1);
			break;
		default:
			break;
	}
}

int CCommand::create_File_Call(int iRtuAddr,int iInfoAddr,b_ui8 cmdType,b_ui16 fileName,b_ui8 secName)
{
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;

	int i=0;
	if(cmdType <0 && cmdType>5)
		return -1;
	_bCmdType = cmdType;
	_bTi = IEC104_F_SC_NA;
	_argSize = 3;
	_arg = new b_ui8[_argSize];
	for(i=0;i<2;i++)
		_arg[i]=*((b_ui8*)&fileName +i);
	_arg[i] = secName;
	return 1;
}

int CCommand::create_File_Affirm(int iRtuAddr,int iInfoAddr,b_ui16 fileName,b_ui8 secName,b_ui8 affirmFlag)
{
	_iRtuAddr = iRtuAddr;
	_iInfoAddr = iInfoAddr;

	_bTi = IEC104_F_AF_NA;
	_argSize = 4;
	_arg = new b_ui8[_argSize];
	_arg[0] = *(b_ui8*)&fileName;
	_arg[1] =*((b_ui8*)&fileName+1);
	_arg[2] = secName;
	_arg[3] = affirmFlag;
	return 1;	
}
int CCommand::getTi()
{
	return _bTi;
}
b_ui16 CCommand::host2protocol16(b_ui16 src)
{
	if(checkHostSerial()==0)//equal protocol104
		return src;
	b_ui16 dest;
	dest = ((src>>8)&0x00ff)|((src<<8)&0xff00);
	return dest;
}
b_ui32 CCommand::host2protocol32(b_ui32 src)
{
	if(checkHostSerial()==0)//equal protocol104
		return src;
	b_ui32 dest;
	dest=((src>>24)&0x000000ff)|((src>>8)&0x0000ff00)|((src<<8)&0x00ff0000)|((src<<24)&0xff000000);	
	return dest;
}
int CCommand::checkHostSerial()
{
	union
	{
		b_ui8 buff[2];
		b_ui16 data;
	}tmp;
	tmp.data=0x0102;
	if(tmp.buff[0]==0x01)//big ending
		return 1;
	if(tmp.buff[0]==0x02)//little ending
		return 0;
	else 
		return -1;
}

CCommandManager::CCommandManager()
{
	m_currentCommand=NULL;
}
CCommandManager::~CCommandManager()
{
	if(m_currentCommand)
		delete m_currentCommand;
}

bool CCommandManager::getNextCommand( CCommand& cmd)
{
	if(m_cmdQueue.empty())
		return false;
	if(m_currentCommand==NULL)
		m_currentCommand = new CCommand();
	*m_currentCommand = m_cmdQueue.front();
	cmd = m_cmdQueue.front();
	m_cmdQueue.pop();
	return true;
}

bool CCommandManager::getCurrentCommand( CCommand& cmd)
{
	if(m_currentCommand==NULL)
		return false;
	cmd = *m_currentCommand;
	return true;
}

void CCommandManager::putCommandToQueue(const CCommand &cmd)
{
	m_cmdQueue.push(cmd);
}
