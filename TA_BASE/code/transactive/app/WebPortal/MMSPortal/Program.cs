using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace MMSPortal
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            try
            {  
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
           // Application.Run(new Form1());
            MMSPortalApp app = new MMSPortalApp();
            
            app.StartApplication("MMSPortal", 159);

             app.Exit();
            }
            catch(Exception exception)
            {

            }
        }
    }
}
