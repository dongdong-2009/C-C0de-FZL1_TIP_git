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
using Entity.Trending;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.View
{
    public partial class MarkerData : BaseForm
    {
        private const string CLASS_NAME = "TrendViewer.View.MarkerData";
        private FormType m_formType;
        public void SetFormType(FormType type)
        {
            m_formType = type;
        }
        public MarkerData()
        {
            InitializeComponent();
 
        }

        public override void TranslateCaption()
        {

            this.cancelCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_CancelBtn, LanguageHelper.TrendViewer_CancelBtn_EN);
            this.okCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SubmitBtn, LanguageHelper.TrendViewer_SubmitBtn_EN);
            this.markerEnabledCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_EnableMarkerCb, LanguageHelper.TrendViewer_MarkData_EnableMarkerCb_EN);
            this.fontColorCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SelectColorBtn, LanguageHelper.TrendViewer_SelectColorBtn_EN);
            this.rangeLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_RangeLbl, LanguageHelper.TrendViewer_MarkData_RangeLbl_EN);
            this.backColorCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SelectColorBtn, LanguageHelper.TrendViewer_SelectColorBtn_EN);
            this.startValueLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_StartValLbl, LanguageHelper.TrendViewer_MarkData_StartValLbl_EN);
            this.markerNameLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_MarkerNameLbl, LanguageHelper.TrendViewer_MarkData_MarkerNameLbl_EN);
            this.FColorLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_MarkerFColorLbl, LanguageHelper.TrendViewer_MarkData_MarkerFColorLbl_EN);
            this.BColorLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_MarkerBColorLbl, LanguageHelper.TrendViewer_MarkData_MarkerBColorLbl_EN);
            if(m_formType==FormType.Edit)
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_EditMarkerTitle, LanguageHelper.TrendViewer_MarkData_EditMarkerTitle_EN); 
            }else{
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_AddMarkerTitle, LanguageHelper.TrendViewer_MarkData_AddMarkerTitle_EN); 
            }
            
        }

        public override void AttachListener(STEE.ISCS.MVC.IController controller)
        {
            MarkerDataController ctl = (MarkerDataController)(controller);
             if(m_formType == FormType.Add)
             {
                 this.okCmd.Click += ctl.AddMarkerData;

             }
             else
             {
                 this.okCmd.Click += ctl.ModifyMarker;
             }
        }

       public EtyMarker GetNewMarker()
       {
           EtyMarker marker = new EtyMarker();
           marker.MarkerName = markerNameBox.Text;
           marker.MarkerEnabled = markerEnabledCb.Checked;
           marker.MarkerValue = (double)(valueBox.Value);
           marker.MarkerWidth = (double)(lineWidthBox.Value);           
           marker.MarkerBColor = backColorPanel.BackColor.ToArgb().ToString();
           marker.MarkerFColor = fontColorPanel.BackColor.ToArgb().ToString();
           return marker;

       }

        public string GetMarkerName()
        {
            return markerNameBox.Text.Trim();
        }



        private void MarkerData_Load(object sender, EventArgs e)
        {
            string Function_Name = "MarkerData_Load";
            try
            {
                if (m_formType==FormType.Add)
                {
                    fontColorPanel.BackColor = Color.Black;
                    backColorPanel.BackColor = Color.Orange;
                    this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_AddMarkerTitle, LanguageHelper.TrendViewer_MarkData_AddMarkerTitle_EN);
                }
                else //for "Edit"
                {
                    this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkData_EditMarkerTitle, LanguageHelper.TrendViewer_MarkData_EditMarkerTitle_EN);
                    fontColorPanel.BackColor = Color.Black;
                    backColorPanel.BackColor = Color.Orange;

                    MarkerDataController ctl = (MarkerDataController)(getController());
                    EtyMarker marker = ctl.GetMarker();
                    markerNameBox.Text = marker.MarkerName;

                    backColorPanel.BackColor = Color.FromArgb(Convert.ToInt32(marker.MarkerBColor));
                    fontColorPanel.BackColor = Color.FromArgb(Convert.ToInt32(marker.MarkerFColor));

                    markerEnabledCb.Checked = marker.MarkerEnabled;
                    lineWidthBox.Value = (decimal)(marker.MarkerWidth);
                    valueBox.Value = (decimal)(marker.MarkerValue);
                }
               
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
            

        }



        private void MarkerData_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "MarkerData_Paint";
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

        private void fontColorCmd_Click(object sender, EventArgs e)
        {
            string Function_Name = "fontColorCmd_Click";
            try
            {
                if (colorDialog1.ShowDialog() == DialogResult.OK)
                {
                    fontColorPanel.BackColor = colorDialog1.Color;
                }

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        private void backColorCmd_Click(object sender, EventArgs e)
        {
            string Function_Name = "backColorCmd_Click";
            try
            {
                if (colorDialog1.ShowDialog() == DialogResult.OK)
                {
                    backColorPanel.BackColor = colorDialog1.Color;
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }
    }
}