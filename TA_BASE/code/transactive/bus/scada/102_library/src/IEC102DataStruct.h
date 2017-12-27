#ifndef IEC102_DATA_STRUCT_H
#define IEC102_DATA_STRUCT_H

#include <string>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include <ctime>
#include "core/timers/src/TimedWaitSemaphore.h"

namespace TA_Base_Bus
{
	static const unsigned char  FRAME_102ASDU_MAX_SIZE 				= 0xFF;
	static const unsigned char  FRAME_FIX_FROMAT_LENGTH 			= 0x06;
    static const unsigned char  FRAM_SINGLE_FORMAT_LENGTH 			= 0X01;
    static const unsigned char  CONTROL_FIELD_LENGTH	 			= 0X01;

    static const unsigned int INTEGRATED_TOTAL_WITH_IOA_SIZE   	= 7;
    static const unsigned int INTEGRATED_TOTAL_NO_IOA_SIZE     	= 6;

    static const unsigned int MEASUREMENT_WITH_IOA_SIZE   		= 6;
    static const unsigned int MEASUREMENT_NO_IOA_SIZE     		= 5;

    static const unsigned int QUERY_WITH_ADDR_TIMEA_SIZE   		= 12;
    static const unsigned int QUERY_WITH_ADDR_SIZE		   		= 2;

    static const unsigned int INIT_COMPLETED_SIZE		   		= 2;

    static const unsigned int CONTROL_FIELD_SIZE	 			= 1;
    static const unsigned int TIMEA_SIZE   						= 5;
    static const unsigned int TIMEB_SIZE   						= 7;

	static const unsigned int ASDU_MIN_SIZE 					= 6;
	static const unsigned int FRAME_VARIABLE_FORMAT_MIN_SIZE 	= 15;
	static const unsigned int FRAME_VARIABLE_FORMAT_MAX_SIZE 	= 255;	
	static const unsigned int FRAME_FIX_FROMAT_SIZE 			= 6;
    static const unsigned int FRAME_SINGLE_FORMAT_SIZE	 		= 1;

	static const unsigned char FRAME_START_CHAR_10H = 0x10;
	static const unsigned char FRAME_START_CHAR_68H = 0x68;
	static const unsigned char FRAME_END_CHAR_16H	= 0x16;
	static const unsigned char FRAME_SINGLE_CHAR    = 0xE5;
	
	static const unsigned char FUN_CODE_0   = 0x00;  	// reset communication unit Fixed frame
	static const unsigned char FUN_CODE_1   = 0x01;  
	static const unsigned char FUN_CODE_2   = 0x02;  
	static const unsigned char FUN_CODE_3   = 0x03;  	// request data 			 Variable frame
	static const unsigned char FUN_CODE_4   = 0x04;  
	static const unsigned char FUN_CODE_5   = 0x05;  
	static const unsigned char FUN_CODE_6   = 0x06; 
	static const unsigned char FUN_CODE_7   = 0x07; 
	static const unsigned char FUN_CODE_8   = 0x08;  
	static const unsigned char FUN_CODE_9   = 0x09;  	// request state of link	 Fixed frame
	static const unsigned char FUN_CODE_10  = 0x0A; 	// request level I data 	 Fixed frame
	static const unsigned char FUN_CODE_11  = 0x0B;  	// response link state is good			Fixed frame

	// cause of transmission
	static const unsigned char COT_3  = 0x03;
	static const unsigned char COT_4  = 0x04; //initialization
	static const unsigned char COT_5  = 0x05; //request or be requested
	static const unsigned char COT_6  = 0x06; //activate
	static const unsigned char COT_7  = 0x07; //activate confirm
	static const unsigned char COT_8  = 0x08; //stop activate 
	static const unsigned char COT_9  = 0x09; //stop activate confirm
	static const unsigned char COT_10 = 0x0A; //activate terminate
	static const unsigned char COT_13 = 0x0D; //no requested record
	static const unsigned char COT_14 = 0x0E; //no asdu type of requested
	static const unsigned char COT_15 = 0x0F; //由主站发送的应用数据单元中的记录地址不可知
	static const unsigned char COT_16 = 0x10; //由主站发送的应用数据单元的地址规范不可知
	static const unsigned char COT_17 = 0x11; //无所请求的信息体
	static const unsigned char COT_18 = 0x12; //无所请求的累积时段
	static const unsigned char COT_48 = 0x30; // synchronize time with slave

	// Record address
	static const unsigned char RAD_0  = 0x00; //default
	static const unsigned char RAD_1  = 0x01; //从记账（计费）时段开始的电能累计量的记录地址
	static const unsigned char RAD_11 = 0xB;  //电能累计量累计时段1的记录地址 
	static const unsigned char RAD_12 = 0xC;  //电能累计量累计时段2的记录地址
	static const unsigned char RAD_13 = 0xD;  //电能累计量累计时段3的记录地址
	static const unsigned char RAD_21 = 0x15; //电能累计量（日值）累计时段1的记录地址 
	static const unsigned char RAD_22 = 0x16; //电能累计量（日值）累计时段2的记录地址
	static const unsigned char RAD_23 = 0x17; //电能累计量（日值）累计时段3的记录地址
	static const unsigned char RAD_31 = 0x1F; //电能累计量（周/旬值）累计时段1的记录地址
	static const unsigned char RAD_32 = 0x20; //电能累计量（周/旬值）累计时段2的记录地址
	static const unsigned char RAD_33 = 0x21; //电能累计量（周/旬值）累计时段3的记录地址
	static const unsigned char RAD_41 = 0x29; //电能累计量（月值）累计时段1的记录地址
	static const unsigned char RAD_42 = 0x2A; //电能累计量（月值）累计时段2的记录地址
	static const unsigned char RAD_43 = 0x2B; //电能累计量（月值）累计时段3的记录地址
	static const unsigned char RAD_50 = 0x32; //最早的单点信息
	static const unsigned char RAD_51 = 0x33; //单点信息的全部记录
	static const unsigned char RAD_52 = 0x34; //单点信息记录区段1
	static const unsigned char RAD_53 = 0x35; //单点信息记录区段2
	static const unsigned char RAD_54 = 0x36; //单点信息记录区段3
	static const unsigned char RAD_55 = 0x37; //单点信息记录区段4

