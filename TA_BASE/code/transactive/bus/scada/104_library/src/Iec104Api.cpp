
#include "Iec104Api.h"
#include "Btype.h"


esca::CMutex* IEC104_API::m_api_mutex=NULL;
 IEC104_API* IEC104_API:: api_ =NULL;

IEC104_API::IEC104_API()
	:m_mngMutex(false),
	ade_(NULL),
	logCallBackFunc(NULL),
	cmdMng_(NULL),
	handler_(NULL)
{

	ade_ = new CAdaptee();
	cmdMng_ = new CCommandManager();
	handler_ = new IEC104_Event_Handler(); 
}

IEC104_API::~IEC104_API()
{
	if(ade_)
		delete ade_;
	if(cmdMng_)
		delete cmdMng_;
	if(handler_)
		handler_;
}

void IEC104_API::setLocalAddr(std::string strLocalAddr)
{
	m_linkPara.strLocalAddr=strLocalAddr;
}

std::string IEC104_API::getVersion()
{
	return VERSION;
}
IEC104_API * IEC104_API::getInstance()
{
	if(api_==NULL)
	{
		esca::CLock lock(m_api_mutex);
		if(api_==NULL)
			api_ = new IEC104_API();
	}
	return api_;

}
int IEC104_API::registerHandler(IEC104_Event_Handler * handler)
{
	if(handler == NULL)
		return -1;
	if(handler!=handler_)
	{
		IEC104_Event_Handler *tmp = handler_;
		handler_ = handler;
		delete tmp;
	}
	return 1;
}
int IEC104_API::setSockReadRetries(int tryTimes)
{
	if(tryTimes<=0)
		return -1;
	CIecSocket::setReadRetries(tryTimes);
	return 0;
}
void IEC104_API::setWorkMode(int mode)
{
	ade_->setWorkMode(mode);
}
void IEC104_API::setT0(b_ui8 T0)
{
	m_linkPara.T0 = T0;
}
void IEC104_API::setT1(b_ui8 T1)
{
	m_linkPara.T1= T1;
}
void IEC104_API::setT2(b_ui8 T2)
{
	m_linkPara.T2 = T2;
}
void IEC104_API::setT3(b_ui32 T3)
{
	m_linkPara.T3 = T3;
}
void IEC104_API::setT4(b_i32 T4)
{
	m_linkPara.T4 = T4;
}
void IEC104_API::setK(b_ui8 k)
{
	m_linkPara.K = k;
}
void IEC104_API::setW(b_ui8 w)
{
	m_linkPara.W = w;
}
void IEC104_API::setCotLen(b_ui8 CotLen)
{
	m_AppPara.CotLen = CotLen;
}
void IEC104_API::setRtuAddr(b_ui16 rtuAddr)
{
	m_AppPara.rtuAddr = rtuAddr;
}
void IEC104_API::setAsduAddrLen(b_ui8 AsduAddrLen)
{
	m_AppPara.AsduAddrLen = AsduAddrLen;
}
void IEC104_API::setInfoAddrLen(b_ui8 InfoAddrLen)
{
	m_AppPara.InfAddrLen = InfoAddrLen;
}

int IEC104_API::addPScadaLink(int groupId,const std::string strIpAddr,int port)
{
	int i=m_linkPara.hostNum;
	if(i==4)
		return -1;
	m_linkPara.m_hostPara[i].hostAddr= strIpAddr;
	m_linkPara.m_hostPara[i].port = port;
	m_linkPara.m_hostPara[i].groupId = groupId;
	m_linkPara.hostNum++;
	return 0;
}

 void IEC104_API::init()
{
	CMutex::Init();
	if(m_api_mutex)
		delete m_api_mutex;
	 m_api_mutex = new esca::CMutex(false);
}

 void IEC104_API::resetLink()
{
	ade_->resetLink();
}

void IEC104_API::stop()
{
	ade_->killALLConnectThread();
	this->Kill();
}
void IEC104_API::cleanUp()
{
	CMutex::CleanUp();
	if(m_api_mutex)
		delete m_api_mutex;
}
 

#ifdef WIN32
	DWORD IEC104_API::Main()
#else
	b_ui64 IEC104_API::Main()
#endif
{
	if((ade_->start_main(handler_)) == 0)
		return 0;
	return -1;
}

void IEC104_API::putCommand(const CCommand &cmd)
{
	esca::CLock lock(&m_mngMutex);
	cmdMng_->putCommandToQueue(cmd);
}

