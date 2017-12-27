using System;
using System.Collections.Generic;
using System.Text;

namespace STEE.ISCS.MulLanguage
{
    public class LanguageTypeHelper
    {
        private const string CLASS_NAME = "STEE.ISCS.MulLanguage.LanguageHelper";

        private const string DateFormat_CH = "yyyy-MM-dd";
        private const string DateFormat_EN = "dd-MM-yyyy";

        private static LanguageTypeHelper s_LangHelper;
        private static Object s_objectLock = new Object();
        private LanguageType m_langType;    //which language will be displayed.

        private LanguageTypeHelper()
        {
            m_langType = LanguageType.Default;
        }

        public static LanguageTypeHelper GetInstance()
        {
            if (s_LangHelper != null) return s_LangHelper;
            lock (s_objectLock)
            {
                if (s_LangHelper == null) s_LangHelper = new LanguageTypeHelper();
            }
            return s_LangHelper;
        }

        public static void ReleaseInstance()
        {
            lock (s_objectLock)
            {
                if (s_LangHelper != null)
                {
                    s_LangHelper = null;
                }
            }
        }

        /// <summary>
        /// Set language type, it should be called before displaying any UI.
        /// this function will be replaced by a configuation file
        /// </summary>
        /// <param name="langType"></param>
        public void SetLanaguageType(LanguageType langType)
        {
            m_langType = langType;
            if(langType == LanguageType.Chinese)
            {
                DateTimeFormatter.getInstance().setDateFormatStr(DateFormat_CH);
            }
            else
            {
                DateTimeFormatter.getInstance().setDateFormatStr(DateFormat_EN);
            }
        }

        public LanguageType GetCurrentLanguageType()
        {
            return m_langType;
        }

        public LanguageType GetLanTypeByLanStr(string languageStr)
        {
            languageStr = languageStr.ToLower();

            if( languageStr == LanguageStr.ENGLISH)
            {
                return LanguageType.English;
            }
            if (languageStr == LanguageStr.CHINESE)
            {
                return LanguageType.Chinese;
            }
            return LanguageType.English;
        }

    }
}
