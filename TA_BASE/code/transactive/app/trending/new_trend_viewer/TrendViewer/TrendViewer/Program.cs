using System;
using System.Runtime.InteropServices;

using System.Collections.Generic;
using System.Windows.Forms;
using STEE.ISCS.Log;
using DAO.Trending.Helper;

using STEE.ISCS.MVC;
using TrendViewer.View;
using TrendViewer.Common;
using TrendViewer.Controller;
using STEE.ISCS.MulLanguage;
using TrendingHelper;
using System.Threading;

namespace TrendViewer
{
    static public class Program
    {
        private const string CLASS_NAME = "TrendViewer.Program";

        [DllImport("kernel32.dll")]
        static extern IntPtr CreateEvent(IntPtr lpEventAttributes, bool bManualReset, bool bInitialState, string lpName);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        internal static extern IntPtr OpenEvent(int desiredAccess, bool inheritHandle, string name);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool CloseHandle(IntPtr hObject);

        //create a win32 event, name as Global\\TrendViewerEvent_01, or Global\\TrendViewerEvent_02,... if the event doesn't exist.
        //and set log file path+name by the above event.
        static string  CreateWin32EventAndSetLogFile()
        {
            string Function_Name = "CreateWin32EventAndSetLogFile";

            try
            {
                IntPtr _Attributes = IntPtr.Zero;
                IntPtr hEvent = IntPtr.Zero;
                string eventName = "";
                for (int i = 1; i < TrendViewConst.INSTANCE_MAX_NUM +1 ; i++)
                {
                    eventName = TrendViewConst.SYSTEM_EVENT_NAME_PREFIX + i;
                    hEvent = OpenEvent(0x1F0003, false, eventName);  //0x001F0003:EVENT_ALL_ACCESS
                    if (hEvent == IntPtr.Zero)  //means this event does not exist!
                    {
                        hEvent = CreateEvent(_Attributes, false, false, eventName);
                        LogHelper.setLogFile( TrendViewConst.LOG_FILE_PATH_PRERIX + i + ".log");
                        LogHelper.Info(CLASS_NAME, Function_Name, "Create Event:" + eventName + ", and set log file.");
                        break;
                    }
                    else
                    {
                        CloseHandle(hEvent);
                    }
                }
                return eventName;
                
            }
            catch (System.Exception ex)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, "Exception caught: " + ex.ToString());
                return "";
            }
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            string Function_Name = "Main";
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            // Add the event handler for handling UI thread exceptions to the event.
            Application.ThreadException += new ThreadExceptionEventHandler(Program.UIThreadExceptionHandler);

            try
            {
                ConfigureFileHelper.GetInstance().init();

                //LanguageTypeHelper.GetInstance().SetLanaguageType(LanguageType.English); 
                LanguageType type = LanguageTypeHelper.GetInstance().GetLanTypeByLanStr(ConfigureFileHelper.GetInstance().LanguageStr);
                LanguageTypeHelper.GetInstance().SetLanaguageType(type);

                DAOHelper.SetEncodingChange(ConfigureFileHelper.GetInstance().EncodingChange);

                LanguageHelper.InitAllStrings();

                string eventName = CreateWin32EventAndSetLogFile();
                
                ViewManager.GetInstance().RegisterViewFactory(new TrendingViewFactory());
                IView view = ViewManager.GetInstance().GetView(TrendViewConst.TrendView);  //viewID is "" 
                Form frm = (Form)view;
                if (args.Length > 0)
                {
                    TrendViewController trendController = (TrendViewController)view.getController();
                    trendController.DrawTrendView(ref frm, args[0]);
                    
                }

                Application.Run(frm);
                
                //event will be closed automatically.
                LogHelper.Info(CLASS_NAME, Function_Name, "Closed Event:" + eventName);
            }
            catch (Exception localExecption)
            {
                LogHelper.Error("TrendViewer.Program", Function_Name,localExecption.ToString());
            }
        }

        private static void UIThreadExceptionHandler(object sender, ThreadExceptionEventArgs t)
        { 
            MessageBoxDialog.Show(
                  StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_UnhandledError, LanguageHelper.TrendViewer_Msg_UnhandledError_EN),
                   StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                  MessageBoxButtons.OK, MessageBoxIcon.Error);

            LogHelper.Error("TrendViewer.Program", "UIThreadExceptionHandler", t.Exception.ToString());
        }



    }
}