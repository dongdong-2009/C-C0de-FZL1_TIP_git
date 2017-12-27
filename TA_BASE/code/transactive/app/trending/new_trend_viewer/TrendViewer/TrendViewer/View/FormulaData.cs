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
    public partial class FormulaData : BaseForm
    {
        private const string CLASS_NAME = "FormulaData";
        private FormType m_formType;


        public FormulaData()
        {
            InitializeComponent();
            translateCaptionToCn();
        }

        private void translateCaptionToCn()
        {

            this.colorLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColorLbl, LanguageHelper.TrendViewer_ColorLbl_EN);
            this.lineTypeLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LineTypeLbl, LanguageHelper.TrendViewer_LineTypeLbl_EN);
            this.cancelCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_CancelBtn, LanguageHelper.TrendViewer_CancelBtn_EN);
            this.okCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SubmitBtn, LanguageHelper.TrendViewer_SubmitBtn_EN);
            this.colorCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_SelectColorBtn, LanguageHelper.TrendViewer_SelectColorBtn_EN);
            this.dataPointEnabledCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_DPEnabledCb, LanguageHelper.TrendViewer_DPEnabledCb_EN);
            this.labelEnabledCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LblEnabledCb, LanguageHelper.TrendViewer_LblEnabledCb_EN);
            this.LegendLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LableLbl, LanguageHelper.TrendViewer_LableLbl_EN);
            this.exampleLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaData_EgLbl, LanguageHelper.TrendViewer_FormulaData_EgLbl_EN);
            this.formulaLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaData_FormulaLbl, LanguageHelper.TrendViewer_FormulaData_FormulaLbl_EN);
            if(m_formType==FormType.Edit)
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaData_EditTitle, LanguageHelper.TrendViewer_FormulaData_EditTitle_EN);
            }else
            {
                this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaData_AddTitle, LanguageHelper.TrendViewer_FormulaData_AddTitle_EN);
            }
            
        }

        public void SetFormType(FormType type)
        {
            m_formType = type;
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

        public EtyFormula GetNewFormula()
        {
            EtyFormula formula = new EtyFormula();
            formula.DPEquation = equationBox.Text;
            formula.DPType = TrendViewerHelper.convertLineTypeToEnum(seriesTypeBox.Text);
            formula.DPColor = colorPanel.BackColor.ToArgb().ToString();
            formula.DPEnabled = dataPointEnabledCb.Checked;
            formula.DPLblEnabled = labelEnabledCb.Checked;
            formula.DPLblName = dataLegendBox.Text;
            return formula;
        }

        public override void AttachListener(STEE.ISCS.MVC.IController controller)
        {
            FormulaDataController ctl = (FormulaDataController)controller;
            if (m_formType == FormType.Add)
            {
                this.okCmd.Click += ctl.AddFormulaData;
            }
            else
            {
                this.okCmd.Click += ctl.ModifyFormula;
            }
            
        }

        public string GetFormulaName()
        {
            return dataLegendBox.Text.Trim();
        }



        private bool isNumeric(string val, System.Globalization.NumberStyles NumberStyle)
        {
            Double result;
            return Double.TryParse(val, NumberStyle, System.Globalization.CultureInfo.CurrentCulture, out result);
        }

        private void EditDataPoint_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "EditDataPoint_Paint";
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

        private void EditDataPointFormula_Load(object sender, EventArgs e)
        {
            string Function_Name = "EditDataPointFormula_Load";
            try
            {
                colorPanel.BackColor = Color.Black;
                seriesTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeLine, LanguageHelper.TrendViewer_LintTypeLine_EN));
                seriesTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeSpline, LanguageHelper.TrendViewer_LintTypeSpline_EN));

                if (m_formType == FormType.Add)
                {
                    seriesTypeBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeLine, LanguageHelper.TrendViewer_LintTypeLine_EN);
                    this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaData_AddTitle, LanguageHelper.TrendViewer_FormulaData_AddTitle_EN);
                }
                else //for "Edit"
                {

                    FormulaDataController ctl = (FormulaDataController)(getController());
                    EtyFormula formula = ctl.GetFormula();

                    equationBox.Text = formula.DPEquation;
                    seriesTypeBox.Text = TrendViewerHelper.convertLineTypeToDispLan(formula.DPType);
                    dataLegendBox.Text = formula.DPLblName;
                    if (isNumeric(formula.DPColor, System.Globalization.NumberStyles.Number))
                    {
                        colorPanel.BackColor = Color.FromArgb(Convert.ToInt32(formula.DPColor));
                    }
                    dataPointEnabledCb.Checked = formula.DPEnabled;
                    labelEnabledCb.Checked = formula.DPLblEnabled;
                    this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaData_EditTitle, LanguageHelper.TrendViewer_FormulaData_EditTitle_EN);
                }

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        private void checkValidDataPointsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string Function_Name = "checkValidDataPointsToolStripMenuItem_Click";
            try
            {
                MessageBoxDialog.Show("Active Datapoint as follows : \n" + CheckValidVariables(), " ", MessageBoxButtons.OK, MessageBoxIcon.Information);


            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
           
        }

        public string CheckValidVariables()
        {
            string Function_Name = "CheckValidVariables";
            string variableList = "";
            try
            {
                if (equationBox.Text.Trim() == "") return "";
                if (equationBox.Text.IndexOf("::") < 0) return "";
                string[] variableArray = equationBox.Text.Split(new string[] { "::" }, StringSplitOptions.None);

                if (variableArray.Length % 2 == 0) return "Invalid code";

                for (int i = 0; i < variableArray.Length; i++)
                {
                    if (i % 2 == 1)
                    {
                        if (variableArray[i].Length > 0 && variableArray[i].LastIndexOf('\n') == -1)
                        {
                            if (variableList.IndexOf(variableArray[i]) < 0)
                                variableList = variableList + variableArray[i] + ",";
                        }
                    }
                }

                if (variableList.Length < 1) return "";

                variableList = variableList.Substring(0, variableList.Length - 1);


            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            return variableList;


        }
    }
}