	//define ASDU type id
	//C_XXX is control direction 
	//M_XXX is monitor direction
	static const unsigned char C_CI_NA_B_2	=	0xAA; //读一个指定的过去累计时段和一个选定的地址范围的复费率记帐(计费)电能累计量
	static const unsigned char M_IT_TA_B_2	=	0xA0; //复费率记帐(计费)电能累计量
	static const unsigned char C_YC_NA_2	=	0xAB; //读指定地址范围的遥测当前值
	static const unsigned char C_YC_TA_2    =	0xAC; //读一个指定的过去时段和一个选定的地址范围的遥测量C_YC_TA_2
	static const unsigned char M_YC_NA_2	=	0xA1; //遥测当前值
	static const unsigned char M_YC_TA_2	=   0xA2; //遥测历史值
	static const unsigned char C_XL_NA_2	=   0xAD; //读指定地址范围的当前最大需量
	static const unsigned char C_XL_NB_2	=   0xAE; //读指定地址范围的上月最大需量
	static const unsigned char M_XL_TA_2    =   0xA3; //最大需量
	static const unsigned char C_SP_NA_2    =	0x65; //读带时间戳的单点信息
	static const unsigned char C_SP_NB_2    =   0x66; //读指定时间带时间戳的单点信息
	static const unsigned char M_SP_TA_2	=	0x01;  //带时间戳的单点信息
	static const unsigned char C_TI_NA_2	=   0x67; //读电能累计量数据终端设备的当前系统时间
	static const unsigned char M_TI_TA_2	=	0x48; //电能累计量数据终端设备的当前系统时间
	static const unsigned char C_RD_NA_2	=   0x64; //读制造厂和产品规范
	static const unsigned char P_MP_NA_2	=	0x47; //电能累计量数据终端设备的制造厂和产品规范
	static const unsigned char M_EI_NA_2	=	0x46; //初始化结束 
	static const unsigned char C_CI_NC_2	=	0x6A; //读一个指定的过去累计时段的记帐（计费）电能累计量
	static const unsigned char C_CI_ND_2	=   0x6B; //读一个指定的过去累计时段和一个选定的地址范围的记帐（计费）电能累计量
	static const unsigned char C_CI_NR_2	=   0x78; //读一个选定的时间范围和一个选定的地址范围的记帐(计费)电能累计量	
	static const unsigned char M_IT_TA_2	=   0x02;  //记帐(计费)电能累计量，每个量为四个八位位组
	static const unsigned char C_CI_NG_2	=   0x6E; //读一个指定的过去累计时段的周期地复位的记帐（计费）电能累计量
	static const unsigned char C_CI_NH_2	=   0x6F; //读一个指定的过去累计时段和一个选定的地址范围的周期地复位的记帐（计费）电能累计量
	static const unsigned char C_CI_NS_2	=   0x79; //读周期地复位的一个选定的时间范围和一个选定的地址范围的记帐(计费)电能累计
	static const unsigned char M_IT_TD_2	=	0x05;  //周期复位记帐(计费)电能累计量，每个量为四个八位位组
	static const unsigned char C_SYN_TA_2	=   0x80; //电能累计量数据终端系统时间同步命令
	static const unsigned char M_SYN_TA_2	=   0x80; //电能累计量数据终端系统时间同步确认
	static const unsigned char C_MRD_NA_2 	= 	0xD3; //211
	static const unsigned char M_MRD_NA_2 	= 	0x8D; //141
	static const unsigned char C_LOGIN_NA_2 = 	0xDF; //223
	static const unsigned char M_LOGIN_NA_2 = 	0x99; //153, positive response for login
	static const unsigned char C_TRD_NA_2 	= 	0xCC; //204
	static const unsigned char M_TRD_NA_2 	= 	0x86; //134, positive response for cmd 204

    //TimeA Index
    static const unsigned int TIMEA_MIN    = 0;
    static const unsigned int TIMEA_HOUR   = 1;
    static const unsigned int TIMEA_DAY    = 2;
    static const unsigned int TIMEA_WEEK   = 2;
    static const unsigned int TIMEA_MONTH  = 3;
    static const unsigned int TIMEA_YEAR   = 4;
    
    //TimeB Index
    static const unsigned int TIMEB_MILSEC = 0;
    static const unsigned int TIMEB_SEC    = 1;
    static const unsigned int TIMEB_MIN    = 2;
    static const unsigned int TIMEB_HOUR   = 3;
    static const unsigned int TIMEB_DAY    = 4;
    static const unsigned int TIMEB_WEEK   = 4;
    static const unsigned int TIMEB_MONTH  = 5;
    static const unsigned int TIMEB_YEAR   = 6;
	
    //EnergyData Index
    static const unsigned int ENGYDATA_IOA          = 0;
    static const unsigned int ENGYDATA_POWER_LL     = 1;
    static const unsigned int ENGYDATA_POWER_LH     = 2;
    static const unsigned int ENGYDATA_POWER_HL     = 3;    
    static const unsigned int ENGYDATA_POWER_HH     = 4;
    static const unsigned int ENGYDATA_STATUS       = 5;
    static const unsigned int ENGYDATA_FMCNT        = 5;
    static const unsigned int ENGYDATA_CS           = 6;

