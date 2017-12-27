using System;
using System.Collections.Generic;
using System.Text;
using System.Globalization;

namespace STEE.ISCS.MulLanguage
{
    /// <summary>
    /// This class helps to support multiple language in UI
    /// </summary>
    public class StringHelper
    {
        private const string CLASS_NAME = "STEE.ISCS.MulLanguage";

        private static StringHelper s_StringHelper;
        private static Object s_objectLock = new Object();
        private Dictionary<string, StringItem> m_StringItemDic; 
       // private LanguageType m_langType;    //which language will be displayed.

        private StringHelper()
        {
            m_StringItemDic = new Dictionary<string, StringItem>();
           // m_langType = LanguageType.Default;

        }
        
        public static StringHelper GetInstance()
        {
            if (s_StringHelper != null) return s_StringHelper;
            lock (s_objectLock)
            {
                if (s_StringHelper == null) s_StringHelper = new StringHelper();
            }
            return s_StringHelper;
        }

        public static void ReleaseInstance()
        {            
            lock (s_objectLock)
            {
                if (s_StringHelper != null)
                {
                    s_StringHelper.RemoveAllStringItem();
                    s_StringHelper.m_StringItemDic = null;
                    s_StringHelper = null;
                }
            }
        }

        

        /// <summary>
        /// Set language type, it should be called before displaying any UI.
        /// this function will be replaced by a configuation file
        /// </summary>
        /// <param name="langType"></param>
       /* public void SetLanaguageType(LanguageType langType)
        {
            m_langType = langType;
        }
        */
       
        /// <summary>
        /// Add string item, it is a temporary solution.
        /// it will be replaced by reading from database table.
        /// </summary>
        /// <param name="ID">ID should follow this format, AppName_FormName_StringName/AppName_StringName</param>
        /// <param name="TextEnglish"></param>
        /// <param name="TextChinese"></param>
        /// <param name="Description"></param>
        public void  AddStringItem(string ID, string TextEnglish, string TextChinese, string Description)
        {
            if (m_StringItemDic.ContainsKey(ID))
            {
                throw new ArgumentException("Duplicated ID:" + ID);
            }
            StringItem item = new StringItem();
            item.ID = ID;
            item.TextChinese = TextChinese;
            item.TextEnglish = TextEnglish;
            item.Description = Description;
            m_StringItemDic.Add(ID, item);
        }


        public void RemoveStringItem(string ID)
        {
            if (!m_StringItemDic.ContainsKey(ID))
            {
                throw new ArgumentException("No found ID:" + ID);
            }
            StringItem item = m_StringItemDic[ID];
            item = null;
            m_StringItemDic.Remove(ID);
        }

        public void RemoveAllStringItem()
        {
            foreach (KeyValuePair<string, StringItem> pair in m_StringItemDic)
            {
                StringItem value = m_StringItemDic[pair.Key];
                value = null;
            }

            m_StringItemDic.Clear();
        }
        
        /// <summary>
        /// Get string value
        /// </summary>
        /// <param name="ID">ID of string</param>
        /// <param name="defaultValue">default English value</param>
        /// <returns></returns>
        public string getStringValue(string ID, string defaultValue)
        {
            if (!m_StringItemDic.ContainsKey(ID))
            {
                return defaultValue;
            }
            StringItem item = m_StringItemDic[ID];
            switch (LanguageTypeHelper.GetInstance().GetCurrentLanguageType())
            {
                
                case LanguageType.English:
                    return item.TextEnglish;
                case LanguageType.Chinese:
                    return item.TextChinese;
                default:
                    return item.TextEnglish;
            }
            
        }





        

    }
}
