using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Drawing;
using System.IO;


using STEE.ISCS.Log;
using STEE.ISCS.MVC;
using STEE.ISCS.MulLanguage;
using Entity.Trending;
using TrendingHelper;

using OPCSampleGrpConfig.Common;
using OPCSampleGrpConfig.View;
using OPCSampleGrpConfig.Model;

namespace OPCSampleGrpConfig.Controller
{
    
    /// <summary>
    /// Controller class of OPCDataSelector View.
    /// </summary>
    public class OPCDataSelectorController : IController
    {
        private const string CLASS_NAME = "OPCSampleGrpConfig.Controller.OPCDataSelectorController";

        private OPCDataSelectorModel m_model;
        private  OPCDataSelector m_view;
        //current OPC server name and its host
        private List<string> m_opcServeNames = null;
        private List<string> m_opcServerHost = null;
        //Current selected Sample Group ID
        public  string m_localIntervalGrpID = "";
        public string m_localIntervalGrpLocation = "";
        private string m_filterString = "";
        private string m_currentDataNode = "";
        private string m_serverRootName = "";
        private string m_currentOpcServerName = "";
        private const string CONFIGFOLDER = "./";

        public OPCDataSelectorController()
        {
            m_opcServeNames = new List<string>();
            m_opcServerHost = new List<string>();
            m_serverRootName = ConfigureFileHelper.GetInstance().OPCServerRootName;
            m_opcServerHost.Add(ConfigureFileHelper.GetInstance().OPCDTSever1Host);
            m_opcServerHost.Add(ConfigureFileHelper.GetInstance().OPCDTSever2Host);
            m_opcServeNames.Add(ConfigureFileHelper.GetInstance().OPCDTSever1Name);
            m_opcServeNames.Add(ConfigureFileHelper.GetInstance().OPCDTSever2Name);  

        }

        /// <summary>
        /// Callback function which links controller with its view and model classes.
        /// </summary>
        /// <param name="model"></param>
        /// <param name="view"></param>
        public override void AttachCallBack(IModel model, IView view)
        {
            m_model = (OPCDataSelectorModel)model;
            m_view = (OPCDataSelector)view;
            //intialize View control with controller data
            m_view.InitializeOpcServerBox(m_opcServeNames,m_opcServerHost);
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
        }
        
        /// <summary>
        /// Returns DataPoints of the selected server name
        /// </summary>
        /// <param name="opcServerName">datapoint server name</param>
        /// <param name="filterString">filter string - such as datapoint name</param>
        public void GetDataPointList(string opcServerName,string filterString)
        {
           // m_view.TotalDTRecord = m_model.GetTotalDataPointCount(opcServerName, filterString);
            m_view.SetDataPointList(m_model.GetAllOPCDataPoints(opcServerName, filterString));
        }          

        /// <summary>
        /// ServerName ComboBox event handler function. 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void opcServerIPBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            //based on selection, change the currentservername and refresh the datanode tree and its selected datapoint.
            m_currentOpcServerName = "";
            System.Windows.Forms.ComboBox opcServerIPBox = (System.Windows.Forms.ComboBox)sender;
            if (opcServerIPBox.SelectedIndex == 0)
            {
               m_currentOpcServerName = m_opcServeNames[0];             
            }
            if (opcServerIPBox.SelectedIndex == 1)
            {
                m_currentOpcServerName = m_opcServeNames[1];               
            }
            m_view.SetServerNameText(m_currentOpcServerName);
            m_view.SetTreeNodeList(m_model.GetAllOPCDataNodes(m_serverRootName, m_currentOpcServerName));

            string filterString = " AND  DP_GRP_ID = " + m_localIntervalGrpID;
            m_view.SetSelectedDataPointList(m_model.GetAllOPCDataPoints(m_currentOpcServerName, filterString));      
        }

