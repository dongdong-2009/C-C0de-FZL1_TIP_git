#include "bus/scada/102_library/src/IEC102DataStruct.h"
#include <sstream>
#include "boost/format.hpp"
#include "core/utilities/src/DebugUtil.h"
#include <iomanip> 
#include "bus/scada/102_library/src/HexHelper.h"
#include <algorithm>
#include <stdio.h>
#include <time.h>

namespace TA_Base_Bus
{
    void Iec102FormatLog::logErrorInfo(const std::string& strDesc, const std::vector<unsigned char>& data )
    {
        FUNCTION_ENTRY( "logErrorInfo" );
		m_log.str("");
        std::stringstream ss;
		ss << strDesc << NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_NAME_CONFIG ) % ss.str() << Iec102Utility::uCharVecToStr( data );
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }

    void Iec102FormatLog::logErrorLength(const std::string& strDesc, const std::vector<unsigned char>& data )
    {
        FUNCTION_ENTRY( "logErrorLength" );
		m_log.str("");
        std::stringstream ss;
		ss << "Incorrect " << strDesc << " Length [" << data.size() << "]   ";
		m_log << boost::format( PRINT_NAME_CONFIG ) % ss.str() << Iec102Utility::uCharVecToStr( data );
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }

    void Iec102FormatLog::logParseLine(const std::string& strHeader,const std::vector<unsigned char>& data)
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
        std::stringstream ss;
		ss << strHeader << NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_CONFIG ) % ss.str() % Iec102Utility::uCharVecToStr( data );
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }

    void Iec102FormatLog::logParseLine(const std::string& strHeader,const std::vector<unsigned char>& data, const std::string& strExplain )
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
        std::stringstream ss;
		ss << strHeader << NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_CONFIG ) % ss.str() % Iec102Utility::uCharVecToStr( data ) << " // " << strExplain;
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }	

    void Iec102FormatLog::logParseLine(const std::string& strHeader,const std::string& data)
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
        std::stringstream ss;
		ss << strHeader <<  NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_CONFIG ) % ss.str() % TA_Base_Bus::HexHelper::hexToStr(data);
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }	

	void Iec102FormatLog::logParseLine(const std::string& strHeader,const std::string& data, const std::string& strExplain)
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
        std::stringstream ss;
		ss << strHeader << NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_CONFIG ) % ss.str() % TA_Base_Bus::HexHelper::hexToStr(data) << " // " << strExplain;
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }	

    void Iec102FormatLog::logParseLine(const std::string& strHeader,unsigned char char1, unsigned char char2,unsigned char char3, unsigned char char4)
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
        std::stringstream ss,ss1;
		ss << Iec102Utility::uCharToStr(char1) << " " << Iec102Utility::uCharToStr(char2) << " " << Iec102Utility::uCharToStr(char3) << " " << Iec102Utility::uCharToStr(char4);
		ss1 << strHeader << NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_CONFIG ) % ss1.str() % ss.str();
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }

    void Iec102FormatLog::logParseLine(const std::string& strHeader,unsigned char char1, unsigned char char2,unsigned char char3, unsigned char char4, const std::string& strExplain)
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
        std::stringstream ss,ss1;
		ss << Iec102Utility::uCharToStr(char1) << " " << Iec102Utility::uCharToStr(char2) << " " << Iec102Utility::uCharToStr(char3) << " " << Iec102Utility::uCharToStr(char4);
		ss1 << strHeader << NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_CONFIG ) % ss1.str() % ss.str() << " // " << strExplain;
		m_debugInfos.push_back(m_log.str());
         FUNCTION_EXIT;
    }

    void Iec102FormatLog::logParseLine(const std::string& strHeader,unsigned char char1, unsigned char char2)
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
        std::stringstream ss,ss1;
		ss << Iec102Utility::uCharToStr(char1) << " " << Iec102Utility::uCharToStr(char2);
		ss1 << strHeader <<  NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_CONFIG ) % ss1.str() % ss.str();
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }

    void Iec102FormatLog::logParseLine(const std::string& strHeader,unsigned char char1, unsigned char char2, const std::string& strExplain)
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
        std::stringstream ss,ss1;
		ss << Iec102Utility::uCharToStr(char1) << " " << Iec102Utility::uCharToStr(char2);
		ss1 << strHeader << NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_CONFIG ) % ss1.str() % ss.str() << " // " << strExplain;
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }

    void Iec102FormatLog::logParseLine(const std::string& strHeader,unsigned char char1)
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
		std::stringstream ss;
		ss << strHeader << NAME_FORMAT_FOOTER;
 		m_log << boost::format( PRINT_CONFIG ) % ss.str() % Iec102Utility::uCharToStr(char1);
		m_debugInfos.push_back(m_log.str());
        FUNCTION_EXIT;
    }

	void Iec102FormatLog::logParseLine(const std::string& strHeader,unsigned char char1, const std::string& strExplain)
    {
        FUNCTION_ENTRY( "logParseLine" );
		m_log.str("");
		std::stringstream ss;
		ss << strHeader  << NAME_FORMAT_FOOTER;
		m_log << boost::format( PRINT_CONFIG ) % ss.str() % Iec102Utility::uCharToStr(char1) << " // " << strExplain;
		m_debugInfos.push_back(m_log.str());
         FUNCTION_EXIT;
    }

    void Iec102FormatLog::logAppend(const std::vector<std::string>& debugInfos)
    {
        FUNCTION_ENTRY( "logAppend" );

		for (unsigned int i = 0; i < debugInfos.size(); i++)
		{
			m_debugInfos.push_back(debugInfos[i]);
		}
		FUNCTION_EXIT;
    }

    void Iec102FormatLog::printDebugInfos()
    {
        FUNCTION_ENTRY( "printDebugInfos" );

		for (unsigned int i = 0; i < m_debugInfos.size(); i++)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", m_debugInfos[i].c_str() );
		}
		FUNCTION_EXIT;
    }

    // **************
    // Iec102StructHelper
    // **************
    Iec102StructHelper::Iec102StructHelper(Iec102Struct* pIEC102Struct)
    : 
    m_pIEC102Struct(pIEC102Struct)
    {
        FUNCTION_ENTRY( "Iec102StructHelper" );
        FUNCTION_EXIT;
    }

    void Iec102StructHelper::parseData(const std::vector<unsigned char>& data)
    {
        FUNCTION_ENTRY( "parseData" );
		
		E_IEC102_STRUCT_TYPE type = m_pIEC102Struct->getStructType();
		unsigned int nWantLen = 0;
		unsigned int dataSize = data.size();

		if (ASDU_TYPE == type) 
		{
			if (ASDU_MIN_SIZE > dataSize)
			{
				m_pIEC102Struct->logLengthError();
				FUNCTION_EXIT;
				return;
			}
			else
			{
				Asdu* pAsdu = dynamic_cast<Asdu*>(m_pIEC102Struct);
				unsigned char ucType	= data[ASDU_TYPE_IDENTIFIER];
				unsigned char ucStruct	= data[ASDU_VSQ] & 0x80;
				int ucNum				= int(data[ASDU_VSQ] & 0x7F);

				switch (ucType)
				{
				case M_EI_NA_2: 					//70
					nWantLen = ASDU_MIN_SIZE + INIT_COMPLETED_SIZE;
					break;

				case C_YC_NA_2: 					//171
					nWantLen = ASDU_MIN_SIZE + QUERY_WITH_ADDR_SIZE;
					break;
					
				case C_YC_TA_2: 					//172
				case C_CI_NR_2: 					//120
					nWantLen = ASDU_MIN_SIZE + QUERY_WITH_ADDR_TIMEA_SIZE;
					break;
					
				case M_YC_NA_2: 					//161
					if (0x80 == ucStruct)
					{
						nWantLen = ASDU_MIN_SIZE + 1 +	ucNum * MEASUREMENT_NO_IOA_SIZE;
					}
					else
					{
						nWantLen = ASDU_MIN_SIZE + ucNum * MEASUREMENT_WITH_IOA_SIZE ;
					}
					break;
					
				case M_YC_TA_2: 					//162
					if (0x80 == ucStruct )
					{
						nWantLen = ASDU_MIN_SIZE + 1 +	(ucNum * MEASUREMENT_NO_IOA_SIZE) + TIMEA_SIZE;
					}
					else
					{
						nWantLen = ASDU_MIN_SIZE + (ucNum * MEASUREMENT_WITH_IOA_SIZE) + TIMEA_SIZE;
					}
					break;
				
				case M_IT_TA_2: 					//02
					if (0x80 == ucStruct )
					{
						nWantLen = ASDU_MIN_SIZE + 1 +	(ucNum * INTEGRATED_TOTAL_NO_IOA_SIZE) + TIMEA_SIZE;
					}
					else
					{
						nWantLen = ASDU_MIN_SIZE + (ucNum * INTEGRATED_TOTAL_WITH_IOA_SIZE) + TIMEA_SIZE;
					}
					break;
					
				default:
					pAsdu->setUnknownInforObjectType();
					break;
				}
				
				if (dataSize != nWantLen)
				{
					pAsdu->setBadCalcBodySize();
					
				}
			}
		}

		else if (VARIABLE_FRAME_TYPE == type)
		{
			if (FRAME_VARIABLE_FORMAT_MIN_SIZE > dataSize  || FRAME_VARIABLE_FORMAT_MAX_SIZE < dataSize)
			{
				m_pIEC102Struct->logLengthError();
				FUNCTION_EXIT;
				return;
			
			}
			else
			{
				int nInfoLen = static_cast<int>( data[VAR_FRAME_SIZE_1]);
				
				nWantLen = 	nInfoLen + 6;

				if (dataSize != nWantLen)
				{
					Iec102VariableFrame* pVarFrame = dynamic_cast<Iec102VariableFrame*>(m_pIEC102Struct);
					pVarFrame->setBadCalcBodySize();
				}
			}			
		}
		else
		{
			unsigned int structSize = m_pIEC102Struct->getSize();

			if (dataSize != structSize )
			{
				m_pIEC102Struct->logLengthError();
				FUNCTION_EXIT;
				return;
			}
		}
		m_pIEC102Struct->setupStruct(data,this);
	    FUNCTION_EXIT;
    }	


    // **************
    // Iec102Struct
    // **************
    Iec102Struct::Iec102Struct(std::string strName, E_IEC102_STRUCT_TYPE type,const std::vector<unsigned char>& data)
    :
    m_rawData(data),
	m_valid(true),
	m_name(strName),
	m_type(type)
    {
        FUNCTION_ENTRY( "Iec102Struct" );
        FUNCTION_EXIT;
    }

	Iec102Struct::Iec102Struct(std::string strName, E_IEC102_STRUCT_TYPE type)
    :
	m_valid(true),
	m_name(strName),
	m_type(type)
    {
        FUNCTION_ENTRY( "Iec102Struct" );
        FUNCTION_EXIT;
    }
    
    void Iec102Struct::logLengthError()
    {
        FUNCTION_ENTRY( "setLengthError" );
		setValid(false);
		m_formatLog.logErrorLength(m_name,m_rawData);
        FUNCTION_EXIT;
    }

    std::string Iec102Struct::convertRawData()
    {
        FUNCTION_ENTRY( "convertRawData" );
		std::string strRawData;

		if (0 != m_rawData.size())
		{
			for (unsigned int i = 0; i < m_rawData.size(); i++)
			{
				strRawData += m_rawData[i];
			}			
		}

        FUNCTION_EXIT;
		return strRawData;
    }

    // **************
    // ControlFieldSend
    // **************
    // used by parsing control field
    ControlFieldSend::ControlFieldSend( const std::vector<unsigned char>& data, E102FrameType frameType )
    :
	Iec102Struct(NAME_CONTROL_FIELD,CONTROL_FIELD_SEND_TYPE,data),
    m_frameType(frameType),
	m_helper(this)
    {
        FUNCTION_ENTRY( "ControlFieldSend" );
		m_helper.parseData(data);
        // LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }

    ControlFieldSend::ControlFieldSend( unsigned char data, E102FrameType frameType )
    :
	Iec102Struct(NAME_CONTROL_FIELD,CONTROL_FIELD_SEND_TYPE),
	m_direction(static_cast<E_IEC102_DIRECTION>(data & 0x40)),
	m_funCode( data & 0x0F ),
	m_controlField(data),
    m_fcb(static_cast<E_IEC102_FCB>(data & 0x20)),
    m_fcv(static_cast<E_IEC102_FCV>(data & 0x10)),
    m_frameType(frameType),
	m_helper(this)
    {
        FUNCTION_ENTRY( "ControlFieldSend" );
		std::vector<unsigned char> controlFieldData;
		controlFieldData.push_back(data);
		setRawData(controlFieldData);
        checkAndLog();
        // LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }
    
    // used by create control field
    ControlFieldSend::ControlFieldSend( E_IEC102_FCB fcb, E_IEC102_FCV fcv, unsigned char funCode, E102FrameType frameType )
    :
	Iec102Struct(NAME_CONTROL_FIELD,CONTROL_FIELD_SEND_TYPE),
    m_fcb(fcb),
    m_fcv(fcv),
	m_frameType(frameType),
	m_helper(this)
    {
        FUNCTION_ENTRY( "ControlFieldSend" );
		m_direction = IEC102_CONTROL_DIRECTION;
		m_funCode = funCode & 0x0F;	
        m_controlField = static_cast<unsigned char>(m_direction) + static_cast<unsigned char>(m_fcb) + 
                            static_cast<unsigned char>(m_fcv) + m_funCode; 
		std::vector<unsigned char> controlFieldData;
		controlFieldData.push_back(m_controlField);
		setRawData(controlFieldData);		
        checkAndLog();
        //LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }

	void ControlFieldSend::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper)
	{
        FUNCTION_ENTRY( "setupStruct" );

		if (pHelper != &m_helper) {	return;	}

		bool bValid = isValid();
		if (!bValid) { return; }


		m_controlField	= 	data[0];
		m_direction		= 	static_cast<E_IEC102_DIRECTION>(m_controlField & 0x40);
		m_funCode		=  	m_controlField & 0x0F;
		m_fcb			=	static_cast<E_IEC102_FCB>(m_controlField & 0x20);
		m_fcv			= 	static_cast<E_IEC102_FCV>(m_controlField & 0x10);
		
		checkAndLog();
		FUNCTION_EXIT;
	}
    
    void ControlFieldSend::checkAndLog()
    {
        FUNCTION_ENTRY( "checkAndLog" );
 		Iec102FormatLog* pLog = getFormatLog();

        std::stringstream ss;
			
        ss << "DIRECTION(" << Iec102Utility::uCharToStr( static_cast<unsigned char>(m_direction) ) << ") " ;
        ss << "FCB(" << Iec102Utility::uCharToStr(static_cast<unsigned char>(m_fcb)) << ") ";
        ss << "FCV(" << Iec102Utility::uCharToStr(static_cast<unsigned char>(m_fcv)) << ") ";
        ss << "FUNC_CODE(" << Iec102Utility::uCharToStr( m_funCode ) << ")";
		
        if ( m_direction !=  IEC102_CONTROL_DIRECTION )
        {
			setValid(false);
            ss << " [Incorrect Direction. Direction:MONITOR]";
        }
        
        
        if ( (m_fcv == IEC102_FCV_FALSE) && (m_fcb == IEC102_FCB_FALSE) && !( m_funCode == FUN_CODE_0 || m_funCode == FUN_CODE_9 ) )
        {
			setValid(false);
            ss << " [Incorrect FCB_0_FCV_0_FUNCCODE. FUNC_CODE within (" << Iec102Utility::uCharToStr( FUN_CODE_0 ) << "," 
				<< Iec102Utility::uCharToStr( FUN_CODE_9 ) << ")]";
        }
        else if ( m_fcv == IEC102_FCV_TRUE && ( m_funCode != FUN_CODE_3 && m_funCode != FUN_CODE_10 && m_funCode != FUN_CODE_11 ) )
        {
			setValid(false);
            ss << " [Incorrect FCV_1_FUNCCODE. FUNC_CODE within (" << Iec102Utility::uCharToStr( FUN_CODE_3 ) << "," 
				<< Iec102Utility::uCharToStr( FUN_CODE_10 ) << "," << Iec102Utility::uCharToStr( FUN_CODE_11 ) << ") when FCV(1)]";
        }
        
        if ( m_frameType == EF_FORMAT && !( m_funCode == FUN_CODE_0 || m_funCode == FUN_CODE_9 || m_funCode == FUN_CODE_10 || m_funCode == FUN_CODE_11  ) )
        {
			setValid(false);
            ss << " [Incorrect FIXED_FRAME_FUNCCODE. FUNC_CODE within (" << Iec102Utility::uCharToStr( FUN_CODE_0 ) << "," 
				<< Iec102Utility::uCharToStr( FUN_CODE_9 ) << "," << Iec102Utility::uCharToStr( FUN_CODE_10 ) << "," 
				<< Iec102Utility::uCharToStr( FUN_CODE_11 )<< ")]";
        }    
        else if ( m_frameType == EV_FORMAT &&  m_funCode != FUN_CODE_3  )
        {
			setValid(false);
            ss << " [Incorrect VARIABLE_FRAME_FUNCCODE FUNC_CODE within (" << Iec102Utility::uCharToStr( FUN_CODE_3 ) << ")]";
        }

		pLog->logParseLine(NAME_CONTROL_FIELD,m_controlField,ss.str());
        FUNCTION_EXIT;
    }
    
    // **************
    // ControlFieldRecv
    // **************
    // used by parsing control field
    ControlFieldRecv::ControlFieldRecv( const std::vector<unsigned char>& data, E102FrameType frameType )
    :
	Iec102Struct(NAME_CONTROL_FIELD,CONTROL_FIELD_SEND_TYPE,data),
    m_frameType(frameType),
	m_helper(this)
    {
        FUNCTION_ENTRY( "ControlFieldSend" );
		m_helper.parseData(data);
        // LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }

    ControlFieldRecv::ControlFieldRecv( unsigned char data, E102FrameType frameType )
    :
	Iec102Struct(NAME_CONTROL_FIELD,CONTROL_FIELD_RECV_TYPE),
	m_direction(static_cast<E_IEC102_DIRECTION>(data & 0x40)),
	m_funCode(data & 0x0F),
	m_controlField(data),
    m_acd(static_cast<E_IEC102_ACD>(data & 0x20)),
    m_dfc(static_cast<E_IEC102_DFC>(data & 0x10)),
	m_frameType(frameType),
	m_helper(this)
    {
        FUNCTION_ENTRY( "ControlFieldRecv" );
		std::vector<unsigned char> controlFieldData;
		controlFieldData.push_back(data);
		setRawData(controlFieldData);			
        checkAndLog();
        FUNCTION_EXIT;
    }
    
    // used by create control field
    ControlFieldRecv::ControlFieldRecv( E_IEC102_ACD acd, E_IEC102_DFC dfc, unsigned char funCode, E102FrameType frameType )
    :
	Iec102Struct(NAME_CONTROL_FIELD,CONTROL_FIELD_RECV_TYPE),
    m_acd(acd),
    m_dfc(dfc),
	m_frameType(frameType),
	m_helper(this)
    {
        FUNCTION_ENTRY( "ControlFieldRecv" );
		m_direction = IEC102_MONITOR_DIRECTION;
		m_funCode = funCode & 0x0F;
		
		m_controlField = static_cast<unsigned char>(m_direction) + static_cast<unsigned char>(m_acd) + 
						 static_cast<unsigned char>(m_dfc) + m_funCode; // get whole data;
		std::vector<unsigned char> controlFieldData;
		controlFieldData.push_back(m_controlField);
		setRawData(controlFieldData);							 
        checkAndLog();
        // LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }

	void ControlFieldRecv::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper)
	{
        FUNCTION_ENTRY( "setupStruct" );

		if (pHelper != &m_helper) {	return;	}

		bool bValid = isValid();
		if (!bValid) { return; }


		m_controlField	= 	data[0];
		m_direction		= 	static_cast<E_IEC102_DIRECTION>(m_controlField & 0x40);
		m_funCode		=  	m_controlField & 0x0F;
		m_acd			=	static_cast<E_IEC102_ACD>(m_controlField & 0x20);
		m_dfc			= 	static_cast<E_IEC102_DFC>(m_controlField & 0x10);

		checkAndLog();
		FUNCTION_EXIT;

	}
        
    void ControlFieldRecv::checkAndLog()
    {
        FUNCTION_ENTRY( "checkAndLog" );
 		Iec102FormatLog* pLog = getFormatLog();
		std::stringstream ss;
        ss << "DIRECTION(" << Iec102Utility::uCharToStr( static_cast<unsigned char>(m_direction)) << ") " ;
        ss << "ACD(" << Iec102Utility::uCharToStr(static_cast<unsigned char>(m_acd)) << ") ";
        ss << "DFC(" << Iec102Utility::uCharToStr(static_cast<unsigned char>(m_dfc)) << ") ";
        ss << "FUNC_CODE(" << Iec102Utility::uCharToStr( m_funCode ) << ")";
		
        if ( m_direction !=  IEC102_MONITOR_DIRECTION )
        {
			setValid(false);
            ss << " [Incorrect Direction. Direction:CONTROL]";
        }
        
        
        if ( m_frameType == EF_FORMAT && !( m_funCode != FUN_CODE_0 || m_funCode != FUN_CODE_1 || 
                                         m_funCode != FUN_CODE_9 || m_funCode != FUN_CODE_11  ) )
        {
			setValid(false);
            ss << " [Incorrect FIXED_FRAME_FUNCCODE FUNC_CODE within (" << Iec102Utility::uCharToStr( FUN_CODE_0 ) << "," 
				<< Iec102Utility::uCharToStr( FUN_CODE_1 ) << "," << Iec102Utility::uCharToStr( FUN_CODE_9 ) << "," 
				<< Iec102Utility::uCharToStr( FUN_CODE_11 )<< ")]";
        }    
        else if ( m_frameType == EV_FORMAT &&  m_funCode != FUN_CODE_8  )
        {
			setValid(false);
            ss << " [Incorrect VARIABLE_FRAME_FUNCCODE FUNC_CODE within (" << Iec102Utility::uCharToStr( FUN_CODE_8 ) << ")]";
        }
		pLog->logParseLine(NAME_CONTROL_FIELD,m_controlField,ss.str());
        FUNCTION_EXIT;
    }

	
    // **************
    // Iec102FixedLenFrame
    // **************
    Iec102FixedLenFrame::Iec102FixedLenFrame( const std::vector<unsigned char>& data,unsigned char localLowAddr, unsigned char localHighAddr )
    :
	Iec102Struct(NAME_FIXED_FRAME,FIXED_FRAME_TYPE,data),
	m_pControlField( NULL),
    m_localLowAddr(localLowAddr),
	m_localHighAddr(localHighAddr),
	m_direction(IEC102_MONITOR_DIRECTION),
	m_helper(this)
	{
		FUNCTION_ENTRY( "Iec102FixedLenFrame" );
		m_helper.parseData(data);
		printDebugInfo();
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
		FUNCTION_EXIT;
    }

    Iec102FixedLenFrame::Iec102FixedLenFrame(unsigned char localLowAddr, unsigned char localHighAddr,E_IEC102_FCB fcb, E_IEC102_FCV fcv, unsigned char funCode )
    :
    Iec102Struct(NAME_FIXED_FRAME,FIXED_FRAME_TYPE),
    m_header( FRAME_START_CHAR_10H ),
    m_pControlField( NULL),
    m_addrL(localLowAddr),
    m_addrH(localHighAddr ),
    m_checkSum( 0 ),
    m_trailer( FRAME_END_CHAR_16H ),
    m_localLowAddr(localLowAddr),
	m_localHighAddr(localHighAddr),
	m_direction(IEC102_CONTROL_DIRECTION),
	m_helper(this)
    {
		FUNCTION_ENTRY( "Iec102FixedLenFrame" );
 		m_pControlField = new ControlFieldSend( fcb, fcv, funCode, EF_FORMAT );
		m_controlField = getControlFieldUC();
		setCheckSum();
		createRawData();
		checkAndLog();
		printDebugInfo();
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }
	
    Iec102FixedLenFrame::~Iec102FixedLenFrame( )
    {
		FUNCTION_ENTRY( "~Iec102FixedLenFrame" );
     	if (NULL != m_pControlField)
    	{
        	delete m_pControlField;
    	}
        FUNCTION_EXIT;
    }

	EFlowControlState Iec102FixedLenFrame::getFlowControlState()
	{
		FUNCTION_ENTRY( "getFlowControlState" );
 		bool bValid = isFrameValid();

		if (!bValid)
		{
			return DATA_INVALID;
		}
		ControlFieldRecv* pControlField = dynamic_cast<ControlFieldRecv*>(m_pControlField);

		E_IEC102_ACD acd = pControlField->getACD();
		E_IEC102_DFC dfc = pControlField->getDFC();
		unsigned char funcCode = pControlField->getFuncode();

		if (IEC102_DFC_TRUE == dfc)
		{
			return E_M_BUFFER_FULL;
		}	

		if (IEC102_ACD_TRUE == acd && FUN_CODE_11 == funcCode)
		{
			return E_M_ACTIVE_I_DATA_RESPONSE_LINK_STATE;
		}

		if (IEC102_ACD_FALSE == acd && FUN_CODE_11 == funcCode)
		{
			return E_M_RESPONSE_LINK_STATE;
		}

		if (IEC102_ACD_TRUE == acd && FUN_CODE_0 == funcCode)
		{
			return E_M_ACTIVE_I_DATA_RESPONSE_RESET_LINK;
		}

		if (IEC102_ACD_FALSE == acd && FUN_CODE_0 == funcCode)
		{
			return E_M_RESPONSE_RESET_LINK;
		}

		if (FUN_CODE_1 == funcCode)
		{
			return E_M_LINK_BUSY;
		}		

		if (FUN_CODE_9 == funcCode)
		{
			return E_M_NO_DATA;
		}
			
		FUNCTION_EXIT;
		return E_M_UNKNOWN_CONTROL_STATE;
	}

	void Iec102FixedLenFrame::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper) 
	{
		FUNCTION_ENTRY( "setupStruct" );

		if (pHelper != &m_helper) {	return;	}
		
		bool bValid = isValid();
		if (!bValid) { return; }


		m_header	= data[FIXED_FRAME_HEADER];
		m_controlField = data[FIXED_FRAME_CONFIELD];
		m_addrL 	= data[FIXED_FRAME_ADDRL];
		m_addrH 	= data[FIXED_FRAME_ADDRH];
		m_checkSum	= data[FIXED_FRAME_CHECKSUM];
		m_trailer	= data[FIXED_FRAME_TRAILER];
		
		m_pControlField = new ControlFieldRecv( data[FIXED_FRAME_CONFIELD], EF_FORMAT );
		
		checkAndLog();
		FUNCTION_EXIT;

	}
	    
	void Iec102FixedLenFrame::setCheckSum() 
	{ 
		FUNCTION_ENTRY( "setCheckSum" );
 		m_checkSum = (getControlFieldUC() + m_addrL + m_addrH) % 256; 
        FUNCTION_EXIT;
	}

	unsigned char Iec102FixedLenFrame::getControlFieldUC()
	{
		FUNCTION_ENTRY( "getControlFieldUC" );
 		std::string strControlField = m_pControlField->getRawData();
        FUNCTION_EXIT;
		return static_cast<unsigned char>(strControlField[0]);
	}

	
    void Iec102FixedLenFrame::checkAndLog( )
    {
		FUNCTION_ENTRY( "checkAndLog" );
 		Iec102FormatLog* pLog = getFormatLog();
	    //pLog->logStartLine("List the detail information of the Fixed Len Frame:");

        if ( m_header != FRAME_START_CHAR_10H )
        {
			setValid(false);
 			pLog->logParseLine(NAME_FIXED_FRAME_HEADER,m_header,"[Incorrect Fixed Frame Header. Expected:10]");
        }
		else
		{
			pLog->logParseLine(NAME_FIXED_FRAME_HEADER,m_header);
		}
		
		if (!m_pControlField->isValid())
		{
			setValid(false);
 		}
		pLog->logAppend( m_pControlField->getDebugInfo());

		if (m_localLowAddr != m_addrL || m_localHighAddr != m_addrH)
		{
			setValid(false);
 			std::stringstream ss;
			ss << "[Incorrect Address. Expected Address:" << Iec102Utility::uCharToStr( m_localLowAddr ) 
				<< " "<< Iec102Utility::uCharToStr( m_localHighAddr ) << "]";

			pLog->logParseLine(NAME_EQUIPMENT_ADDRESS,m_addrL,m_addrH,ss.str());
		}        
		else
		{
			pLog->logParseLine(NAME_EQUIPMENT_ADDRESS,m_addrL,m_addrH);
		}
			
        unsigned char checkSum = ( m_controlField + m_addrL + m_addrH ) % 256;
        if ( m_checkSum != checkSum )
        {
 			setValid(false);
 			std::stringstream ss;
            ss << "[Incorrect CheckSum. Received:" << Iec102Utility::uCharToStr( m_checkSum ) << ",Expected:" << Iec102Utility::uCharToStr( checkSum ) << "]";
			pLog->logParseLine(NAME_FIXED_FRAME_CHECKSUM,m_checkSum,ss.str());
        }
		else
		{
			pLog->logParseLine(NAME_FIXED_FRAME_CHECKSUM,m_checkSum);
		}
        
        if ( m_trailer != FRAME_END_CHAR_16H )
        {
 			setValid(false);
 			pLog->logParseLine(NAME_FIXED_FRAME_TRAILER,m_trailer,"[Incorrect Fixed Frame Trailer. Expected:16]");
        }
		else
		{
			pLog->logParseLine(NAME_FIXED_FRAME_TRAILER,m_trailer);
		}
        FUNCTION_EXIT;
    }
    
    void Iec102FixedLenFrame::createRawData()
    {
        FUNCTION_ENTRY( "createRawData" );
		std::vector<unsigned char> rawData;

		rawData.push_back(m_header);
		rawData.push_back(m_controlField);
		rawData.push_back(m_addrL);
		rawData.push_back(m_addrH);
		rawData.push_back(m_checkSum);
		rawData.push_back(m_trailer);			

		setRawData(rawData); 
        FUNCTION_EXIT;
    }	

    ITInforObject::ITInforObject( 
				const std::vector<unsigned char>& data, 
				unsigned char lowVirtualAddr, 
				unsigned char highVirtualAddr, 
				int arithSum, 
				TimeA* timeA )
		:
	Iec102Struct(NAME_IT_INFORMATION_OBJECT,INTEGREATED_TOTAL_WITH_ADDRESS_TYPE,data),
	m_arithSum(arithSum),
	m_timeA(timeA),
	m_lowVirtualAddr(lowVirtualAddr),
	m_highVirtualAddr(highVirtualAddr),
	m_size(INTEGRATED_TOTAL_WITH_IOA_SIZE),
	m_helper(this)	
    {
        FUNCTION_ENTRY( "ITInforObject" );
		m_helper.parseData(data);
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }	

    ITInforObject::ITInforObject( 
				const std::vector<unsigned char>& data, 
				unsigned char lowVirtualAddr, 
				unsigned char highVirtualAddr, 
				int arithSum, 
				unsigned char ioa, 
				TimeA* timeA)
		:
	Iec102Struct(NAME_IT_INFORMATION_OBJECT,INTEGREATED_TOTAL_NO_ADDRESS_TYPE,data),
	m_ioa(ioa),
	m_arithSum(arithSum),
	m_timeA(timeA),
	m_lowVirtualAddr(lowVirtualAddr),
	m_highVirtualAddr(highVirtualAddr),
	m_size(INTEGRATED_TOTAL_NO_IOA_SIZE),
	m_helper(this)		
    {
        FUNCTION_ENTRY( "ITInforObject" );
		m_helper.parseData(data);
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }	

	void ITInforObject::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper)
	{
		FUNCTION_ENTRY( "setupStruct" );
		
		if (pHelper != &m_helper) {	return;	}

		bool bValid = isValid();
		if (!bValid) { return; }

		E_IEC102_STRUCT_TYPE type = getStructType();
		if (INTEGREATED_TOTAL_NO_ADDRESS_TYPE == type )
		{
			m_powerData_LL	= data[ENGYDATA_POWER_LL - 1];
			m_powerData_LH	= data[ENGYDATA_POWER_LH - 1];
			m_powerData_HL	= data[ENGYDATA_POWER_HL - 1];
			m_powerData_HH	= data[ENGYDATA_POWER_HH - 1];
			m_frameCntAll	= data[ENGYDATA_FMCNT -1];
			m_checkSum		= data[ENGYDATA_CS -1];
		}
		else
		{
			m_ioa			= data[ENGYDATA_IOA];
			m_powerData_LL	= data[ENGYDATA_POWER_LL];
			m_powerData_LH	= data[ENGYDATA_POWER_LH];
			m_powerData_HL	= data[ENGYDATA_POWER_HL];
			m_powerData_HH	= data[ENGYDATA_POWER_HH];
			m_frameCntAll	= data[ENGYDATA_FMCNT];
			m_checkSum		= data[ENGYDATA_CS];		
		}
		unsigned short nVirtualAddress = static_cast<unsigned short>(m_highVirtualAddr)*256 + static_cast<unsigned short>(m_lowVirtualAddr);
		if (0 == nVirtualAddress)
		{
			nVirtualAddress = 1;
		}
		m_baseAddr = (nVirtualAddress - 1) * 255;

		producePowerData();
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
		checkAndLog();
		FUNCTION_EXIT;

	}

	void ITInforObject::checkAndLog()
	{
		FUNCTION_ENTRY( "checkAndLog" );
		std::stringstream ss;

		int sum =m_arithSum + m_powerData_LL + m_powerData_LH + m_powerData_HL + m_powerData_HH + m_frameCntAll;

		if (NULL != m_timeA)
		{
			std::string strTimeRawData = m_timeA->getRawData();
			for (unsigned int i = 0 ; i < strTimeRawData.length(); i++)
			{
				sum += static_cast<unsigned char>(strTimeRawData[i]);
			}
		}

		E_IEC102_STRUCT_TYPE type = getStructType();
		if (INTEGREATED_TOTAL_WITH_ADDRESS_TYPE == type )
		{
			ss << "IOA("  << Iec102Utility::uCharToStr(m_ioa) << ") ";
            sum += ( m_baseAddr + m_ioa ); // based on feedback, the ioa address should be converted to real address
		}

		ss << "FRAMECOUNT("  << Iec102Utility::uCharToStr(m_frameCntAll & 0x1F) << ") ";
        ss << "CHECKSUM(" << Iec102Utility::uCharToStr( m_checkSum ) << ") ";
		ss << "ADDRESS(" << std::setfill('0') << std::setw(3) << static_cast<unsigned int>(m_ioa) + m_baseAddr << ") ";
		std::string strValid = m_isValid?"Y":"N";
		ss << "VALID("  << strValid << ") ";
		ss << "VALUE("   <<  m_dataValue << ")";

		unsigned char mod = static_cast<unsigned char>(sum % 256);
        if ( m_checkSum != mod )
        {
 			setValid(false);
            ss << " [Incorrect CHECKSUM. Current:" << Iec102Utility::uCharToStr( m_checkSum ) <<" Expected:" << Iec102Utility::uCharToStr( mod ) << "]";
        }	
		Iec102FormatLog* pLog = getFormatLog();
		pLog->logParseLine(NAME_IT_INFORMATION_OBJECT,getRawData(),ss.str());
        FUNCTION_EXIT;
	
	}

	void ITInforObject::producePowerData()
	{
		FUNCTION_ENTRY( "producePowerData" );
        m_dataValue =  (static_cast<int>(m_powerData_HH) << 24) + (static_cast<int>(m_powerData_HL) << 16) + (static_cast<int>(m_powerData_LH) << 8)  + static_cast<int>(m_powerData_LL);

		m_address 	=  static_cast<unsigned int>(m_ioa);
		m_isValid	=  getDataValid();
 		m_frameCount = static_cast<unsigned int>(m_frameCntAll & 0x1F);

		m_data.address = m_baseAddr + m_address;
		m_data.dataValue = m_dataValue;
		m_data.valid = m_isValid;
		m_data.frameCnt = m_frameCount;

		if (NULL != m_timeA)
		{
			m_data.timeValue = m_timeA->getTimeT();
		}
		else
		{
			m_data.timeValue = time(NULL);
		}
        FUNCTION_EXIT;
	}

	bool ITInforObject::getDataValid()
	{
		FUNCTION_ENTRY( "getDataValid" );
		unsigned char dataValid = m_frameCntAll & 0x80;

		if (0x00 == dataValid)
		{
			FUNCTION_EXIT;
			return true;
		}
		else
		{
	        FUNCTION_EXIT;
			return false;
		}
	}
	
    RMInforObject::RMInforObject( const std::vector<unsigned char>& data, unsigned char lowVirtualAddr, unsigned char highVirtualAddr, TimeA* timeA )
		:
	Iec102Struct(NAME_RM_INFORMATION_OBJECT,REMOTE_MEASUREMENT_WITH_ADDRESS_TYPE,data),
	m_timeA(timeA),
	m_lowVirtualAddr(lowVirtualAddr),
	m_highVirtualAddr(highVirtualAddr),
	m_size(MEASUREMENT_WITH_IOA_SIZE),
	m_helper(this)		
    {
        FUNCTION_ENTRY( "RMInforObject" );
		m_helper.parseData(data);
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }	

    RMInforObject::RMInforObject( const std::vector<unsigned char>& data, unsigned char lowVirtualAddr, unsigned char highVirtualAddr, unsigned char ioa, TimeA* timeA)
		:
	Iec102Struct(NAME_RM_INFORMATION_OBJECT,REMOTE_MEASUREMENT_NO_ADDRESS_TYPE,data),
	m_ioa(ioa),
	m_timeA(timeA),
	m_lowVirtualAddr(lowVirtualAddr),
	m_highVirtualAddr(highVirtualAddr),
	m_size(MEASUREMENT_NO_IOA_SIZE),
	m_helper(this)		
    {
        FUNCTION_ENTRY( "RMInforObject" );
		m_helper.parseData(data);
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }	

	void RMInforObject::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper)
	{
		FUNCTION_ENTRY( "setupStruct" );
		if (pHelper != &m_helper) {	return;	}

		bool bValid = isValid();
		if (!bValid) { return; }
		
		E_IEC102_STRUCT_TYPE type = getStructType();
		if (REMOTE_MEASUREMENT_NO_ADDRESS_TYPE == type )
		{
			m_measureData_LL	= data[REMOTE_MEASSURE_DATA_LL - 1];
			m_measureData_LH	= data[REMOTE_MEASSURE_DATA_LH - 1];
			m_measureData_HL	= data[REMOTE_MEASSURE_DATA_HL - 1];
			m_measureData_HH	= data[REMOTE_MEASSURE_DATA_HH - 1];
			m_frameCntAll		= data[REMOTE_MEASSURE_FRAME_COUNT -1];
		}
		else
		{
			m_ioa				= data[REMOTE_MEASSURE_IOA];
			m_measureData_LL	= data[REMOTE_MEASSURE_DATA_LL];
			m_measureData_LH	= data[REMOTE_MEASSURE_DATA_LH];
			m_measureData_HL	= data[REMOTE_MEASSURE_DATA_HL];
			m_measureData_HH	= data[REMOTE_MEASSURE_DATA_HH];
			m_frameCntAll		= data[REMOTE_MEASSURE_FRAME_COUNT];		
		}
		unsigned short nVirtualAddress = static_cast<unsigned short>(m_highVirtualAddr)*256 + static_cast<unsigned short>(m_lowVirtualAddr);
		if (0 == nVirtualAddress)
		{
			nVirtualAddress = 1;
		}
		m_baseAddr = (nVirtualAddress - 1) * 255;
		produceMeasureData();

		checkAndLog();
		FUNCTION_EXIT;
	}

	void RMInforObject::checkAndLog()
	{
        FUNCTION_ENTRY( "checkAndLog" );
		std::stringstream ss;

		E_IEC102_STRUCT_TYPE type = getStructType();
		if (REMOTE_MEASUREMENT_WITH_ADDRESS_TYPE == type )
		{
			ss << "IOA("  << Iec102Utility::uCharToStr(m_ioa) << ") ";
		}
		ss << "FRAMECOUNT("  << Iec102Utility::uCharToStr(m_frameCntAll) << ") ";
		ss << "ADDRESS(" << std::setfill('0') << std::setw(3) << static_cast<unsigned int>(m_ioa) + m_baseAddr << ") ";
		std::string strValid = m_isValid?"Y":"N";
		ss << "VALID("  << strValid << ") ";
		ss << "VALUE("   <<  m_dataValue << ")";
		
		Iec102FormatLog* pLog = getFormatLog();
		pLog->logParseLine(NAME_RM_INFORMATION_OBJECT,getRawData(),ss.str());
        FUNCTION_EXIT;
	
	}

	void RMInforObject::produceMeasureData()
	{
        FUNCTION_ENTRY( "produceMeasureData" );
        m_dataValue =  (static_cast<int>(m_measureData_HH) << 24) + (static_cast<int>(m_measureData_HL) << 16) + (static_cast<int>(m_measureData_LH) << 8)  + static_cast<int>(m_measureData_LL);
		m_address 	=  static_cast<unsigned int>(m_ioa);
		m_isValid	=  getDataValid();
 		m_frameCount = static_cast<unsigned int>(m_frameCntAll & 0x1F);

		m_data.address = m_baseAddr + m_address;
		m_data.dataValue = m_dataValue;
		m_data.valid = m_isValid;
		m_data.frameCnt = m_frameCount;

		if (NULL != m_timeA)
		{
			m_data.timeValue = m_timeA->getTimeT();
		}
		else
		{
			m_data.timeValue = time(NULL);
		}
        FUNCTION_EXIT;
	}

	bool RMInforObject::getDataValid()
	{
		FUNCTION_ENTRY( "getDataValid" );
		unsigned char dataValid = m_frameCntAll & 0x80;

		if (0x00 == dataValid)
		{
			FUNCTION_EXIT;
			return true;
		}
		else
		{
	        FUNCTION_EXIT;
			return false;
		}

	}
	

    QueryWithAddrTimeA::QueryWithAddrTimeA(const std::vector<unsigned char>& data)
		:
	Iec102Struct(NAME_QUERY_WITH_ADDR_TIMEA,QUERY_WITH_ADDR_TIMEA_TYPE,data),
	m_startTime(NULL),
	m_endTime(NULL),	
	m_helper(this)		
    {
        FUNCTION_ENTRY( "QueryWithAddrTimeA" );
		m_helper.parseData(data);
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }	

    QueryWithAddrTimeA::QueryWithAddrTimeA(unsigned char startAddr, unsigned char endAddr, time_t startTime, time_t endTime)
		:
	Iec102Struct(NAME_QUERY_WITH_ADDR_TIMEA,QUERY_WITH_ADDR_TIMEA_TYPE),
	m_startAddr(startAddr),
	m_endAddr(endAddr),
	m_startTime(new TimeA(startTime)),
	m_endTime(new TimeA(endTime)),
	m_helper(this)		
    {
        FUNCTION_ENTRY( "QueryWithAddrTimeA" );
		createRawData();
		checkAndLog();
        FUNCTION_EXIT;
    }	

	QueryWithAddrTimeA::~QueryWithAddrTimeA()
	{
		FUNCTION_ENTRY( "~QueryWithAddrTimeA" );
		
		if (NULL != m_startTime)
		{
			delete m_startTime;
		}
		
		if (NULL != m_endTime)
		{
			delete m_endTime;
		}
		
        FUNCTION_EXIT;
	}

	void QueryWithAddrTimeA::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper)
	{
		FUNCTION_ENTRY( "setupStruct" );
		if (pHelper != &m_helper) {	return;	}
		
		bool bValid = isValid();
		if (!bValid) { return; }	
		
		m_startAddr = data[QUERY_START_ADDR];
		m_endAddr	= data[QUERY_END_ADDR];
		
		m_startTime = new TimeA( std::vector<unsigned char>( &data[QUERY_START_TIMEA_BEGIN], &data[QUERY_START_TIMEA_END+1] ));
		m_endTime	= new TimeA( std::vector<unsigned char>( &data[QUERY_END_TIMEA_BEGIN], &data[QUERY_END_TIMEA_END+1] ));

		checkAndLog();
		FUNCTION_EXIT;
	}

	void QueryWithAddrTimeA::checkAndLog()
	{
		FUNCTION_ENTRY( "checkAndLog" );
		Iec102FormatLog* pLog = getFormatLog();
		std::stringstream ss;
		ss << "START(" << int(m_startAddr) <<") END("	<< int(m_endAddr) << ")";
		pLog->logParseLine(NAME_QUERY_ADDRESS,m_startAddr,m_endAddr,ss.str());
		pLog->logAppend(m_startTime->getDebugInfo());
		pLog->logAppend(m_endTime->getDebugInfo());		
        FUNCTION_EXIT;
	}

    void QueryWithAddrTimeA::createRawData()
    {
        FUNCTION_ENTRY( "createRawData" );
		std::vector<unsigned char> rawData;

		rawData.push_back(m_startAddr);
		rawData.push_back(m_endAddr);

		std::string strTimeA = m_startTime->getRawData();
		for (unsigned int i = 0; i < strTimeA.length() ; i++)
		{
			rawData.push_back(static_cast<unsigned char>(strTimeA[i]));
		}

		strTimeA = m_endTime->getRawData();
		for (unsigned int i = 0; i < strTimeA.length() ; i++)
		{
			rawData.push_back(static_cast<unsigned char>(strTimeA[i]));
		}
				
		setRawData(rawData); 
        FUNCTION_EXIT;
    }	

    QueryWithAddr::QueryWithAddr(const std::vector<unsigned char>& data)
	:
	Iec102Struct(NAME_QUERY_WITH_ADDR,QUERY_WITH_ADDR_TYPE,data),
	m_helper(this)
    {
        FUNCTION_ENTRY( "QueryWithAddr" );
		m_helper.parseData(data);
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }	

    QueryWithAddr::QueryWithAddr(unsigned char startAddr, unsigned char endAddr)
		:
	Iec102Struct(NAME_QUERY_WITH_ADDR,QUERY_WITH_ADDR_TYPE),
	m_startAddr(startAddr),
	m_endAddr(endAddr),
	m_helper(this)
    {
        FUNCTION_ENTRY( "QueryWithAddr" );
		createRawData();
		checkAndLog();
        FUNCTION_EXIT;
    }	

	void QueryWithAddr::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper)
	{
		FUNCTION_ENTRY( "setupStruct" );
		if (pHelper != &m_helper) {	return;	}
		
		bool bValid = isValid();
		if (!bValid) { return; }

		m_startAddr	= data[QUERY_START_ADDR];
		m_endAddr	= data[QUERY_END_ADDR];

		checkAndLog();
		FUNCTION_EXIT;
	}

	void QueryWithAddr::checkAndLog()
	{
		FUNCTION_ENTRY( "checkAndLog" );
		Iec102FormatLog* pLog = getFormatLog();
		std::stringstream ss;
		ss << "START(" << int(m_startAddr) <<") END("	<< int(m_endAddr) << ")";
		pLog->logParseLine(NAME_QUERY_ADDRESS,m_startAddr,m_endAddr,ss.str());
        FUNCTION_EXIT;
	}

    void QueryWithAddr::createRawData()
    {
        FUNCTION_ENTRY( "createRawData" );
		std::vector<unsigned char> rawData;

		rawData.push_back(m_startAddr);
		rawData.push_back(m_endAddr);
		setRawData(rawData); 
        FUNCTION_EXIT;
    }	

    InitCompleted::InitCompleted(const std::vector<unsigned char>& data)
	:
	Iec102Struct(NAME_INIT_COMPLETED,INIT_COMPLETED_TYPE,data),
	m_helper(this)
    {
        FUNCTION_ENTRY( "InitCompleted" );
		m_helper.parseData(data);
        FUNCTION_EXIT;
    }	

	void InitCompleted::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper)
	{
		FUNCTION_ENTRY( "setupStruct" );
		if (pHelper != &m_helper) {	return;	}
		
		bool bValid = isValid();
		if (!bValid) { return; }

		m_ioa	= data[INIT_COMPLETED_IOA];
		m_coi	= data[INIT_COMPLETED_COI];

		checkAndLog();
		FUNCTION_EXIT;
	}

	void InitCompleted::checkAndLog()
	{
		FUNCTION_ENTRY( "checkAndLog" );
		Iec102FormatLog* pLog = getFormatLog();
		pLog->logParseLine(NAME_INIT_COMPLETED_IOA,m_ioa);
		pLog->logParseLine(NAME_INIT_COMPLETED_COI,m_coi);
        FUNCTION_EXIT;
	}

    Asdu::Asdu(const std::vector<unsigned char>& data, 
				unsigned char localVirtualLowAddr, 
				unsigned char localVirtualHighAddr)
	:
	Iec102Struct(NAME_ASDU,ASDU_TYPE,data),
	m_headerSum(0),
	m_pInforObjects(NULL),
	m_pTimeA(NULL),
	m_helper(this),
	m_lowVirtualAddr(localVirtualLowAddr),
	m_highVirtualAddr(localVirtualHighAddr),
	m_unknownInforObjectType(false),
	m_badCalcBodySize(false)
    {
        FUNCTION_ENTRY( "Asdu" );
		m_helper.parseData(data);
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }	
	
	Asdu::Asdu(unsigned char type,
		unsigned char localVirtualLowAddr,	
		unsigned char localVirtualHighAddr,
		unsigned char startAddr, 
		unsigned char endAddr)
	:
	Iec102Struct(NAME_ASDU,ASDU_TYPE),
	m_typeId(type),
	m_tadL(localVirtualLowAddr),
	m_tadH(localVirtualHighAddr),
	m_rad(0x00),
	m_headerSum(0),
	m_pInforObjects(NULL),
	m_pTimeA(NULL),
	m_helper(this),
	m_lowVirtualAddr(localVirtualLowAddr),
	m_highVirtualAddr(localVirtualHighAddr),
	m_unknownInforObjectType(false),
	m_badCalcBodySize(false)	
	{
        FUNCTION_ENTRY( "Asdu" );
		//only process 171 currently
		if (C_YC_NA_2 == type)
		{
			setValid(true);
			m_vsq 	 = 0x01;
			m_cot	 = COT_6;

			setValid(true);
 			m_pInforObjects = new std::vector<Iec102Struct*>();
			Iec102Struct* pInfo = new QueryWithAddr(startAddr,endAddr);
			m_pInforObjects->push_back(pInfo);
			createRawData();
			checkAndLog();			
			return;
		}
		setValid(false);
        FUNCTION_EXIT;		
	}
	
	Asdu::Asdu(unsigned char type,
		unsigned char localVirtualLowAddr,	
		unsigned char localVirtualHighAddr,
		unsigned char rad,
		unsigned char startAddr, 
		unsigned char endAddr,
		time_t startTime,
		time_t endTime)
	:
	Iec102Struct(NAME_ASDU,ASDU_TYPE),
	m_typeId(type),
	m_tadL(localVirtualLowAddr),
	m_tadH(localVirtualHighAddr),
	m_rad(rad),
	m_headerSum(0),
	m_pInforObjects(NULL),
	m_pTimeA(NULL),
	m_helper(this),
	m_lowVirtualAddr(localVirtualLowAddr),
	m_highVirtualAddr(localVirtualHighAddr),
	m_unknownInforObjectType(false),
	m_badCalcBodySize(false)
	{
        FUNCTION_ENTRY( "Asdu" );
		//only process 172/120 currently
		if (C_YC_TA_2 == type || C_CI_NR_2 == type)
		{
			setValid(true);
			m_vsq 	 = 0x01;
			m_cot	 = COT_6;
			
			m_pInforObjects = new std::vector<Iec102Struct*>();
			Iec102Struct* pInfo = new QueryWithAddrTimeA(startAddr,endAddr,startTime,endTime);
			
			m_pInforObjects->push_back(pInfo);
			createRawData();
			checkAndLog();
			FUNCTION_EXIT;		
			return;
		}
		setValid(false);

        FUNCTION_EXIT;		
	}
	


	Asdu::~Asdu()
	{
		FUNCTION_ENTRY( "~Asdu" );
		
		if (NULL != m_pTimeA)
		{
			delete m_pTimeA;
		}
		
		if (NULL != m_pInforObjects)
		{
			std::vector<Iec102Struct*>::const_iterator it = m_pInforObjects->begin();
			while (it != m_pInforObjects->end())
			{
				if (NULL != *it)
				{
					delete *it;
					//*it = NULL;
				}
				it++;
			}

			delete m_pInforObjects;
			m_pInforObjects = NULL;
		}

        FUNCTION_EXIT;
	
	}

	void Asdu::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper) 
	{
		FUNCTION_ENTRY( "setupStruct" );
		if (pHelper != &m_helper) {	return;	}
		
		bool bValid = isValid();
		if (!bValid) { return; }

		m_typeId = data[ASDU_TYPE_IDENTIFIER];
		m_vsq	 = data[ASDU_VSQ];
		m_cot	 = data[ASDU_COT];
		m_tadL	 = data[ASDU_ADDR_LOW];
		m_tadH 	 = data[ASDU_ADDR_HIGH];
		m_rad	 = data[ASDU_RAD];
		m_ioa	 = 0x00;

		if (m_unknownInforObjectType & m_badCalcBodySize)
		{
			m_unknownAsduBody.resize(300);
			std::copy(data.begin() + ASDU_BODY_START,data.end() - 1,m_unknownAsduBody.begin());

			checkAndLog();
			FUNCTION_EXIT;
			
			return;
		}
		m_headerSum = m_typeId + m_tadL + m_tadH + m_rad;

		getInforTypeLengthDefinition(data[ASDU_TYPE_IDENTIFIER],(data[ASDU_VSQ] & 0x80));
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_asduLen(ioaLen=%d, inforObjectLen=%d, publicTimeLen=%d, inforObjectType=%d)", 
			m_asduLen.ioaLen,m_asduLen.inforObjectLen,m_asduLen.publicTimeLen,m_asduLen.inforObjectType );
		
		if (0 != m_asduLen.ioaLen)
		{
			m_ioa = data[ASDU_IOA];
		}

		if (0 != m_asduLen.publicTimeLen)
		{
			unsigned int nDataLen = data.size();
			std::vector<unsigned char> dataTimeA(&data[nDataLen - TIMEA_SIZE],&data[nDataLen]);
			m_pTimeA = new TimeA(dataTimeA);
		}

		int nNum = int(data[ASDU_VSQ] & 0x7F);
		if (0 != nNum)
		{
			m_pInforObjects = new std::vector<Iec102Struct*>();
			int nHeaderSize = ASDU_MIN_SIZE + m_asduLen.ioaLen;
			int nInforObjectLen = m_asduLen.inforObjectLen;
			unsigned char ioa = m_ioa;
			for (int i = 0 ; i < nNum; i++)
			{
				std::vector<unsigned char> dataInforOject(&data[nHeaderSize + nInforObjectLen * i],&data[nHeaderSize + nInforObjectLen * i + nInforObjectLen]);
				Iec102Struct* pInfo;
				switch (m_typeId)
				{
				case M_EI_NA_2: 					//171
					pInfo = new InitCompleted(dataInforOject);
					
					break;

				case C_YC_NA_2: 					//171
					pInfo = new QueryWithAddr(dataInforOject);
					
					break;
				case C_YC_TA_2: 					//172
				case C_CI_NR_2: 					//120
					pInfo = new QueryWithAddrTimeA(dataInforOject);
					break;
					
				case M_YC_NA_2: 					//161 0xA1
					if (0 != m_asduLen.ioaLen)
					{
						pInfo = new RMInforObject(dataInforOject,m_lowVirtualAddr,m_highVirtualAddr,ioa);
						ioa++;
					}
					else
					{
						pInfo = new RMInforObject(dataInforOject,m_lowVirtualAddr,m_highVirtualAddr);
					}
					break;
					
				case M_YC_TA_2: 					//162 0xA2
					if (0 != m_asduLen.ioaLen)
					{
						pInfo = new RMInforObject(dataInforOject,m_lowVirtualAddr,m_highVirtualAddr,ioa,m_pTimeA);
						ioa++;
					}
					else
					{
						pInfo = new RMInforObject(dataInforOject,m_lowVirtualAddr,m_highVirtualAddr,m_pTimeA);
					}				
					break;
					
				case M_IT_TA_2: 					//02 0x02
					if (0 != m_asduLen.ioaLen)
					{
						pInfo = new ITInforObject(dataInforOject,m_lowVirtualAddr,m_highVirtualAddr,m_headerSum,ioa,m_pTimeA);
						ioa++;
					}
					else
					{
						pInfo = new ITInforObject(dataInforOject,m_lowVirtualAddr,m_highVirtualAddr,m_headerSum,m_pTimeA);
					}				
					break;

				default:
					break;
				}
				m_pInforObjects->push_back(pInfo);
			}
		}
		
		checkAndLog();
		FUNCTION_EXIT;

	}

	void Asdu::logBadAsduBody(const std::string strDesc) 
	{
		FUNCTION_ENTRY( "logBadAsduBody" );
		Iec102FormatLog* pLog = getFormatLog();
		pLog->logErrorInfo(strDesc,m_unknownAsduBody);
		FUNCTION_EXIT;
	}

	void Asdu::checkAndLog()
	{
		FUNCTION_ENTRY( "checkAndLog" );
		Iec102FormatLog* pLog = getFormatLog();

		//Type Identification
		std::stringstream ss;
		ss << "TYPE ID(" <<static_cast<unsigned int>(m_typeId) << ")";
		if (m_unknownInforObjectType)
		{
			setValid(false);
			ss  << " [Unknown Type Identification]";
		}

		pLog->logParseLine(NAME_TYPE_IDENTIFIER,m_typeId,ss.str());
		
		//VSQ
		ss.str("");
		ss << "SQ(" << Iec102Utility::uCharToStr(m_vsq & 0x80) << ") NUM(" << Iec102Utility::uCharToStr(m_vsq & 0x7F) << ")";

		pLog->logParseLine(NAME_VSQ,m_vsq,ss.str());

		//COT
		pLog->logParseLine(NAME_COT,m_cot);

		//Virtual Address

		if ( m_typeId != M_EI_NA_2 && (m_lowVirtualAddr != m_tadL || m_highVirtualAddr != m_tadH) )
		{
			setValid(false);
			std::stringstream ss;
			ss << "[Incorrect Virtual Address. Expected: " << Iec102Utility::uCharToStr( m_lowVirtualAddr ) 
				<< " "<< Iec102Utility::uCharToStr( m_highVirtualAddr ) << "]";

			pLog->logParseLine(NAME_VIRTUAL_ADDR,m_tadL,m_tadH,ss.str());
		}        
		else
		{
			pLog->logParseLine(NAME_VIRTUAL_ADDR,m_tadL,m_tadH);
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "%s", "Start to output infor object" );

 
		//RAD
		pLog->logParseLine(NAME_RAD,m_rad);

		//if Asdu Body is unknown, just log it.
		if (m_unknownInforObjectType)
		{
			setValid(false);
			logBadAsduBody(NAME_UNKNOWN_ASDU_BODY);
			return;
		}

		if (m_badCalcBodySize)
		{
			setValid(false);
			logBadAsduBody(NAME_BAD_CALC_ASDU_BODY);
			return;
		}

		//InforObject
		if (0 != m_pInforObjects->size())
		{
			for (unsigned int i = 0; i < m_pInforObjects->size(); i++)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "No.%d Infor Object", i );
				Iec102Struct* pInfoObject = m_pInforObjects->at(i);
				bool bValid = pInfoObject->isValid();
				if (!bValid)
				{
					setValid(false);					
				}
				
				pLog->logAppend(pInfoObject->getDebugInfo());
			}
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "%s", "Finish to output infor object" );
		
		if (NULL != m_pTimeA)
		{
			if (!m_pTimeA->isValid())
			{
				setValid(false);
			}
			pLog->logAppend( m_pTimeA->getDebugInfo());
		}

		FUNCTION_EXIT;
	}

	void Asdu::getInforTypeLengthDefinition(unsigned char ucType, unsigned char ucStruct)
	{
		FUNCTION_ENTRY( "getInforTypeLengthDefinition" );

		m_asduLen.ioaLen = 0;
		m_asduLen.inforObjectLen = 0;
		m_asduLen.publicTimeLen = 0;
		m_asduLen.inforObjectType = UNKNOWN_TYPE;
		switch (ucType)
		{
		case M_EI_NA_2: 					//70
			m_asduLen.inforObjectLen = INIT_COMPLETED_SIZE;
			m_asduLen.inforObjectType= INIT_COMPLETED_TYPE;
			break;

		case C_YC_NA_2: 					//171
			m_asduLen.inforObjectLen = QUERY_WITH_ADDR_SIZE;
			m_asduLen.inforObjectType= QUERY_WITH_ADDR_TYPE;
			break;
			
		case C_YC_TA_2: 					//172
		case C_CI_NR_2: 					//120
			m_asduLen.inforObjectLen = QUERY_WITH_ADDR_TIMEA_SIZE;
			m_asduLen.inforObjectType= QUERY_WITH_ADDR_TIMEA_TYPE;
			break;
			
		case M_YC_NA_2: 					//161
			if (0x80 == ucStruct)
			{
				m_asduLen.ioaLen = 1;
				m_asduLen.inforObjectLen = MEASUREMENT_NO_IOA_SIZE;
				m_asduLen.inforObjectType= REMOTE_MEASUREMENT_NO_ADDRESS_TYPE;
			}
			else
			{
				m_asduLen.inforObjectLen = MEASUREMENT_WITH_IOA_SIZE;
				m_asduLen.inforObjectType= REMOTE_MEASUREMENT_WITH_ADDRESS_TYPE;
			}
			break;
			
		case M_YC_TA_2: 					//162
			if (0x80 == ucStruct)
			{
				m_asduLen.ioaLen = 1;
				m_asduLen.inforObjectLen = MEASUREMENT_NO_IOA_SIZE;
				m_asduLen.inforObjectType= REMOTE_MEASUREMENT_NO_ADDRESS_TYPE;
			}
			else
			{
				m_asduLen.inforObjectLen = MEASUREMENT_WITH_IOA_SIZE;
				m_asduLen.inforObjectType= REMOTE_MEASUREMENT_WITH_ADDRESS_TYPE;
			}
			m_asduLen.publicTimeLen = TIMEA_SIZE;
			break;
			
		case M_IT_TA_2: 					//02
			if (0x80 == ucStruct)
			{
				m_asduLen.ioaLen = 1;
				m_asduLen.inforObjectLen = INTEGRATED_TOTAL_NO_IOA_SIZE;
				m_asduLen.inforObjectType= INTEGREATED_TOTAL_NO_ADDRESS_TYPE;
			}
			else
			{
				m_asduLen.inforObjectLen = INTEGRATED_TOTAL_WITH_IOA_SIZE;
				m_asduLen.inforObjectType= INTEGREATED_TOTAL_WITH_ADDRESS_TYPE;
			}
			m_asduLen.publicTimeLen = TIMEA_SIZE;
			break;
			
		default:
			break;
		}

		FUNCTION_EXIT;
		return ;
	}

    void Asdu::createRawData()
    {
        FUNCTION_ENTRY( "createRawData" );
		std::vector<unsigned char> rawData;

		rawData.push_back(m_typeId);
		rawData.push_back(m_vsq);
		rawData.push_back(m_cot);
		rawData.push_back(m_tadL);
		rawData.push_back(m_tadH);
		rawData.push_back(m_rad);

		if (0 != m_pInforObjects->size())
		{
			for (unsigned int i = 0; i < m_pInforObjects->size(); i++)
			{
				Iec102Struct* pInfoObject = m_pInforObjects->at(i);

				std::string dataInforObject = pInfoObject->getRawData();
				for (unsigned int j = 0; j < dataInforObject.length() ; j++)
				{
					rawData.push_back(static_cast<unsigned char>(dataInforObject[j]));
				}
			}
		}

		setRawData(rawData); 
        FUNCTION_EXIT;
    }	

	std::vector<IEC102Data> Asdu::getData()
	{
		std::vector<IEC102Data> dataTemp;

		if (isValid())
		{
			if (M_YC_NA_2 == m_typeId || M_YC_TA_2 == m_typeId || M_IT_TA_2 == m_typeId)
			{
				for (unsigned int i = 0 ; i < m_pInforObjects->size(); i++)
				{
					Iec102Struct* pInfoObject = m_pInforObjects->at(i);
					std::vector<IEC102Data> dataObject = pInfoObject->getData();

					for (unsigned int j = 0 ; j < dataObject.size(); j++)
					{
						dataTemp.push_back(dataObject[j]);
					}				
				}
			}
		}
		return dataTemp;
	}

    Iec102VariableFrame::Iec102VariableFrame(const std::vector<unsigned char>& data,
														E_IEC102_DIRECTION direction,
														unsigned char localLowAddr, 
														unsigned char localHighAddr,
														unsigned char localVirtualLowAddr, 
														unsigned char localVirtualHighAddr)
	:
	Iec102Struct(NAME_VAR_FRAME,VARIABLE_FRAME_TYPE,data),
	m_pControlField(NULL),
	m_pAsdu(NULL),
	m_localLowAddr(localLowAddr),
	m_localHighAddr(localHighAddr),
	m_virtualLowAddr(localVirtualLowAddr),
	m_virtualHighAddr(localVirtualHighAddr),
	m_direction(direction),
	m_helper(this),
	m_badCalcBodySize(false)
    {
        FUNCTION_ENTRY( "Iec102VariableFrame" );
		m_helper.parseData(data);

		printDebugInfo();
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
        FUNCTION_EXIT;
    }	

	//used for create 120/162 frame
	Iec102VariableFrame::Iec102VariableFrame(
								E_IEC102_FCB fcb, 
								E_IEC102_FCV fcv, 
								unsigned char funCode,
								unsigned char type,
								unsigned char localLowAddr, 
								unsigned char localHighAddr,
								unsigned char localVirtualLowAddr,
								unsigned char localVirtualHighAddr,
								unsigned char startAddr, 
								unsigned char endAddr)
	:
	Iec102Struct(NAME_VAR_FRAME,VARIABLE_FRAME_TYPE),
	m_headerStart(FRAME_START_CHAR_68H),		  
	m_headerEnd(FRAME_START_CHAR_68H),		  
	m_pControlField(new ControlFieldSend(fcb,fcv,funCode,EV_FORMAT)),
	m_addrL(localLowAddr),
	m_addrH(localHighAddr),
	m_pAsdu(new Asdu(type,localVirtualLowAddr,localVirtualHighAddr,startAddr,endAddr)),
	m_trailer(FRAME_END_CHAR_16H),	
	m_localLowAddr(localLowAddr),
	m_localHighAddr(localHighAddr),
	m_virtualLowAddr(localVirtualLowAddr),
	m_virtualHighAddr(localVirtualHighAddr),
	m_direction(IEC102_CONTROL_DIRECTION),
	m_helper(this),
	m_badCalcBodySize(false)								
	{
        FUNCTION_ENTRY( "Iec102VariableFrame" );
		m_controlField = getControlFieldUC();
		unsigned int asduSize = (m_pAsdu->getRawData()).length();
		m_headerLen1 = static_cast<unsigned char>(asduSize + 3);
		m_headerLen2 = m_headerLen1;
		m_checkSum = setCheckSum();
		createRawData();
		checkAndLog();		
		printDebugInfo();
        FUNCTION_EXIT;
	}
	
	//Used for create 161
	Iec102VariableFrame::Iec102VariableFrame(
								E_IEC102_FCB  fcb, 
								E_IEC102_FCV  fcv, 
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
								time_t endTime)
	:
	Iec102Struct(NAME_VAR_FRAME,VARIABLE_FRAME_TYPE),
	m_headerStart(FRAME_START_CHAR_68H),		  
	m_headerEnd(FRAME_START_CHAR_68H),		  
	m_pControlField(new ControlFieldSend(fcb,fcv,funCode,EV_FORMAT)),
	m_addrL(localLowAddr),
	m_addrH(localHighAddr),
	m_pAsdu(new Asdu(type,localVirtualLowAddr,localVirtualHighAddr,rad,startAddr,endAddr,startTime,endTime)),
	m_trailer(FRAME_END_CHAR_16H),		
	m_localLowAddr(localLowAddr),	
	m_localHighAddr(localHighAddr),
	m_virtualLowAddr(localVirtualLowAddr),
	m_virtualHighAddr(localVirtualHighAddr),
	m_direction(IEC102_CONTROL_DIRECTION),
	m_helper(this),
	m_badCalcBodySize(false)
	{
        FUNCTION_ENTRY( "Iec102VariableFrame" );
		m_controlField = getControlFieldUC();
		unsigned int asduSize = (m_pAsdu->getRawData()).length();
		m_headerLen1 = static_cast<unsigned char>(asduSize + 3);
		m_headerLen2 = m_headerLen1;
		m_checkSum = setCheckSum();
		createRawData();
		checkAndLog();		
		printDebugInfo();		
        FUNCTION_EXIT;
	}							

	Iec102VariableFrame::~Iec102VariableFrame()
	{
		FUNCTION_ENTRY( "~Asdu" );

		if (NULL != m_pControlField)
		{
			delete m_pControlField;
		}

		if (NULL != m_pAsdu)
		{
			delete m_pAsdu;
		}

        FUNCTION_EXIT;
	
	}

	EFlowControlState Iec102VariableFrame::getFlowControlState()
	{
		FUNCTION_ENTRY( "getFlowControlState" );
 		bool bValid = isFrameValid();

		if (!bValid)
		{
			return DATA_INVALID;
		}
		
		ControlFieldRecv* pControlField = dynamic_cast<ControlFieldRecv*>(m_pControlField);

		Asdu* pAsdu = getAsdu();
		unsigned char cot = pAsdu->getCOT();
		
		E_IEC102_ACD acd = pControlField->getACD();
		E_IEC102_DFC dfc = pControlField->getDFC();
		//unsigned char funcCode = pControlField->getFuncode();

		if (IEC102_DFC_TRUE == dfc)
		{
			FUNCTION_EXIT;
			return E_M_BUFFER_FULL;
		}	

		if (IEC102_ACD_FALSE == acd && COT_4 == cot)
		{
			FUNCTION_EXIT;
			return E_M_INIT_COMPLETED;
		}

		if (COT_7 == cot)
		{
			FUNCTION_EXIT;
			return E_M_RESPONSE_DATA_COT7;
		}

		if (COT_10 == cot)
		{
			FUNCTION_EXIT;
			return E_M_RESPONSE_DATA_COTA;
		}

		if (COT_13 == cot)
		{
			FUNCTION_EXIT;
			return E_M_RESPONSE_DATA_COTD;
		}

		if (COT_5 == cot)
		{
			FUNCTION_EXIT;
			return E_M_RESPONSE_DATA_COT5;
		}

		FUNCTION_EXIT;
		return E_M_UNKNOWN_CONTROL_STATE;

	}	
	void Iec102VariableFrame::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper) 
	{
		FUNCTION_ENTRY( "setupStruct" );
		if (pHelper != &m_helper) {	return;	}
		
		bool bValid = isValid();
		if (!bValid) { return; }

		unsigned int dataSize = data.size();
        m_headerStart	= data[VAR_FRAME_HEADER_1];        
		m_headerLen1	= data[VAR_FRAME_SIZE_1];
        m_headerLen2	= data[VAR_FRAME_SIZE_2];        
		m_headerEnd		= data[VAR_FRAME_HEADER_2];
		m_controlField	= data[VAR_FRAME_CONFIELD];
		m_addrL			= data[VAR_FRAME_ADDRL];		
		m_addrH			= data[VAR_FRAME_ADDRH];
		m_checkSum		= data[dataSize - VAR_FRAME_CHECKSUM_BACKWARDS];		
		m_trailer		= data[dataSize - VAR_FRAME_TRAILER_BACKWARDS];		

		if (m_direction == IEC102_MONITOR_DIRECTION )
		{
			m_pControlField = new ControlFieldRecv( data[VAR_FRAME_CONFIELD], EV_FORMAT );
		}	
		else
		{
			m_pControlField = new ControlFieldSend( data[VAR_FRAME_CONFIELD], EV_FORMAT );
		}

		if (m_badCalcBodySize)
		{
			m_unknownAsduBody.resize(300);
			std::copy(data.begin() + VAR_FRAME_ASDU_START,data.end() - VAR_FRAME_CHECKSUM_BACKWARDS,m_unknownAsduBody.begin());

			checkAndLog();
			FUNCTION_EXIT;
			return;
		}
		std::vector<unsigned char> dataAsdu(&data[VAR_FRAME_ASDU_START],&data[dataSize - VAR_FRAME_ASDU_END_BACKWARDS + 1]);
		m_pAsdu = new Asdu(dataAsdu,m_virtualLowAddr,m_virtualHighAddr);
		
		checkAndLog();
		FUNCTION_EXIT;

	}

	unsigned char Iec102VariableFrame::verifyCheckSum()
	{
		FUNCTION_ENTRY( "verifyCheckSum");
		std::vector<unsigned char>* pRawData = getRawDataVector();
	
		int sum = 0;
		std::vector<unsigned char>::iterator it = pRawData->begin() + VAR_FRAME_CONFIELD ;
		for (; it != pRawData->end() - VAR_FRAME_CHECKSUM_BACKWARDS; it++)
		{
			sum += *it;
		}
		unsigned char checkSum = static_cast<unsigned char>(sum % 256);
		FUNCTION_EXIT;
		return checkSum;
	}

	unsigned char Iec102VariableFrame::setCheckSum()
	{
		FUNCTION_ENTRY( "getCheckSum");

		int sum =m_controlField + m_addrL + m_addrH;

		std::string strAsdu = m_pAsdu->getRawData();
		for (unsigned int j = 0; j < strAsdu.length() ; j++)
		{
			sum += static_cast<unsigned char>(strAsdu[j]);
		}

		unsigned char checkSum = static_cast<unsigned char>(sum % 256);
		FUNCTION_EXIT;
		return checkSum;
	}

	void Iec102VariableFrame::checkAndLog()
	{
		FUNCTION_ENTRY( "checkAndLog" );
		Iec102FormatLog* pLog = getFormatLog();
		
        //pLog->logStartLine("List the detail information of the Variable Length Frame:");

        if ( (m_headerStart != FRAME_START_CHAR_68H) ||
			 (m_headerEnd   != FRAME_START_CHAR_68H) ||
			 (m_headerLen1   != m_headerLen2))
        {
 			setValid(false);
 			pLog->logParseLine(NAME_VAR_FRAME_HEADER,m_headerStart,m_headerLen1,m_headerLen2,m_headerEnd,"[Incorrect Variable Frame Header]");
        }
		else
		{
			std::stringstream ss;
			ss << "USER DATA LENGTH(" << static_cast<unsigned int>(m_headerLen1) << ")";
		
			pLog->logParseLine(NAME_VAR_FRAME_HEADER,m_headerStart,m_headerLen1,m_headerLen2,m_headerEnd,ss.str());
		}
		
		if (!m_pControlField->isValid())
		{
 			setValid(false);
		}
		pLog->logAppend( m_pControlField->getDebugInfo());

		if (m_localLowAddr != m_addrL || m_localHighAddr != m_addrH)
		{
 			setValid(false);
			std::stringstream ss;
			ss << "[Incorrect Address. Expected Address:" << Iec102Utility::uCharToStr( m_localLowAddr ) 
				<< " "<< Iec102Utility::uCharToStr( m_localHighAddr ) << "]";

			pLog->logParseLine(NAME_EQUIPMENT_ADDRESS,m_addrL,m_addrH,ss.str());
		}        
		else
		{
			pLog->logParseLine(NAME_EQUIPMENT_ADDRESS,m_addrL,m_addrH);
		}

		if (m_badCalcBodySize)
		{
			setValid(false);
			pLog->logErrorInfo(NAME_BAD_CALC_VAR_FARME,m_unknownAsduBody);
			return;
		}
		if (!m_pAsdu->isValid())
		{
 			setValid(false);
		}
		pLog->logAppend( m_pAsdu->getDebugInfo());
		
        unsigned char checkSum = verifyCheckSum();
        if ( m_checkSum != checkSum )
        {
 			setValid(false);
			std::stringstream ss;
            ss << "[Incorrect CheckSum. Received:" << Iec102Utility::uCharToStr( m_checkSum ) << ",Expected:" << Iec102Utility::uCharToStr( checkSum ) << "]";
			pLog->logParseLine(NAME_VAR_FRAME_CHECKSUM,m_checkSum,ss.str());
        }
		else
		{
			pLog->logParseLine(NAME_VAR_FRAME_CHECKSUM,m_checkSum);
		}
        
        if ( m_trailer != FRAME_END_CHAR_16H )
        {
 			setValid(false);
			pLog->logParseLine(NAME_VAR_FRAME_TRAILER,m_trailer,"[Incorrect Variable Frame Trailer. Expected:16]");
        }
		else
		{
			pLog->logParseLine(NAME_VAR_FRAME_TRAILER,m_trailer);
		}		
		FUNCTION_EXIT;
	}

	std::vector<IEC102Data> Iec102VariableFrame::getData()
	{
		
		std::vector<IEC102Data> dataTemp;

		if (isValid())
		{
			dataTemp = m_pAsdu->getData();
		}

		return dataTemp;
	}

	std::vector<PowerEnergyData> Iec102VariableFrame::getEnergyData()
	{
		
		std::vector<PowerEnergyData> dataTemp;
		std::vector<IEC102Data> dataIEC102 = getData();
		for (unsigned int i = 0; i < dataIEC102.size(); i++)
		{
			PowerEnergyData energyData;
			energyData.valid = dataIEC102[i].valid;
			energyData.address = dataIEC102[i].address;
			energyData.PowerResult = dataIEC102[i].dataValue;
			energyData.dataTime = dataIEC102[i].timeValue;
			
			dataTemp.push_back(energyData);
		}
		
		return dataTemp;
	}
	
    void Iec102VariableFrame::createRawData()
    {
        FUNCTION_ENTRY( "createRawData" );
		std::vector<unsigned char> rawData;

		rawData.push_back(m_headerStart);
		rawData.push_back(m_headerLen1);
		rawData.push_back(m_headerLen2);
		rawData.push_back(m_headerEnd);
		rawData.push_back(m_controlField);
		rawData.push_back(m_addrL);
		rawData.push_back(m_addrH);

		std::string strAsdu = m_pAsdu->getRawData();
		for (unsigned int j = 0; j < strAsdu.length() ; j++)
		{
			rawData.push_back(static_cast<unsigned char>(strAsdu[j]));
		}

		rawData.push_back(m_checkSum);
		rawData.push_back(m_trailer);

		setRawData(rawData);  
        FUNCTION_EXIT;
    }	

	unsigned char Iec102VariableFrame::getControlFieldUC()
	{
		FUNCTION_ENTRY( "getControlFieldUC" );
 		std::string strControlField = m_pControlField->getRawData();
        FUNCTION_EXIT;
		return static_cast<unsigned char>(strControlField[0]);
	}

    Iec102SingleFrame::Iec102SingleFrame(const unsigned char data)
		:
	Iec102Struct(NAME_SINGLE_FRAME,SINGLE_FARME_TYPE),
	m_singleE5(data),
	m_helper(this)	
    {
        FUNCTION_ENTRY( "Iec102SingleFrame" );
		createRawData();
		checkAndLog();
        FUNCTION_EXIT;
    }	

	EFlowControlState Iec102SingleFrame::getFlowControlState()
	{
		FUNCTION_ENTRY( "getFlowControlState" );
 		bool bValid = isFrameValid();
		if (bValid)
		{
			FUNCTION_EXIT;
			return E_M_0E5H_CON;
		}
		else
		{
			FUNCTION_EXIT;
			return DATA_INVALID;		
		}
	}


    Iec102SingleFrame::Iec102SingleFrame(const std::vector<unsigned char>& data)
		:
	Iec102Struct(NAME_SINGLE_FRAME,SINGLE_FARME_TYPE,data),
	m_helper(this)	
    {
        FUNCTION_ENTRY( "Iec102SingleFrame" );
		m_helper.parseData(data);
		printDebugInfo();
        FUNCTION_EXIT;
    }	

    void Iec102SingleFrame::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper)
    {
        FUNCTION_ENTRY( "setupStruct" );
		if (pHelper != &m_helper) {	return;	}
		bool bValid = isValid();
		
		if (!bValid) { return; }

		m_singleE5 = data[SINGLE_FRAME_START];

		checkAndLog();
		FUNCTION_EXIT;

        FUNCTION_EXIT;
    }	

	void Iec102SingleFrame::createRawData() 
	{
		FUNCTION_ENTRY( "createRawData" );
		std::vector<unsigned char> rawData;

		rawData.push_back(m_singleE5);
		setRawData(rawData); 

		FUNCTION_EXIT;
	}

	void Iec102SingleFrame::checkAndLog() 
	{
		FUNCTION_ENTRY( "checkAndLog" );
		Iec102FormatLog* pLog = getFormatLog();
		if (FRAME_SINGLE_CHAR != m_singleE5)
		{
			setValid(false);
			pLog->logParseLine(NAME_SINGLE_FRAME,m_singleE5,"[Incorrect Single Frame. Expected:0xE5]");
		}
		else
		{
			pLog->logParseLine(NAME_SINGLE_FRAME,m_singleE5);
		}

		FUNCTION_EXIT;
	}

	ControlStateObserver::ControlStateObserver(unsigned long timeOutMs)
		:  m_timeOutMs(timeOutMs),
		   m_semaphore(0)
	{
		FUNCTION_ENTRY("ControlStateObserver()");
		FUNCTION_EXIT;
	}

	ControlStateObserver::~ControlStateObserver()	
	{
		FUNCTION_ENTRY("~ControlStateObserver()");
		m_semaphore.post();
		FUNCTION_EXIT;		
	}

	bool ControlStateObserver::waitForReply()
	{
		FUNCTION_ENTRY("waitForReply()");
		//int result = m_condition.timedWait(m_timeOutMs);
		bool result = m_semaphore.timedWait(m_timeOutMs);

		if (result)
		{
			FUNCTION_EXIT;
			return true;
		}
		else
		{
			//m_isValid = false;
			FUNCTION_EXIT;
			return false;
		}
	}

	bool ControlStateObserver::notifyStateChange(EFlowControlState currentState)
	{
		FUNCTION_ENTRY("notifyStateChange()");
		if ( m_stateObserved.find(currentState) != m_stateObserved.end())
		{
			m_notifyState = currentState;
			m_semaphore.post();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Notify State %d(%s)",currentState,getControlStateDescription(currentState).c_str());
			FUNCTION_EXIT;
			return true;
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "can not notify State %d(%s)",currentState,getControlStateDescription(currentState).c_str());
			FUNCTION_EXIT;
			return false;
		}
	}

	void ControlStateObserver::addObservedState(EFlowControlState state)
	{
		FUNCTION_ENTRY("addObservedState()");
		m_stateObserved.insert(state);
		FUNCTION_EXIT;
	}
	
	std::string ControlStateObserver::getControlStateDescription(EFlowControlState currentState)
	{
		std::string strDesc;
		switch (currentState)
		{
		case E_M_UNKNOWN_CONTROL_STATE:
			strDesc = "E_M_UNKNOWN_CONTROL_STATE";
			break;

		case E_M_BUFFER_FULL:
			strDesc = "E_M_BUFFER_FULL";
			break;

		case E_M_ACTIVE_I_DATA_RESPONSE_LINK_STATE:
			strDesc = "E_M_ACTIVE_I_DATA_RESPONSE_LINK_STATE";
			break;

		case E_M_RESPONSE_LINK_STATE:
			strDesc = "E_M_RESPONSE_LINK_STATE";
			break;

		case E_M_ACTIVE_I_DATA_RESPONSE_RESET_LINK:
			strDesc = "E_M_ACTIVE_I_DATA_RESPONSE_RESET_LINK";
			break;

		case E_M_RESPONSE_RESET_LINK:
			strDesc = "E_M_RESPONSE_RESET_LINK";
			break;

		case E_M_LINK_BUSY:
			strDesc = "E_M_LINK_BUSY";
			break;

		case E_M_NO_DATA:
			strDesc = "E_M_NO_DATA";
			break;			

		case E_M_INIT_COMPLETED:
			strDesc = "E_M_INIT_COMPLETED";
			break;

		case E_M_ACTIVE_I_DATA:
			strDesc = "E_M_ACTIVE_I_DATA";
			break;

		case E_M_RESPONSE_DATA_COT4:
			strDesc = "E_M_RESPONSE_DATA_COT4";
			break;

		case E_M_RESPONSE_DATA_COT5:
			strDesc = "E_M_RESPONSE_DATA_COT5";
			break;

		case E_M_RESPONSE_DATA_COT7:
			strDesc = "E_M_RESPONSE_DATA_COT7";
			break;

		case E_M_RESPONSE_DATA_COTA:
			strDesc = "E_M_RESPONSE_DATA_COTA";
			break;

		case E_M_RESPONSE_DATA_COTD:
			strDesc = "E_M_RESPONSE_DATA_COTD";
			break;			

		case E_M_0E5H_CON:
			strDesc = "E_M_0E5H_CON";
			break;			

		case CONNECTION_LOST:
			strDesc = "CONNECTION_LOST";
			break;			

		case DATA_INVALID:
			strDesc = "DATA_INVALID";
			break;			

		case STOP_PROCESS:
			strDesc = "STOP_PROCESS";
			break;			

		case WRITE_FAILED:
			strDesc = "WRITE_FAILED";
			break;			

		default:
			strDesc = "UNKNOWN STATE";
			break;
		}
		return strDesc;
	}

    std::string Iec102Utility::uCharVecToStr( const std::vector<unsigned char>& vec )
    {
        FUNCTION_ENTRY( "Iec102Utility::uCharVecToStr" );
        std::ostringstream ss;
        for ( unsigned int i = 0; i < vec.size( ); i++ )
        {
			ss << std::uppercase << std::setfill( '0' ) << std::setw(2) << std::hex << (unsigned int)vec[i] << " ";
        }
        std::string result = ss.str();
        FUNCTION_EXIT;
        return result;
    }
    
    std::string Iec102Utility::uCharToStr( const unsigned char uChar )
    {
        FUNCTION_ENTRY( "Iec102Utility::uCharToStr" );
        std::stringstream ss;
        ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)uChar;
        std::string result = ss.str();
        FUNCTION_EXIT;
        return result;
    }


	// **************
	// TimeA
	// **************
	TimeA::TimeA( const std::vector<unsigned char>& data )
	:
	Iec102Struct(NAME_TIMEA,TIMEA_TYPE,data),
	m_helper(this)
	{
		FUNCTION_ENTRY( "TimeA" );
		m_helper.parseData(data);
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
		FUNCTION_EXIT;
	}
	
	TimeA::TimeA(time_t timeValue)
	:
	Iec102Struct(NAME_TIMEA,TIMEA_TYPE),
	m_timeT(timeValue),
	m_helper(this)
	{
		FUNCTION_ENTRY( "TimeA" );
		struct tm* timeInfo;
		timeInfo = localtime(&timeValue);

		m_minute = (unsigned char)(timeInfo->tm_min);
		m_hour = (unsigned char)(timeInfo->tm_hour);
		
		m_day = 0x1F & (unsigned char)(timeInfo->tm_mday);								//day of the month, 5 lower bits

		if (timeInfo->tm_wday == 0)														//tm_wday=0 for sunday in tm
			timeInfo->tm_wday = 7;
		m_week = 0x07 & (unsigned char)(timeInfo->tm_wday);


		m_month = (unsigned char)(timeInfo->tm_mon+1);									//tm_mon=0 for January
		if (timeInfo->tm_year >= 100)													//tm_year is year since 1900
			timeInfo->tm_year -= 100;													
		m_year = (unsigned char)(timeInfo->tm_year);	
		
		createRawData();
		checkAndLog();
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
		FUNCTION_EXIT;
	}

	void TimeA::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper) 
	{
		FUNCTION_ENTRY( "setupStruct" );
		bool bValid = isValid();
		if (!bValid) { return; }

		m_year	= data[TIMEA_YEAR] & 0x7F;
		m_month = data[TIMEA_MONTH] & 0x0F;
		m_week	= data[TIMEA_WEEK] >> 5;
		m_day	= data[TIMEA_DAY] & 0x1F;
		m_hour	= data[TIMEA_HOUR] & 0x1F;
		m_minute= data[TIMEA_MIN] & 0x3F;		
		struct tm tm1;
		tm1.tm_sec = 0;
		tm1.tm_min = int(m_minute);
		tm1.tm_hour = int(m_hour);
		tm1.tm_mday = int(m_day); 
		tm1.tm_mon = int(m_month - 1);
		tm1.tm_year = 2000 + int(m_year) - 1900;
		m_timeT = mktime ( &tm1 );	
		
		checkAndLog();
		FUNCTION_EXIT;
	}

	std::string TimeA::getTimeString( )
	{
		char timeString[32] = {0};
		strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localtime(&m_timeT));	
		std::string strTime(timeString);
		return strTime;

	}
	void TimeA::checkAndLog( )
	{
        FUNCTION_ENTRY( "checkAndLog" );
		std::stringstream ss;	
 		Iec102FormatLog* pLog = getFormatLog();

		ss << "YEAR("  << std::setfill('0') << std::setw(2) << (unsigned int)m_year 
		   << ") MONTH(" << std::setfill('0') << std::setw(2) << (unsigned int)m_month << ") "
		   << "WEEK(" << std::setfill('0') << std::setw(2) << (unsigned int)m_week 
		   << ") DAY(" << std::setfill('0') << std::setw(2) << (unsigned int) m_day << ") "
		   << "HOUR(" << std::setfill('0') << std::setw(2) << (unsigned int)m_hour 
		   << ") MINUTE(" << std::setfill('0') << std::setw(2) << (unsigned int)m_minute << ")";

  		if (m_timeT == 0)
		{	
 			setValid(false);
			ss << "[Incorrect DateTime]";
			pLog->logParseLine(NAME_TIMEA,getRawData(),ss.str());
			return;
		}
		
		if ( m_year > 99 )
		{
 			setValid(false);
			ss << "[Incorrect YEAR] ";
		}

		if ( m_month > 12 || m_month < 1 )
		{
 			setValid(false);
			ss << "[Incorrect MONTH] ";
		}

		if ( m_week > 7 || m_week < 1)
		{
 			setValid(false);
			ss << "[Incorrect WEEK] ";
		}

		if ( m_day > 31 || m_day < 1)
		{
 			setValid(false);
			ss << "[Incorrect DAY] ";
		}

		if ( m_hour > 23 )
		{
 			setValid(false);
			ss << "[Incorrect HOUR] ";
		}

		if ( m_minute > 59 )
		{
 			setValid(false);
			ss << "[Incorrect MINUTE]";
		}

		if (isValid())
		{
			ss << " DateTime(" <<getTimeString() << ")";
		}
		pLog->logParseLine(NAME_TIMEA,getRawData(),ss.str());
		
		FUNCTION_EXIT;
	}

    void TimeA::createRawData()
    {
        FUNCTION_ENTRY( "createRawData" );
		std::vector<unsigned char> rawData;

		rawData.push_back(m_minute);
		rawData.push_back(m_hour);
		rawData.push_back((m_week << 5) + m_day);
		rawData.push_back(m_month);
		rawData.push_back(m_year);

		setRawData(rawData); 
        FUNCTION_EXIT;
    }	
	
	// **************
	// TimeB
	// **************
	TimeB::TimeB( const std::vector<unsigned char>& data )
	:
	Iec102Struct(NAME_TIMEB,TIMEB_TYPE,data),
	m_helper(this)
	{
		FUNCTION_ENTRY( "TimeB" );
		m_helper.parseData(data);

		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
		FUNCTION_EXIT;
	}

	TimeB::TimeB(time_t timeValue )
	:
	Iec102Struct(NAME_TIMEB,TIMEB_TYPE),
	m_timeT(timeValue),
	m_helper(this)
	{
		FUNCTION_ENTRY( "TimeB" );
		struct tm* timeInfo;
		timeInfo = localtime(&timeValue);

		m_millisecond = 0;
		m_second = (unsigned char)((timeInfo->tm_sec) << 2);								
		m_minute = (unsigned char)(timeInfo->tm_min);
		m_hour = (unsigned char)(timeInfo->tm_hour);
		
		m_day = 0x1F & (unsigned char)(timeInfo->tm_mday);								//day of the month, 5 lower bits

		if (timeInfo->tm_wday == 0)														//tm_wday=0 for sunday in tm
			timeInfo->tm_wday = 7;
		m_week = 0x07 & (unsigned char)(timeInfo->tm_wday);


		m_month = (unsigned char)(timeInfo->tm_mon+1);									//tm_mon=0 for January
		if (timeInfo->tm_year >= 100)																			//tm_year is year since 1900
			timeInfo->tm_year -= 100;																			//lks
		m_year = (unsigned char)(timeInfo->tm_year);							

		m_timeT = timeValue;
		createRawData();
		checkAndLog();
		FUNCTION_EXIT;
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", getDebugInfo().c_str() );
	}

	void TimeB::setupStruct(const std::vector<unsigned char>& data,Iec102StructHelper* pHelper) 
	{
		FUNCTION_ENTRY( "setupStruct" );
		bool bValid = isValid();
		if (!bValid) { return; }


		m_year	= data[TIMEB_YEAR] & 0x7F;
		m_month = data[TIMEB_MONTH] & 0x0F;
		m_week	= data[TIMEB_WEEK] >> 5;
		m_day	= data[TIMEB_DAY] & 0x1F;
		m_hour	= data[TIMEB_HOUR] & 0x1F;
		m_minute= data[TIMEB_MIN] & 0x3F;		
		m_millisecond  = ((data[TIMEB_SEC] & 0x03) << 8) + data[TIMEB_MILSEC];
		m_second	   = data[TIMEB_SEC] >> 2;
		struct tm tm1;
		tm1.tm_sec = 0;
		tm1.tm_min = int(m_minute);
		tm1.tm_hour = int(m_hour);
		tm1.tm_mday = int(m_day); 
		tm1.tm_mon = int(m_month - 1);
		tm1.tm_year = 2000 + int(m_year) - 1900;
		m_timeT = mktime ( &tm1 );	

		checkAndLog();
		FUNCTION_EXIT;
	}

	void TimeB::checkAndLog( )
	{
		FUNCTION_ENTRY( "checkAndLog" );
		std::stringstream ss;	
 		Iec102FormatLog* pLog = getFormatLog();

		ss << "YEAR(" << std::setfill('0') << std::setw(2) << (unsigned int)m_year 
			<< ") MONTH(" << std::setfill('0') << std::setw(2) << (unsigned int)m_month << ") "
		   << "WEEK(" << std::setfill('0') << std::setw(2) << (unsigned int)m_week 
		   << ") DAY(" << std::setfill('0') << std::setw(2) << (unsigned int) m_day << ") "
		   << "HOUR(" << std::setfill('0') << std::setw(2) << (unsigned int)m_hour 
		   << ") MINUTE(" << std::setfill('0') << std::setw(2) << (unsigned int)m_minute << ") "
		   << "SECOND(" << std::setfill('0') << std::setw(2) << (unsigned int)m_second 
		   << ") MILLISECOND(" << std::setfill('0') << std::setw(3) << (unsigned int)m_millisecond << ")";	

		if ( m_timeT == 0 )
		{
 			setValid(false);
			ss << "[Incorrect DateTime]";
			pLog->logParseLine(NAME_TIMEB,getRawData(),ss.str());
			return;
		}

		
		if ( m_year > 99 )
		{
 			setValid(false);
			ss << "[Incorrect YEAR] ";
		}

		if ( m_month > 12 || m_month < 1 )
		{
 			setValid(false);
			ss << "[Incorrect MONTH] ";
		}

		if ( m_week > 7 || m_week < 1)
		{
 			setValid(false);
			ss << "[Incorrect WEEK] ";
		}

		if ( m_day > 31 || m_day < 1)
		{
 			setValid(false);
			ss << "[Incorrect DAY] ";
		}

		if ( m_hour > 23 )
		{
 			setValid(false);
			ss << "[Incorrect HOUR] ";
		}

		if ( m_minute > 59 )
		{
 			setValid(false);
			ss << "[Incorrect MINUTE]";
		}

		if ( m_second > 59 )
		{
 			setValid(false);
			ss << " [Incorrect SECOND]";
		}

		if ( m_millisecond > 999 )
		{
 			setValid(false);
			ss << " [Incorrect MILLISECOND]";
		}

		pLog->logParseLine(NAME_TIMEB,getRawData(),ss.str());
		
		FUNCTION_EXIT;
	}

	void TimeB::createRawData()
	{
		FUNCTION_ENTRY( "createRawData" );
		std::vector<unsigned char> rawData;

		rawData.push_back((unsigned char)m_millisecond);
		rawData.push_back((m_second << 2) + (m_millisecond >> 8));
		rawData.push_back(m_minute);
		rawData.push_back(m_hour);
		rawData.push_back((m_week << 5) + m_day);
		rawData.push_back(m_month);
		rawData.push_back(m_year);

		setRawData(rawData); 
		FUNCTION_EXIT;
	}	

}

