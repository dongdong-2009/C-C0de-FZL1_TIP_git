using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using System.Diagnostics;
using System.Runtime.InteropServices;

using STEE.ISCS.Log;
using STEE.ISCS.OPCProxy;

namespace OPCDataLogger
{
    /// <summary>
    /// This class is responsible for the display of OPCDataLogger form and 
    /// is responisble for intialising DataLoggerCache for logging.
    /// </summary>
    public partial class OPCDataLogger : Form
    {
        private const string CLASS_NAME = "OPCDataLogger.OPCDataLogger";

        private bool m_exitFlag = false;
        public Process m_process = null;

        //application shutdown delegate
        private delegate void ApplicationExitCallback();

        /// <summary>
        /// Checks for the exitence of this process in the system, if not starts logging.
        /// </summary>
        public OPCDataLogger()
        {
            string Function_Name = "OPCDataLogger";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            System.Diagnostics.Process[] ps = System.Diagnostics.Process.
                 GetProcessesByName(System.Diagnostics.Process.GetCurrentProcess().ProcessName);
            m_process = Process.GetCurrentProcess();

            if (ps.Length > 1)
            {
                LogHelper.Error("Already an instance is running");
                m_process.Kill();
            }

            InitializeComponent();

            OPCDataPointManager.GetInstance().ExitApplicationEvent += CloseApplication;
            OPCDataPointManager.GetInstance().InitializeOPCSetting(m_process.Id.ToString());

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }   

        /// <summary>
        ///  Terminates the application.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OPCDataLogger_FormClosed(object sender, FormClosedEventArgs e)
        {
            string Function_Name = "OPCDataLogger_FormClosed";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            OPCDataPointManager.GetInstance().TerminateAndWait();
            OPCDataPointManager.ReleaseInstance();

            //System.Threading.Thread.Sleep(1000);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        #region Display Icon

        //  <summary>
        //  Resize Event - To hide this clas when it is minimized
        //  </summary>
        private void OPCDataLogger_Resize(object sender, EventArgs e)
        {
            string Function_Name = "OPCDataLogger_Resize";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (FormWindowState.Minimized == WindowState)
                Hide();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        //  <summary>
        //  OPCDataLogger Load Event - Intialization of basic parameters
        //  </summary>
        private void OPCDataLogger_Load(object sender, EventArgs e)
        {
            string Function_Name = "OPCDataLogger_Load";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (FormWindowState.Minimized == WindowState)
                Hide();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        private void OPCDataLogger_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "OPCDataLogger_Paint";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            Graphics obj_Graph = e.Graphics;

            LinearGradientBrush b = new LinearGradientBrush(new System.Drawing.Rectangle(0, 0, this.Width, this.Height), Color.LightBlue, Color.BlanchedAlmond, LinearGradientMode.Vertical);

            obj_Graph.FillRectangle(b, new System.Drawing.Rectangle(0, 0, this.Width, this.Height));

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Hides the application till the user selects close menu.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OPCDataLogger_FormClosing(object sender, FormClosingEventArgs e)
        {
            string Function_Name = "OPCDataLogger_FormClosing";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (!m_exitFlag)
            {
                e.Cancel = true;
                Hide();
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// This method is responsible for setting exit flag and close the application.
        /// </summary>
        public void CloseApplication()
        {
            string Function_Name = "CloseApplication";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            m_exitFlag = true;
            //get main application form 
            FormCollection forms = Application.OpenForms;
            Form mainFrm = forms[0];
            if (mainFrm.InvokeRequired)
            {
                mainFrm.BeginInvoke(new ApplicationExitCallback(CloseApplication));              
            }
            else
            {
                //m_process.Kill();
                Application.Exit();
                System.Environment.Exit(0);
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Closes the applicaiton.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void closeApplicationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string Function_Name = "closeApplicationToolStripMenuItem_Click";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            CloseApplication();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        public void RestoreMenuStrip_Click(object sender, System.EventArgs e)
        {
            string Function_Name = "RestoreMenuStrip_Click";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
           //restart the DataLogger.
            // terminate the data logging thread first.
            //OPCDataPointManager.GetInstance().TerminateThread();
            OPCDataPointManager.GetInstance().TerminateAndWait();
            //OPCServerProxy.ReleaseInstance();
            try
            {
                OPCServerProxy.GetInstance().RemoveAllOPCItem();
            }
            catch (Exception Ex)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, Ex);
                //OPCBridge service is shutdown/crashed.
                //@todo
            }
            OPCDataPointManager.ReleaseInstance();
            System.Threading.Thread.Sleep(1000);
            OPCDataPointManager.GetInstance().ExitApplicationEvent += CloseApplication;
            OPCDataPointManager.GetInstance().InitializeOPCSetting(m_process.Id.ToString());

            //show the GUI to user to indicate restore is done.
            Show();
            this.WindowState = System.Windows.Forms.FormWindowState.Normal;
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        #endregion
    }
}