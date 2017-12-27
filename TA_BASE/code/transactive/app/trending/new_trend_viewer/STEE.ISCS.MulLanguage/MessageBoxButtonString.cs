using System;
using System.Collections.Generic;
using System.Text;

using STEE.ISCS.Log;

namespace STEE.ISCS.MulLanguage
{
    class MessageBoxButtonString
    {
        private const string CLASS_NAME = "MessageBoxButtonString";
        //const
        public const string MESSAGEBOX_OKBUTTON = "MessageBoxForm_OKButton";
        public const string MESSAGEBOX_YESBUTTON = "MessageBoxForm_YesButton";
        public const string MESSAGEBOX_NOBUTTON = "MessageBoxForm_NoButton";
        public const string MESSAGEBOX_CANCELBUTTON = "MessageBoxForm_CancelButton";
        public const string MESSAGEBOX_RETRYBUTTON = "MessageBoxForm_RetryButton";
        public const string MESSAGEBOX_ABORTBUTTON = "MessageBoxForm_AbortButton";
        public const string MESSAGEBOX_IGNOREBUTTON = "MessageBoxForm_IgnoreButton";

        public const string MSG_OK_BUTTON_EN = "OK";
        public const string MSG_YES_BUTTON_EN = "Yes";
        public const string MSG_NO_BUTTON_EN = "No";
        public const string MSG_CANCEL_BUTTON_EN = "Cancel";
        public const string MSG_RETRY_BUTTON_EN = "Retry";
        public const string MSG_ABORT_BUTTON_EN = "Abort";
        public const string MSG_IGNORE_BUTTON_EN = "Ignore";

        private static MessageBoxButtonString s_MessageBoxButtonString = null;
        private bool m_init = false;

        public static MessageBoxButtonString GetInstance()
        {
            if (s_MessageBoxButtonString == null)
            {
                s_MessageBoxButtonString = new MessageBoxButtonString();
            }
            return s_MessageBoxButtonString;
        }

        public static void ReleaseInstance()
        {
            if (s_MessageBoxButtonString != null)
            {
                s_MessageBoxButtonString.RemoveStringHelper();
                s_MessageBoxButtonString = null;
            }
        }

        private MessageBoxButtonString()
        {

        }
        public void AddStringHelper()
        {
            string Function_Name = "AddStringHelper";
            try
            {
                if (!m_init)
                {
                    StringHelper.GetInstance().AddStringItem(MESSAGEBOX_OKBUTTON, MSG_OK_BUTTON_EN, ChineseString.MSG_OK_BUTTON_CH, "Message Box OK Button");
                    StringHelper.GetInstance().AddStringItem(MESSAGEBOX_YESBUTTON, MSG_YES_BUTTON_EN, ChineseString.MSG_YES_BUTTON_CH, "Message Box OK Button");
                    StringHelper.GetInstance().AddStringItem(MESSAGEBOX_NOBUTTON, MSG_NO_BUTTON_EN, ChineseString.MSG_NO_BUTTON_CH, "Message Box OK Button");
                    StringHelper.GetInstance().AddStringItem(MESSAGEBOX_CANCELBUTTON, MSG_CANCEL_BUTTON_EN, ChineseString.MSG_CANCEL_BUTTON_CH, "Message Box OK Button");
                    StringHelper.GetInstance().AddStringItem(MESSAGEBOX_RETRYBUTTON, MSG_RETRY_BUTTON_EN, ChineseString.MSG_RETRY_BUTTON_CH, "Message Box OK Button");
                    StringHelper.GetInstance().AddStringItem(MESSAGEBOX_ABORTBUTTON, MSG_ABORT_BUTTON_EN, ChineseString.MSG_ABORT_BUTTON_CH, "Message Box OK Button");
                    StringHelper.GetInstance().AddStringItem(MESSAGEBOX_IGNOREBUTTON, MSG_IGNORE_BUTTON_EN, ChineseString.MSG_IGNORE_BUTTON_CH, "Message Box OK Button");
                    m_init = true;
                }
            }
            catch (Exception localexception)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localexception.ToString());                
            }
        }

        public void RemoveStringHelper()
        {
            string Function_Name = "RemoveStringHelper";
            try
            {
                if (m_init)
                {
                    StringHelper.GetInstance().RemoveStringItem(MESSAGEBOX_OKBUTTON);
                    StringHelper.GetInstance().RemoveStringItem(MESSAGEBOX_YESBUTTON);
                    StringHelper.GetInstance().RemoveStringItem(MESSAGEBOX_NOBUTTON);
                    StringHelper.GetInstance().RemoveStringItem(MESSAGEBOX_CANCELBUTTON);
                    StringHelper.GetInstance().RemoveStringItem(MESSAGEBOX_RETRYBUTTON);
                    StringHelper.GetInstance().RemoveStringItem(MESSAGEBOX_ABORTBUTTON);
                    StringHelper.GetInstance().RemoveStringItem(MESSAGEBOX_IGNOREBUTTON);
                }
            }
            catch (Exception localexception)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localexception.ToString());
            }
        }
    }
}