int IEC104_API::getCommand(CCommand &cmd)
{

	esca::CLock lock(&m_mngMutex);
	return cmdMng_->getNextCommand(cmd);
}


int IEC104_API::singleCommandSelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 Scs,b_ui8 QU)
{
	if(Scs > 1)
		return -3;
	if(QU>31)
		return -4;
	CCommand cmd;
	cmd.create_SingleCommand(iRtuAddr,iInfoAddr, Scs,CCommand::SELECT,b_false, QU);
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::singleCommandExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 Scs, b_ui8 QU)
{
	if(Scs> 1)
		return -3;
	if(QU>31)
		return -4;
	CCommand cmd;
	cmd.create_SingleCommand(iRtuAddr,iInfoAddr, Scs,CCommand::EXECUTE, b_false, QU);
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::singleCommandUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 Scs,b_ui8 QU)
{
	if(Scs > 1)
		return -3;
	if(QU>31)
		return -4;
	CCommand cmd;
	cmd.create_SingleCommand(iRtuAddr,iInfoAddr, Scs,CCommand::CANCEL, b_false, QU);
	IEC104_API::putCommand( cmd);
	return 0;
}
/*
*clock synchronization command
*/
int IEC104_API::syncTime(b_ui16 iRtuAddr)
{
	CCommand cmd;
	cmd.create_Time_Syn();
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::call_file_dir(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 secName)
{
	CCommand cmd;
	cmd.create_File_Call(rtuAddr, infoAddr, CCommand::CALL_DIR, fileName, secName);
	IEC104_API::putCommand( cmd);
	return 1;
}
/*
*File transfer,call file
*122
*iRtuAddr :rtu address
*iInfoAddr: information objects address
*fileName :file name
*secName :section name
*/
int IEC104_API::call_file(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 secName)
{
	CCommand cmd;
	cmd.create_File_Call(rtuAddr, infoAddr, CCommand::CALL_FILE, fileName, secName);
	IEC104_API::putCommand( cmd);
	return 1;
}
/*
*File transfer,call section
*122
*iRtuAddr :rtu address
*iInfoAddr :informaiton objects address
*fileName :file name
*secName :section name 
*/
int IEC104_API::call_file_section(int rtuAddr,int infoAddr, b_ui16 fileName, b_ui8 secName)
{
	CCommand cmd;
	cmd.create_File_Call(rtuAddr, infoAddr, CCommand::CALL_SECTION, fileName, secName);
	IEC104_API::putCommand( cmd);
	return 1;
}
/*
*File transfer,selectting file
*122
*iRtuAddr :rtu address
*iInfoAddr :informaiton objects address
*fileName :file name
*secName :section name 
*/
int IEC104_API::select_file(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 secName)
{
	CCommand cmd;
	cmd.create_File_Call(rtuAddr, infoAddr, CCommand::SELECT_FILE, fileName, secName);
	IEC104_API::putCommand( cmd);
	return 1;
}
/*
*File transfer,selctting section
*122
*iRtuAddr :rtu address
*iInfoAddr :informaiton objects address
*fileName :file name
*secName :section name 
*/
int IEC104_API::select_file_section(int rtuAddr, int infoAddr, b_ui16 fileName, b_ui8 secName)
{
	CCommand cmd;
	cmd.create_File_Call(rtuAddr, infoAddr, CCommand::SELECT_SECTION, fileName, secName);
	IEC104_API::putCommand( cmd);
	return 1;
}
/*
*File transfer,ack file, ack section
*124
*iRtuAddr :rtu address
*iInfoAddr :informaiton objects address
*fileName :file name
*secName :section name 
affirmFlag =CCommand::ACK_FILE:ack file
affirmFlag =CCommand::NACK_FILE:unack file
affirmFlag =CCommand::ACK_SECTION:ack section
affirmFlag =CCommand::NACK_FILE:unack section
*/

int IEC104_API::affirm_file(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 secName,b_ui8 affirmFlag)
{
	CCommand cmd;
	cmd.create_File_Affirm(rtuAddr, infoAddr, fileName, secName, affirmFlag);
	IEC104_API::putCommand( cmd);
	return 1;
}

//=========================103 extension--------------//

/*
* Call fix value
*  145
*  rtuAddr:rtu address
*  protectDevNo:the total sequence of protect equipment
*  areaNo = 0xfe  £¬call current area£»areaNo = call the fix area
*/
int IEC104_API::call_value(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo)
{
	CCommand cmd;
	cmd.create_Call_Value(rtuAddr, protectDevNo,  areaNo);
	IEC104_API::putCommand( cmd);
	return 1;
}

/*
* Call current area number of current value
*  145
*  rtuAddr :rtu address
*  protectDevNo: the total sequence number of protect equipment
*/
int IEC104_API::call_value_area_no(int rtuAddr, b_ui8 protectDevNo)
{
	CCommand cmd;
	cmd.create_Call_Value_Area_No(rtuAddr,  protectDevNo);
	IEC104_API::putCommand( cmd);
	return 1;
}

/*
* Selction,execution and undoing of changing area number of fix value
* 147
* rtuAddr rtu address
*  protectDevNo :the total sequence number of protect equipment
*  areaNo :Change the area number to a fix number
*/
int IEC104_API::modify_value_are_no_select(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo)
{
	CCommand cmd;
	cmd.create_Modify_Value_Area_No(rtuAddr, protectDevNo,areaNo, CCommand::SELECT);
	IEC104_API::putCommand( cmd);
	return 1;
}
int IEC104_API::modify_value_area_no_exec(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo)
{
	CCommand cmd;
	cmd.create_Modify_Value_Area_No(rtuAddr, protectDevNo,areaNo,CCommand::EXECUTE);
	IEC104_API::putCommand( cmd);
	return 1;
}
int IEC104_API::modify_value_area_no_cancle(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo)
{
	CCommand cmd;
	cmd.create_Modify_Value_Area_No(rtuAddr, protectDevNo,areaNo,CCommand::CANCEL);
	IEC104_API::putCommand( cmd);
	return 1;
}
/*
* Selction,execution and undoing of changing fix value
 * 147
 * rtuAddr rtu address
* protectDevNo :the total sequence number of protect equipment
* areaNo 
* fBuf :arrary for storing the fix value
*count :the number of array
*/
int IEC104_API::modify_value_select(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo,float *fBuf, int count)
{
	CCommand cmd;
	cmd.create_Modify_Value( rtuAddr,  protectDevNo,  areaNo, CCommand::SELECT, fBuf, count, 0);
	IEC104_API::putCommand( cmd);
	return 1;
}
int IEC104_API::modify_value_exec(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo)
{
	CCommand cmd;
	cmd.create_Modify_Value(rtuAddr, protectDevNo, areaNo, CCommand::EXECUTE, 0, 0, 0, 0);
	IEC104_API::putCommand( cmd);
	return 1;
}
int IEC104_API::modify_value_cancle(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo)
{
	CCommand cmd;
	cmd.create_Modify_Value(rtuAddr, protectDevNo, areaNo, CCommand::CANCEL, 0, 0, 0, 0);
	IEC104_API::putCommand( cmd);
	return 1;
}
//-------------------------- not belong to GuangFo---------------------------

int IEC104_API::doubleCommandSelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_ui8 bQu)
{
	if(bDcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_DoubleCommand(iRtuAddr,iInfoAddr,bDcs,CCommand::SELECT,b_false,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::doubleCommandExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_ui8 bQu)
{
	if(bDcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_DoubleCommand(iRtuAddr,iInfoAddr,bDcs,CCommand::EXECUTE,b_false,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::doubleCommandUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_ui8 bQu)
{
	if(bDcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_DoubleCommand(iRtuAddr,iInfoAddr,bDcs,CCommand::CANCEL,b_false,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}

//regulating step command
int IEC104_API::stepCommandSelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_ui8 bQu)
{
	if(bRcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_StepCommand(iRtuAddr,iInfoAddr,bRcs,CCommand::SELECT,b_false,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::stepCommandExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_ui8 bQu)
{
	if(bRcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_StepCommand(iRtuAddr,iInfoAddr,bRcs,CCommand::EXECUTE,b_false,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::stepCommandUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_ui8 bQu)
{
	if(bRcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_StepCommand(iRtuAddr,iInfoAddr,bRcs,CCommand::CANCEL,b_false,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}

//set point command, normalized value
int IEC104_API::setPointNVASelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointNva(iRtuAddr, iInfoAddr,Nva, CCommand::SELECT, b_false, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointNVAExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointNva(iRtuAddr, iInfoAddr,Nva, CCommand::EXECUTE, b_false, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointNVAUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointNva(iRtuAddr, iInfoAddr,Nva, CCommand::CANCEL, b_false, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}

//set point command, scaled value
int IEC104_API::setPointSVASelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_ui8 bQl)

{
	if( bQl>127)
		return -5;
	CCommand cmd;
	//cmd.create_SV(iRtuAddr, iInfoAddr,(b_ui8*)&Value,1,CCommand::SELECT,tmflag);
	cmd.create_SetPointSVA(iRtuAddr,iInfoAddr,Sva,CCommand::SELECT, b_false, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointSVAExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSVA(iRtuAddr,iInfoAddr,Sva,CCommand::EXECUTE, b_false, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointSVAUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_ui8 bQl)
{	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSVA(iRtuAddr,iInfoAddr,Sva,CCommand::CANCEL, b_false, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}

//set point command, short floating point number
int IEC104_API::setPointSFASelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSFA(iRtuAddr, iInfoAddr, Sfa,CCommand::SELECT,  b_false, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointSFAExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSFA(iRtuAddr, iInfoAddr, Sfa,CCommand::EXECUTE,  b_false, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointSFAUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSFA(iRtuAddr, iInfoAddr, Sfa,CCommand::CANCEL,  b_false, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::singleCommandWithTimeTagSelect(b_ui16 iRtuAddr, b_ui32 iInfoAddr, b_ui8 Scs, b_ui8 QU)
{
	if(Scs > 1)
		return -3;
	if(QU>31)
		return -4;
	CCommand cmd;
	cmd.create_SingleCommand(iRtuAddr,iInfoAddr, Scs,CCommand::SELECT,b_true, QU);
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::singleCommandWithTimeTagExec(b_ui16 iRtuAddr, b_ui32 iInfoAddr, b_ui8 Scs, b_ui8 QU)
{
	if(Scs> 1)
		return -3;
	if(QU>31)
		return -4;
	CCommand cmd;
	cmd.create_SingleCommand(iRtuAddr,iInfoAddr, Scs,CCommand::EXECUTE, b_true, QU);
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::singleCommandWithTimeTagUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 Scs,b_ui8 QU)
{
	if(Scs > 1)
		return -3;
	if(QU>31)
		return -4;
	CCommand cmd;
	cmd.create_SingleCommand(iRtuAddr,iInfoAddr, Scs,CCommand::CANCEL, b_true, QU);
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::doubleCommandWithTimeTagSelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_ui8 bQu)
{
	if(bDcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_DoubleCommand(iRtuAddr,iInfoAddr,bDcs,CCommand::SELECT,b_true,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::doubleCommandWithTimeTagExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_ui8 bQu)
{
	if(bDcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_DoubleCommand(iRtuAddr,iInfoAddr,bDcs,CCommand::EXECUTE,b_true,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::doubleCommandWithTimeTagUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_ui8 bQu)
{
	if(bDcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_DoubleCommand(iRtuAddr,iInfoAddr,bDcs,CCommand::CANCEL,b_true,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}

//regulating step command
int IEC104_API::stepCommandWithTimeTagSelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_ui8 bQu)
{
	if(bRcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_StepCommand(iRtuAddr,iInfoAddr,bRcs,CCommand::SELECT,b_true,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::stepCommandWithTimeTagExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_ui8 bQu)
{
	if(bRcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_StepCommand(iRtuAddr,iInfoAddr,bRcs,CCommand::EXECUTE,b_true,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::stepCommandWithTimeTagUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_ui8 bQu)
{
	if(bRcs>3)
		return -3;
	if(bQu>31)
		return -5;
	CCommand cmd;
	cmd.create_StepCommand(iRtuAddr,iInfoAddr,bRcs,CCommand::CANCEL,b_true,bQu);
	IEC104_API::putCommand( cmd);
	return 0;
}

//set point command, normalized value
int IEC104_API::setPointNVAWithTimeTagSelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointNva(iRtuAddr, iInfoAddr,Nva, CCommand::SELECT, b_true, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointNVAWithTimeTagExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointNva(iRtuAddr, iInfoAddr,Nva, CCommand::EXECUTE, b_true, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointNVAWithTimeTagUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointNva(iRtuAddr, iInfoAddr,Nva, CCommand::CANCEL, b_true, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}

//set point command, scaled value
int IEC104_API::setPointSVAWithTimeTagSelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	//cmd.create_SV(iRtuAddr, iInfoAddr,(b_ui8*)&Value,1,CCommand::SELECT,tmflag);
	cmd.create_SetPointSVA(iRtuAddr,iInfoAddr,Sva,CCommand::SELECT, b_true, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointSVAWithTimeTagExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSVA(iRtuAddr,iInfoAddr,Sva,CCommand::EXECUTE, b_true, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointSVAWithTimeTagUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_ui8 bQl)
{	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSVA(iRtuAddr,iInfoAddr,Sva,CCommand::CANCEL, b_true, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}

//set point command, short floating point number
int IEC104_API::setPointSFAWithTimeTagSelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSFA(iRtuAddr, iInfoAddr, Sfa,CCommand::SELECT,  b_true, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointSFAWithTimeTagExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSFA(iRtuAddr, iInfoAddr, Sfa,CCommand::EXECUTE,  b_true, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::setPointSFAWithTimeTagUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_ui8 bQl)
{
	if( bQl>127)
		return -5;
	CCommand cmd;
	cmd.create_SetPointSFA(iRtuAddr, iInfoAddr, Sfa,CCommand::CANCEL,  b_true, bQl);
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::bitstring32Command(b_ui16 iRtuAddr,b_ui8 bCot,b_ui32 iInfoAddr,b_ui32 bitstring32)
{
	if(!((bCot==6)||(bCot==8) ))
		return -2;
	CCommand cmd;
	cmd.create_Bitstirng32(iRtuAddr,bCot, iInfoAddr, bitstring32, b_false);
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::bitstring32CommandWithTimeTag(b_ui16 iRtuAddr,b_ui8 bCot,b_ui32 iInfoAddr,b_ui32 bitstring32)
{
	if(!((bCot==6)||(bCot==8) ))
		return -2;
	CCommand cmd;
	cmd.create_Bitstirng32(iRtuAddr, bCot,iInfoAddr, bitstring32, b_true);
	IEC104_API::putCommand( cmd);
	return 0;
}

//============================================================
int IEC104_API::interrogateCounter(b_ui16 iRtuAddr,b_ui8 bCot, b_ui8 bRqt,b_ui8 bFrz)
{	
	if(!((bCot==6)||(bCot==8) ))
		return -2;
	if((bRqt==0 )|| (bRqt>63))
		return -3;
	if(bFrz>3)
		return -4;		
	CCommand cmd;
	cmd.create_Call_Kwh( iRtuAddr,bCot,bRqt, bFrz);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::interrogateGeneral(b_ui16 iRtuAddr,b_ui8 bCot , b_ui8 bQoi)
{
	if(!((bCot==6)||(bCot==8) ))
		return -2;
	if(bQoi==0)
		return -3;
	CCommand cmd;
	cmd.create_Call_All_Data(iRtuAddr, bCot,bQoi);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::readCommand(b_ui16 iRtuAddr,b_ui32 iInfoAddr)
{
	CCommand cmd;
	cmd.create_read_command( iRtuAddr,  iInfoAddr);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::resetProcess(b_ui16 iRtuAddr,b_ui8 bQrp)
{
	if(bQrp==0)
		return -2;
	CCommand cmd;
	cmd.create_reset_process(iRtuAddr,bQrp);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::testWithTimeTag(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 bTsc)
{
	CCommand cmd;
	cmd.create_test_with_time(iRtuAddr,iInfoAddr,bTsc);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::parameterMeasureNVA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 nva,b_ui8 bQpm)
{
	CCommand cmd;
	cmd.create_parameter_measured_value(iRtuAddr,iInfoAddr, (b_ui8*)&nva, bQpm,0);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::parameterMeasureSVA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 sva,b_ui8 bQpm)
{
	CCommand cmd;
	cmd.create_parameter_measured_value(iRtuAddr,iInfoAddr, (b_ui8*)&sva,bQpm, 1);
	IEC104_API::putCommand( cmd);
	return 0;
}
int IEC104_API::parameterMeasureSFA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float sfa,b_ui8 bQpm)
{
	CCommand cmd;
	cmd.create_parameter_measured_value(iRtuAddr,iInfoAddr, (b_ui8*)&sfa,bQpm, 2);
	IEC104_API::putCommand( cmd);
	return 0;
}

int IEC104_API::parameterActive(b_ui16  rtuAddr,b_ui8 bCot,b_ui32 iInfoAddr,b_ui8 bQpa)
{
	if(!((bCot==6)||(bCot==8) ))
		return -2;
	CCommand cmd;
	cmd.create_parameter_activation(rtuAddr,bCot,iInfoAddr,bQpa);
	IEC104_API::putCommand(cmd);
	return 0;
}
//--------------------------end---------------------------

