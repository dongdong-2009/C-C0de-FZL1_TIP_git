using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;



using TrendViewer.Model;
using STEE.ISCS.Log;
using STEE.ISCS.MVC;
using STEE.ISCS.GenericGUI;

using OPCTrendLib;
using Entity.Trending;
using TrendViewer.Controller;
using STEE.ISCS.MulLanguage;


namespace TrendViewer.View
{
    public partial class MarkerList :  BaseForm
    {
        private const string CLASS_NAME = "MarkerList";
        private int RECORDSIZE = 8;


        public MarkerList()
        {
            InitializeComponent();
            InitializeDataMarkerListDataGridView();
        }
        public override void TranslateCaption()
        {

            this.addDataMarkerToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkerList_AddMenu, LanguageHelper.TrendViewer_MarkerList_AddMenu_EN);
            this.loadMarkerConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkerList_LoadMenu, LanguageHelper.TrendViewer_MarkerList_LoadMenu_EN);
            this.saveMarkerConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkerList_SaveMenu, LanguageHelper.TrendViewer_MarkerList_SaveMenu_EN);
            this.markerListGrpBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkerList_GroupBox, LanguageHelper.TrendViewer_MarkerList_GroupBox_EN);
            this.TotalPage_Tlbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TotalPageLbl, LanguageHelper.TrendViewer_TotalPageLbl_EN);
            this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_MarkerList_Title, LanguageHelper.TrendViewer_MarkerList_Title_EN);
        }


        private void DataMarkerViewer_Load(object sender, EventArgs e)
        {
            string Function_Name = "DataMarkerViewer_Load";
            try
            {
                dataMarkerListDataGridViewprogressBar.Visible = false;
                dataMarkerListDataGridViewprogressBar.Maximum = RECORDSIZE;

                PopulateDataMarkerListDataGridView(1);
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

        }




        #region dataMarkerListDataGridView

        private void InitializeDataMarkerListDataGridView()
        {
            //  <summary>
            //  Initialize dataMarkerListDataGridView Columns and Header
            //  </summary>
            string Function_Name = "InitializeDataMarkerListDataGridView";
            try
            {
                
                DataGridViewImageColumn editColumn = new DataGridViewImageColumn();
                editColumn.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColEdit, LanguageHelper.TrendViewer_ColEdit_EN);
                editColumn.Image = Properties.Resources.Notes_16_;
                dataMarkerListDataGridView.Columns.Add(editColumn);
                dataMarkerListDataGridView.Columns[0].Width = 75;
                dataMarkerListDataGridView.Columns[0].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

                DataGridViewImageColumn deleteColumn = new DataGridViewImageColumn();
                deleteColumn.Image = Properties.Resources.Erase_16_;
                deleteColumn.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColDelete, LanguageHelper.TrendViewer_ColDelete_EN);
                dataMarkerListDataGridView.Columns.Add(deleteColumn);
                dataMarkerListDataGridView.Columns[1].Width = 75;
                dataMarkerListDataGridView.Columns[1].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

                dataMarkerListDataGridView.Columns.Add("MARKER_NAME", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColMarkerName, LanguageHelper.TrendViewer_ColMarkerName_EN));
                dataMarkerListDataGridView.Columns[2].Width = 190;
                dataMarkerListDataGridView.Columns.Add("MARKER_WIDTH", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColMarkerWidth, LanguageHelper.TrendViewer_ColMarkerWidth_EN));
                dataMarkerListDataGridView.Columns[3].Width = 150;
                dataMarkerListDataGridView.Columns.Add("MARKER_BCOLOR", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColMarkerBcolor, LanguageHelper.TrendViewer_ColMarkerBcolor_EN)); 
                dataMarkerListDataGridView.Columns[4].SortMode = DataGridViewColumnSortMode.NotSortable;
                dataMarkerListDataGridView.Columns[4].Width = 160;

                dataMarkerListDataGridView.Columns.Add("MARKER_FCOLOR", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColMarkerFontColor, LanguageHelper.TrendViewer_ColMarkerFontColor_EN));  
                dataMarkerListDataGridView.Columns[5].SortMode = DataGridViewColumnSortMode.NotSortable;
                dataMarkerListDataGridView.Columns[5].Width = 160;

                dataMarkerListDataGridView.Columns.Add("MARKER_VALUE", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColMarkerStartVal, LanguageHelper.TrendViewer_ColMarkerStartVal_EN));
                 dataMarkerListDataGridView.Columns[6].Width = 139;
                DataGridViewImageColumn enableMarkerCol = new DataGridViewImageColumn();
                enableMarkerCol.HeaderText = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_ColMarkerEnabled, LanguageHelper.TrendViewer_ColMarkerEnabled_EN);
                enableMarkerCol.Image = Properties.Resources.img_checkmark;
                dataMarkerListDataGridView.Columns.Add(enableMarkerCol);
                dataMarkerListDataGridView.Columns[7].Width = 139;
                dataMarkerListDataGridView.Columns[7].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
              

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }


        public void PopulateDataMarkerListDataGridView(int mode)
        {
            //  <summary>
            //  dataMarkerListDataGridView Data Population
            //  </summary>
            string Function_Name = "PopulateDataMarkerListDataGridView";
            dataMarkerListDataGridViewprogressBar.Visible = true;

            try
            {
                dataMarkerListDataGridViewprogressBar.Value = 0;

                totaldataMarkerListDataGridView.Text = "1";
                if (mode == 1)
                    pageNumdataMarkerListDataGridView.Value = 1;

                MarkerListController ctl = (MarkerListController)getController();

                int totalRecord = ctl.getMarkerTable().Rows.Count;  //m_dataMarkerListDataGridView.RowCount;  //todo check
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
                    totaldataMarkerListDataGridView.Text = totalRecord.ToString();
                    pageNumdataMarkerListDataGridView.Maximum = totalRecord;
                }
                else
                {
                    pageNumdataMarkerListDataGridView.Maximum = 1;
                }

                upperRecord = (int)(RECORDSIZE * pageNumdataMarkerListDataGridView.Value);
                if (pageNumdataMarkerListDataGridView.Value > 1)
                    lowerRecord = (int)(RECORDSIZE * (pageNumdataMarkerListDataGridView.Value - 1)) + 1;
                else
                    lowerRecord = (int)(RECORDSIZE * (pageNumdataMarkerListDataGridView.Value - 1));

                string localSQL = "";
                if (dataMarkerListDataGridView.SortedColumn != null && dataMarkerListDataGridView.Rows.Count > 0)
                {
                    if (dataMarkerListDataGridView.SortedColumn.Name != null && dataMarkerListDataGridView.SortedColumn.Name.Trim() != "")
                    {
                        localSQL = dataMarkerListDataGridView.SortedColumn.Name;
                        if (dataMarkerListDataGridView.SortOrder.ToString().ToLower() != "ascending")
                            localSQL += " DESC ";
                        else
                            localSQL += " ASC ";

                    }
                    else
                        localSQL = " MARKER_NAME ASC ";
                }
                else
                    localSQL = " MARKER_NAME ASC ";


                
                DataRow[] markerRowArray = ctl.getMarkerTable().Select("", localSQL);     

                DataGridViewRow dataGridRow = null;
                DataRow dataRow = null;


                if (upperRecord > markerRowArray.Length)
                {
                    upperRecord = markerRowArray.Length;
                }

                int counter = 0;
                string tempString = "";

                dataMarkerListDataGridView.Rows.Clear();
                if (lowerRecord > 0) lowerRecord--;
                dataMarkerListDataGridViewprogressBar.Value = 0;
                for (int i = lowerRecord; i < upperRecord; i++)
                {
                    dataMarkerListDataGridView.Rows.Add();
                    dataMarkerListDataGridViewprogressBar.Value++;
                    dataGridRow = dataMarkerListDataGridView.Rows[counter];
                    dataRow = markerRowArray[i];

                    tempString = "";
                    tempString = dataRow["MARKER_NAME"].ToString();
                    if (tempString.Length > 100)
                        dataGridRow.Cells[2].Value = tempString.Substring(0, 100) + "...";
                    else
                        dataGridRow.Cells[2].Value = tempString;

                    dataGridRow.Cells[3].Value = dataRow["MARKER_WIDTH"].ToString();

                    if (TrendViewerHelper.isNumeric(dataRow["MARKER_BCOLOR"].ToString(), System.Globalization.NumberStyles.Number))
                    {
                        dataGridRow.Cells[4].Value = "";
                        dataGridRow.Cells[4].Style.BackColor = Color.FromArgb(Convert.ToInt32(dataRow["MARKER_BCOLOR"].ToString()));
                    }

                    if (TrendViewerHelper.isNumeric(dataRow["MARKER_FCOLOR"].ToString(), System.Globalization.NumberStyles.Number))
                    {
                        dataGridRow.Cells[5].Style.BackColor = Color.FromArgb(Convert.ToInt32(dataRow["MARKER_FCOLOR"].ToString()));
                    }

                    dataGridRow.Cells[6].Value = dataRow["MARKER_VALUE"].ToString();


                    if (TrendViewerHelper.ChangeStrToBool(dataRow["MARKER_ENABLED"].ToString()))
                        dataGridRow.Cells[7].Value = Properties.Resources.img_checkmark;
                    else
                        dataGridRow.Cells[7].Value = Properties.Resources.blank;

                    counter++;
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            dataMarkerListDataGridViewprogressBar.Visible = false;
        }


        private void firstdataMarkerListDataGridView_Click(object sender, EventArgs e)
        {
            pageNumdataMarkerListDataGridView.Value = 1;
        }

        private void prevdataMarkerListDataGridView_Click(object sender, EventArgs e)
        {
            if (pageNumdataMarkerListDataGridView.Value > 1)
                pageNumdataMarkerListDataGridView.Value = pageNumdataMarkerListDataGridView.Value - 1;
        }

        private void pageNumdataMarkerListDataGridView_ValueChanged(object sender, EventArgs e)
        {
            PopulateDataMarkerListDataGridView(2);
        }

        private void nextdataMarkerListDataGridView_Click(object sender, EventArgs e)
        {
            if (pageNumdataMarkerListDataGridView.Value < pageNumdataMarkerListDataGridView.Maximum)
                pageNumdataMarkerListDataGridView.Value = pageNumdataMarkerListDataGridView.Value + 1;
        }

        private void lastdataMarkerListDataGridView_Click(object sender, EventArgs e)
        {
            pageNumdataMarkerListDataGridView.Value = pageNumdataMarkerListDataGridView.Maximum;

        }

        private void dataMarkerListDataGridView_Sorted(object sender, EventArgs e)
        {
            PopulateDataMarkerListDataGridView(2);

        }

        #endregion

        public override void AttachListener(IController ctl)
        {
            MarkerListController controller = (MarkerListController)ctl;
            this.addDataMarkerToolStripMenuItem.Click += controller.AddMarker;
            this.saveMarkerConfigToolStripMenuItem.Click += controller.SaveMarker; 
            this.loadMarkerConfigToolStripMenuItem.Click += controller.LoadMarker;
            this.dataMarkerListDataGridView.CellDoubleClick += controller.CellDoubleClick;
            this.FormClosed += controller.FormClose;
        } 

        public string GetMarkNameByRowIndex(int index)
        {
            if(dataMarkerListDataGridView == null || dataMarkerListDataGridView.Rows.Count < index+1)
            {
                return "";
            }
            return dataMarkerListDataGridView.Rows[index].Cells[2].Value.ToString();
        }



        private void DataMarkerViewer_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "DataMarkerViewer_Paint";
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