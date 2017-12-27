/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/IMAPIWrapper.h $
* @author:  Lincy
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/04/13 13:35:44 $
* Last modified by:  $Author: builder $
*
* IMAPWrapper class wraps IMAPI interfaces and functions.
*/
#include "stdafx.h"
#include "IMAPIWrapper.h"
#include "IMAPIFormatDataEvent.h"
#include "Imapi2error.h"
#include "Imapi2fserror.h"
#include "core/utilities/src/DebugUtil.h"

using namespace std;
#define CLIENT_NAME	L"ARCHIEVE_MANAGER"
namespace TA_Base_App
{
	IMAPIWrapper::IMAPIWrapper():
m_discMaster(NULL),
m_selectedRecorder(NULL),
m_discFormatData(NULL)

{

}


IMAPIWrapper::~IMAPIWrapper(){

}


void IMAPIWrapper::Deinitialise()
{
	FUNCTION_ENTRY("Deinitialise");

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"Deinitialising IMAPIWrapper" );

	if (m_discFormatData != NULL)
	{
		m_discFormatData->Release();
	}

	//delete all recorder held with m_discRecorderMap
	std::map<BSTR,IDiscRecorder2*>::iterator it;
	for(it = m_discRecorderMap.begin();it != m_discRecorderMap.end(); it++)
	{

		if(it->second != NULL)
			it->second->Release();

	}

	if (m_discMaster != NULL)
	{
		m_discMaster->Release();
	}

	FUNCTION_EXIT;
}

bool IMAPIWrapper::initialise(bool CD_Archive)
{
	FUNCTION_ENTRY("initialise");

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"initialising IMAPIWrapper" );

	m_CDArchive = CD_Archive;
	if(!InitialisediscMaster())
	{
		//error
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to initialise Disc Master!" );

		return false;
	}

	long totalDevices = 0;
	HRESULT m_hResult = m_discMaster->get_Count(&totalDevices);
	if (FAILED(m_hResult))
	{
		//m_errorMessage.Format(_T("IDiscMaster2->get_Count failed! - Error:0x%08x"), m_hResult);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to retrieve the number of devices. IDiscMaster2->get_Count failed - Error:0x%08x",m_hResult );

		return false;

	}

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"Found %d Devices.",totalDevices );


	for (long deviceIndex = 0; deviceIndex < totalDevices; deviceIndex++)
	{	
		BSTR	uniqueID = NULL;
		HRESULT m_hResult = m_discMaster->get_Item(deviceIndex, &uniqueID);
		if (FAILED(m_hResult))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"IDiscMaster2->get_Item failed - Error:0x%08x",m_hResult );

		}
		if ((uniqueID==NULL)||(SysStringLen(uniqueID)==0))
		{	

			continue;
		}

		//
		// Create an IDiscRecorder2
		//

		//m_errorMessage = _T("There were no writable devices detected!");
		if (!InitialiseDiscRecorder(uniqueID))
		{	
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"Failed to initialize recorder - uniqueID: %s",(LPCTSTR)uniqueID );

			//if (totalDevices == 1)
			//{	
			//	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			//		"Failed to initialize recorder - uniqueID: %s",(LPCTSTR)uniqueID );

			//}
			continue;
		}

	}

	if(m_discRecorderMap.size() == 0)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to Initialize Recorder.");

		m_MediaStatus = NoValidDevice;
		return false;
	}

	//select a suitable recorder for burning
	SelectRecorderForBurning();

	if(m_selectedRecorder == NULL)
	{	
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to Select a Recorder for Archiving.");

		m_MediaStatus = InvalidDrive;
		return false;
	}

	if(!InitialiseDiscFormatData(CLIENT_NAME))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to Initialise DiscFormataData");

		//failed to Initialise DiscFormatData
		FUNCTION_EXIT;
		return false;
	}

	FUNCTION_EXIT;
	return true;

}

