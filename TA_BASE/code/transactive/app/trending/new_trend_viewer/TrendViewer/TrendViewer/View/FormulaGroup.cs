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
    public partial class FormulaGroup : BaseForm
    {
        private const string CLASS_NAME = "FormulaGroup";
        private FormType m_formType;

        public FormulaGroup()
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
            if (m_formType== FormType.Load)
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaList_LoadMenu, LanguageHelper.TrendViewer_FormulaList_LoadMenu_EN);
            }
            else
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaList_SaveMenu, LanguageHelper.TrendViewer_FormulaList_SaveMenu_EN);
            }
        }


        private void cancelCmd_Click(object sender, EventArgs e)
        {

        }

        public override void AttachListener(STEE.ISCS.MVC.IController controller)
        {
            FormulaGroupController ctl = (FormulaGroupController)controller;

            if (m_formType==FormType.Load)
            {
                this.okCmd.Click += ctl.LoadFormulaByGrpName;
            }
            else
            {
                this.okCmd.Click += ctl.SaveFormulaToGrpName;
            }
        }

        public string GetConfigName()
        {
            return configNameBox.Text.Trim();
        }

        public void ShowErrMsgNoCfgName()
        {
            if(m_formType==FormType.Load)
            {
                MessageBoxDialog.Show(
                   StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoConfigToLoad, LanguageHelper.TrendViewer_Msg_NoConfigToLoad_EN),
                   StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                   MessageBoxButtons.OK, MessageBoxIcon.Error);
            }else
            {
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoConfigToSave, LanguageHelper.TrendViewer_Msg_NoConfigToSave_EN), 
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        public void SetCurrentGrpName(string grpName)
        {
            configNameBox.Text = grpName;
        }
        private void LoadConfigData_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "LoadConfigData_Paint";
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

    }
}