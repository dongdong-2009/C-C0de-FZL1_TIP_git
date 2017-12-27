using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Runtime.InteropServices;

using STEE.ISCS.Log;
using STEE.ISCS.GenericGUI;
using STEE.ISCS.MVC;
using STEE.ISCS.MulLanguage;
using Entity.Trending;
using OPCSampleGrpConfig.Common;
using OPCSampleGrpConfig.Controller;



namespace OPCSampleGrpConfig.View
{
   
    /// <summary>
    /// This class is reponsible for coordinating the user Interaction with its Controller
    /// </summary>
    public partial class OPCDataSelector : BaseForm
    {
        private const string CLASS_NAME = "OPCSampleGrpConfig.View.OPCDataSelector";
        #region Declaration

        public const string OPCDATASELECTORFRM = "OPCDataSelector";
        public const string OPCSAMPLEGRPDIC = "OPCSampleGrpDic";
        public const string PARENTINTERVALNAME = "ParentGrpName";
      
        //Grid const
        public const string OPCITEM_KEYID_COLNAME = "KEYID";
        public const string OPCITEM_DATAPTNAME_COLNAME = "DATA_PT_NAME";
        public const string OPCITEM_DATAPTDESC_COLNAME = "DATA_PT_DESC";
        public const string OPCITEM_GRPID_COLNAME = "SAMPLE_GRP_ID";

        public const int OPCITEM_KEYID_COLINDEX = 0;
        public const int OPCITEM_DATAPTNAME_COLINDEX = 1;
        public const int OPCITEM_DATAPTDESC_COLINDEX = 2;
        public const int OPCITEM_GRPID_COLINDEX = 3;

        public const string OPCITEM_KEYID_COLTEXT = "Key ID";

        //const
        public const int SAMPLEGROUP_TEXTBOX_WIDTHPADDING = 5;
        public const int SAMPLEGROUP_TEXTBOX_FORM_WIDTHPADDING = 25;

       
        private Dictionary<string, string> m_sampleGroupDic = new Dictionary<string, string>();
        private List<EtyOPCDataNode> m_treeNodeList = null;
        private List<EtyDataLogDPTrend> m_ConfigDataPointList = null;
        private List<EtyDataLogDPTrend> m_SelectedDataPointList = null;
        private bool m_selectedDataPointsModified = false;

        #endregion