        /// <summary>
        /// DataPoint List DoubleClick Event Handler Function -
        /// This method adds the selected DataPoint from DataPointList to Selected DataPointlist.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void opcItemDataGridView_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0) return;
            string opcItemID = ((System.Windows.Forms.DataGridView)sender).Rows[e.RowIndex].Cells[OPCDataSelector.OPCITEM_DATAPTNAME_COLINDEX].Value.ToString();
            m_view.AddToSelectedListBox(opcItemID);           
        }

        /// <summary>
        /// Selected DataPoint List Doubleclick Event Handler function - 
        /// This method removes the selected datapoint from the selected datapoint list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void selectDataPointListBox_DoubleClick(object sender, EventArgs e)
        {
            System.Windows.Forms.ListBox selectDataPointListBox = (System.Windows.Forms.ListBox)sender;
            //selectDataPointListBox.Items.Remove(selectDataPointListBox.SelectedItem);           
            m_view.RemoveSelectedDataPoint(selectDataPointListBox.SelectedItem); 
        }

        /// <summary>
        /// Tree Mouse Click Event handler Function -
        /// This function is responsible to load the datapoints of the selected datanode.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void treeOpcItems_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            string Function_Name = "treeOpcItems_NodeMouseClick";
            try
            {
                if (e.Node.Name.Trim() != "")
                {
                    if (m_currentDataNode == e.Node.Name.Trim()) return;

                    m_currentDataNode = e.Node.Name.Trim();
                    m_filterString = e.Node.Name.Trim() + ".";
                    //m_filterString = " AND DATA_PT_NAME LIKE '" + m_filterString.Trim() + "%' ";
                    m_filterString = " AND PARENTKEY = " + (double)e.Node.Tag;


                    //m_filterString = " AND et.NAME LIKE '" + m_filterString.Trim() + "%' ";
                    //GetDataPointList(m_currentOpcServerName, m_filterString);
                }
                else
                {
                    if (m_currentDataNode.Trim() == "") return;
                    m_currentDataNode = "";
                    m_filterString = "";
                    //GetDataPointList(m_currentOpcServerName, m_filterString);
                }
                GetDataPointList(m_currentOpcServerName, m_filterString);
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        /// <summary>
        /// Loads the child data node of the selected data node.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void treeOpcItems_AfterSelect(object sender, TreeViewEventArgs e)
        {
            System.Windows.Forms.TreeView treeOpcItems = (System.Windows.Forms.TreeView)sender;
            if (treeOpcItems.SelectedNode.Nodes.Count < 1)
            {
                List<EtyOPCDataNode> childDataNodes = m_model.GetChildOPCDataNodes(treeOpcItems.SelectedNode.Tag.ToString(), m_currentOpcServerName);
                m_view.AddTreeNode(childDataNodes, treeOpcItems.SelectedNode.Nodes);
            }
        }

        /// <summary>
        /// Repaints the Form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void OPCDataSelector_Paint(object sender, PaintEventArgs e)
        {
            Graphics obj_Graph = e.Graphics;

            LinearGradientBrush b = new LinearGradientBrush(new System.Drawing.Rectangle(0, 0, m_view.Width, m_view.Height), Color.LightBlue, Color.BlanchedAlmond, LinearGradientMode.Vertical);

            obj_Graph.FillRectangle(b, new System.Drawing.Rectangle(0, 0, m_view.Width, m_view.Height));
        }


        /// <summary>
        /// Removes the selected datapoints from selected Datapoint.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_view.RemoveSelectedDataPoints();
        }

        /// <summary>
        /// Display DataPoint with that search substring in the DataPoint List.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void searchBtn_Click(object sender, EventArgs e)
        {
            if (m_currentOpcServerName == "") return;
            if (m_view.GetSearchText().Trim() != "")
            {
                m_view.SetDataPointList(m_model.GetDataPointsByName(m_currentOpcServerName, m_view.GetSearchText().ToUpper()));
            }

        }

        public void contextMenuStrip2_Click(object sender, EventArgs e)
        {

        }

        /// <summary>
        /// Adds the selected Datapoint to the Selected datapoint list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void addToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_view.AddSelectedDataPoints();
        }

        /// <summary>
        /// Writes Event Log
        /// </summary>
        /// <param name="messageContent"></param>
        /// <param name="moduleName"></param>
        public void WriteEventLog(string messageContent, string moduleName)
        {
            try
            {
                StreamWriter SW;
                SW = File.AppendText(CONFIGFOLDER + "/logs/mft" + DateTime.Now.ToString("yyyyMMdd") + ".txt");
                SW.WriteLine(moduleName + " [" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + "]" + messageContent);
                SW.Close();
            }
            catch (Exception localException)
            {
                Console.WriteLine(localException.ToString());
            }
        }

        /// <summary>
        /// Saves the selected Datapoint of the specified Sample Grp and Server name in database.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void okCmd_Click(object sender, EventArgs e)
        {
            string Function_Name = "okCmd_Click";
            try
            {                
                if (m_localIntervalGrpID == "") return;
                if (m_view.CheckSelectionListModified())
                {
                    EtyDataLogDPTrend SampleGrpEty = new EtyDataLogDPTrend();
                    SampleGrpEty.OPCSampleGrpId = Convert.ToDouble(m_localIntervalGrpID);
                    SampleGrpEty.Disabled = true;
                    List<string> selectedItems = m_view.GetSelectedDataPoints();
                    List<EtyDataLogDPTrend> selectedDataPoints = new List<EtyDataLogDPTrend>();
                    for (int i = 0; i < selectedItems.Count; i++)
                    {
                        EtyDataLogDPTrend etyDataPoint = new EtyDataLogDPTrend();
                        etyDataPoint.Disabled = false;
                        etyDataPoint.OPCSampleGrpId = Convert.ToDouble(m_localIntervalGrpID);
                        etyDataPoint.OPCDataPointName = selectedItems[i];
                        selectedDataPoints.Add(etyDataPoint);
                    }
                    m_model.UpdateDataPoints(SampleGrpEty, selectedDataPoints);
                }
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_SAVE_MSG,EnglishString.SAVE_SUSSCCESSFUL_MSG));
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
            m_view.Close();
        }


        public void cancelCmd_Click(object sender, EventArgs e)
        {
            m_view.Close();
        }

        public bool CanAddDataPoint(string dataPoint)
        {
            bool bCanAdd = true;
            if (!m_model.IsOccLocation())
            {
                if (m_model.CheckConfiguredByOcc(dataPoint))
                {
                    //display messgae box
                    string messageTxt = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_CANADDDP_MSG, EnglishString.CAN_ADD_DATAPOINT_MSG);

                    if (MessageBoxDialog.Show(string.Format(messageTxt, dataPoint), "", MessageBoxButtons.YesNo, MessageBoxIcon.Question).ToString() == "No")
                    {
                        bCanAdd = false;
                    }
                }
            }
            else
            {
                if (!m_model.CheckStationGrp(dataPoint, m_localIntervalGrpLocation))
                {
                    //Display message Box 
                    string messageTxt = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_CANNOTADDDP_MSG, EnglishString.CANNOT_ADD_DATAPOINT_MSG);
                    MessageBoxDialog.Show(string.Format(messageTxt,dataPoint),"",MessageBoxButtons.OK, MessageBoxIcon.Error);
                    bCanAdd = false;
                }
            }
            return bCanAdd;
        } 
    }
}
