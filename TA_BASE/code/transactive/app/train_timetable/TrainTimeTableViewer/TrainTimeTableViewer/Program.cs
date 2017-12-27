using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Threading;
using STEE.ISCS.MVC;
using TrainTimeTableViewer.View;
using TrainTimeTableViewer.Controller;
using TrainTimeTableViewer.Common;
using NET_ISCS_Bridge;

namespace TrainTimeTableViewer
{
    static class Program
    {
        private const string CLASS_NAME = "TrainTimeTableView.Program";

        
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            string FUNCTION_NAME = "Main";
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);  

            try
            {
                
                TrainTimeTableApp app = new TrainTimeTableApp();

                app.StartApplication("TRAINTIMETABLE VIEWER", TrainTimeTableConst.TRAINTIMETABLE_VIEWER_APP_TYPE);

                app.Exit();
            }
            catch (Exception localExecption)
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, 111, EDebugLevelManaged.DebugError, localExecption.ToString());
            }

           
        }

        
    }
}