    //Remote Measurement Index
    static const unsigned int REMOTE_MEASSURE_IOA           = 0;
    static const unsigned int REMOTE_MEASSURE_DATA_LL      = 1;
    static const unsigned int REMOTE_MEASSURE_DATA_LH      = 2;
    static const unsigned int REMOTE_MEASSURE_DATA_HL      = 3;
    static const unsigned int REMOTE_MEASSURE_DATA_HH      = 4;
    static const unsigned int REMOTE_MEASSURE_FRAME_COUNT   = 5;

    //Query Energy 120 Index
    static const unsigned int QUERY_START_ADDR    		= 0;
    static const unsigned int QUERY_END_ADDR      		= 1;
    static const unsigned int QUERY_START_TIMEA_BEGIN   = 2;
    static const unsigned int QUERY_START_TIMEA_END   	= 6;
    static const unsigned int QUERY_END_TIMEA_BEGIN     = 7;
    static const unsigned int QUERY_END_TIMEA_END     	= 11;

    //Query Energy 120 Index
    static const unsigned int INIT_COMPLETED_IOA   		= 0;
    static const unsigned int INIT_COMPLETED_COI   		= 0;
    
	//Single Frame Index
    static const unsigned int SINGLE_FRAME_START      = 0;

	//Fixed Frame Index
    static const unsigned int FIXED_FRAME_HEADER      = 0;
    static const unsigned int FIXED_FRAME_CONFIELD    = 1;
    static const unsigned int FIXED_FRAME_ADDRL       = 2;
    static const unsigned int FIXED_FRAME_ADDRH       = 3;
    static const unsigned int FIXED_FRAME_CHECKSUM    = 4;
    static const unsigned int FIXED_FRAME_TRAILER     = 5;

	//ASDU fixed part index
    static const unsigned int ASDU_TYPE_IDENTIFIER     = 0;
    static const unsigned int ASDU_VSQ				   = 1;
    static const unsigned int ASDU_COT			       = 2;
    static const unsigned int ASDU_ADDR_LOW		       = 3;
    static const unsigned int ASDU_ADDR_HIGH           = 4;
    static const unsigned int ASDU_RAD			       = 5;
    static const unsigned int ASDU_IOA			       = 6;
    static const unsigned int ASDU_BODY_START	       = 6;

    //Variable Frame Index
    static const unsigned int VAR_FRAME_HEADER_1      = 0;
    static const unsigned int VAR_FRAME_SIZE_1        = 1;
    static const unsigned int VAR_FRAME_SIZE_2        = 2;
    static const unsigned int VAR_FRAME_HEADER_2      = 3;
    static const unsigned int VAR_FRAME_CONFIELD      = 4;
    static const unsigned int VAR_FRAME_ADDRL         = 5;
    static const unsigned int VAR_FRAME_ADDRH         = 6;
    static const unsigned int VAR_FRAME_ASDU_START    = 7;
    static const unsigned int VAR_FRAME_ASDU_END_BACKWARDS    = 3;
    static const unsigned int VAR_FRAME_CHECKSUM_BACKWARDS    = 2; // second last
    static const unsigned int VAR_FRAME_TRAILER_BACKWARDS     = 1; // last one
	

    //Print Width Configuration
    static const std::string PRINT_CONFIG = "%30s%-22s"; // title left aligned 30, value right aligned 50
 	static const std::string PRINT_NAME_CONFIG = "%30s"; // title left aligned 30, value right aligned 50
 	static const std::string NAME_FORMAT_FOOTER = " :  "; // title left aligned 30, value right aligned 50

 	static const std::string NAME_CONTROL_FIELD 		= "Control Field"; 
 	static const std::string NAME_FIXED_FRAME_HEADER 	= "Fixed Frame Header"; 
 	static const std::string NAME_VAR_FRAME_HEADER 		= "Variable Frame Header"; 
 	static const std::string NAME_EQUIPMENT_ADDRESS 	= "Equipment Address"; 
 	static const std::string NAME_FIXED_FRAME_CHECKSUM 		= "Fixed Frame CheckSum"; 
 	static const std::string NAME_VAR_FRAME_CHECKSUM 		= "Variable Frame CheckSum"; 
	static const std::string NAME_FIXED_FRAME_TRAILER 	= "Fixed Frame Trailer";
	static const std::string NAME_VAR_FRAME_TRAILER 	= "Variable Frame Trailer";
	
	static const std::string NAME_UNKNOWN_OBJECT		= "Unknown Information Object";
	static const std::string NAME_IT_INFORMATION_OBJECT	= "Integarated Total Object";
	static const std::string NAME_RM_INFORMATION_OBJECT = "Remote Measurement Object";

	static const std::string NAME_QUERY_ADDRESS			= "Query Address";

	static const std::string NAME_QUERY_WITH_ADDR_TIMEA = "Addr and Time Query Object";
	static const std::string NAME_QUERY_WITH_ADDR		= "Addr Query Object";

	static const std::string NAME_INIT_COMPLETED		= "Init Completed Object";
	static const std::string NAME_INIT_COMPLETED_IOA	= "Init Completed IOA";
	static const std::string NAME_INIT_COMPLETED_COI	= "Init Completed COI";
	
 	static const std::string NAME_TIMEA					= "TimeA"; 
 	static const std::string NAME_TIMEB					= "TimeB"; 
 	static const std::string NAME_SINGLE_FRAME			= "Single Frame"; 
 	static const std::string NAME_FIXED_FRAME			= "Fixed Frame"; 
 	static const std::string NAME_VAR_FRAME				= "Variable Frame"; 
 	static const std::string NAME_ASDU					= "ASDU"; 
 	static const std::string NAME_UNKNOWN_ASDU_BODY 	= "Unknown Asdu Body"; 
 	static const std::string NAME_BAD_CALC_ASDU_BODY 	= "Bad Calc Asdu Body"; 
 	static const std::string NAME_BAD_CALC_VAR_FARME 	= "Bad Calc Var Frame Asdu"; 