bool IMAPIWrapper::InitialisediscMaster()
{
	FUNCTION_ENTRY("InitialisediscMaster");
	HRESULT _hResult;
	//
	// Initialize the IDiscMaster2 Interface
	//
	if (m_discMaster == NULL)
	{
		_hResult = CoCreateInstance(__uuidof(MsftDiscMaster2), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDiscMaster2), (void**)&m_discMaster);
		if (!SUCCEEDED(_hResult))
		{
			//m_errorMessage.Format(_T("Unable to Initialize IDiscMaster2 - Error:0x%08x"), m_hResult);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Unable to Initialize IDiscMaster2 - Error:0x%08x", _hResult);

			FUNCTION_EXIT;
			return false;
		}
	}

	//
	// Verify that we have some device that uses this interface
	//
	VARIANT_BOOL isSupported = VARIANT_FALSE;
	_hResult = m_discMaster->get_IsSupportedEnvironment(&isSupported);
	if (!SUCCEEDED(_hResult))
	{
		//m_errorMessage.Format(_T("IDiscMaster2->get_IsSupportedEnvironment failed! - Error:0x%08x"), m_hResult);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"IDiscMaster2->get_IsSupportedEnvironment failed! - Error:0x%08x", _hResult);

		FUNCTION_EXIT;
		return false;
	}
	if (isSupported == VARIANT_FALSE)
	{
		//m_errorMessage = _T("There were no writable devices detected!");
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"There were no writable devices detected");


		m_MediaStatus = NoValidDevice;

		FUNCTION_EXIT;
		return false;
	}

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"Initialised Disc Master Successfully");

	FUNCTION_EXIT;
	return true;
}

bool IMAPIWrapper::InitialiseDiscRecorder(const BSTR& recorderUniqueId)
{
	FUNCTION_ENTRY("InitialisediscRecorder");

	HRESULT _hResult;

	IDiscRecorder2* _discRecorder;
	//
	// Create an IDiscRecorder2
	//
	_hResult = CoCreateInstance(__uuidof(MsftDiscRecorder2), NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IDiscRecorder2), (void**)&_discRecorder);
	//ASSERT(SUCCEEDED(_hResult));
	if (FAILED(_hResult))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Unable to Initialize MsftDiscRecorder2 - Error:0x%08x", _hResult);

		FUNCTION_EXIT;
		return false;
	}

	_hResult = _discRecorder->InitializeDiscRecorder(recorderUniqueId);
	if (FAILED(_hResult))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to initialise Disc Recorder: Recorder id: %s - Error:0x%08x", 
			LPCTSTR(recorderUniqueId), _hResult);

		FUNCTION_EXIT;
		return false;
	}
	else
	{
		if(m_discRecorderMap.find(recorderUniqueId) == m_discRecorderMap.end())
		{
			m_discRecorderMap.insert(pair<BSTR, IDiscRecorder2*>(recorderUniqueId,_discRecorder));
		}

	}

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"Initialised Disc Recorder Successfully");


	FUNCTION_EXIT;
	return true;
}


vector<string> IMAPIWrapper::GetAllDeviceInfo()
{
	FUNCTION_ENTRY("GetAllDeviceInfo");

	vector<string> AllVolList;
	map<BSTR,IDiscRecorder2*>::iterator it;
	for (it = m_discRecorderMap.begin(); it != m_discRecorderMap.end(); it++)
	{	
		vector<string> AllVolListTemp;


		BSTR productId, vendorId;
		it->second->get_ProductId(&productId);
		it->second->get_VendorId(&vendorId);

		string productIdStr, vendorIdStr, voulmePathStr, hostNameStr;
		productIdStr = BstrToStdString(productId);
		vendorIdStr = BstrToStdString(vendorId);
		voulmePathStr = BstrToStdString(GetVolumePathList(it->second)[0]);

		string DeviceInfo  = voulmePathStr + " " + vendorIdStr + " " +productIdStr;

		AllVolListTemp.push_back(DeviceInfo);
		AllVolList.insert(AllVolList.end(), AllVolListTemp.begin(), AllVolListTemp.end());
	}

	FUNCTION_EXIT;
	return AllVolList;
}

string IMAPIWrapper::GetSelectedDevice()
{
	FUNCTION_ENTRY("GetSelectedDevice");
	if(m_selectedRecorder == NULL)
		return "";

	FUNCTION_EXIT;
	return BstrToStdString(GetVolumePathList(m_selectedRecorder)[0]);
}

