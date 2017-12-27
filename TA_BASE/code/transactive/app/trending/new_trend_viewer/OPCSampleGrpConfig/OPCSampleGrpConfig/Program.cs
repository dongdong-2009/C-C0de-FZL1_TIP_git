using System;
using System.Collections.Generic;
using System.Windows.Forms;
using STEE.ISCS.Log;
using STEE.ISCS.MVC;
using OPCSampleGrpConfig.View;
using STEE.ISCS.MulLanguage;
using OPCSampleGrpConfig.Controller;
using DAO.Trending;
using TrendingHelper;
using DAO.Trending.Common;
using DAO.Trending.Helper;

namespace OPCSampleGrpConfig
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]

        static void Main(string[] args)
        {
            LogHelper.setLogFile("../logs/Log_OPCSampleGrpConfig.txt");
            
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            try
            {
                OPCSampleGrpConfig.Common.FormCaptionHelper.GetInstance().AddCaptionsToStringHelper();
                ConfigureFileHelper.GetInstance().init();
                //Connect to central database
                DBConnectionStrings.GetInstance().AddConnectionString(ConfigureFileHelper.GetInstance().ConnectionStringConfig);                 
 
                //set the language  and encoding change
                LanguageType type = LanguageTypeHelper.GetInstance().GetLanTypeByLanStr(ConfigureFileHelper.GetInstance().LanguageStr);
                LanguageTypeHelper.GetInstance().SetLanaguageType(type);
                DAOHelper.SetEncodingChange(ConfigureFileHelper.GetInstance().EncodingChange);
                ViewManager.GetInstance().RegisterViewFactory(new OPCSampleGrpConfigViewFactory());
                IView view = ViewManager.GetInstance().GetView(OPCSampleGrpConfigStart.OPCSAMPLEGRPCONFIGSTARTFRM);
                Form frm = (Form)view;                
                if (args.Length > 0)
                {
                    OPCSampleGrpConfigStartController sampleGrpController = (OPCSampleGrpConfigStartController)view.getController();
                    sampleGrpController.SetSampleGrpConfigLocation(ref frm, args[0]);
                }
                Application.Run(frm);              
            }
            catch (Exception localExecption)
            {
                Console.WriteLine(localExecption.ToString());                
            }
            DBConnectionStrings.GetInstance().TerminateMonitorThread();
        }
    }
}