#if !defined(IMediaAccess_H_)
#define IMediaAccess_H_
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/IMediaAccess.h $
  * @author:  Lincy
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/04/13 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Interface to access media wrapper functions.
  */

#include <string>
#include <vector>
#include "app/data_management/archive_manager/src/ICallbackBurnStatus.h"
#include "app/data_management/archive_manager/src/MediaStatus.h"

using namespace std;
namespace TA_Base_App
{
	typedef pair<std::string,std::string> FilePathName;
	

	class IMediaAccess
	{

	public:
		/**
          * IMediaAccess
          *
          * Standard constructor.
          *
          */
		IMediaAccess(){
			m_underburnEnabled = false;
			m_ForceOverwrite = false;
			m_burnVerficationEnabled = true;
			m_CDArchive = false;
		}

		/** IMediaAccess
			*
			* Standard destructor.
			*
			*/
		virtual ~IMediaAccess(){};
		
		/** IMediaAccess
			*
			* initialise 
			*
			* initialise hardware for burning.
			*
			* @param	CD_Archive is set to true if archiving media used is a CD.
			*			For DVD set CD_Archive is set to false.
			*
			* @return	Return true if successfully Initialised, else returns false.
			*/
		virtual bool initialise(bool CD_Archive) = 0;

		
		/** IMediaAccess
			*
			* Deinitialise 
			*
			* Deinitialise after burning.
			*/
		virtual void Deinitialise() = 0;
		
		/** IMediaAccess
		  *
          * Burn
          *
          * Creates the ISO item tree, writes the data to CD/DVD and cleans up.
          *
          * @param	List of files name to be burnt.
		  *
		  * @return	Return true on burning successfully, else returns false
          */
		virtual bool Burn(std::vector<FilePathName> FilePathNameVector) = 0;

		/** IMediaAccess
		  *
          * CancelBurn
          *
          * Cancels the burn operation.
          *
          */
		virtual void CancelBurn() = 0;

		
		/** IMediaAccess
		  *
          * EjectMedia
          *
          * Ejects the media
          *
          */
		virtual void EjectMedia() = 0;

		/** IMediaAccess
		  *
          * EjectMedia
          *
          * Get information of all available drives
          *
          * @param	List of files name to be burnt
		  *
		  * @return	Returns a list of device information
          */
		virtual vector<string> GetAllDeviceInfo() = 0;

		/** IMediaAccess
		  *
          * GetSelectedDevice
          *
          * Get device letter of selected device for buring
          *
		  *
		  * @return	Returns device letter of selected device
          */
		virtual string GetSelectedDevice() = 0;

		/** IMediaAccess
		  *
          * GetMediaStatus
          *
          * Get the current status of Media
          *
		  *
		  * @return	MediaStatus
          */
		MediaStatus GetMediaStatus() {return m_MediaStatus;}

		/** IMediaAccess
		  *
          * RegisterBurnStatusCallback
          *
          * Register callback interface
          *
		  * @param - ICallbackBurnStatus interface to be registered 
          */
		void RegisterBurnStatusCallback(ICallbackBurnStatus* callbackBurnStatus) 
		{
			m_CallbackBurnStatus = callbackBurnStatus;
		}
		
	
		/*bool isBurnVerficationEnabled() {return m_burnVerficationEnabled;}
		bool isForceOverwriteSet() {return m_ForceOverwrite;}
		bool isUnderburnEnabled() {return m_underburnEnabled;}*/
		
		/** IMediaAccess
		  *
          * setBurnVerfication
          *
          * To enable burn verification after write operation.
          *
		  * @param - True to enable burn verification, false to disable.
          */
		void setBurnVerfication(bool burnVerificationEnabled){m_burnVerficationEnabled = burnVerificationEnabled;}
		
		/** IMediaAccess
		  *
          * setUnderburn
          *
          * Set  Buffer Underrun Free(BUF) recording
          *
		  * @param - True to enable BUF, false to disable BUF
          */
		void setUnderburn(bool underburnEnabled){m_underburnEnabled = underburnEnabled;}
		
		/** IMediaAccess
		  *
          * setForceOverwrite
          *
          * Set to overwrite CD/DVD even if it is not empty
          *
		  * @param - True to enable force overwrite, false to diable force overwrite
          */
		void setForceOverwrite(bool forceOverwrite){m_ForceOverwrite = forceOverwrite;}
		
		/** IMediaAccess
		  *
          * setVolumeLabel
          *
          * Set the CD/DVD title
          *
		  * @param - name of CD/DVD 
          */
		void setVolumeLabel(string volumeLabel) {m_volumeLabel = volumeLabel;}
		
		/** IMediaAccess
		  *
          * setCdArchive
          *
          * To set the media type to either CD/DVD
          *
		  * @param - True - CD, False - DVD
          */
		void setCdArchive(bool CdArchive) {m_CDArchive = CdArchive;}

	protected:
		ICallbackBurnStatus* m_CallbackBurnStatus;
		MediaStatus m_MediaStatus;

		bool m_burnVerficationEnabled;
		bool m_ForceOverwrite;
		bool m_underburnEnabled;
		bool m_CDArchive;

		string m_volumeLabel;
		

	};


}

#endif 