vector<BSTR> IMAPIWrapper::GetVolumePathList(IDiscRecorder2* _discRecorder)
{	
	FUNCTION_ENTRY("GetVolumePathList");
	vector<BSTR> AllVolList;

	if(_discRecorder != NULL)
	{
		SAFEARRAY*		_volumePathNames;
		HRESULT m_hResult = _discRecorder->get_VolumePathNames(&_volumePathNames);	


		if (FAILED(m_hResult))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"IDiscRecoer->get_VolumePathNames - Error:0x%08x", m_hResult);

			return AllVolList;
		}

		ULONG totalVolumePaths;
		if (_volumePathNames != NULL)
		{
			totalVolumePaths =  _volumePathNames->rgsabound[0].cElements;
		}
		else
		{
			totalVolumePaths = 0;
		}

		BSTR volumeList;
		for (ULONG volIndex = 0; volIndex < totalVolumePaths; volIndex++)
		{
			volumeList = ((VARIANT*)(_volumePathNames->pvData))[volIndex].bstrVal;

		}

		AllVolList.push_back(volumeList);
	}

	FUNCTION_EXIT;
	return AllVolList;
}


void IMAPIWrapper::SelectRecorderForBurning()
{
	FUNCTION_ENTRY("SelectRecorderForBurning");
	map<BSTR,IDiscRecorder2*>::iterator it;

	m_selectedRecorder = NULL;
	for (it = m_discRecorderMap.begin(); it != m_discRecorderMap.end(); it++)
	{
		SAFEARRAY * _supportedProfiles;
		HRESULT m_hResult = it->second->get_SupportedProfiles(&_supportedProfiles);

		if (FAILED(m_hResult))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"IDiscRecoer->get_SupportedProfiles - Error:0x%08x", m_hResult);

			continue;
		}

		ULONG totalSupportedProfiles;
		if (_supportedProfiles != NULL)
		{
			totalSupportedProfiles =  _supportedProfiles->rgsabound[0].cElements;
		}
		else
		{
			totalSupportedProfiles = 0;
		}

		for (ULONG index = 0; index < totalSupportedProfiles; index++)
		{
			int supportProfileType = ((VARIANT*)(_supportedProfiles->pvData))[index].intVal;
			if(m_CDArchive)
			{
				if(isCDProfileType(supportProfileType))
				{
					m_selectedRecorder = it->second;
					break;
				}
			}
			else
			{
				if(isDVDProfileType(supportProfileType))
				{
					m_selectedRecorder = it->second;
					break;
				}
			}
		}


	}

	FUNCTION_EXIT;
	return;

}

bool IMAPIWrapper::isDVDProfileType(int ProfileType)
{
	switch(ProfileType)
	{
	case IMAPI_PROFILE_TYPE_DVD_DASH_RECORDABLE:
	case IMAPI_PROFILE_TYPE_DVD_DASH_REWRITABLE:
	case IMAPI_PROFILE_TYPE_DVD_DASH_RW_SEQUENTIAL:
	case IMAPI_PROFILE_TYPE_DVD_PLUS_R:
	case IMAPI_PROFILE_TYPE_DVD_PLUS_RW:
		return true;
	default:
		return false;

	}
}

bool IMAPIWrapper::isCDProfileType(int ProfileType)
{
	switch(ProfileType)
	{
	case IMAPI_PROFILE_TYPE_CD_RECORDABLE:
	case IMAPI_PROFILE_TYPE_CD_REWRITABLE:
		return true;
	default:
		return false;

	}
}



bool IMAPIWrapper::isDVDMediaType(int MediaType)
{
	switch( MediaType)
	{
	case IMAPI_MEDIA_TYPE_DVDPLUSR:
	case IMAPI_MEDIA_TYPE_DVDPLUSRW:
	case IMAPI_MEDIA_TYPE_DVDDASHR:
	case IMAPI_MEDIA_TYPE_DVDDASHRW:	
		return true;
	default:
		return false;

	}
}

bool IMAPIWrapper::isCDMediaType(int MediaType)
{
	switch( MediaType)
	{
	case IMAPI_MEDIA_TYPE_CDR:
	case IMAPI_MEDIA_TYPE_CDRW:
		return true;
	default:
		return false;

	}
}