 	static const std::string NAME_TYPE_IDENTIFIER		= "Type Identification"; 
 	static const std::string NAME_VSQ					= "VSQ"; 
 	static const std::string NAME_COT					= "COT"; 
 	static const std::string NAME_VIRTUAL_ADDR			= "Virtual Address"; 
 	static const std::string NAME_RAD					= "RAD"; 


	static const unsigned short VIRTUAL_ADDRESS_TRANSIT_VALUE = 255;

	enum E_IEC102_DIRECTION
	{
		IEC102_MONITOR_DIRECTION = 0,	//0X00
		IEC102_CONTROL_DIRECTION = 64,	//0X40
	};

	enum E_IEC102_FCB
	{
		IEC102_FCB_TRUE  = 32,	//0X20
		IEC102_FCB_FALSE = 0,	//0X00
	};

	enum E_IEC102_FCV
	{
		IEC102_FCV_TRUE  = 16,	//0X10
		IEC102_FCV_FALSE = 0,	//0X00
	};

	enum E_IEC102_ACD
	{
		IEC102_ACD_TRUE  = 32,	//0X20
		IEC102_ACD_FALSE = 0,	//0X00	
	};

	enum E_IEC102_DFC
	{
		IEC102_DFC_TRUE  = 16,	//0X10
		IEC102_DFC_FALSE = 0,	//0X00
	};

	enum E_IEC102_STRUCT_TYPE
	{
		UNKNOWN_TYPE,
		CONTROL_FIELD_SEND_TYPE,	
		CONTROL_FIELD_RECV_TYPE,	
		SINGLE_FARME_TYPE,	
		FIXED_FRAME_TYPE,	
		VARIABLE_FRAME_TYPE, 
		TIMEA_TYPE,	
		TIMEB_TYPE, 
		ASDU_TYPE,
		INTEGREATED_TOTAL_WITH_ADDRESS_TYPE,	
		INTEGREATED_TOTAL_NO_ADDRESS_TYPE,	
		REMOTE_MEASUREMENT_WITH_ADDRESS_TYPE, 
		REMOTE_MEASUREMENT_NO_ADDRESS_TYPE,	
		QUERY_WITH_ADDR_TIMEA_TYPE, 
		QUERY_WITH_ADDR_TYPE,
		INIT_COMPLETED_TYPE,
	};

	struct IEC102Data
	{
		bool		  valid;
		unsigned int  address;
		int  		  dataValue;
		unsigned int  frameCnt;
		time_t 		  timeValue;
	};

	struct FlowResult
	{
		bool bSuccess;
		std::vector<IEC102Data> ResultData;
	};

	struct TimeAOld
	{
		unsigned char   year;
		unsigned char	month;
		unsigned char   week;
		unsigned char   day;
		unsigned char	hour;
		unsigned char	minute;
	};

	struct TimeBOld
	{
		unsigned char	year;
		unsigned char	month;
		unsigned char   day;
		unsigned char   week;
		unsigned char	hour;
		unsigned char	minute;
		unsigned char	second;
		unsigned short	millisecond;
	};

	struct SinglePointInfo
	{
		unsigned char spa;
		unsigned char spq;
		bool		  spi;
		struct TimeBOld  sTime;
	};

	struct EnergyData
	{
		int			  offset;
		unsigned long energy;
		unsigned char frameCnt;
		bool		  quality;
	};

	struct MeterEnergyData
	{
		unsigned char meterOrdinalNum;
		std::map<unsigned short, double> dataMap;
		TimeAOld meterReadTime;
	};
	
	struct PowerEnergyData
	{
		bool		 valid;
		unsigned int address;
		unsigned long PowerResult;  
		time_t dataTime;
	};
	
	struct FrameEnergyData
	{
		bool bSuccess;
		std::vector<PowerEnergyData> PowerData;
	};

	struct TimeShareEletricalEnergy
	{
		int			  offset;
		unsigned long totalEnergy;
		unsigned long tarrif1;
		unsigned long tarrif2;
		unsigned long tarrif3;
		unsigned long tarrif4;
		unsigned long tarrif5;
		unsigned char frameCnt;
		bool		  quality;
	};

	struct RemoteMeasurementData
	{
		int				offset;
		unsigned long	measurementData;
		bool			quality;
	};

	struct AsduSubLen
	{
		unsigned int 	ioaLen;
		unsigned int 	inforObjectLen;
		unsigned int	publicTimeLen;
		E_IEC102_STRUCT_TYPE inforObjectType;
	};


	enum E102FrameType
	{
		EUNKONWN_FROMAT = 0, // unknown format
		EV_FORMAT,		// frame format of variable length
		EF_FORMAT,     // frame format of fix length
		ES_FORMAT      // frame of E5H
	};

