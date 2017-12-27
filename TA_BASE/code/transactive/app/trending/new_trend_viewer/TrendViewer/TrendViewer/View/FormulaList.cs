using System;
using System.Collections;
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
using STEE.ISCS.MVC;
using TrendViewer.Controller;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.View
{
    public partial class FormulaList : BaseForm
    {
        private const string CLASS_NAME = "FormulaList";
        private int RECORDSIZE = 8;

        public FormulaList()
        {
            InitializeComponent();
            InitializeDataPointListDataGridView();
        }

        public override void TranslateCaption()
        {
            this.formulaListGrpBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaList_GroupBox, LanguageHelper.TrendViewer_FormulaList_GroupBox_EN);
            this.TotalPageLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TotalPageLbl, LanguageHelper.TrendViewer_TotalPageLbl_EN);
            this.addDataPointToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaList_AddMenu, LanguageHelper.TrendViewer_FormulaList_AddMenu_EN);
            this.loadDataConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaList_LoadMenu, LanguageHelper.TrendViewer_FormulaList_LoadMenu_EN);
            this.saveDataConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaList_SaveMenu, LanguageHelper.TrendViewer_FormulaList_SaveMenu_EN);
            this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_FormulaList_Title, LanguageHelper.TrendViewer_FormulaList_Title_EN); 
        }

        #region DataPointListDataGridView

        private void InitializeDataPointListDataGridView()
        {
            //  <summary>
            //  Initialize DataPointListDataGridView Columns and Header
            //  </summary>
            string Function_Name = "InitializeDataPointListDataGridView";
            try
            {
                DataGridViewImageColumn editColumn1 = new DataGridViewImageColumn();
                editColumn1.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDPEnabled, LanguageHelper.TrendViewer_ColDPEnabled_EN);
                editColumn1.Image = Properties.Resources.img_checkmark;
                formulaListDataGridView.Columns.Add(editColumn1);
                formulaListDataGridView.Columns[0].Width = 80;
                formulaListDataGridView.Columns[0].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

                DataGridViewImageColumn editColumn2 = new DataGridViewImageColumn();
                editColumn2.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColLblEnabled, LanguageHelper.TrendViewer_ColLblEnabled_EN); 
                editColumn2.Image = Properties.Resources.img_checkmark;
                formulaListDataGridView.Columns.Add(editColumn2);
                formulaListDataGridView.Columns[1].Width = 100;
                formulaListDataGridView.Columns[1].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

                formulaListDataGridView.Columns.Add("DATA_PT_LBL_NAME", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColLengend, LanguageHelper.TrendViewer_ColLengend_EN));
                formulaListDataGridView.Columns[2].Width = 208;

                formulaListDataGridView.Columns.Add("DATA_PT_TYPE", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDPType, LanguageHelper.TrendViewer_ColDPType_EN));

                formulaListDataGridView.Columns.Add("DATA_PT_COLOR", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDPColor, LanguageHelper.TrendViewer_ColDPColor_EN));  
                formulaListDataGridView.Columns[4].SortMode = DataGridViewColumnSortMode.NotSortable;


                formulaListDataGridView.Columns.Add("DATA_PT_EQUATION", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColFormula, LanguageHelper.TrendViewer_ColFormula_EN));
                formulaListDataGridView.Columns[5].Width = 350;


                DataGridViewImageColumn editColumn = new DataGridViewImageColumn();
                editColumn.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColEdit, LanguageHelper.TrendViewer_ColEdit_EN);
                editColumn.Image = Properties.Resources.Notes_16_;
                formulaListDataGridView.Columns.Add(editColumn);
                formulaListDataGridView.Columns[6].Width = 75;
                formulaListDataGridView.Columns[6].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

                DataGridViewImageColumn deleteColumn = new DataGridViewImageColumn();
                deleteColumn.Image = Properties.Resources.Erase_16_;
                deleteColumn.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDelete, LanguageHelper.TrendViewer_ColDelete_EN); ;
                formulaListDataGridView.Columns.Add(deleteColumn);
                formulaListDataGridView.Columns[7].Width = 75;
                formulaListDataGridView.Columns[7].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;


            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }


        private void firstdataPointListDataGridView_Click(object sender, EventArgs e)
        {
            pageNumDataPointListDataGridView.Value = 1;
        }

        private void prevdataPointListDataGridView_Click(object sender, EventArgs e)
        {
            if (pageNumDataPointListDataGridView.Value > 1)
                pageNumDataPointListDataGridView.Value = pageNumDataPointListDataGridView.Value - 1;
        }

        private void pageNumdataPointListDataGridView_ValueChanged(object sender, EventArgs e)
        {
            PopulateDataPointListDataGridView(2);
        }

        private void nextdataPointListDataGridView_Click(object sender, EventArgs e)
        {
            if (pageNumDataPointListDataGridView.Value < pageNumDataPointListDataGridView.Maximum)
                pageNumDataPointListDataGridView.Value = pageNumDataPointListDataGridView.Value + 1;
        }

        private void lastdataPointListDataGridView_Click(object sender, EventArgs e)
        {
            pageNumDataPointListDataGridView.Value = pageNumDataPointListDataGridView.Maximum;

        }

        private void dataPointListDataGridView_Sorted(object sender, EventArgs e)
        {
            PopulateDataPointListDataGridView(2);
        }

        public void PopulateDataPointListDataGridView(int mode)
        {
            //  <summary>
            //  DataPointListDataGridView Data Population
            //  </summary>
            string Function_Name = "PopulateDataPointListDataGridView";
            formulaListDataGridViewprogressBar.Visible = true;

            try
            {
                formulaListDataGridViewprogressBar.Value = 0;

                totalDataPointListDataGridView.Text = "1";
                if (mode == 1)
                    pageNumDataPointListDataGridView.Value = 1;

                FormulaListController ctl = (FormulaListController)getController();
                int totalRecord = ctl.getFormulaTable().Rows.Count; 
                int upperRecord = 0;
                int lowerRecord = 0;
                if (totalRecord > 0)
                {
                    if (totalRecord % RECORDSIZE > 0)
                    {
                        totalRecord = totalRecord / RECORDSIZE;
                        totalRecord++;
                    }
                    else
                    {
                        totalRecord = totalRecord / RECORDSIZE;
                    }
                    totalDataPointListDataGridView.Text = totalRecord.ToString();
                    pageNumDataPointListDataGridView.Maximum = totalRecord;
                }
                else
                {
                    pageNumDataPointListDataGridView.Maximum = 1;
                }

                upperRecord = (int)(RECORDSIZE * pageNumDataPointListDataGridView.Value);
                if (pageNumDataPointListDataGridView.Value > 1)
                    lowerRecord = (int)(RECORDSIZE * (pageNumDataPointListDataGridView.Value - 1)) + 1;
                else
                    lowerRecord = (int)(RECORDSIZE * (pageNumDataPointListDataGridView.Value - 1));

                string localSQL = "";
                if (formulaListDataGridView.SortedColumn != null && formulaListDataGridView.Rows.Count > 0)
                {
                    if (formulaListDataGridView.SortedColumn.Name != null && formulaListDataGridView.SortedColumn.Name.Trim() != "")
                    {
                        localSQL = formulaListDataGridView.SortedColumn.Name;
                        if (formulaListDataGridView.SortOrder.ToString().ToLower() != "ascending")
                            localSQL += " DESC ";
                        else
                            localSQL += " ASC ";

                    }
                    else
                        localSQL = " DATA_PT_LBL_NAME ASC ";
                }
                else
                    localSQL = " DATA_PT_LBL_NAME ASC ";


                DataRow[] dataRowArray = ctl.getFormulaTable().Select("", localSQL);   


                DataGridViewRow dataGridRow = null;
                DataRow dataRow = null;


                if (upperRecord > dataRowArray.Length)
                {
                    upperRecord = dataRowArray.Length;
                }

                int counter = 0;
                string tempString = "";
                formulaListDataGridView.Rows.Clear();
                if (lowerRecord > 0) lowerRecord--;
                formulaListDataGridViewprogressBar.Value = 0;
                for (int i = lowerRecord; i < upperRecord; i++)
                {
                    formulaListDataGridView.Rows.Add();
                    formulaListDataGridViewprogressBar.Value++;
                    dataGridRow = formulaListDataGridView.Rows[counter];
                    dataRow = dataRowArray[i];


                    tempString = "";
                    tempString = dataRow["DATA_PT_LBL_NAME"].ToString();
                    if (tempString.Length > 100)
                        dataGridRow.Cells[2].Value = tempString.Substring(0, 100) + "...";
                    else
                        dataGridRow.Cells[2].Value = tempString;

                    dataGridRow.Cells[3].Value = dataRow["DATA_PT_TYPE"].ToString();

                    if (TrendViewerHelper.isNumeric(dataRow["DATA_PT_COLOR"].ToString(), System.Globalization.NumberStyles.Number))
                    {
                        dataGridRow.Cells[4].Value = "";
                        dataGridRow.Cells[4].Style.BackColor = Color.FromArgb(Convert.ToInt32(dataRow["DATA_PT_COLOR"].ToString()));
                    }

                    tempString = "";
                    tempString = dataRow["DATA_PT_EQUATION"].ToString();
                    if (tempString.Length > 100)
                        dataGridRow.Cells[5].Value = tempString.Substring(0, 100) + "...";
                    else
                        dataGridRow.Cells[5].Value = tempString;

                    if (TrendViewerHelper.ChangeStrToBool(dataRow["DATA_PT_ENABLED"].ToString()))
                        dataGridRow.Cells[0].Value = Properties.Resources.img_checkmark;
                    else
                        dataGridRow.Cells[0].Value = Properties.Resources.blank;
                    if (TrendViewerHelper.ChangeStrToBool(dataRow["DATA_PT_LBL_ENABLED"].ToString()))
                        dataGridRow.Cells[1].Value = Properties.Resources.img_checkmark;
                    else
                        dataGridRow.Cells[1].Value = Properties.Resources.blank;
                    counter++;
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            formulaListDataGridViewprogressBar.Visible = false;
        }


        #endregion

        public override void AttachListener(IController ctl)
        {
            FormulaListController controller = (FormulaListController)ctl;
            this.addDataPointToolStripMenuItem.Click += controller.AddFormula;
            this.saveDataConfigToolStripMenuItem.Click += controller.SaveFormula;
            this.loadDataConfigToolStripMenuItem.Click += controller.LoadFormula;
            this.FormClosed += controller.FormClose;

        } 

        private void dataPointListDataGridView_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0) return;

            string Function_Name = "dataPointListDataGridView_CellDoubleClick";
            FormulaListController controller = (FormulaListController)getController();
            string formulaLblName = formulaListDataGridView.Rows[e.RowIndex].Cells[2].Value.ToString();
            try
            {
                if (e.ColumnIndex == 7)
                {

                    if (
                        MessageBoxDialog.Show(
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DelConfirm, LanguageHelper.TrendViewer_Msg_DelConfirm_EN),
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_SysMsg, LanguageHelper.TrendViewer_Msg_SysMsg_EN),
                        MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)

                    {
                        controller.DeleteByFormulaLblName(formulaLblName);

                    }
                }
                else if (e.ColumnIndex == 6)
                {
                    controller.EditFormula(formulaLblName);

                }


            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

        }


        private void DataPointViewer_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "DataPointViewer_Paint";
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

        private void DataPointFormulaViewer_Load(object sender, EventArgs e)
        {
            string Function_Name = "DataPointFormulaViewer_Load";
            try
            {
                formulaListDataGridViewprogressBar.Visible = false;
                formulaListDataGridViewprogressBar.Maximum = RECORDSIZE;

                PopulateDataPointListDataGridView(1);
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

    }
}