bool IMAPIWrapper::InitialiseDiscFormatData(BSTR clientName)
{
	FUNCTION_ENTRY("InitialiseDiscFormatData");

	if(m_selectedRecorder == NULL)
	{	
		FUNCTION_EXIT;
		return false;
	}

	//
	// Initialize the IDiscFormat2Data Interface
	//
	HRESULT m_hResult = CoCreateInstance(__uuidof(MsftDiscFormat2Data), NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IDiscFormat2Data), (void**)&m_discFormatData);
	if (!SUCCEEDED(m_hResult))
	{
		//m_errorMessage.Format(_T("Unable to Initialize IDiscFormat2Data - Error:0x%08x"), m_hResult);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Unable to Initialize IDiscFormat2Data - Error:0x%08x", m_hResult);

		FUNCTION_EXIT;
		return false;
	}

	//
	// Setup the Disc Format Information
	//
	VARIANT_BOOL isSupported = VARIANT_FALSE;
	m_hResult = m_discFormatData->IsRecorderSupported(m_selectedRecorder, &isSupported);
	if (isSupported == VARIANT_FALSE)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Selected recorder is not supported by IDiscFormat2Data - Error:0x%08x", m_hResult);

		m_MediaStatus = InvalidDrive;

		FUNCTION_EXIT;
		return false;
	}

	m_hResult = m_discFormatData->put_Recorder(m_selectedRecorder);
	if (!SUCCEEDED(m_hResult))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"IDiscFormat2Data->put_Recorder Failed - Error:0x%08x", m_hResult);


		updateMediaStauts(m_hResult);

		FUNCTION_EXIT;
		return false;
	}


	const CString temp = L"TestApp";
	m_hResult = m_discFormatData->put_ClientName(temp.AllocSysString());
	if (!SUCCEEDED(m_hResult))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"IDiscFormat2Data->put_ClientName Failed - Error:0x%08x", m_hResult);

		updateMediaStauts(m_hResult);

		FUNCTION_EXIT;
		return false;
	}

	SAFEARRAY* _mediaTypesArray;
	m_hResult = m_discFormatData->get_SupportedMediaTypes(&_mediaTypesArray);
	if (!SUCCEEDED(m_hResult))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"IDiscFormat2Data->get_SupportedMediaTypes Failed - Error:0x%08x", m_hResult);

		updateMediaStauts(m_hResult);

		FUNCTION_EXIT;
		return false;
	}

	//check if media type is supported by current implementation of m_discFormatData
	ULONG totalSupportedMediaTypes;
	if (_mediaTypesArray != NULL)
	{
		totalSupportedMediaTypes =  _mediaTypesArray->rgsabound[0].cElements;
	}
	else
	{
		totalSupportedMediaTypes = 0;
	}

	for (ULONG index = 0; index < totalSupportedMediaTypes; index++)
	{
		int supportMediaType = ((VARIANT*)(_mediaTypesArray->pvData))[index].intVal;
		if(m_CDArchive)
		{
			if(isCDMediaType(supportMediaType))
			{	
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"IDiscFormat2Data Initialised with CD Media Type");

				FUNCTION_EXIT;
				return true;
			}
		}
		else
		{
			if(isDVDMediaType(supportMediaType))
			{	
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"IDiscFormat2Data Initialised with DVD Media Type");

				FUNCTION_EXIT;
				return true;
			}
		}

	}

	//log no supported Media Type
	m_MediaStatus = InvalidDrive;
	FUNCTION_EXIT;
	return false;
}

