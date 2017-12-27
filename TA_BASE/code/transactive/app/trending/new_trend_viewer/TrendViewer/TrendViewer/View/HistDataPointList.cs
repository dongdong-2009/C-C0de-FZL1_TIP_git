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
using OPCTrendLib.OPCData;
using STEE.ISCS.Log;
using STEE.ISCS.GenericGUI;
using STEE.ISCS.MVC;
using TrendViewer.Controller;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.View
{
    public partial class HistDataPointList : BaseForm
    {
        private const string CLASS_NAME = "HistDataPointList";
        private int RECORDSIZE = 8;  //todo put into common

        public HistDataPointList()
        {
            InitializeComponent();
            dataPointListDataGridViewprogressBar.Visible = false;
            dataPointListDataGridViewprogressBar.Maximum = RECORDSIZE;
        }

        public override void TranslateCaption()
        {

            this.histDpListGrpBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_HistDPList_GroupBox, LanguageHelper.TrendViewer_HistDPList_GroupBox_EN);
            this.TotalPage_Tlbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TotalPageLbl, LanguageHelper.TrendViewer_TotalPageLbl_EN);
            this.addDataPointToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_HistDPList_AddMenu, LanguageHelper.TrendViewer_HistDPList_AddMenu_EN);
            this.loadDataConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_HistDPList_LoadMenu, LanguageHelper.TrendViewer_HistDPList_LoadMenu_EN);
            this.saveDataConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_HistDPList_SaveMenu, LanguageHelper.TrendViewer_HistDPList_SaveMenu_EN);
            this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_HistDPList_Title, LanguageHelper.TrendViewer_HistDPList_Title_EN); 
        }


        public override void AttachListener(IController ctl)
        {
            HistDataPointListController controller = (HistDataPointListController)ctl;
            this.addDataPointToolStripMenuItem.Click += controller.AddHistDataPoint;
            this.saveDataConfigToolStripMenuItem.Click += controller.SaveHistDataPoint;
            this.loadDataConfigToolStripMenuItem.Click += controller.LoadHistDataPoint;
            this.dataPointListDataGridView.CellDoubleClick += controller.CellDoubleClick;
            this.dataPointListDataGridView.Sorted += controller.HistDataPointListDataGridView_Sorted;
            this.pageNumDataPointListDataGridView.ValueChanged += controller.PageNumdataPointListDataGridView_ValueChanged ;
            this.FormClosed += controller.FormClose;
        }


        #region DataPointListDataGridView

        public string GetHistDataPointNameByRowIndex(int index)
        {
            if (dataPointListDataGridView == null || dataPointListDataGridView.Rows.Count < index + 1)
            {
                return "";
            }
            return dataPointListDataGridView.Rows[index].Cells["DATA_PT_NAME"].Value.ToString();
        }


        private void InitializeDataPointListDataGridView()
        {
            //  <summary>
            //  Initialize DataPointListDataGridView Columns and Header
            //  </summary>
            string Function_Name = "InitializeDataPointListDataGridView";
            try
            {
                DataGridViewImageColumn editColumn = new DataGridViewImageColumn();
                editColumn.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColEdit, LanguageHelper.TrendViewer_ColEdit_EN);
                editColumn.Image = Properties.Resources.Notes_16_;

                dataPointListDataGridView.Columns.Add(editColumn);
                dataPointListDataGridView.Columns[0].Width = 50;
                dataPointListDataGridView.Columns[0].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

                DataGridViewImageColumn deleteColumn = new DataGridViewImageColumn();
                deleteColumn.Image = Properties.Resources.Erase_16_;
                deleteColumn.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDelete, LanguageHelper.TrendViewer_ColDelete_EN);

                dataPointListDataGridView.Columns.Add(deleteColumn);
                dataPointListDataGridView.Columns[1].Width = 50;
                dataPointListDataGridView.Columns[1].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;


                dataPointListDataGridView.Columns.Add("DATA_PT_NAME", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDPName, LanguageHelper.TrendViewer_ColDPName_EN));
                dataPointListDataGridView.Columns[2].Width = 200;
                dataPointListDataGridView.Columns.Add("DATA_PT_LBL_NAME", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColLengend, LanguageHelper.TrendViewer_ColLengend_EN));
                dataPointListDataGridView.Columns[3].Width = 200;

                dataPointListDataGridView.Columns.Add("DATA_PT_TYPE", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDPType, LanguageHelper.TrendViewer_ColDPType_EN));

                dataPointListDataGridView.Columns.Add("DATA_PT_COLOR", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDPColor, LanguageHelper.TrendViewer_ColDPColor_EN));
                dataPointListDataGridView.Columns[5].SortMode = DataGridViewColumnSortMode.NotSortable;
                dataPointListDataGridView.Columns[5].Width = 68;
                dataPointListDataGridView.Columns.Add("DATA_PT_DT", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColStartDate, LanguageHelper.TrendViewer_ColStartDate_EN));
                dataPointListDataGridView.Columns.Add("DATA_PT_DT_END", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColEndDate, LanguageHelper.TrendViewer_ColEndDate_EN));

                DataGridViewImageColumn editColumn1 = new DataGridViewImageColumn();
                editColumn1.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDPEnabled, LanguageHelper.TrendViewer_ColDPEnabled_EN);
                editColumn1.Image = Properties.Resources.img_checkmark;
                dataPointListDataGridView.Columns.Add(editColumn1);
                dataPointListDataGridView.Columns[8].Width = 120;
                dataPointListDataGridView.Columns[8].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

                DataGridViewImageColumn editColumn2 = new DataGridViewImageColumn();
                editColumn2.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColLblEnabled, LanguageHelper.TrendViewer_ColLblEnabled_EN);
                editColumn2.Image = Properties.Resources.img_checkmark;
                dataPointListDataGridView.Columns.Add(editColumn2);
                dataPointListDataGridView.Columns[9].Width = 100;
                dataPointListDataGridView.Columns[9].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                dataPointListDataGridView.Columns.Add("DATA_PT_SERVER", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColServer, LanguageHelper.TrendViewer_ColServer_EN));
                dataPointListDataGridView.Columns[10].Width = 180;
                dataPointListDataGridView.Columns[10].Visible = false;

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


        private void nextdataPointListDataGridView_Click(object sender, EventArgs e)
        {
            if (pageNumDataPointListDataGridView.Value < pageNumDataPointListDataGridView.Maximum)
                pageNumDataPointListDataGridView.Value = pageNumDataPointListDataGridView.Value + 1;
        }

        private void lastdataPointListDataGridView_Click(object sender, EventArgs e)
        {
            pageNumDataPointListDataGridView.Value = pageNumDataPointListDataGridView.Maximum;

        }



        public void PopulateDataPointListDataGridView(int mode, DataTable dpList)
        {
            //  <summary>
            //  DataPointListDataGridView Data Population
            //  </summary>
            string Function_Name = "PopulateDataPointListDataGridView";

            dataPointListDataGridViewprogressBar.Visible = true;
            try
            {

                dataPointListDataGridViewprogressBar.Value = 0;

                totalDataPointListDataGridView.Text = "1";
                if (mode == 1)
                    pageNumDataPointListDataGridView.Value = 1;

                int totalRecord = dpList.Rows.Count;
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
                if (dataPointListDataGridView.SortedColumn != null && dataPointListDataGridView.Rows.Count > 0)
                {
                    if (dataPointListDataGridView.SortedColumn.Name != null && dataPointListDataGridView.SortedColumn.Name.Trim() != "")
                    {
                        localSQL = dataPointListDataGridView.SortedColumn.Name;
                        if (dataPointListDataGridView.SortOrder.ToString().ToLower() != "ascending")
                            localSQL += " DESC ";
                        else
                            localSQL += " ASC ";

                    }
                    else
                        localSQL = " DATA_PT_NAME ASC ";
                }
                else
                    localSQL = " DATA_PT_NAME ASC ";


                DataRow[] dataRowArray = dpList.Select("", localSQL);

                DataGridViewRow dataGridRow = null;
                DataRow dataRow = null;


                if (upperRecord > dataRowArray.Length)
                {
                    upperRecord = dataRowArray.Length;
                }

                int counter = 0;
                string tempString = "";
                dataPointListDataGridView.Rows.Clear();
                if (lowerRecord > 0) lowerRecord--;
                dataPointListDataGridViewprogressBar.Value = 0;
                for (int i = lowerRecord; i < upperRecord; i++)
                {
                    dataPointListDataGridView.Rows.Add();
                    dataPointListDataGridViewprogressBar.Value++;
                    dataGridRow = dataPointListDataGridView.Rows[counter];
                    dataRow = dataRowArray[i];

                    tempString = "";
                    tempString = dataRow["DATA_PT_NAME"].ToString();
                    if (tempString.Length > 100)
                        dataGridRow.Cells[2].Value = tempString.Substring(0, 100) + "...";
                    else
                        dataGridRow.Cells[2].Value = tempString;

                    tempString = "";
                    tempString = dataRow["DATA_PT_LBL_NAME"].ToString();
                    if (tempString.Length > 100)
                        dataGridRow.Cells[3].Value = tempString.Substring(0, 100) + "...";
                    else
                        dataGridRow.Cells[3].Value = tempString;

                    dataGridRow.Cells[4].Value = dataRow["DATA_PT_TYPE"].ToString();

                    if (TrendViewerHelper.isNumeric(dataRow["DATA_PT_COLOR"].ToString(), System.Globalization.NumberStyles.Number))
                    {
                        dataGridRow.Cells[5].Value = "";
                        dataGridRow.Cells[5].Style.BackColor = Color.FromArgb(Convert.ToInt32(dataRow["DATA_PT_COLOR"].ToString()));
                    }

                    tempString = "";
                    tempString = dataRow["DATA_PT_SERVER"].ToString();
                    if (tempString.Length > 100)
                        dataGridRow.Cells[10].Value = tempString.Substring(0, 100) + "...";
                    else
                        dataGridRow.Cells[10].Value = tempString;

                    dataGridRow.Cells[6].Value = ((DateTime)dataRow["DATA_PT_DT"]).ToString(DateTimeFormatter.getInstance().getDateFormatStr());
                    dataGridRow.Cells[7].Value = ((DateTime)dataRow["DATA_PT_DT_END"]).ToString(DateTimeFormatter.getInstance().getDateFormatStr());


                    if (TrendViewerHelper.ChangeStrToBool(dataRow["DATA_PT_ENABLED"].ToString()))
                        dataGridRow.Cells[8].Value = Properties.Resources.img_checkmark;
                    else
                        dataGridRow.Cells[8].Value = Properties.Resources.blank;

                    if (TrendViewerHelper.ChangeStrToBool(dataRow["DATA_PT_LBL_ENABLED"].ToString()))
                        dataGridRow.Cells[9].Value = Properties.Resources.img_checkmark;
                    else
                        dataGridRow.Cells[9].Value = Properties.Resources.blank;

                    counter++;

                }

            }
            catch (Exception localException)
            {
                
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            dataPointListDataGridViewprogressBar.Visible = false;
        }


        #endregion




        public void Display(int mode, DataTable dpList)
        {
            string Function_Name = "Display";
            try
            {
                InitializeDataPointListDataGridView();
                PopulateDataPointListDataGridView(mode, dpList);
            }
            catch (Exception localException)
            {
                
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        private void DataPointHistoryViewer_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "DataPointHistoryViewer_Paint";
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
