using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
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
    /// Controller class of OPCSampleGroupConfigStart View.
    /// </summary>
    public class OPCSampleGrpConfigStartController:IController
    {
        private const string CLASS_NAME = "OPCSampleGrpConfigStartController";

        private int leftLocation = 0;

        // List of all sample group last loaded
        private List<EtyDataLogDPGroupTrend> m_DPGroupTrendList = null;
        private OPCSampleGrpConfigStartModel m_model = null;
        private OPCSampleGrpConfigStart m_view = null;
        private const string CONFIGFOLDER = "./";
        //flag to decide to load sample group from database or use already retrived list
        public bool m_loadFromDB = true;

        /// <summary>
        ///  Callback function which links controller with its view and model classes.
        /// </summary>
        /// <param name="model"></param>
        /// <param name="view"></param>
        public override void AttachCallBack(IModel model, IView view)
        {
            m_model = (OPCSampleGrpConfigStartModel)model;
            m_view = (OPCSampleGrpConfigStart)view;
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file

        }

        public void SetSampleGrpConfigLocation( ref Form frm, string paramList)
        {
            string[] arg = paramList.Split(new char[] { ',' }, StringSplitOptions.None);
            int screenWidth = ConfigureFileHelper.GetInstance().ScreenWidth;
            const int OFFSET = 200; 

            if (arg!=null && arg.Length > 0)
            {
                if (arg[0].Trim().ToLower() == "view-edit-0")
                {
                    this.leftLocation = OFFSET;
                }
                else if (arg[0].Trim().ToLower() == "view-edit-1")
                {
                    this.leftLocation = screenWidth + OFFSET;
                }
                else if (arg[0].Trim().ToLower() == "view-edit-2")
                {
                    this.leftLocation = screenWidth*2 + OFFSET;
                }
                frm.StartPosition = FormStartPosition.Manual;
                frm.Location = new Point(leftLocation, 0x9b);
            }
            
        }

        /// <summary>
        ///  Connect to Oracle
        /// </summary>
        /// <param name="connectionString">Oracle connection string</param>
        /// <returns>true - connected , false - failed</returns>
        public bool GetDatabaseConnection()
        {
            return m_model.OpenConnection();
        }

        /// <summary>
        /// This method saves edited or new sample Group into the database.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void SaveCommand(object sender, EventArgs e)
        {
            string Function_Name = "SaveCommand";
            try
            {
                EtyDataLogDPGroupTrend etyDPGrpTrend = m_view.GetEditFields();
                if (etyDPGrpTrend == null)
                    return;
                if (etyDPGrpTrend.NewData && IsOPCSampleGrpExist(etyDPGrpTrend.SampleGrpName,etyDPGrpTrend.LocationKey))
                {
                    MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLECONFIG_GRPEXISTS_MSG, EnglishString.MSG_NAME_EXIST));
                    return;
                }
                if (!etyDPGrpTrend.NewData)
                    m_model.UpdateOPCSampleGrp(etyDPGrpTrend);
                else
                    m_model.InsertOPCSampleGrp(etyDPGrpTrend);
 MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCDATASELECTOR_SAVE_MSG, EnglishString.SAVE_SUSSCCESSFUL_MSG));
                //load all sample group from database
                m_loadFromDB = true;
                m_view.PopulateIntervalConfigDataGridView(2);
                m_view.Clear();
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        /// <summary>
        /// Column Doubleclick Event Handler Function.
        /// Enable User to edit/delete/configDatapoints of the selected Sample Group.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void IntervalConfigDataGridView_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string Function_Name = "IntervalConfigDataGridView_CellDoubleClick";
            try
            {
                if (e.RowIndex < 0) return;
                string localIntervalGrpID = m_view.GetDataGridValue(e.RowIndex, OPCSampleGrpConfigStart.OPCSAMPLEGRP_IDCOL_INDEX);

                //Load selected sample group in the edit group controls
                if (e.ColumnIndex == OPCSampleGrpConfigStart.OPCSAMPLEGRP_EDITCOLUMN_INDEX)
                {
                    LoadEditData(localIntervalGrpID);
                }
                
                // Deletes the selected sampel group and refresh the datagrid.
                else if (e.ColumnIndex == OPCSampleGrpConfigStart.OPCSAMPLEGRP_DELETECOLUMN_INDEX)
                {
                    string messageTxt = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLECONFIG_DELETEMSG, EnglishString.MSG_DELETE_CONFIRM);
                    if (MessageBoxDialog.Show(string.Format(messageTxt, m_view.GetDataGridValue(e.RowIndex, 1)),                        
                        StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLECONFIG_DELETEMSGTITLE, EnglishString.MSG_SYSTEM_TITLE),MessageBoxButtons.YesNo, MessageBoxIcon.Question).ToString() == "Yes")
                    {
                        bool result = m_model.DeleteOPCSampleGrp(localIntervalGrpID);
                        if (!result)
                        {
                            MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLECONFIG_DELETEFAILED_MSG,EnglishString.MSG_DELETE_MSG));
                        }
                        else
                        {
                            m_DPGroupTrendList.Remove(GetOPCSampleGrp(localIntervalGrpID));
                            m_view.PopulateIntervalConfigDataGridView(2);
                        }
                    }
                }

                    // Opens the DataPoint selector form
                else if (e.ColumnIndex == OPCSampleGrpConfigStart.OPCSAMPLEGRP_CONFIGCOLUMN_INDEX)
                {
                    IView view = ViewManager.GetInstance().GetView(OPCDataSelector.OPCDATASELECTORFRM);
                    ((OPCDataSelectorController)view.getController()).m_localIntervalGrpID = localIntervalGrpID;
                    ((OPCDataSelectorController)view.getController()).m_localIntervalGrpLocation = m_view.GetDataGridValue(e.RowIndex, OPCSampleGrpConfigStart.OPCSAMPLEGRP_LOCATIONNAME_INDEX);
                    view.PutObject(OPCDataSelector.PARENTINTERVALNAME, m_view.GetDataGridValue(e.RowIndex, OPCSampleGrpConfigStart.OPCSAMPLEGRP_NAMECOL_INDEX));
                    view.ShowView(true);
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        /// <summary>
        ///  Loads selected Datapoint in the Editor controls
        /// </summary>
        /// <param name="editIntervalGrpID"></param>
        private void LoadEditData(string editIntervalGrpID)
        {
            m_view.ResetEditFields();
            if (editIntervalGrpID == "") return;
            EtyDataLogDPGroupTrend etyDPGrpTrend = GetOPCSampleGrp(editIntervalGrpID);
            if (etyDPGrpTrend != null)
            {
                m_view.SetEditFields(etyDPGrpTrend);
            }
        }

        /// <summary>
        /// Retrieves All sorted Sample Group from the Database
        /// </summary>
        /// <param name="sortCol">column used for sorting</param>
        /// <param name="sortingOrder">sorting order - DESC,ASC</param>
        /// <returns>sorted Sample Group List</returns>
        public List<EtyDataLogDPGroupTrend> RetriveAllOPCGrp(string sortCol, string sortingOrder , bool reload)
        {
           // m_view.OPCSampleGrpList = m_model.GetAllOPCSampelGrp(sortCol, sortingOrder);
            if (m_loadFromDB || reload)
            {
                m_DPGroupTrendList = m_model.GetAllOPCSampelGrp(sortCol, sortingOrder);
                m_loadFromDB = false;
            }
           return m_DPGroupTrendList;
        }

        /// <summary>
        /// Retrieves the total count of the Sample Group in the Database
        /// </summary>
        public void RetriveTotalCount()
        {
            if (m_loadFromDB)
            {
                m_view.TotalNumberGrp = m_model.GetOPCGrpCount();
            }
            else
            {
                m_view.TotalNumberGrp = m_DPGroupTrendList.Count;
            }
        }
    
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void AddLoggerInterval_Paint_1(object sender, PaintEventArgs e)
        {
            Graphics obj_Graph = e.Graphics;

            LinearGradientBrush b = new LinearGradientBrush(new System.Drawing.Rectangle(0, 0, m_view.Width, m_view.Height), Color.LightBlue, Color.BlanchedAlmond, LinearGradientMode.Vertical);

            obj_Graph.FillRectangle(b, new System.Drawing.Rectangle(0, 0, m_view.Width, m_view.Height));
        }

        /// <summary>
        /// 
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
        /// Checks the specified Sample Group name is already exists or not.
        /// </summary>
        /// <param name="OPCSampleGrpName">Sample Group to be searched</param>
        /// <returns>true - exist, false - not exists</returns>
        public bool IsOPCSampleGrpExist(string OPCSampleGrpName, double LocationKey)
        {
            EtyDataLogDPGroupTrend etyGrp = m_DPGroupTrendList.Find(delegate(EtyDataLogDPGroupTrend obj) 
            {
                bool bFound = false;
                if ((obj.SampleGrpName.ToUpper().Equals(OPCSampleGrpName.ToUpper())) &&
                    (obj.LocationKey.Equals(LocationKey))
                    )
                {
                    bFound = true;
                }
                return bFound;
            });
            if (etyGrp == null)
            {
                return false;
            }

            return true;
        }

        /// <summary>
        /// Get the Sample Group details based on ID.
        /// </summary>
        /// <param name="OPCSampleGroupId">Sample Group ID</param>
        /// <returns>Sample Group Entity</returns>
        public EtyDataLogDPGroupTrend GetOPCSampleGrp(string OPCSampleGroupId)
        {
            double GrpId = Convert.ToDouble(OPCSampleGroupId);
            return m_DPGroupTrendList.Find(delegate(EtyDataLogDPGroupTrend obj) { return obj.SampleGrpID == GrpId; });
        }


    }
}