	enum EFlowControlState
	{
		E_M_UNKNOWN_CONTROL_STATE,	// UNKNOWN CONTROL STATE
		E_M_BUFFER_FULL,			// DFC = 1 BUFFER FULL, WILL WAIT AND RETRY
		E_M_ACTIVE_I_DATA_RESPONSE_LINK_STATE,			// ACD = 1 I DATA WILL BE PROVIDED, NEED TO REQUEST. ALSO RESPONSE LINK STATE
		E_M_RESPONSE_LINK_STATE,    // response for link state Funciton code = 0x0B
		E_M_ACTIVE_I_DATA_RESPONSE_RESET_LINK,			// ACD = 1 I DATA WILL BE PROVIDED, NEED TO REQUEST. ALSO RESPONSE LINK STATE
		E_M_RESPONSE_RESET_LINK,    // response for link state Funciton code = 0x00
		E_M_LINK_BUSY,				// link busy, will retry again. Function code = 0x01
		E_M_NO_DATA,				// No resquested data. Function code = 0x09
		E_M_INIT_COMPLETED,			// cot = 4
		E_M_ACTIVE_I_DATA,			// active data provided. 
		E_M_RESPONSE_DATA_COT4, 	// init over
		E_M_RESPONSE_DATA_COT5,
		E_M_RESPONSE_DATA_COT7, 	// Image frame confirm
		E_M_RESPONSE_DATA_COTA, 	// Image frame over
		E_M_RESPONSE_DATA_COTD, 	// no requested data
		E_M_0E5H_CON,	 // confirm and no data
		CONNECTION_LOST,	// Physical connection lost
		DATA_INVALID,		// Invalid data
		STOP_PROCESS,		// Stop Process
		WRITE_FAILED,			// Wait Fail
	};

	enum E102FlowStep
	{
		STEP_START 			= 1,  // Start Step
		STEP_FINISH			= 2,  // Finish Step
		STEP_ABORT			= 3,  // Abnormal abort Step
		STEP_CALL_I_DATA	= 4,  // CALL_I_DATA Step
		STEP_CALL_II_DATA	= 5,  // CALL_II_DATA Step

		STEP_INIT_LINK		= 6,  // Init Link Step
		STEP_RESET_LINK		= 7,  // Reset link Step

		STEP_REQ_120		= 8,  // Request 120 Data Step
		STEP_REQ_171		= 9,  // Request 171 Data Step
		STEP_REQ_172		= 10, // Request 172 Data Step
	};
	
    class Iec102Utility
    {
    public:
        static std::string uCharVecToStr( const std::vector<unsigned char>& vec );
        static std::string uCharToStr( const unsigned char uChar );
    };

    class Iec102FormatLog
	{
    public:
		Iec102FormatLog(){};
		~Iec102FormatLog(){};
		void logErrorInfo(const std::string& strDesc, const std::vector<unsigned char>& data );
        void logErrorLength( const std::string& strDesc, const std::vector<unsigned char>& data );
		
        void logParseLine( const std::string& strHeader,const std::vector<unsigned char>& data);
        void logParseLine( const std::string& strHeader,const std::vector<unsigned char>& data, const std::string& strExplain );
        void logParseLine( const std::string& strHeader,const std::string& data);
        void logParseLine( const std::string& strHeader,const std::string& data, const std::string& strExplain );
		void logParseLine( const std::string& strHeader,unsigned char char1, unsigned char char2,unsigned char char3, unsigned char char4);
		void logParseLine( const std::string& strHeader,unsigned char char1, unsigned char char2,unsigned char char3, unsigned char char4, const std::string& strExplain );
        void logParseLine( const std::string& strHeader,unsigned char char1, unsigned char char2 );
        void logParseLine( const std::string& strHeader,unsigned char char1, unsigned char char2, const std::string& strExplain );
        void logParseLine( const std::string& strHeader,unsigned char char1 );
       	void logParseLine( const std::string& strHeader,unsigned char char1, const std::string& strExplain );
       	void logAppend(const std::vector<std::string>& debugInfos);

		std::vector<std::string> getDebugInfos() {return m_debugInfos;};
		void printDebugInfos();
		
	private:
        Iec102FormatLog& operator=(const Iec102FormatLog& rhs);
        Iec102FormatLog(const Iec102FormatLog&);	
		void addDebugInfo(const std::string& strDebugInfo);

        std::stringstream   	 m_log;
		std::vector<std::string> m_debugInfos;
    };

    class Iec102StructInterface
    {
    public:
        //virtual ~Iec102StructInterface(){}; 
        virtual bool isValid( ) = 0;
        virtual std::vector<std::string> getDebugInfo() = 0;
        virtual std::string getRawData() = 0;
    };

    class Iec102FrameInterface
    {
    public:
        virtual bool isFrameValid() 	= 0;
  		virtual EFlowControlState getFlowControlState() = 0;
    };

	class Iec102Struct;
	class Iec102StructHelper
	{
	public:
		Iec102StructHelper(Iec102Struct* pIEC102Struct);
		virtual ~Iec102StructHelper() {}
		virtual void parseData(const std::vector<unsigned char>& data);
		
	private:
		Iec102Struct* m_pIEC102Struct;
	};


    class Iec102Struct : public Iec102StructInterface
    {
    public:
		Iec102Struct(std::string strName, E_IEC102_STRUCT_TYPE type,const std::vector<unsigned char>& data);
		Iec102Struct(std::string strName, E_IEC102_STRUCT_TYPE type);
		virtual ~Iec102Struct(){}
        virtual bool isValid() { return m_valid; }
        virtual std::vector<std::string> getDebugInfo() { return m_formatLog.getDebugInfos(); }
        virtual std::string getRawData(){ return convertRawData(); }
        virtual std::vector<IEC102Data>   getData() 
		{
			std::vector<IEC102Data> dataTemp;
			return dataTemp; 
		}

        virtual void printDebugInfo(){ m_formatLog.printDebugInfos(); }

		//Used for Iec102StructHelper
		E_IEC102_STRUCT_TYPE getStructType() { return m_type; }
		virtual unsigned int getSize() {return 0;}
		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper){}
		void logLengthError();


	protected:
        virtual void checkAndLog(){}
		Iec102FormatLog* getFormatLog() { return &m_formatLog; }
		std::vector<unsigned char>* getRawDataVector() { return &m_rawData; }
		void setValid(bool valid) {m_valid = valid;}

		//Used for derived class to set raw data.
		//Only the self constructor without passing data to use.
		void setRawData(const std::vector<unsigned char>& data) {m_rawData = data;}
		
	private:
		std::string convertRawData();
		
