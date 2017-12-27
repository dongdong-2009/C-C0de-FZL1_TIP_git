/******************************************************************
*  File name: Iec104Def.h
*  Version:  1.0
*  Description: definition of the CIec104 class
*
********************************************************************/
#ifdef _MSC_VER
#pragma once
#endif

#ifndef _IEC104DEF_H
#define _IEC104DEF_H

#define	IEC104_APCI_SIZE				6
#define	IEC104_MAX_ASDU_SIZE			249
#define	IEC104_MAX_APDU_SIZE			255
#define IEC104_MAX_TXQUE_LEN			30
// define timeout		
#define IEC104_STD_T0   30	//Time-out of connection establishment
#define IEC104_STD_T1   15 //Time-out of send or test APDUs
#define IEC104_STD_T2   10	//Time-out for acknowledges in case of no datamessages t2 < t1
#define IEC104_STD_T3   20	//Time-out for sending test frames in case of a long idle time
#define IEC104_EX_T4     -1

//define k and w
#define IEC104_STD_K	12  //Maximum difference receive sequence numberto send state variable
#define IEC104_STD_W	8   //Latest acknowledge after receiving w I format APDUs

#define IEC104_STD_COT_LEN   2 //cause of transfer len
#define IEC104_STD_COA_LEN   2 //common address len
#define IEC104_STD_IAD_LEN   3 //infmation address len

#define IEC104_FRAME_I					1
#define IEC104_FRAME_S					2
#define IEC104_FRAME_U_START_ACT		3
#define IEC104_FRAME_U_START_CON		4
#define IEC104_FRAME_U_STOP_ACT			5
#define IEC104_FRAME_U_STOP_CON			6
#define IEC104_FRAME_U_TEST_ACT			7
#define IEC104_FRAME_U_TEST_CON			8

//state of like:
//#define IEC104_LK_STATE_STARTED                  0x0
//#define IEC104_LK_STATE_STOPED                    0x80

#define IEC104_LK_STATE_UNRESET			0
#define IEC104_LK_STATE_CONN				1
#define IEC104_LK_STATE_WAITPEER			2	//WAIT ACK/NACK FOR DATA
#define IEC104_LK_STATE_DBCONN				3	//WAIT ACK/NACK FOR RESET
#define IEC104_LK_STATE_DBWAITPEER			4	//WAIT FOR LINKSTATUS

#define IEC104_LK_STATE_STOPPED                     0X00
#define IEC104_LK_STATE_STARTED_PEND	          0X01	
#define IEC104_LK_STATE_STARTED                     0X02
#define IEC104_LK_STATE_STOPPED_PEND	          0X03


#define IEC104_LK_CMD_NONE                         0x00
#define IEC104_LK_CMD_STARTDT                         0x01
#define IEC104_LK_CMD_STOPDT                           0x02
#define IEC104_LK_CMD_UTEST                   0x03
//#define IEC104_LK_CMD_UTEST_CON                      0x04
#define IEC104_LK_CMD_SENDFLAG                        0x80

//state of appliction layer
#define IEC104_APP_STATE_IDLE						0
#define IEC104_APP_STATE_WAITALLDATACONF			10//wait for interrogation command confirmation
#define IEC104_APP_STATE_WAITTIMECONF				11//wait for clock synchronization command confirmation   
#define IEC104_APP_STATE_WAITALLKWHCONF			12//wait for counter interrogation command confirmation
#define IEC104_APP_STATE_WAITYKCONF					13//wait for control command confirmation
#define IEC104_APP_STATE_WAITRPCONF					14//wait for reset progress command confirmation
#define IEC104_APP_STATE_WAITFRZKWHCONF			15// wait for  counter interrogation command confirmation
#define IEC104_APP_STATE_WAITTESTCONF				16//wait for test command confirmation
#define IEC104_APP_STATE_WAITRDCMDCONF	                    17//wait for read command confirmation

#define IEC104_APP_STATE_WAITSVCONF                       18//wait for set point command confirmation
#define IEC104_APP_STATE_WAITSVFINISH                    19//wait for set point command finish

//
#define IEC104_APP_STATE_WAITALLKWH					21
#define IEC104_APP_STATE_WAITGRPDATA				22
#define IEC104_APP_STATE_WAITGRPKWH					23
#define IEC104_APP_STATE_WAITALLDATA				24
#define IEC104_APP_STATE_WAITYKFINISH				25
#define	IEC104_APP_STATE_WAITRPEND					26
//103 extension
#define    IEC104_APP_STATE_WAITCALLVALUECONF                                 27
#define    IEC104_APP_STATE_WAITMIDIFYVALUECONF                             28


//define format type
#define IEC104_FRMTYPE_SEND_NC					1
#define IEC104_FRMTYPE_SEND_WC					2
#define IEC104_FRMTYPE_REQ_RESP					3

