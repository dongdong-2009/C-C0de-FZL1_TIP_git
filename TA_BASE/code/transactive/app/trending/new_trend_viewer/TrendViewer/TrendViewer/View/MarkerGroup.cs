using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using OPCTrendLib;
using STEE.ISCS.Log;
using STEE.ISCS.GenericGUI;
using TrendViewer.Controller;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.View
{
    public partial class MarkerGroup : BaseForm
    {
        private const string CLASS_NAME = "MarkerGroup";
        private FormType m_formType;

        public MarkerGroup()
        {
            InitializeComponent();
        }

       public void SetFormType(FormType type)
       {
           m_formType = type;
       }

        public override void TranslateCaption()
        {
            this.okCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ConfirmBtn, LanguageHelper.TrendViewer_ConfirmBtn_EN);
            this.cancelCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_CancelBtn, LanguageHelper.TrendViewer_CancelBtn_EN);
            this.configNameLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_GroupConfigurationLbl, LanguageHelper.TrendViewer_GroupConfigurationLbl_EN);
            if(m_formType == FormType.Load)
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LoadMarker_Title, LanguageHelper.TrendViewer_LoadMarker_Title_EN);
            }
            else
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SaveMarker_Title, LanguageHelper.TrendViewer_SaveMarker_Title_EN);
            }
            
        }
        private void cancelCmd_Click(object sender, EventArgs e)
        {

        }


        public string GetConfigName()
        {
            return configNameBox.Text.Trim();
        }

        public override void AttachListener(STEE.ISCS.MVC.IController controller)
        {
            MarkerGroupController ctl = (MarkerGroupController)controller;
            if (m_formType == FormType.Load)
            {
                this.okCmd.Click += ctl.LoadMarkerByGrpName;
            }
            else
            {
                this.okCmd.Click += ctl.SaveMarkerToGrpName;
            }
        }
                    
        public void SetCurrentGrpName(string grpName)
        {
            configNameBox.Text = grpName;
        }
        public void FillConfigNameBox(List<string> grpNames)
        {
            if (m_formType == FormType.Load)
            {
                configNameBox.DropDownStyle = ComboBoxStyle.DropDownList;
            }
            else
            {
                configNameBox.DropDownStyle = ComboBoxStyle.DropDown;
            }

            configNameBox.Items.Clear();

            if(grpNames!=null)
            {
                for (int i = 0; i < grpNames.Count; i++)
                {
                    configNameBox.Items.Add(grpNames[i]);
                }
            }

        }

        private void LoadConfigMarker_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "LoadConfigMarker_Paint";
            try
            {
                Graphics obj_Graph = e.Graphics;

                LinearGradientBrush b = new LinearGradientBrush(new System.Drawing.Rectangle(0, 0, this.Width, this.Height), Color.LightBlue, Color.BlanchedAlmond, LinearGradientMode.Vertical);

                obj_Graph.FillRectangle(b, new System.Drawing.Rectangle(0, 0, this.Width, this.Height));

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }
    }
}