        public OPCDataSelector()
        {
            string Function_Name = "OPCDataSelector";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            
            InitializeComponent();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="objectName"></param>
        /// <param name="obj"></param>
        public override void PutObject(String objectName, Object obj)
        {
            string Function_Name = "PutObject";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (objectName.Equals(PARENTINTERVALNAME))
            {
                this.intervalNameTextBox.Text = (string)(obj);

                // adjust the size of Group name box based on the size.
                System.Drawing.SizeF mySize;
                using (System.Drawing.Graphics g = this.CreateGraphics())
                {
                    mySize = g.MeasureString(this.intervalNameTextBox.Text, this.intervalNameTextBox.Font);
                }
                // Resize the textbox  
                this.intervalNameTextBox.Width = (int)Math.Round(mySize.Width, 0);
                this.intervalNameTextBox.Width += SAMPLEGROUP_TEXTBOX_WIDTHPADDING;
                //if not exceeding the form size.
                int width = this.intervalNameTextBox.Location.X + this.intervalNameTextBox.Width;
                if (this.Width <= width)
                {
                    this.intervalNameTextBox.Width = this.Width - SAMPLEGROUP_TEXTBOX_FORM_WIDTHPADDING;
                }            
            }
            else if (objectName.Equals(OPCSAMPLEGRPDIC))
            {
                this.m_sampleGroupDic = (Dictionary<string, string>)(obj);
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Translate the captions to Chinese Langauage
        /// </summary>
        public override void TranslateCaption()
        {
            InitializeControlsCaptions();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ctl"></param>
        public override void AttachListener(IController ctl)
        {
            OPCDataSelectorController controller = (OPCDataSelectorController)ctl;
            this.opcItemDataGridView.CellDoubleClick += controller.opcItemDataGridView_CellDoubleClick;
            this.contextMenuStrip2.Click += controller.contextMenuStrip2_Click;
            this.addToolStripMenuItem.Click += controller.addToolStripMenuItem_Click;
            this.treeOpcItems.AfterSelect += controller.treeOpcItems_AfterSelect;
            this.treeOpcItems.NodeMouseClick += controller.treeOpcItems_NodeMouseClick;
            this.opcServerIPBox.SelectedIndexChanged += controller.opcServerIPBox_SelectedIndexChanged;
            this.selectDataPointListBox.DoubleClick += controller.selectDataPointListBox_DoubleClick;
            this.removeToolStripMenuItem.Click += controller.removeToolStripMenuItem_Click;
            this.searchBtn.Click += controller.searchBtn_Click;
            this.Paint += controller.OPCDataSelector_Paint;
            this.okCmd.Click += controller.okCmd_Click;
            this.cancelCmd.Click += controller.cancelCmd_Click;
        }
 
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OPCDataSelector_Load(object sender, EventArgs e)
        {
            string Function_Name = "OPCDataSelector_Load";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            //load datanode and its corresponding child datanode
            InitializeControlsCaptions();
            opcServerIPBox.SelectedIndex = 0;            
            InitializeOpcItemDataGridView();
            opcItemDataGridViewDataGridViewprogressBar.Visible = false;

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Intializes the Form Control caption based on current langauage.
        /// </summary>
        private void InitializeControlsCaptions()
        {
            string Function_Name = "InitializeControlsCaptions";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            this.serverInfoBox.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_SERVERBOX_TEXT, this.serverInfoBox.Text);
            this.addToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_ADDMENU, this.addToolStripMenuItem.Text);
            this.okCmd.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_OK_BUTTON, this.okCmd.Text);
            this.serverNameBox.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_SERVERNAME_TEXT, this.serverNameBox.Text);
            this.label8.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_LABEL8, this.label8.Text);
            this.removeToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_REMOVEMENU, this.removeToolStripMenuItem.Text);
            this.groupBox1.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_GROUPBOX, this.groupBox1.Text);
            this.label1.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_LABEL1, this.label1.Text);
            this.label2.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_LABEL2, this.label2.Text);
            this.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_FORM_TITLE, this.Text);
            this.cancelCmd.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_CANCEL_BUTTON, this.cancelCmd.Text);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Intialize the OPC Server name control.
        /// </summary>
        /// <param name="serverNames"></param>
        /// <param name="serverHosts"></param>
        public void InitializeOpcServerBox(List<string> serverNames, List<string> serverHosts)
        {
            string Function_Name = "InitializeOpcServerBox";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            opcServerIPBox.Items.Clear();
            opcServerIPBox.Items.Add(serverHosts[0] + " - " + serverNames[0]);
            opcServerIPBox.Items.Add(serverHosts[1] + " - " + serverNames[1]);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Initializes DataPoint List Control.
        /// </summary>
         private void InitializeOpcItemDataGridView()
        {
            //  <summary>
            //  Initialize VariableDataGridView Columns and Header
            //  </summary>
            opcItemDataGridView.Columns.Add(OPCITEM_KEYID_COLNAME, OPCITEM_KEYID_COLTEXT);
            opcItemDataGridView.Columns.Add(OPCITEM_DATAPTNAME_COLNAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCITEM_DATAPTNAME_COLTEXT, EnglishString.OPC_DP_NAME));
            opcItemDataGridView.Columns.Add(OPCITEM_DATAPTDESC_COLNAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCITEM_DATAPTDESC_COLTEXT, EnglishString.DESCRIPTION));
            opcItemDataGridView.Columns.Add(OPCITEM_GRPID_COLNAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCITEM_GRPID_COLTEXT, EnglishString.DATA_GROUP_NAME));

            opcItemDataGridView.Columns[OPCITEM_KEYID_COLINDEX].Visible = false;
            opcItemDataGridView.Columns[OPCITEM_DATAPTNAME_COLINDEX].Width = 400; ;
            opcItemDataGridView.Columns[OPCITEM_DATAPTDESC_COLINDEX].Width = 275;
            opcItemDataGridView.Columns[OPCITEM_GRPID_COLINDEX].Width = 255;
            opcItemDataGridView.Columns[OPCITEM_DATAPTDESC_COLINDEX].Visible = false;
        }      

        /// <summary>
        /// Builds Tree control with the DataNodes based on selected Servername.
        /// </summary>
        private void BuildDataNodeStructure()
        {            
            treeOpcItems.Nodes.Clear();
            AddTreeNode(m_treeNodeList, treeOpcItems.Nodes);                 
        }

        /// <summary>
        /// Adds Tree Node to the Tree Control.
        /// </summary>
        /// <param name="opcDataNodes">Datanodes to be added</param>
        /// <param name="treeNode">Tree Node to which datanodes are added</param>
        public void AddTreeNode(List<EtyOPCDataNode> opcDataNodes, TreeNodeCollection treeNode)
        {
            int counter = 0;
            foreach (EtyOPCDataNode etyOPCDTNode in opcDataNodes)
            {
                if (etyOPCDTNode.OPCDataNodeName != null)
                {
                    if (etyOPCDTNode.OPCDataNodeDesc != null)
                    {
                        treeNode.Add(etyOPCDTNode.OPCDataNodeName, etyOPCDTNode.OPCDataNodeName + " - " + etyOPCDTNode.OPCDataNodeDesc);
                    }
                    else
                    {
                        treeNode.Add(etyOPCDTNode.OPCDataNodeName, etyOPCDTNode.OPCDataNodeName);
                    }
                    treeNode[counter].Tag = etyOPCDTNode.OPCDataNodeId;
                }
                counter++;
            }          
        }

        /// <summary>
        /// Loads the DataPoints in the DataPointList Control.
        /// </summary>
        private void LoadDataPointGrid()
        {
            string Function_Name = "LoadDataPointGrid";
            try
            {
                opcItemDataGridView.Rows.Clear();
                if (m_ConfigDataPointList.Count > 0)
                {
                    opcItemDataGridViewDataGridViewprogressBar.Maximum = m_ConfigDataPointList.Count;
                    opcItemDataGridViewDataGridViewprogressBar.Visible = true;
                    opcItemDataGridViewDataGridViewprogressBar.Value = 0;
                }
                DataGridViewRow dataGridRow = null;
                int counter = 0;
                foreach (EtyDataLogDPTrend etyDTPoint in m_ConfigDataPointList)
                {
                    opcItemDataGridViewDataGridViewprogressBar.Value++;
                    opcItemDataGridView.Rows.Add();
                    dataGridRow = opcItemDataGridView.Rows[counter];
                    dataGridRow.Cells[OPCITEM_KEYID_COLINDEX].Value = etyDTPoint.EntityKey;
                    dataGridRow.Cells[OPCITEM_DATAPTNAME_COLINDEX].Value = etyDTPoint.OPCDataPointName;
                    dataGridRow.Cells[OPCITEM_DATAPTDESC_COLINDEX].Value = etyDTPoint.OPCDataPointDesc;
                    double grpID = etyDTPoint.OPCSampleGrpId;
                    if (grpID != -1)
                    {
                     //   dataGridRow.Cells[OPCITEM_GRPID_COLINDEX].Value = m_sampleGroupDic[grpID.ToString()];
                        string value;
                        m_sampleGroupDic.TryGetValue(grpID.ToString(), out value);
                        dataGridRow.Cells[OPCITEM_GRPID_COLINDEX].Value = value;
                    }
                    counter++;
                }
            }
            catch (Exception localExcpetion)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localExcpetion.ToString());
            }
            opcItemDataGridViewDataGridViewprogressBar.Visible = false;
        }

        /// <summary>
        /// Loads Selected DataPoint control.
        /// </summary>
        private void loadSelectedDatapoint()
        {
            selectDataPointListBox.Items.Clear();
            foreach (EtyDataLogDPTrend etyDPoint in m_SelectedDataPointList)
            {
                selectDataPointListBox.Items.Add(etyDPoint.OPCDataPointName);
            }
        }

        /// <summary>
        /// Add selected datapoints to selectDataPoint control.
        /// </summary>
        public void AddSelectedDataPoints()
        {
            string Function_Name = "AddSelectedDataPoints";
            try
            {
                for (int i = 0; i < opcItemDataGridView.SelectedRows.Count; i++)
                {
                    AddToSelectedListBox(opcItemDataGridView.SelectedRows[i].Cells[OPCITEM_DATAPTNAME_COLINDEX].Value.ToString());
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        /// <summary>
        /// Add Datapoint to Select Datapoint list control.
        /// </summary>
        /// <param name="opcItemID"></param>
        public void AddToSelectedListBox(string opcItemID)
        {
            if (!selectDataPointListBox.Items.Contains(opcItemID))
            {
                if (((OPCDataSelectorController)getController()).CanAddDataPoint(opcItemID))
                {
                    m_selectedDataPointsModified = true;
                    selectDataPointListBox.Items.Add(opcItemID);
                }
            }
        }

        /// <summary>
        /// Removes datapoint from selected datapoint list control.
        /// </summary>
        public void RemoveSelectedDataPoints()
        {
            string Function_Name = "RemoveSelectedDataPoints";
            try
            {
                int tempInt = selectDataPointListBox.SelectedItems.Count;
                for (int i = 0; i < tempInt; i++)
                {
                    RemoveSelectedDataPoint(selectDataPointListBox.SelectedItems[0]);
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        public void RemoveSelectedDataPoint(object value)
        {
            m_selectedDataPointsModified = true;
            selectDataPointListBox.Items.Remove(value);
        }

        /// <summary>
        /// Returns Selected DataPoint Control items.
        /// </summary>
        /// <returns></returns>
        public List<string> GetSelectedDataPoints()
        {
            List<string> selectedItems = new List<string>();
            for (int i = 0; i < selectDataPointListBox.Items.Count; i++)
            {
                selectedItems.Add(selectDataPointListBox.Items[i].ToString());
            }
            return selectedItems;
        }

        /// <summary>
        /// Returns whether the selected datapoint control is modified or not
        /// </summary>
        /// <returns>true - modified, false - not modified</returns>
        public bool CheckSelectionListModified()
        {
            return m_selectedDataPointsModified;
        }

        /// <summary>
        /// Refresh the tree control based on new datanodes
        /// </summary>
        /// <param name="value"></param>
        public void SetTreeNodeList(List<EtyOPCDataNode> value)
        {
            m_treeNodeList = value;
            //refresh the tree view todo
            opcItemDataGridView.Rows.Clear();
            BuildDataNodeStructure();
        }


        /// <summary>
        /// Refresh Datapoint List based on new datapoints
        /// </summary>
        /// <param name="value"></param>
        public void SetDataPointList(List<EtyDataLogDPTrend> value)
        {
            m_ConfigDataPointList = value;
            //Refresh DataPointGrid
            LoadDataPointGrid();
        }

        /// <summary>
        /// Refresh Selected DataPointsList based on new dataPoints
        /// </summary>
        /// <param name="value"></param>
        public void SetSelectedDataPointList(List<EtyDataLogDPTrend> value)
        {
            m_SelectedDataPointList = value;
            //Refresh SelectedListBox
            loadSelectedDatapoint();
        }

        /// <summary>
        /// Sets the specified servername in servename combo box 
        /// </summary>
        public void SetServerNameText(string value)
        {
            this.serverNameBox.Text = value;
        }

        /// <summary>
        /// Returns Search Text box text. 
        /// </summary>
        public string GetSearchText()
        {
            return searchTextBox.Text;
        }
    }
}