//dfine the data type of application layer
#define IEC104_M_SP_NA					1	//single-point information
#define IEC104_M_DP_NA					3	//double-point information
#define IEC104_M_ST_NA					5	//step position information
#define IEC104_M_BO_NA					7	//bitstring of 32 bits
#define IEC104_M_ME_NA					9	//measured value, normalized value
#define IEC104_M_ME_NB					11	//measured value, scaled value
#define IEC104_M_ME_NC					13	//measured value, short floating point number
#define IEC104_M_IT_NA					15	//integrated totals
#define IEC104_M_SP_NB					20	//packed single-point information with status change detection
#define IEC104_M_ME_ND					21	//measured value, normalized value without quality descriptor
#define IEC104_M_SP_TB					30	//single-point information with time tag CP56Time2a
#define IEC104_M_DP_TB					31	//double-point information with time tag CP56Time2a
#define IEC104_M_ST_TB					32	//step position information with time tag CP56Time2a
#define IEC104_M_BO_TB					33	//bitstring of 32 bit with time tag CP56Time2a
#define IEC104_M_ME_TD					34	//measured value, normalized value with time tag CP56Time2a
#define IEC104_M_ME_TE					35	//measured value, scaled value with time tag CP56Time2a
#define IEC104_M_ME_TF					36	//measured value, short floating point number with time tag CP56Time2a
#define IEC104_M_IT_TB					37	//integrated totals with time tag CP56Time2a
#define IEC104_M_EP_TD					38	//event of protection equipment with time tag CP56Time2a
#define IEC104_M_EP_TE					39	//packed start events of protection equipment with time tag CP56Time2a
#define IEC104_M_EP_TF					40	//packed output circuit information of protection equipment with time tag CP56Time2a

//Process information in control direction
#define IEC104_C_SC_NA					45	//single command
#define IEC104_C_DC_NA					46 // double command
#define IEC104_C_RC_NA					47	//regulating step command
#define IEC104_C_SE_NA					48	//set point command, normalized value
#define IEC104_C_SE_NB					49	//set point command, scaled value
#define IEC104_C_SE_NC					50	//set point command, short floating point number
#define IEC104_C_BO_NA					51	//bitstring of 32 bits

#define IEC104_C_SC_TA					58	//single command with time tag CP56Time2a
#define IEC104_C_DC_TA					59 // double command with time tag CP56Time2a
#define IEC104_C_RC_TA					60	//regulating step command with time tag CP56Time2a
#define IEC104_C_SE_TA					61	//set point command, normalized value with time tag CP56Time2a
#define IEC104_C_SE_TB					62	//set point command, scaled value with time tag CP56Time2a
#define IEC104_C_SE_TC					63	//set point command, short floating-point number with time tag CP56Time2a
#define IEC104_C_BO_TA					64	//bitstring of 32 bits with time tag CP56Time2a

//System information in monitor direction
#define IEC104_M_EI_NA					70	//end of initialization

//System information in control direction
#define IEC104_C_IC_NA				100	//interrogation command
#define IEC104_C_CI_NA				101	//counter interrogation command
#define IEC104_C_RD_NA				102	//read command
#define IEC104_C_CS_NA				103	//clock synchronization command
#define IEC104_C_RP_NA				105 //reset process command
#define IEC104_C_TS_TA				107 //test command with time tag CP56Time2a

//Parameter in control direction
#define IEC104_P_ME_NA				110 //parameter of measured value, normalized value
#define IEC104_P_ME_NB				111 //parameter of measured value, scaled value
#define IEC104_P_ME_NC				112 //parameter of measured value, short floating-point number
#define IEC104_P_NC_NA				113 //parameter activation

//
#define IEC104_F_FR_NA				120 //file ready
#define IEC104_F_SR_NA				121 //section ready
#define IEC104_F_SC_NA				122 //call directory, select file, call file, call section
#define IEC104_F_LS_NA				123 //last section, last segment
#define IEC104_F_AF_NA				124 //ack file, ack section
#define IEC104_F_SG_NA				125 //segment
#define IEC104_F_DR_NA				126 //directory

//103 extension
#define IEC104_E_PA_NA                            140
#define IEC104_E_CD_NA                              145
#define IEC104_E_CR_NA                              146
#define IEC104_E_MD_NA                              147
#define IEC104_E_148					 148

//cause of transfer
#define IEC104_COT_PER_CYC					1
#define IEC104_COT_BACKSCAN					2
#define IEC104_COT_SPONT					3
#define IEC104_COT_INIT						4
#define IEC104_COT_REQ						5
#define IEC104_COT_ACT						6
#define IEC104_COT_ACT_CON					7
#define IEC104_COT_DEACT					8
#define IEC104_COT_DEACT_CON				9
#define IEC104_COT_ACT_TERM					10
#define IEC104_COT_FILE_TRANS					13


