using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using STEE.ISCS.Log;
using STEE.ISCS.GenericGUI;
using TrendViewer.Controller;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.View
{
    public partial class EditChartTitle : BaseForm
    {
        private const string CLASS_NAME = "TrendViewer.View.EditChartTitle";
        public EditChartTitle()
        {
            InitializeComponent();
        }

        public override void TranslateCaption()
        {
            this.okCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SubmitBtn, LanguageHelper.TrendViewer_SubmitBtn_EN);
            this.cancelCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_CancelBtn, LanguageHelper.TrendViewer_CancelBtn_EN);
            this.TrendTitleLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_EditChartForm_ChartTitleLbl, LanguageHelper.TrendViewer_EditChartForm_ChartTitleLbl_EN);
            this.XTitleLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_EditChartForm_XTitleLbl, LanguageHelper.TrendViewer_EditChartForm_XTitleLbl_EN);
            this.YTitleLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_EditChartForm_YTitleLbl, LanguageHelper.TrendViewer_EditChartForm_YTitleLbl_EN);
            this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_EditChartForm_Title, LanguageHelper.TrendViewer_EditChartForm_Title_EN);
        }

        public string[] GetTitles()
        {
            string[] titles = new string[3];
            titles[0] = this.chartTitleBox.Text;
            titles[1] = this.xTitleBox.Text ;
            titles[2] = this.yTitleBox.Text;
            return titles;
        }

        public override void AttachListener(STEE.ISCS.MVC.IController controller)
        {
            ChangeTrendTitleController ctl = (ChangeTrendTitleController)getController();
            this.okCmd.Click += ctl.ModifyTitles;
        }


        public void Display(string[] titles)
        {
            if (titles.Length < 3) return;
            chartTitleBox.Text = titles[0];
            xTitleBox.Text = titles[1];
            yTitleBox.Text = titles[2];
        }

        private void EditChartData_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "EditChartData_Paint";
            try
            {
                Graphics obj_Graph = e.Graphics;

                LinearGradientBrush b = new LinearGradientBrush(new System.Drawing.Rectangle(0, 0, this.Width, this.Height), Color.LightBlue, Color.BlanchedAlmond, LinearGradientMode.Vertical);

                obj_Graph.FillRectangle(b, new System.Drawing.Rectangle(0, 0, this.Width, this.Height));

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
            }
        }

    }
}