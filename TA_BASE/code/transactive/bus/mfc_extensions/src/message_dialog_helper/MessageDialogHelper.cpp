#include "..\stdafx.h"
#include "bus/mfc_extensions/src/message_dialog_helper/MessageDialogHelper.h"
#include "bus/mfc_extensions/src/message_box/XMessageBox.h"
#include <cassert>


#define ID_CLOSE   10
#define ID_TIMEOUT 11
#define ID_TIMER   123


namespace TA_Base_Bus
{

    const std::string MessageDialogHelper::CANCEL   = "CANCEL";
    const std::string MessageDialogHelper::TIMEOUT  = "TIMEOUT";
    const std::string MessageDialogHelper::CLOSE    = "CLOSE";


    IMPLEMENT_DYNAMIC(MessageDialogHelper, CDialog)


    MessageDialogHelper::MessageDialogHelper(CWnd* pParent /*=NULL*/)
        : CDialog(MessageDialogHelper::IDD, pParent),
          m_timeoutSeconds(0)
    {
    }

	MessageDialogHelper::MessageDialogHelper( const std::vector<std::string>& buttonNames, const std::string& caption, const std::string& message, UINT nStyle, const int timeoutSeconds/* =0 */ )
		: CDialog(MessageDialogHelper::IDD, NULL),
		  m_caption(caption),
		  m_message(message),
		  m_timeoutSeconds(timeoutSeconds)
	{
		for ( size_t i = 0; i < buttonNames.size(); ++i )
		{
			m_buttonID2NameMap[IDC_MESSAGE_BUTTON1 + i] = buttonNames[i];
		}
	}


    MessageDialogHelper::~MessageDialogHelper()
    {
    }


    void MessageDialogHelper::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
    }


    BEGIN_MESSAGE_MAP(MessageDialogHelper, CDialog)
        ON_BN_CLICKED(IDC_MESSAGE_BUTTON1, &MessageDialogHelper::OnBnClickedMessageButton1)
        ON_BN_CLICKED(IDC_MESSAGE_BUTTON2, &MessageDialogHelper::OnBnClickedMessageButton2)
        ON_BN_CLICKED(IDC_MESSAGE_BUTTON3, &MessageDialogHelper::OnBnClickedMessageButton3)
        ON_BN_CLICKED(IDC_MESSAGE_BUTTON4, &MessageDialogHelper::OnBnClickedMessageButton4)
        ON_WM_TIMER()
    END_MESSAGE_MAP()


    // MessageDialogHelper message handlers

    BOOL MessageDialogHelper::OnInitDialog()
    {
        CDialog::OnInitDialog();

        for ( size_t i = IDC_MESSAGE_BUTTON1; i <= IDC_MESSAGE_BUTTON4; ++i )
        {
            GetDlgItem(i)->ShowWindow(SW_HIDE);
        }

        for ( std::map<int, std::string>::iterator it = m_buttonID2NameMap.begin(); it != m_buttonID2NameMap.end(); ++it )
        {
            GetDlgItem(it->first)->SetWindowText( it->second.c_str() );
            GetDlgItem(it->first)->ShowWindow(SW_SHOW);
        }

        GetDlgItem(IDC_STATIC_MESSAGE)->SetWindowText( m_message.c_str() );
        SetWindowText(m_caption.c_str());

        if ( m_timeoutSeconds != 0 )
        {
            m_startTime = time(NULL);
            SetTimer(ID_TIMER, 1000, NULL);
        }

        return TRUE;
    }


    const std::string& MessageDialogHelper::show()
    {
        switch ( DoModal() )
        {
        case IDOK:
            return m_result;

        case IDCANCEL:
            return CANCEL;

        case ID_TIMEOUT:
            return TIMEOUT;

        case ID_CLOSE:
        default:
            return CLOSE;
        }
    }


    void MessageDialogHelper::close()
    {
        if ( IsWindow(m_hWnd) )
        {
            EndDialog(ID_CLOSE);
        }
    }


    void MessageDialogHelper::OnBnClickedMessageButton1()
    {
        buttonClicked(IDC_MESSAGE_BUTTON1);
    }


    void MessageDialogHelper::OnBnClickedMessageButton2()
    {
        buttonClicked(IDC_MESSAGE_BUTTON2);
    }


    void MessageDialogHelper::OnBnClickedMessageButton3()
    {
        buttonClicked(IDC_MESSAGE_BUTTON3);
    }


    void MessageDialogHelper::OnBnClickedMessageButton4()
    {
        buttonClicked(IDC_MESSAGE_BUTTON4);
    }


    void MessageDialogHelper::buttonClicked(int buttonID)
    {
        m_result = m_buttonID2NameMap[buttonID];

        KillTimer(ID_TIMER);
        EndDialog(IDOK);
    }


    void MessageDialogHelper::OnTimer(UINT_PTR nIDEvent)
    {
        int elapsedTime = time(NULL) - m_startTime;

        if ( m_timeoutSeconds <= elapsedTime )
        {
            KillTimer(ID_TIMER);
            EndDialog(ID_TIMEOUT);
        }

        CDialog::OnTimer(nIDEvent);
    }


	int MessageDialogHelper::showMessageBox( HWND hwnd, LPCTSTR lpszMessage, LPCTSTR lpszCaption, UINT uStyle, UINT uHelpId, int timeoutSeconds )
	{
        struct Closer
        {
            Closer( MessageDialogHelper* helper, int timeoutSeconds )
                : m_thread( NULL ),
                  m_helper( helper )
            {
                if ( 0 < timeoutSeconds )
                {
                    m_helper->setTimeout( timeoutSeconds + 1 );
                    m_thread = ::CreateThread( NULL, 0, threadFun, m_helper, 0, NULL );
                }
            }

            ~Closer()
            {
                if ( m_thread != NULL )
                {
                    ::WaitForSingleObject( m_thread, INFINITE );
                    ::CloseHandle( m_thread );
                }
            }

            static DWORD WINAPI threadFun( LPVOID helper )
            {
                MessageDialogHelper* m_helper = reinterpret_cast<MessageDialogHelper*>(helper);

                DWORD wait = m_helper->waitPopup();
                assert( WAIT_OBJECT_0 == wait ); // !!!oops - before victory was won he died!!!

                m_helper->startCountdown();

                while ( true )
                {
                    if ( true == m_helper->isClosed() )
                    {
                        return 0;
                    }

                    if ( true == m_helper->isTimeout() )
                    {
                        m_helper->closeMessageBox();
                        return 0;
                    }

                    ::Sleep(100);
                }
            }

            HANDLE m_thread;
            MessageDialogHelper* m_helper;
        };

        Closer closer( this, timeoutSeconds );

        return m_XMessageBoxProxy.XMessageBox(hwnd, lpszMessage,lpszCaption, uStyle, uHelpId);
	}


    bool MessageDialogHelper::isClosed()
    {
        return m_XMessageBoxProxy.isClosed();
    }


    void MessageDialogHelper::closeMessageBox()
    {
        m_XMessageBoxProxy.close();
    }


    DWORD MessageDialogHelper::waitPopup()
    {
        return m_XMessageBoxProxy.waitPopup( m_timeoutSeconds * 1000 );
    }


    bool MessageDialogHelper::isTimeout()
    {
        return ( m_timeoutSeconds <= ( time(NULL) - m_startTime ) );
    }


    void MessageDialogHelper::setTimeout( const int timeoutSeconds )
    {
        m_timeoutSeconds = timeoutSeconds;
    }


    void MessageDialogHelper::startCountdown()
    {
        m_startTime = time(NULL);
    }

}
