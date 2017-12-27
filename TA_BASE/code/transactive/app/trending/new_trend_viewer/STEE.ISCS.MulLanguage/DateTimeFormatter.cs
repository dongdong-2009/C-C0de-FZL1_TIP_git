using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace STEE.ISCS.MulLanguage
{
    public class DateTimeFormatter
    {
        private static DateTimeFormatter s_DateTimeFormatter;
        private static Object s_objectLock = new Object();
        
        private string m_DateFormatStr;

        private DateTimeFormatter()
        {

        }

        public static DateTimeFormatter getInstance()
        {
            if (s_DateTimeFormatter != null) return s_DateTimeFormatter;
            lock (s_objectLock)
            {
                if (s_DateTimeFormatter == null) s_DateTimeFormatter = new DateTimeFormatter();
            }
            return s_DateTimeFormatter;
        }

        public static void ReleaseInstance()
        {
            lock (s_objectLock)
            {
                if (s_DateTimeFormatter != null)
                {
                    s_DateTimeFormatter = null;
                }
            }
        }

        public void setDateFormatStr(string formatStr)
        {
            m_DateFormatStr = formatStr; 
        }

        public string getDateFormatStr()
        {
            return m_DateFormatStr;
        }

    }
}
