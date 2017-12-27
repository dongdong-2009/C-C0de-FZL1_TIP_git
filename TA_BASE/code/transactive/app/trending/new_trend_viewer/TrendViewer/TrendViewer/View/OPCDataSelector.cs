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
using TrendViewer.Common;
using STEE.ISCS.MulLanguage;
using TrendingHelper;

namespace TrendViewer.View
{
    public partial class OPCDataSelector : BaseForm
    {
        private const string CLASS_NAME = "OPCDataSelector";
        public string opcItemID = "";
        public OPCDataSelector()
        {
            InitializeComponent();
            opcServerHostBox.Text = ConfigureFileHelper.GetInstance().HostIPAddress;
            dataSource.Text = ConfigureFileHelper.GetInstance().OPCServerName;
        }


        public override void TranslateCaption()
        {
            this.dpList.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_OPCDataSelector_GrpBoxName, LanguageHelper.TrendViewer_OPCDataSelector_GrpBoxName_EN);
            this.OPCServerLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_OPCDataSelector_OPCServerLbl, LanguageHelper.TrendViewer_OPCDataSelector_OPCServerLbl_EN);
            this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_OPCDataSelector_FormTitle, LanguageHelper.TrendViewer_OPCDataSelector_FormTitle_EN); 
        }


        public void Display(Dictionary<ulong, EtyEntity> entityMap )
        {
            InitializeOpcItemDataGridView();
            LoadDataNodeStructure(entityMap);
        }


        private void InitializeOpcItemDataGridView()
        {
            string Function_Name = "InitializeOpcItemDataGridView";
            //  <summary>
            //  Initialize VariableDataGridView Columns and Header
            //  </summary>
            try
            {
                DataGridViewImageColumn iconImage = new DataGridViewImageColumn();
                iconImage.Image = Properties.Resources.blank;
                iconImage.HeaderText = "Image";

                opcItemDataGridView.Columns.Add("KEYID", "Key ID");
                opcItemDataGridView.Columns.Add("DATA_PT_NAME", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_OPCDataSelector_OPCDPColumnName, LanguageHelper.TrendViewer_OPCDataSelector_OPCDPColumnName_EN));
                opcItemDataGridView.Columns.Add("DATA_PT_DESC", StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_OPCDataSelector_DesColumnName, LanguageHelper.TrendViewer_OPCDataSelector_DesColumnName_EN));
                opcItemDataGridView.Columns.Add(iconImage);
                opcItemDataGridView.Columns.Add("DISABLE", "Disabled");

                opcItemDataGridView.Columns[0].Visible = false;
                opcItemDataGridView.Columns[1].Width = 350; ;
                opcItemDataGridView.Columns[2].Width = 275;
                opcItemDataGridView.Columns[3].Width = 75;
                opcItemDataGridView.Columns[3].Visible = false;
                opcItemDataGridView.Columns[4].Visible = false;

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }




        private void LoadDataNodeStructure(Dictionary<ulong, EtyEntity> entityMap)
        {
            string Function_Name = "LoadDataNodeStructure";
            try
            {
                opcItemDataGridView.Rows.Clear();
                BuildDataNodeStructure(entityMap);
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        private void BuildDataNodeStructure(Dictionary<ulong, EtyEntity> entityMap)
        {
            treeOpcItems.Nodes.Clear();

            int counter = 0;

            if (entityMap == null) return;
            foreach (  KeyValuePair<ulong, EtyEntity> item in entityMap )
            {
                treeOpcItems.Nodes.Add(item.Value.Name,item.Value.Name + " - " + item.Value.Description);

                treeOpcItems.Nodes[counter].Tag = item.Value.Pkey;
                counter++;
            }

        }

        public TreeNode  getSelectedNode()
        {
            return  treeOpcItems.SelectedNode;
            
        }

        public override void AttachListener(STEE.ISCS.MVC.IController controller)
        {
            OPCDataSelectorController ctl = (OPCDataSelectorController)(getController());

            this.treeOpcItems.AfterSelect += ctl.AfterSelect;
            this.opcItemDataGridView.CellDoubleClick += ctl.opcItemDataGridView_CellDoubleClick;

        }



        public void LoadDataPointToGridView(Dictionary<ulong, EtyEntity> dpMap)
        {
            opcItemDataGridView.Rows.Clear();
            DataGridViewRow dataGridRow = null;
            int counter = 0;
            foreach(KeyValuePair<ulong, EtyEntity> dp in dpMap)
            {
                opcItemDataGridView.Rows.Add();
                dataGridRow = opcItemDataGridView.Rows[counter];
                
                dataGridRow.Cells[0].Value = dp.Value.Pkey;
            
                dataGridRow.Cells[1].Value = dp.Value.Name;
            
                dataGridRow.Cells[2].Value = dp.Value.Description;
                counter++;
            }
        }


        public void LoadDataNodeChildren(ref TreeNode localNodes, Dictionary<ulong, EtyEntity> childNodes)
        {
            int count=0;
            foreach (KeyValuePair<ulong, EtyEntity> node in childNodes)
            {
                localNodes.Nodes.Add(node.Value.Name, node.Value.Name+ " - " + node.Value.Description);
                localNodes.Nodes[count].Tag = node.Value.Pkey;
                count++;
            }
            
        }

        private void OPCDataSelector_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "OPCDataSelector_Paint";
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



        public EtyEntity  getDPInGridView(int p)
        {
            EtyEntity dp = new EtyEntity(); 
            dp.Pkey = Convert.ToUInt64(opcItemDataGridView.Rows[p].Cells[0].Value.ToString());
            dp.Name = opcItemDataGridView.Rows[p].Cells[1].Value.ToString();
            dp.Description = opcItemDataGridView.Rows[p].Cells[2].Value.ToString();

            return dp;
        }
    }
}