bool IMAPIWrapper::CreateMediaFileSystem(IMAPI_MEDIA_PHYSICAL_TYPE mediaType, IStream** ppDataStream, std::vector<FilePathName> filePathNameVector)
{
	FUNCTION_ENTRY("CreateMediaFileSystem");

	IFileSystemImage*		image = NULL;
	IFileSystemImageResult*	imageResult = NULL;
	IFsiDirectoryItem*		rootItem = NULL;
	//CString					message;
	bool					returnVal = false;

	HRESULT hr = CoCreateInstance(CLSID_MsftFileSystemImage,
		NULL, CLSCTX_ALL, __uuidof(IFileSystemImage), (void**)&image);
	if (FAILED(hr) || (image == NULL))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to create IFileSystemImage Interface - Error:0x%08x", hr);

		FUNCTION_EXIT;
		return false;
	}

	CString volumeLabel (m_volumeLabel.c_str());
	image->put_FileSystemsToCreate((FsiFileSystems)(FsiFileSystemJoliet|FsiFileSystemISO9660));
	image->put_VolumeName(volumeLabel.AllocSysString());
	image->ChooseImageDefaultsForMediaType(mediaType);

	//
	// Get the image root
	//
	hr = image->get_Root(&rootItem);
	if (SUCCEEDED(hr))
	{
		IStream* fileStream = NULL;
		for(int i =0; i < filePathNameVector.size(); i++)
		{
			CString filefullpath = filePathNameVector[i].first.c_str();
			CString filename = filePathNameVector[i].second.c_str();


			SHCreateStreamOnFileEx( filefullpath.AllocSysString(), 
				STGM_READ|STGM_SHARE_DENY_NONE|STGM_DELETEONRELEASE,
				FILE_ATTRIBUTE_NORMAL, 
				FALSE, 
				NULL, 
				&fileStream );


			if (fileStream != NULL)
			{

				hr = rootItem->AddFile(filename.AllocSysString(), fileStream);
				if (FAILED(hr))
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"Failed IFsiDirectoryItem->AddFile - Error:0x%08x", hr);

				}
				
				//
				// Release the IStream
				//
				fileStream->Release();
			}
		}
	}

	// Create the result image
	hr = image->CreateResultImage(&imageResult);
	if (SUCCEEDED(hr))
	{
		//
		// Get the stream
		//
		hr = imageResult->get_ImageStream(ppDataStream);
		if (SUCCEEDED(hr))
		{
			returnVal = true;
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Failed IFileSystemImageResult->get_ImageStream! - Error:0x%08x", hr);

		}

	}
	else
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed IFileSystemImage->CreateResultImage! - Error:0x%08x", hr);
	}


	//
	// Cleanup
	//
	if (image != NULL)
		image->Release();
	if (imageResult != NULL)
		imageResult->Release();
	if (rootItem != NULL)
		rootItem->Release();
	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"File System created");

	FUNCTION_EXIT;
	return returnVal;
}


bool IMAPIWrapper::Burn( std::vector<FilePathName> filePathNameVector)
{

	FUNCTION_ENTRY("Burn");
	//create Filesystem
	//
	// Get the media type currently in the recording device
	//
	IMAPI_MEDIA_PHYSICAL_TYPE mediaType = IMAPI_MEDIA_TYPE_UNKNOWN;
	m_discFormatData->get_CurrentPhysicalMediaType(&mediaType);


	//
	// Create the file system
	//
	IStream* dataStream = NULL;
	if (!CreateMediaFileSystem( mediaType, &dataStream, filePathNameVector ))
	{	// CreateMediaFileSystem reported error to UI
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to create file system!");

			FUNCTION_EXIT;
		return false;
	}

	if (m_discFormatData == NULL)
	{	
		FUNCTION_EXIT;
		return false;
	}

	/*if (hNotificationWnd == NULL)
	return false;*/

	if (dataStream == NULL)
	{
		FUNCTION_EXIT;	
		return false;
	}

	//m_hNotificationWnd = hNotificationWnd;

	// Create the event sink
	CIMAPIFormatDataEvent* eventSink = CIMAPIFormatDataEvent::CreateEventSink();
	if (eventSink == NULL)
	{	
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Unable to create event sink!");

			FUNCTION_EXIT;
		return false;
	}

	if (!eventSink->ConnectDiscFormatData(m_discFormatData))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Unable to connect event sink with interface!");

			FUNCTION_EXIT;
		return false;
	}

	if(m_CallbackBurnStatus == NULL)
	{
		FUNCTION_EXIT;
		return false;
	}
	eventSink->SetCallBackBurnStatus(m_CallbackBurnStatus);

	//Set Burn Options
	//m_discFormatData->put_ForceMediaToBeClosed(m_closeMedia ? VARIANT_TRUE : VARIANT_FALSE);
	m_discFormatData->put_ForceOverwrite(m_ForceOverwrite? VARIANT_TRUE : VARIANT_FALSE);
	m_discFormatData->put_BufferUnderrunFreeDisabled(!m_underburnEnabled? VARIANT_FALSE: VARIANT_TRUE);


	HRESULT m_hResult = m_discFormatData->Write(dataStream);

	delete eventSink;
	//
	// Release the IStream after burning
	//
	dataStream->Release();

	if (SUCCEEDED(m_hResult))
	{	
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Write Completed Successfully!");

		m_MediaStatus = WriteSuccessful;
		return true;
	}

	updateMediaStauts(m_hResult);

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
		"Write Failed!");

	FUNCTION_EXIT;
	return false;

}



void IMAPIWrapper::EjectMedia()
{	
	FUNCTION_ENTRY("EjectMedia");
	
	m_selectedRecorder->EjectMedia();

	FUNCTION_EXIT;
}

