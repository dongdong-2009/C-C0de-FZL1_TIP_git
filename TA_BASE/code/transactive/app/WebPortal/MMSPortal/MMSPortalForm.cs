using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace MMSPortal
{
    public partial class MMSPortalForm : Form
    {
        private string m_StartURL;
        private string m_ExitURL;
        private int m_refresh = 0; //
        private string username = "XXX" ;
        private string password = "YYY";

        public MMSPortalForm()
        {
            InitializeComponent();
        }

        public void setParameters(string startUrl,string exitUrl,int refresh, int frmwidth, int frmHeight, string windowCaption, bool enableMax)
        {
            m_StartURL = startUrl;
            //replace username and password
            if (m_StartURL.Contains("u="))
            {
                m_StartURL = m_StartURL.Replace("<username>", username);
                m_StartURL = m_StartURL.Replace("<password>", password);
            }
            m_ExitURL = exitUrl;
            if (refresh != 0)
            {
                m_refresh = refresh * 60 * 1000; //to msec
                this.refreshTimer.Interval = m_refresh;
                this.refreshTimer.Enabled = true;
            }           
            webBrowser1.Navigate(m_StartURL);
            this.Size = new Size(frmwidth, frmHeight);
            this.Text = windowCaption;
            this.MaximizeBox = enableMax;
        }


        void timer1_Tick(object sender, System.EventArgs e)
        {
            webBrowser1.Refresh();
        }


        void webBrowser1_Navigated(object sender, System.Windows.Forms.WebBrowserNavigatedEventArgs e)
        {
            //this.Text = "Navigated:" + e.Url.ToString();
            if (m_ExitURL != "")
            {
                if (e.Url.ToString().Contains(m_ExitURL))
                {
                    this.Close();
                }
            }
        }


        void webBrowser1_DocumentCompleted(object sender, System.Windows.Forms.WebBrowserDocumentCompletedEventArgs e)
        {
            //this.Text = "Loaded:" + e.Url.ToString();
        }

    }
}
