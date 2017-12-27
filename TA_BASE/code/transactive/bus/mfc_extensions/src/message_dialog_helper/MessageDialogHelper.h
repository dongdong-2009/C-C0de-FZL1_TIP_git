#pragma once
#include "bus/mfc_extensions/src/Resource.h"
#include "bus/mfc_extensions/src/message_box/XMessageBox.h"
#include <string>
#include <vector>
#include <map>

// MessageDialogHelper dialog

namespace TA_Base_Bus
{

// Make this work in a DLL
#ifdef IMPORT_EXTENSIONS
	class __declspec(dllimport) MessageDialogHelper : public CDialog
#else
	class AFX_EXT_CLASS MessageDialogHelper : public CDialog
#endif
    {
        DECLARE_DYNAMIC(MessageDialogHelper)

    public:

        MessageDialogHelper(CWnd* pParent = NULL);   // standard constructor
		MessageDialogHelper( const std::vector<std::string>& buttonNames, const std::string& caption, const std::string& message, UINT nStyle, const int timeoutSeconds = 0 );
		virtual ~MessageDialogHelper();

		int showMessageBox( HWND hwnd,
                            LPCTSTR lpszMessage,
                            LPCTSTR lpszCaption = NULL,
                            UINT uStyle = MB_OK|MB_ICONEXCLAMATION,
                            UINT uHelpId = 0,
                            int timeoutSeconds = 0 );

		void closeMessageBox();

        // Dialog Data
        enum { IDD = IDD_DIALOG_MESSAGE };

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();

    public:

        const std::string& show();
        void close();

    private:

        afx_msg void OnBnClickedMessageButton1();
        afx_msg void OnBnClickedMessageButton2();
        afx_msg void OnBnClickedMessageButton3();
        afx_msg void OnBnClickedMessageButton4();
        afx_msg void OnTimer(UINT_PTR nIDEvent);
        void buttonClicked( int buttonID );

        bool isTimeout();
        bool isClosed();
        DWORD waitPopup();
        void setTimeout( const int timeoutSeconds );
        void startCountdown();

    private:

        std::map<int, std::string> m_buttonID2NameMap;
        std::string m_caption;
        std::string m_message;
        int m_timeoutSeconds;
        std::string m_result;
        time_t m_startTime;
        XMessageBoxProxy m_XMessageBoxProxy;

    public:

        static const std::string CANCEL;
        static const std::string TIMEOUT;
        static const std::string CLOSE;

        DECLARE_MESSAGE_MAP()
    };

}