#define IEC104_COT_RESP_CALLALL				20
#define IEC104_COT_RESP_GRP1				21
#define IEC104_COT_RESP_GRP2				22
#define IEC104_COT_RESP_GRP3				23
#define IEC104_COT_RESP_GRP4				24
#define IEC104_COT_RESP_GRP5				25
#define IEC104_COT_RESP_GRP6				26
#define IEC104_COT_RESP_GRP7				27
#define IEC104_COT_RESP_GRP8				28
#define IEC104_COT_RESP_GRP9				29
#define IEC104_COT_RESP_GRP10				30
#define IEC104_COT_RESP_GRP11				31
#define IEC104_COT_RESP_GRP12				32
#define IEC104_COT_RESP_GRP13				33
#define IEC104_COT_RESP_GRP14				34
#define IEC104_COT_RESP_GRP15				35
#define IEC104_COT_RESP_GRP16				36

#define IEC104_COT_RESP_CO					37
#define IEC104_COT_RESP_CO_GRP1				38
#define IEC104_COT_RESP_CO_GRP2				39
#define IEC104_COT_RESP_CO_GRP3				40
#define IEC104_COT_RESP_CO_GRP4				41
#define IEC104_COT_UNKNOWN					45
#define IEC104_COT_TRI_SPONT				46

//define link layer commands

#define IEC104_CMD_NON						0
#define IEC104_CMD_START_ACT				1
#define IEC104_CMD_START_CON				2
#define IEC104_CMD_STOP_ACT					3
#define IEC104_CMD_STOP_CON					4
#define IEC104_CMD_TEST_ACT					5
#define IEC104_CMD_TEST_CON					6

#define IEC104_CMD_SEND_S					10
#define IEC104_CMD_PEND                                  11	
//define app layer commands
#define IEC104_CMD_CALLALLDATA				20
#define IEC104_CMD_SYNTIME					21
#define IEC104_CMD_CALLALLKWH				22

#define IEC104_CMD_CALLGRPKWH				23
#define IEC104_CMD_SENDYK					24
#define IEC104_CMD_SETEQU					25
#define IEC104_CMD_FRZALLKWH				26
#define IEC104_CMD_RESETPROC				27
#define IEC104_CMD_TEST						28

#define IEC104_CTRL_NON						0
#define IEC104_CTRL_EXECUTE					1
#define IEC104_CTRL_SELECT					2
#define IEC104_CTRL_CANCEL					3

//FILE_TRANS_CMD
#define FILE_CMD_NONE                                0
#define FILE_CMD_REQ_DIR                          1
#define FILE_CMD_REQ_FILE                         2
#define FILE_CMD_REQ_SECTION                 3
#define FILE_CMD_SELECT_FILE                    4
#define FILE_CMD_SELECT_SECTION             5

#define IEC104_CMD_EX_CALLVALUE        29
#define IEC104_CMD_EX_MODIFYVALUE  30


#define	IEC104_BUFF_SIZE				10240
#define     IEC104MAX_TIME_CNT                         1073741824
#define     IEC104_TIME_STOP                           2073741824
#define 	IEC104_TIME_FACTOR                         1000
//#define     IEC104_T4_MAX_TIME_CNT                  1073741824//1024*1024*1024
//#define     IEC104_T4_TIME_STOP                      2073741824


#define IEC104_WORKMODE_NOMAL			1
#define IEC104_WORKMODE_STANDBY			0

#define LINK_DISCONNECT                        0X01
#define HOST_STATUS_CHANGE                 0X02

#define UNKNOWN_HOST_STATUS   -1
#define MASTER_HOST_STATUS       0
#define SLAVE_HOST_STATUS          1

#define CAUSE_LINK_CONNECT                    0X00
#define CAUSE_LINK_DISCONNECT			0X01
#define CAUSE_STATUS_SLAVE2MASTER	0X02
#define CAUSE_STATUS_MASTER2SLAVE	0X03
#define CAUSE_WORKMODE_MONITOR2CONTROLER 0X04
#define CAUSE_WORKMODE_CONTROLER2MONITOR   0X05

#define HOST_STATUS_UNKNOWN			-1
#define HOST_STATUS_MASTER			0
#define HOST_STATUS_SLAVE				1

#define LOG_LEVEL_DEBUG                     0
#define LOG_LEVEL_INFO						1
#define LOG_LEVEL_WARNING                   2
#define LOG_LEVEL_ERROR                     3

#define LINK_STATUS_CONNECT						1
#define LINK_STATUS_DISCONNECT					0

#define VERSION  "3.1.5"

#endif 

