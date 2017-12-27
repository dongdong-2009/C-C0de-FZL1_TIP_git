#if !defined(IMAPIWrapper_H_)
#define IMAPIWrapper_H_
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
#include <imapi2.h>
#include <imapi2error.h>
#include <imapi2fs.h>
#include <imapi2fserror.h>
#include "app/data_management/archive_manager/src/IMediaAccess.h"
#include <map>


namespace TA_Base_App
{

	class IMAPIWrapper : public IMediaAccess
	{

	public:
		/** IMAPIWrapper
			*
			* Standard constructor.
			*
			*/
		IMAPIWrapper();

		/** IMAPIWrapper
			*
			* Standard destructor.
			*
			*/
		virtual ~IMAPIWrapper();
		virtual bool Burn(std::vector<FilePathName> FilePathNameVector);
		virtual void CancelBurn();
		virtual void Deinitialise();
		virtual bool initialise(bool CD_Archive);
		virtual void EjectMedia();
		virtual vector<string> GetAllDeviceInfo();
		virtual string GetSelectedDevice();
	private:
		IDiscMaster2* m_discMaster;
		IDiscRecorder2* m_selectedRecorder;
		std::map<BSTR,IDiscRecorder2*> m_discRecorderMap;
		IDiscFormat2Data*	m_discFormatData;
		
		SAFEARRAY*		m_volumePathNames;
		BSTR			m_recorderUniqueId;

		/** IMAPIWrapper
			*
			*	Creates and initializes the IDiscMaster2 interface.
			*
			*
			*/
		bool InitialisediscMaster();
		
		/** IMAPIWrapper
			*
			*	InitialiseDiscFormatData
			*
			*	Creates and initializes the IDiscFormat2Data interface
			*
			*	@param -	BSTR clientName
			*				ClientName refers to application using IMAPI interface.
			*
			*/
		bool IMAPIWrapper::InitialiseDiscFormatData(BSTR clientName);

		/** IMAPIWrapper
			*
			*	InitialiseDiscRecorder
			*
			*	Creates and initializes the IDiscRecorder2 interface
			*
			* @param -  const BSTR recorderUniqueId	
			*			The recorder's unique id retrieved from	the IDiscMaster2 interface.
			*
			*/
		bool InitialiseDiscRecorder(const BSTR& recorderUniqueId);

		
		
		/** IMAPIWrapper
			*
			*	GetVolumePathList
			*
			*	Get the volume path for the disc recoder.
			*
			* @param - IDiscRecorder2* _discRecorder
			*		   Pointer to discRecorder for which the volume path has to determined
			*/
		vector<BSTR> GetVolumePathList(IDiscRecorder2* _discRecorder);

		/** IMAPIWrapper
			*
			*	SelectRecorderForBurning
			*
			*	Select a suitable recorder for burning.
			*
			*/
		void SelectRecorderForBurning();

		/** IMAPIWrapper
			*
			*	isDVDProfileType
			*
			*	Checks if the input profile type is of DVD profile type.
			*
			*/
		bool isDVDProfileType(int supportedProfileType);

		/** IMAPIWrapper
			*
			*	isDVDMediaType
			*
			*	Checks if the input media type is of DVD media type.
			*
			*/
		bool isDVDMediaType(int MediaType);

		/** IMAPIWrapper
			*
			*	isCDProfileType
			*
			*	Checks if the input profile type is of CD profile type.
			*
			*/		
		bool isCDProfileType(int supportedProfileType);

		/** IMAPIWrapper
			*
			*	isCDMediaType
			*
			*	Checks if the input media type is of CD media type.
			*
			*/
		bool isCDMediaType(int MediaType);
		
		/** IMAPIWrapper
			*
			*	CreateMediaFileSystem
			*
			*	Create File System of the files to be burnt.
			*
			*/
		bool CreateMediaFileSystem(IMAPI_MEDIA_PHYSICAL_TYPE mediaType, IStream** ppDataStream,
			std::vector<FilePathName> filePathNameVector);

		/** IMAPIWrapper
			*
			*	updateMediaStauts
			*
			*	Updates MediaStatus based on the return result.
			*
			*	@param - HRESULT hresult
			*			 return result for IMAPI functions
			*
			*/
		void updateMediaStauts(HRESULT hresult);
		

		/** IMAPIWrapper
			*
			*	BstrToStdString
			*
			*	Convert BSTR to string.
			*
			*	@param - BSTR bstr
			*			 input BSTR to convert
			*
			*	@param - string& dst
			*			to store converted string
			*
			*	@param  - int cp
			*			 Code page to used for convertion. Default (UTF 8)
			*
			*	return	- converted string
			*/
		std::string& BstrToStdString(const BSTR bstr, std::string& dst, int cp = CP_UTF8);
		
		/** IMAPIWrapper
			*
			*	BstrToStdString
			*
			*	Convert BSTR to string.
			*	
			*	@param - BSTR bstr
			*			 input BSTR to convert
			*
			*	@param  - int cp
			*			 Code page to used for convertion. Default (UTF 8)
			*
			*	return	- converted string
			*/
		std::string BstrToStdString(BSTR bstr, int cp = CP_UTF8);
		
		

	};
}
#endif