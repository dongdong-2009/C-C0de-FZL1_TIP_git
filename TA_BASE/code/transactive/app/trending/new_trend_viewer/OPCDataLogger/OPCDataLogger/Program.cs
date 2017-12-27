using System;
using System.Collections.Generic;
using System.Windows.Forms;
using STEE.ISCS.MulLanguage;
using DAO.Trending;

namespace OPCDataLogger
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            ConfigureFileHelper.GetInstance().init();
            
            //LanguageType type = LanguageTypeHelper.GetInstance().GetLanTypeByLanStr(ConfigureFileHelper.GetInstance().LanguageStr);
            //LanguageTypeHelper.GetInstance().SetLanaguageType(type);

            DAOHelper.SetEncodingChange(ConfigureFileHelper.GetInstance().EncodingChange);

            STEE.ISCS.Log.LogHelper.setLogFile("../logs/Log_OPCDataLogger.txt");
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new OPCDataLogger());
        }
    }
}