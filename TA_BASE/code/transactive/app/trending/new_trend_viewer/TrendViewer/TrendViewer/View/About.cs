using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using STEE.ISCS.MVC;
using STEE.ISCS.GenericGUI;
using TrendViewer.Common;
using STEE.ISCS.MulLanguage;
using TrendingHelper;

namespace TrendViewer.View
{
    public partial class About : STEE.ISCS.GenericGUI.BaseForm
    {
        public About():this("")
        {

        }

        public About(string aboutMsg)
        {
            InitializeComponent();                                  
            aboutMsgLabel.Text =ConfigureFileHelper.GetInstance().AboutMessage ; 
        }

        public  override void TranslateCaption()
        {
            this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_AboutFormTitle, LanguageHelper.TrendViewer_AboutFormTitle_EN);
        }
        private void About_Load(object sender, EventArgs e)
        {
//             int xPos = this.Width / 2;
//             int yPos = this.Height / 2;
            //aboutMsgLabel.Location = new Point((xPos - aboutMsgLabel.Width / 2), yPos - aboutMsgLabel.Height / 2);
        }
        public override void PutObject(string objectName, object obj)
        {
            string aboutMsg = (string)obj;
            aboutMsgLabel.Text = aboutMsg; 
        }
    }
}