void IMAPIWrapper::CancelBurn()
{
	FUNCTION_ENTRY("CancelBurn");

	if(m_discFormatData == NULL)
		return;

	m_discFormatData->CancelWrite();
	
	FUNCTION_EXIT;
}

}
// convert a BSTR to a std::string. 
std::string& IMAPIWrapper::BstrToStdString(const BSTR bstr, std::string& dst, int cp)
{
	if (!bstr)
	{
		// define NULL functionality. I just clear the target.
		dst.clear();
		return dst;
	}

	// request content length in single-chars through a terminating
	//  nullchar in the BSTR. note: BSTR's support imbedded nullchars,
	//  so this will only convert through the first nullchar.
	int res = WideCharToMultiByte(cp, 0, bstr, -1, NULL, 0, NULL, NULL);
	if (res > 0)
	{
		dst.resize(res);
		WideCharToMultiByte(cp, 0, bstr, -1, &dst[0], res, NULL, NULL);
	}
	else
	{    // no content. clear target
		dst.clear();
	}
	return dst;
}

// conversion with temp.
std::string IMAPIWrapper::BstrToStdString(BSTR bstr, int cp)
{
	std::string str;
	BstrToStdString(bstr, str, cp);

	//to remove null character at the end of string
	str = std::string(str.c_str());

	return str;
}

void IMAPIWrapper::updateMediaStauts(HRESULT hresult)
{	
	FUNCTION_ENTRY("updateMediaStatus");
	switch (hresult)
	{
		//case (E_IMAPI_BURN_VERIFICATION_FAILED):
	case(E_IMAPI_REQUEST_CANCELLED):
		m_MediaStatus = WriteCancelled;
		break;

	case (E_IMAPI_RECORDER_MEDIA_NO_MEDIA):
		m_MediaStatus = NoMedia;
		break;

	case (E_IMAPI_RECORDER_MEDIA_INCOMPATIBLE):
	case (E_IMAPI_RECORDER_MEDIA_UPSIDE_DOWN):
	case (E_IMAPI_DF2DATA_MEDIA_IS_NOT_SUPPORTED):
		m_MediaStatus = MediaIncompatible;
		break;
		//busy
	case (E_IMAPI_RECORDER_MEDIA_BECOMING_READY):
	case (E_IMAPI_RECORDER_MEDIA_FORMAT_IN_PROGRESS):
	case (E_IMAPI_RECORDER_MEDIA_BUSY):
	case (E_IMAPI_DF2DATA_WRITE_IN_PROGRESS):
	case (E_IMAPI_RECORDER_LOCKED):
		m_MediaStatus = DriveBusy;
		break;

	case (E_IMAPI_RECORDER_INVALID_MODE_PARAMETERS):
	case (E_IMAPI_RECORDER_MEDIA_WRITE_PROTECTED):
		m_MediaStatus = MediaWriteProtected;
		break;

	case (E_IMAPI_RECORDER_DVD_STRUCTURE_NOT_PRESENT):
		m_MediaStatus = InvalidDrive;
		break;

	case (E_IMAPI_LOSS_OF_STREAMING):
	case (E_IMAPI_UNEXPECTED_RESPONSE_FROM_DEVICE):
		m_MediaStatus = WriteFailed;
		break;

	case (E_IMAPI_DF2DATA_STREAM_TOO_LARGE_FOR_CURRENT_MEDIA):
		m_MediaStatus = MediaOutOfMemory;
		break;

	case (E_IMAPI_DF2DATA_MEDIA_NOT_BLANK):
		m_MediaStatus = MediaNotEmpty;
		break;
	default:
		m_MediaStatus = Unknown;
		break;

	FUNCTION_EXIT;
	}
}


//bool IMAPIWrapper::setBurnVerification(IDiscFormat2Data  *DataWriter )
//{
//	IBurnVerification *burnVerifier = NULL;
//
//	HRESULT hr = DataWriter->QueryInterface(IID_PPV_ARGS(&burnVerifier));
//
//	if (SUCCEEDED(hr))
//	{
//		hr = burnVerifier->put_BurnVerificationLevel(VerificationLevel);
//	}
//
//	//if (burnVerifier != NULL)
//	//{
//	//	burnVerifier->Release();
//	//	burnVerifier = NULL;
//	//}
//
//	return true;
//}
