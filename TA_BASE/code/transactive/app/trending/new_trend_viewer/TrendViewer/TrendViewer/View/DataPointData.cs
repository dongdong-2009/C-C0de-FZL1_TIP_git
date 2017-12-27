using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using OPCTrendLib;

using OPCTrendLib.OPCData;
using STEE.ISCS.Log;
using STEE.ISCS.GenericGUI;
using TrendViewer.Controller;
using Entity.Trending;
using TrendViewer.Common;
using STEE.ISCS.MulLanguage;
using TrendingHelper;

namespace TrendViewer.View
{
    public partial class DataPointData : BaseForm
    {
        private string CLASS_NAME = "DataPointData";
        private FormType m_formType = FormType.Add;

        public void SetFormType (FormType type)
        {
            m_formType = type;
        }

        public DataPointData()
        {
            InitializeComponent();

        }

        public override void TranslateCaption()
        {

            this.cancelCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_CancelBtn, LanguageHelper.TrendViewer_CancelBtn_EN);
            this.okCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SubmitBtn, LanguageHelper.TrendViewer_SubmitBtn_EN);
            this.selectColorCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SelectColorBtn, LanguageHelper.TrendViewer_SelectColorBtn_EN);
            this.browseCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_BrowseBtn, LanguageHelper.TrendViewer_BrowseBtn_EN);
            this.hostNameLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Host, LanguageHelper.TrendViewer_Host_EN);
            this.serverLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ServerNameLbl, LanguageHelper.TrendViewer_ServerNameLbl_EN);
            this.dataPointVisibleCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_DPVisibleCb, LanguageHelper.TrendViewer_DPVisibleCb_EN);
            this.labelEnabledCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LblEnabledCb, LanguageHelper.TrendViewer_LblEnabledCb_EN);
            this.dataPointEnabledCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_DPEnabledCb, LanguageHelper.TrendViewer_DPEnabledCb_EN);
            this.legendLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LableLbl, LanguageHelper.TrendViewer_LableLbl_EN);
            this.lintTypeLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LineTypeLbl, LanguageHelper.TrendViewer_LineTypeLbl_EN);
            this.colorLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColorLbl, LanguageHelper.TrendViewer_ColorLbl_EN);
            this.dataPointLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_DPLbl, LanguageHelper.TrendViewer_DPLbl_EN);
            if (m_formType==FormType.Add)
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_AddDPTitle, LanguageHelper.TrendViewer_AddDPTitle_EN);
            }
            else
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_EditDPTitle, LanguageHelper.TrendViewer_EditDPTitle_EN);
            }
        }

        public override void AttachListener(STEE.ISCS.MVC.IController controller)
        {
            DataPointDataController ctl = (DataPointDataController)(controller);
            this.browseCmd.Click += ctl.BrowseDP;

            if (m_formType == FormType.Add)
            {
                this.okCmd.Click += ctl.AddDataPointData;

            }
            else
            {
                this.okCmd.Click += ctl.ModifyDataPoint;
            }
        }

        public EtyDataPoint GetNewDataPoint()
        {
            EtyDataPoint dataPoint = new EtyDataPoint();

            dataPoint.DPHost = opcServerHostBox.Text;
            dataPoint.DPServer = opcServerNameBox.Text;
            dataPoint.DPName = dataPointBox.Text;
            dataPoint.DPType = TrendViewerHelper.convertLineTypeToEnum(seriesTypeBox.Text);
            dataPoint.DPLblName = dataLegendBox.Text;
            dataPoint.DPColor = colorPanel.BackColor.ToArgb().ToString();
            dataPoint.DPEnabled = dataPointEnabledCb.Checked;
            dataPoint.DPLblEnabled = labelEnabledCb.Checked;
            dataPoint.DPVisible = dataPointVisibleCb.Checked;

            return dataPoint;

        }

        public string GetDataPointName()
        {
            return dataPointBox.Text.Trim();
        }

        public string GetDPLblName()
        {
            return dataPointBox.Text.Trim();
        }


        private void colorCmd_Click(object sender, EventArgs e)
        {
            string Function_Name = "colorCmd_Click";
            try
            {
                if (colorDialog1.ShowDialog() == DialogResult.OK)
                {
                    colorPanel.BackColor = colorDialog1.Color;
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }



        public void UpdateDPNameAndLabel(EtyDataPoint dp)
        {
            dataPointBox.Text = dp.DPName.ToString();
            dataLegendBox.Text = dp.DPLblName;
        }

        public void Display(EtyDataPoint dp)
        {
            string Function_Name = "Display";
            colorPanel.BackColor = Color.Black;

            try
            {
                seriesTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeLine, LanguageHelper.TrendViewer_LintTypeLine_EN));
                seriesTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeSpline, LanguageHelper.TrendViewer_LintTypeSpline_EN));
                seriesTypeBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeLine, LanguageHelper.TrendViewer_LintTypeLine_EN);
                opcServerHostBox.Text = ConfigureFileHelper.GetInstance().HostIPAddress; //dp.DPHost.ToString();
                opcServerNameBox.Text = ConfigureFileHelper.GetInstance().OPCServerName;//dp.DPServer.ToString();

                if (m_formType == FormType.Edit)
                {
                    opcServerHostBox.Text = dp.DPHost.ToString();
                    opcServerNameBox.Text = dp.DPServer.ToString();
                    dataPointBox.Text = dp.DPName.ToString();
                    seriesTypeBox.Text = TrendViewerHelper.convertLineTypeToDispLan(dp.DPType);
                    dataLegendBox.Text = dp.DPLblName;
                    if (TrendViewerHelper.isNumeric(dp.DPColor, System.Globalization.NumberStyles.Number))
                    {
                        colorPanel.BackColor = Color.FromArgb(Convert.ToInt32(dp.DPColor));
                    }
                    dataPointEnabledCb.Checked = dp.DPEnabled;
                    labelEnabledCb.Checked = dp.DPLblEnabled;
                    dataPointVisibleCb.Checked = dp.DPVisible;
                }

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        private void DataPoint_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "DataPoint_Paint";
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
