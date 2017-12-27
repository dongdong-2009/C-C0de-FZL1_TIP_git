using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using OPCTrendLib.OPCData;
using OPCTrendLib;
using STEE.ISCS.Log;
using STEE.ISCS.GenericGUI;
using Entity.Trending;
using TrendViewer.Common;
using TrendViewer.Controller;
using STEE.ISCS.MulLanguage;
using TrendingHelper;

namespace TrendViewer.View
{
    public partial class HistDataPointData : BaseForm
    {
        private const string CLASS_NAME = "TrendViewer.View.HistDataPointData";
        FormType m_formType = FormType.Add;

        public void SetFormType(FormType type)
        {
            m_formType = type;
        }

        public HistDataPointData()
        {
            
            InitializeComponent();

            this.historyStartDateBox.CustomFormat = DateTimeFormatter.getInstance().getDateFormatStr();
            this.historyEndDateBox.CustomFormat = DateTimeFormatter.getInstance().getDateFormatStr();

        }
        public override void TranslateCaption()
        {

            this.browseCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_BrowseBtn, LanguageHelper.TrendViewer_BrowseBtn_EN);
            this.colorCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SelectColorBtn, LanguageHelper.TrendViewer_SelectColorBtn_EN);
            this.endTimeLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_HistDataPointData_EndDateLbl, LanguageHelper.TrendViewer_HistDataPointData_EndDateLbl_EN);
            this.hostNameLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Host, LanguageHelper.TrendViewer_Host_EN);
            this.ServerLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ServerNameLbl, LanguageHelper.TrendViewer_ServerNameLbl_EN);
            this.startTimeLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_HistDataPointData_StartDateLbl, LanguageHelper.TrendViewer_HistDataPointData_StartDateLbl_EN);
            this.legendLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LableLbl, LanguageHelper.TrendViewer_LableLbl_EN);
            this.labelEnabledCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LblEnabledCb, LanguageHelper.TrendViewer_LblEnabledCb_EN);
            this.dataPointEnabledCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_DPEnabledCb, LanguageHelper.TrendViewer_DPEnabledCb_EN);
            this.cancelCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_CancelBtn, LanguageHelper.TrendViewer_CancelBtn_EN);
            this.okCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SubmitBtn, LanguageHelper.TrendViewer_SubmitBtn_EN);
            this.lineTypeLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LineTypeLbl, LanguageHelper.TrendViewer_LineTypeLbl_EN);
            this.dataPointLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_DPLbl, LanguageHelper.TrendViewer_DPLbl_EN);

            if (m_formType == FormType.Add)
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_HistDataPointData_AddDPTitle, LanguageHelper.TrendViewer_HistDataPointData_AddDPTitle_EN);
            }
            else
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_HistDataPointData_EditDPTitle, LanguageHelper.TrendViewer_HistDataPointData_EditDPTitle_EN);
            }

            this.colorLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColorLbl, LanguageHelper.TrendViewer_ColorLbl_EN); 

        }

        public string GetHistDataPointName()
        {
            return dataPointBox.Text.Trim();
        }

        public string GetHistDPLblName()
        {
            return dataPointBox.Text.Trim();
        }

        public EtyHistDataPoint GetNewHistDataPoint()
        {
            EtyHistDataPoint dataPoint = new EtyHistDataPoint();

            dataPoint.DPHost = opcServerHostBox.Text;
            dataPoint.DPServer = opcServerNameBox.Text;
            dataPoint.DPName = dataPointBox.Text;
            dataPoint.DPType = TrendViewerHelper.convertLineTypeToEnum(seriesTypeBox.Text);
            dataPoint.DPLblName = dataLegendBox.Text;
            dataPoint.DPColor = colorPanel.BackColor.ToArgb().ToString();
            dataPoint.DPEnabled = dataPointEnabledCb.Checked;
            dataPoint.DPLblEnabled = labelEnabledCb.Checked;
            dataPoint.DPStartDateTime = historyStartDateBox.Value;
            dataPoint.DPEndDateTime = historyEndDateBox.Value;

            return dataPoint;

        }
        public void UpdateHistDPNameAndLabel(EtyHistDataPoint dp)
        {
            dataPointBox.Text = dp.DPName.ToString();
            dataLegendBox.Text = dp.DPLblName;
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

                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
            }
        }

        public override void AttachListener(STEE.ISCS.MVC.IController controller)
        {
            HistDataPointDataController ctl = (HistDataPointDataController)(controller);
            this.browseCmd.Click += ctl.BrowseDP;

            if (m_formType == FormType.Add)
            {
                this.okCmd.Click += ctl.AddHistDataPointData;

            }
            else
            {
                this.okCmd.Click += ctl.ModifyHistDataPoint;
            }
        }


        public DateTime GetStartDate()
        {
            return historyStartDateBox.Value;
        }

        public DateTime GetEndDate()
        {
            return historyEndDateBox.Value;
        }



        private void AddDataPoint_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "AddDataPoint_Paint";
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



        public void Display(EtyHistDataPoint dp)
        {
            string Function_Name = "Display";
            opcServerHostBox.Text = ConfigureFileHelper.GetInstance().HostIPAddress; //dp.DPHost.ToString();
            opcServerNameBox.Text = ConfigureFileHelper.GetInstance().OPCServerName;//dp.DPServer.ToString();

            colorPanel.BackColor = Color.Black;

            DateTime currentDT = new DateTime(DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day, 0, 0, 0);
            historyStartDateBox.Value = currentDT;
            currentDT = new DateTime(DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day, 23, 59, 59);
            historyEndDateBox.Value = currentDT;

            try
            {
                seriesTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeLine, LanguageHelper.TrendViewer_LintTypeLine_EN));
                seriesTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeSpline, LanguageHelper.TrendViewer_LintTypeSpline_EN));
                seriesTypeBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeLine, LanguageHelper.TrendViewer_LintTypeLine_EN);
                if (this.m_formType == FormType.Edit)
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

                    historyStartDateBox.Value = dp.DPStartDateTime;
                    historyEndDateBox.Value = dp.DPEndDateTime;
                }

            }
            catch (Exception localException)
            {
                
                LogHelper.Error(CLASS_NAME,Function_Name ,localException.ToString());
            }

        }

        private void cancelCmd_Click(object sender, EventArgs e)
        {

        }


    }
}