		std::vector<unsigned char>	m_rawData;
        bool                 		m_valid;
		Iec102FormatLog		 		m_formatLog;
		std::string 				m_name;
		E_IEC102_STRUCT_TYPE		m_type;
    };

    class ControlFieldSend : public Iec102Struct
    {
    public:
		ControlFieldSend( const std::vector<unsigned char>& data, E102FrameType frameType);
        ControlFieldSend( unsigned char data, E102FrameType frameType); // constructor for parsing
        ControlFieldSend( E_IEC102_FCB fcb, E_IEC102_FCV fcv, unsigned char funCode, E102FrameType frameType); // constructor for create controlfield
        ~ControlFieldSend(){}; 

		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);

		unsigned char  getContorlField(){return m_controlField;}
		virtual unsigned int getSize() {return CONTROL_FIELD_SIZE;}
		
    private:
        void  checkAndLog();
        
	    E_IEC102_DIRECTION   m_direction;
        unsigned char        m_funCode;
		unsigned char		 m_controlField;

		E_IEC102_FCB		 m_fcb;
		E_IEC102_FCV		 m_fcv;
		E102FrameType 		 m_frameType; 
		Iec102StructHelper	 m_helper;
    };
    
    class ControlFieldRecv : public Iec102Struct
    {
    public:
		ControlFieldRecv( const std::vector<unsigned char>& data, E102FrameType frameType);
        ControlFieldRecv( unsigned char data, E102FrameType frameType ); // constructor for parsing
        ControlFieldRecv( E_IEC102_ACD acd, E_IEC102_DFC dfc, unsigned char funCode, E102FrameType frameType ); // constructor for create controlfield
        ~ControlFieldRecv(){}; 

		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);

		unsigned char  getContorlField(){return m_controlField;}
		virtual unsigned int getSize() {return CONTROL_FIELD_SIZE;}

		E_IEC102_ACD getACD(){return m_acd;}
		E_IEC102_DFC getDFC(){return m_dfc;}
		unsigned char getFuncode(){return m_funCode;}
    private:
        void  checkAndLog();
        
	    E_IEC102_DIRECTION   m_direction;
        unsigned char        m_funCode;
		unsigned char		 m_controlField;
		
        E_IEC102_ACD         m_acd;
        E_IEC102_DFC         m_dfc;

		E102FrameType 		 m_frameType; 
		Iec102StructHelper	 m_helper;
    };

	class Iec102FixedLenFrame : public Iec102Struct, public Iec102FrameInterface
    {
    public:
		//construction for incoming FixedLenFrame; aim to verify and process
        Iec102FixedLenFrame( const std::vector<unsigned char>& data,unsigned char localLowAddr, unsigned char localHighAddr );
		//construction for outgoing FixedLenFrame; aim to verify and process
        Iec102FixedLenFrame(unsigned char localLowAddr, unsigned char localHighAddr, E_IEC102_FCB fcb, E_IEC102_FCV fcv, unsigned char funCode  );
        ~Iec102FixedLenFrame();

        virtual bool isFrameValid() {return isValid();}
 		virtual EFlowControlState getFlowControlState();
 
        Iec102Struct*   getControlField() { return m_pControlField; }
		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
		virtual unsigned int getSize() {return FRAME_FIX_FROMAT_SIZE;}
   
    private:
        void  setCheckSum(); 
        void checkAndLog();
		unsigned char	getControlFieldUC();
		void createRawData();

        unsigned char       m_header;
        unsigned char       m_controlField;
		
        Iec102Struct* m_pControlField;

		unsigned char       m_addrL;
        unsigned char       m_addrH;
        unsigned char       m_checkSum;
        unsigned char       m_trailer;
		
		unsigned char		m_localLowAddr;
		unsigned char		m_localHighAddr;
		E_IEC102_DIRECTION  m_direction;

        Iec102StructHelper  m_helper;

    };

    class TimeA : public Iec102Struct
	{
    public:
        TimeA( const std::vector<unsigned char>& data ); // design for parsing data
        TimeA( time_t timeValue );
        ~TimeA() {};

		time_t getTimeT() {return m_timeT; }
		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
		virtual unsigned int getSize() {return static_cast<int>(TIMEA_SIZE);}

		void checkAndLog();
		
    private:
		TimeA& operator=(const TimeA& rhs);
		TimeA(const TimeA&);	
		TimeA();
		void createRawData();
		std::string getTimeString();

        unsigned char       m_year;
		unsigned char	    m_month;
		unsigned char       m_week;
		unsigned char       m_day;
		unsigned char	    m_hour;
		unsigned char	    m_minute;	

		time_t				m_timeT;

		Iec102StructHelper	m_helper;
	};

	class TimeB : public Iec102Struct
	{
    public:
        TimeB( const std::vector<unsigned char>& data ); // design for parsing data
        TimeB( time_t timeValue );
        ~TimeB( ){};

		time_t getTimeT() {return m_timeT; }
		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
		virtual unsigned int getSize() {return static_cast<int>(TIMEB_SIZE);}

        void  checkAndLog( );

	protected:

    private:
        TimeB& operator=(const TimeB& rhs);
        TimeB(const TimeB&);		
		TimeB();
		void createRawData();
    
        unsigned short	m_millisecond;
		unsigned char	m_second;
        unsigned char   m_year;
		unsigned char	m_month;
		unsigned char   m_week;
		unsigned char   m_day;
		unsigned char	m_hour;
		unsigned char	m_minute;	

		time_t			m_timeT;

		Iec102StructHelper	m_helper;
		
	};

	//Integrated Total Power Energy Data
    class ITInforObject : public Iec102Struct
    {
    public:
		//Used for the Integarated Tatal data with Address
        ITInforObject( const std::vector<unsigned char>& data, unsigned char lowVirtualAddr, unsigned char highVirtualAddr, int arithSum, TimeA* timeA = NULL);

		//Used for the Integarated Tatal data without Address
        ITInforObject( const std::vector<unsigned char>& data, unsigned char lowVirtualAddr, unsigned char highVirtualAddr, int arithSum, unsigned char ioa, TimeA* timeA = NULL);
        ~ITInforObject(){}

		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
		virtual unsigned int getSize() {return static_cast<int>(m_size);}
        virtual std::vector<IEC102Data>   getData() 
		{
			std::vector<IEC102Data> dataTemp;
			dataTemp.push_back(m_data);
			return dataTemp; 
		}   
    private:
        void checkAndLog();
		void producePowerData();
		bool getDataValid();

        unsigned char       m_ioa;
        unsigned char       m_powerData_LL;
        unsigned char       m_powerData_LH;
        unsigned char       m_powerData_HL;
        unsigned char       m_powerData_HH;
        unsigned char       m_frameCntAll;
        unsigned char       m_checkSum;

		unsigned short		m_baseAddr;
		unsigned int 		m_address;
		int					m_dataValue;
		bool 				m_isValid;
		unsigned int 		m_frameCount;

		int			 		m_arithSum;
		TimeA*				m_timeA;
		unsigned char		m_lowVirtualAddr;
		unsigned char		m_highVirtualAddr;
		IEC102Data			m_data;

		unsigned char		m_size;
		Iec102StructHelper	m_helper;
    };


	//Remote Measurement Data
    class RMInforObject : public Iec102Struct
    {
    public:
		//Used for the Integarated Tatal data with Address
        RMInforObject( const std::vector<unsigned char>& data, unsigned char lowVirtualAddr, unsigned char highVirtualAddr, TimeA* timeA = NULL);

		//Used for the Integarated Tatal data without Address
        RMInforObject( const std::vector<unsigned char>& data, unsigned char lowVirtualAddr, unsigned char highVirtualAddr, unsigned char ioa, TimeA* timeA =NULL);
        ~RMInforObject(){}

		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
 		virtual unsigned int getSize() {return static_cast<int>(m_size);}
        virtual std::vector<IEC102Data> getData() 
		{
			std::vector<IEC102Data> dataTemp;
			dataTemp.push_back(m_data);
			return dataTemp; 
		}     
    private:
        void checkAndLog();
		void produceMeasureData();
		bool getDataValid();

        unsigned char       m_ioa;
        unsigned char       m_measureData_LL;
        unsigned char       m_measureData_LH;
        unsigned char       m_measureData_HL;
        unsigned char       m_measureData_HH;
        unsigned char       m_frameCntAll;

		unsigned short		m_baseAddr;

		unsigned int 		m_address;
		int					m_dataValue;
		bool 				m_isValid;
		unsigned int 		m_frameCount;

		TimeA*				m_timeA;
		unsigned char		m_lowVirtualAddr;
		unsigned char		m_highVirtualAddr;
		IEC102Data			m_data;

		unsigned char		m_size;
		Iec102StructHelper	m_helper;
    };

    class QueryWithAddrTimeA : public Iec102Struct
    {
    public:
        QueryWithAddrTimeA( const std::vector<unsigned char>& data );
        QueryWithAddrTimeA(unsigned char startAddr, unsigned char endAddr, time_t startTime, time_t endTime );
		virtual ~QueryWithAddrTimeA();

		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
		virtual unsigned int getSize() {return static_cast<int>(QUERY_WITH_ADDR_TIMEA_SIZE);}

    
    private:
        void checkAndLog( );
		void createRawData();
    
        unsigned char       m_startAddr;
        unsigned char       m_endAddr;
        TimeA*              m_startTime;
        TimeA*              m_endTime;

		Iec102StructHelper	m_helper;

    };

    class QueryWithAddr : public Iec102Struct
    {
    public:
        QueryWithAddr( const std::vector<unsigned char>& data );
        QueryWithAddr(unsigned char startAddr, unsigned char endAddr);
        ~QueryWithAddr(){}
		
		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
		virtual unsigned int getSize() {return static_cast<int>(QUERY_WITH_ADDR_SIZE);}
    private:
        void checkAndLog( );
		void createRawData();
     
        unsigned char       m_startAddr;
        unsigned char       m_endAddr;
		
		Iec102StructHelper	m_helper;
    };

    class InitCompleted : public Iec102Struct
    {
    public:
        InitCompleted( const std::vector<unsigned char>& data );
        ~InitCompleted(){}
		
		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
		virtual unsigned int getSize() {return static_cast<int>(INIT_COMPLETED_SIZE);}
    private:
        void checkAndLog( );

	 	unsigned char		m_ioa;
        unsigned char       m_coi;
		
		Iec102StructHelper	m_helper;
    };

	class Asdu : public Iec102Struct
    {
    public:
        Asdu(const std::vector<unsigned char>& data,
			unsigned char localVirtualLowAddr,	
			unsigned char localVirtualHighAddr);

		//Used for create Asdu with the parameter
        Asdu(unsigned char type,
			unsigned char localVirtualLowAddr,	
			unsigned char localVirtualHighAddr,
			unsigned char startAddr, 
			unsigned char endAddr);
		
		Asdu(unsigned char type,
			unsigned char localVirtualLowAddr,	
			unsigned char localVirtualHighAddr,
			unsigned char rad,
			unsigned char startAddr, 
			unsigned char endAddr,
			time_t startTime,
			time_t endTime);
		
 		~Asdu();
		

		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
		void setUnknownInforObjectType() 	{m_unknownInforObjectType = true;}
		void setBadCalcBodySize()	{m_badCalcBodySize = true;}
		unsigned char getCOT() {return m_cot;}
		unsigned char getTypeID() {return m_typeId;}
		
        virtual std::vector<IEC102Data> getData(); 


    private:
		Asdu();
        Asdu& operator=(const Asdu& rhs);
        Asdu(const Asdu&);	
		void checkAndLog();
		void logBadAsduBody(const std::string strDesc);	
		void getInforTypeLengthDefinition(unsigned char ucType, unsigned char ucStruct);
		void createRawData();
		
        unsigned char                   m_typeId;
        unsigned char                   m_vsq;
        unsigned char                   m_cot;
        unsigned char                   m_tadL;
        unsigned char                   m_tadH;
        unsigned char                   m_rad;
        unsigned char                   m_ioa;

		int								m_headerSum;

		std::vector<Iec102Struct*>*		m_pInforObjects;
        TimeA*		                    m_pTimeA;
        Iec102StructHelper      		m_helper;

		unsigned char 					m_lowVirtualAddr;
		unsigned char 					m_highVirtualAddr;
		AsduSubLen 						m_asduLen;

		bool							m_unknownInforObjectType;
		bool							m_badCalcBodySize;
		std::vector<unsigned char>		m_unknownAsduBody;
    };

	class Iec102VariableFrame : public Iec102Struct,public Iec102FrameInterface
    {
    public:
        Iec102VariableFrame(const std::vector<unsigned char>& data,
									E_IEC102_DIRECTION direction,
									unsigned char localLowAddr, 
									unsigned char localHighAddr,
									unsigned char localVirtualLowAddr,
									unsigned char localVirtualHighAddr);

		//used for create 120/162 frame
        Iec102VariableFrame(
									E_IEC102_FCB fcb, 
									E_IEC102_FCV fcv, 
									unsigned char funCode,
									unsigned char type,
									unsigned char localLowAddr, 
									unsigned char localHighAddr,
									unsigned char localVirtualLowAddr,
									unsigned char localVirtualHighAddr,
									unsigned char startAddr, 
									unsigned char endAddr);	
		//Used for create 161
        Iec102VariableFrame(
									E_IEC102_FCB fcb, 
									E_IEC102_FCV fcv, 
									unsigned char funCode,
									unsigned char type,
									unsigned char localLowAddr, 
									unsigned char localHighAddr,
									unsigned char localVirtualLowAddr,
									unsigned char localVirtualHighAddr,
									unsigned char rad,
									unsigned char startAddr, 
									unsigned char endAddr,
									time_t startTime,
									time_t endTime);				
		~Iec102VariableFrame();
		
		void setBadCalcBodySize() {m_badCalcBodySize = true;}
		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);

		virtual bool isFrameValid() {return isValid();}
	 	virtual EFlowControlState getFlowControlState();
		std::vector<PowerEnergyData> getEnergyData();
		virtual std::vector<IEC102Data> getData(); 
		
    private:
		Iec102VariableFrame();
        Iec102VariableFrame& operator=(const Iec102VariableFrame& rhs);
        Iec102VariableFrame(const Iec102VariableFrame&);	
		void checkAndLog();
		unsigned char verifyCheckSum();
		unsigned char setCheckSum();
		Asdu* getAsdu(){ return dynamic_cast<Asdu*>(m_pAsdu);}
		void createRawData();
		unsigned char getControlFieldUC();
			
        unsigned char                   m_headerStart;        
		unsigned char                   m_headerLen1;
        unsigned char                   m_headerLen2;        
		unsigned char                   m_headerEnd;
		unsigned char                   m_controlField;
        Iec102Struct*       			m_pControlField;
		unsigned char					m_addrL;		
		unsigned char					m_addrH;
		Iec102Struct*					m_pAsdu;
		unsigned char					m_checkSum;		
		unsigned char					m_trailer;

		unsigned char					m_localLowAddr;		
		unsigned char					m_localHighAddr;
		unsigned char 					m_virtualLowAddr;
		unsigned char 					m_virtualHighAddr;		
		E_IEC102_DIRECTION 				m_direction;
		Iec102StructHelper				m_helper;

		bool							m_badCalcBodySize;
		std::vector<unsigned char>		m_unknownAsduBody;
		
		
    };

    class Iec102SingleFrame : public Iec102Struct, public Iec102FrameInterface
    {
	public:        
		Iec102SingleFrame( const unsigned char data ); 
		Iec102SingleFrame( const std::vector<unsigned char>& data );
		~Iec102SingleFrame( ) { };        
		virtual void setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper);
		virtual unsigned int getSize() {return FRAME_SINGLE_FORMAT_SIZE;}

		virtual bool isFrameValid() {return isValid();}
	 	virtual EFlowControlState getFlowControlState();

	private:        
		Iec102SingleFrame();
        Iec102VariableFrame& operator=(const Iec102SingleFrame& rhs);
        Iec102SingleFrame(const Iec102SingleFrame&);	

 
		void checkAndLog();
		void createRawData();
 		unsigned char			m_singleE5;;	 
		Iec102StructHelper		m_helper;
	};
	
	class ControlStateObserver
	{
	public:
		ControlStateObserver(unsigned long timeOutMs); 

		virtual ~ControlStateObserver();

		bool waitForReply();

		bool notifyStateChange(EFlowControlState currentState);

		void addObservedState(EFlowControlState state);
		
		//bool isValid() {return m_isValid;}
		std::string getControlStateDescription(EFlowControlState currentState);

		EFlowControlState getNotifiedState() {return m_notifyState;}
	private:
		//bool						m_isValid;
		EFlowControlState			m_notifyState;
		unsigned long				m_timeOutMs;
		std::set<EFlowControlState> m_stateObserved;
		//TA_Base_Core::Condition		m_condition;
		TA_Base_Core::TimedWaitSemaphore m_semaphore;
	};

}

#endif

