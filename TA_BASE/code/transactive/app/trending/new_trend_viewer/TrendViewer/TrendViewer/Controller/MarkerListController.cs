using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using TrendViewer.Model;
using System.Windows.Forms;
using System.Data;
using Entity.Trending;
using TrendViewer.View;
using STEE.ISCS.Log;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.Controller
{
   public class MarkerListController:IController
    {
        private const string CLASS_NAME = "MarkerListController";
        private DataTable m_markerTable = null;
        private MarkerListModel m_Model;
        private TrendViewer.View.MarkerList m_View;
        private string m_grpName = "";

        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (MarkerListModel)model;
            m_View = (TrendViewer.View.MarkerList)view;
        } 

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file
            
            if (notifyObj.Name != m_View.ViewID) return;

            //will only handle the notification when the ViewID is matching.

            if(notifyObj.Type==DataNotificaitonConst.SelectMarkGroupToLoad)
            {
                SetMarkerTable(m_Model.GetMarkerListByGrp((string)(notifyObj.Body)));
                m_grpName = (string)(notifyObj.Body);
                m_View.PopulateDataMarkerListDataGridView(1);
            }
            if(notifyObj.Type==DataNotificaitonConst.SaveMarkerToGroup)
            {
                m_grpName = (string)(notifyObj.Body);
                SaveMarkerListToGroup((string)(notifyObj.Body));
            }
            if(notifyObj.Type==DataNotificaitonConst.AddMarker)
            {
                AddMarkerToList((EtyMarker)(notifyObj.Body));

            }
            if (notifyObj.Type == DataNotificaitonConst.ModifyMarker)
            {
                KeyValuePair<string ,EtyMarker> pair = (KeyValuePair<string ,EtyMarker>)(notifyObj.Body);
                ModifyMarker(pair.Key, pair.Value);  
            }
        }

        private void setMarkRowByEty(ref DataRow  markerRow, EtyMarker marker)
        {
            markerRow["MARKER_NAME"] = marker.MarkerName;
            markerRow["MARKER_WIDTH"] = marker.MarkerWidth.ToString();
            markerRow["MARKER_BCOLOR"] = marker.MarkerBColor;
            markerRow["MARKER_VALUE"] = marker.MarkerValue.ToString();
            if (marker.MarkerEnabled == true)
            {
                markerRow["MARKER_ENABLED"] = "Y";
            }
            else
            {
                markerRow["MARKER_ENABLED"] = "N";
            }

            markerRow["MARKER_FCOLOR"] = marker.MarkerFColor;
        }

        public void SetMarkerEtyByRow(ref EtyMarker marker, DataRow markerRow)
        {
            marker.MarkerName = markerRow["MARKER_NAME"].ToString();
            marker.MarkerWidth = System.Convert.ToDouble(markerRow["MARKER_WIDTH"]);
            marker.MarkerBColor = markerRow["MARKER_BCOLOR"].ToString();
            marker.MarkerFColor = markerRow["MARKER_FCOLOR"].ToString();
            marker.MarkerValue = System.Convert.ToDouble(markerRow["MARKER_VALUE"]);
            if ("Y" == markerRow["MARKER_ENABLED"].ToString())
            {
                marker.MarkerEnabled = true;
            }
            else
            {
                marker.MarkerEnabled = false;
            }
        }

        public void ModifyMarker(string markerName, EtyMarker marker)
        {
            foreach (DataRow markerRow in m_markerTable.Rows)  //will only have 1 record
            {
                if (markerName == markerRow["MARKER_NAME"].ToString())
                {
                    DataRow row = markerRow;
                    setMarkRowByEty(ref row, marker);
                }
            }

            m_View.PopulateDataMarkerListDataGridView(1); 
        }

        public void AddMarkerToList(EtyMarker marker)
        {
            DataRow newDataPointMarkerRow = m_markerTable.NewRow();
            setMarkRowByEty(ref newDataPointMarkerRow, marker);

            m_markerTable.Rows.Add(newDataPointMarkerRow);
            m_View.PopulateDataMarkerListDataGridView(1); 
    
        }


        public void SaveMarkerListToGroup(string grpName)
        {
            string Function_Name = "SaveMarkerListToGroup";

            List<EtyMarker> markList = new List<EtyMarker>();
            foreach (DataRow markerRow in m_markerTable.Rows)
            {
                EtyMarker marker = new EtyMarker();
                SetMarkerEtyByRow(ref marker, markerRow);
                markList.Add(marker);
            }

            try
            {
                m_Model.SaveMarkerListToGroup(markList, grpName);
            }
            catch (Exception ex)
            {
                TrendViewerHelper.HandleEx(ex);
            }
        }
        public void initMarkerTable()
        {
            if (m_markerTable != null) return;

            m_markerTable = new DataTable("TRENDVIEWER_MARKER");

            m_markerTable.Columns.Add("CONFIG_NAME", System.Type.GetType("System.String"));
            m_markerTable.Columns.Add("MARKER_NAME", System.Type.GetType("System.String"));
            m_markerTable.Columns.Add("MARKER_WIDTH", System.Type.GetType("System.String"));
            m_markerTable.Columns.Add("MARKER_VALUE", System.Type.GetType("System.String"));
            m_markerTable.Columns.Add("MARKER_ENABLED", System.Type.GetType("System.String"));
            m_markerTable.Columns.Add("MARKER_BCOLOR", System.Type.GetType("System.String"));
            m_markerTable.Columns.Add("MARKER_FCOLOR", System.Type.GetType("System.String"));
        }

        public DataTable getMarkerTable()
        {
            return m_markerTable;
        }

        public void CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string Function_Name = "CellDoubleClick";
            if (e.RowIndex < 0) return;

            string markerName = m_View.GetMarkNameByRowIndex(e.RowIndex);

            try
            {
                if (e.ColumnIndex == 1) 
                {
                    if (MessageBoxDialog.Show(
                                StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DelConfirm, LanguageHelper.TrendViewer_Msg_DelConfirm_EN),
                                StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_SysMsg, LanguageHelper.TrendViewer_Msg_SysMsg_EN),
                                MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                    {
                        DeleteByMarkerName(markerName);
                    }

                }
                else if (e.ColumnIndex == 0)
                {
                    EditMarker(markerName);
                }

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        public void LoadMarker(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.LoadConfigMarker, m_View.ViewID);

            MarkerGroupController ctl = (MarkerGroupController)(view.getController());
            ctl.SetFormType(FormType.Load);
            ctl.InitMarkGrp();

            ((MarkerGroup)(view)).SetFormType(FormType.Load);
            ((MarkerGroup)view).FillConfigNameBox(ctl.GetMarkerGrpNames());
            ((MarkerGroup)view).SetCurrentGrpName(m_grpName);
            view.ShowView(true);
        }
        public void SetGrpName(string grpName)
        {
            m_grpName = grpName;
        }
        public void SetMarkerTable (List<EtyMarker> markerList)
        {
            m_markerTable.Rows.Clear();
            DataRow newDataPointMarkerRow = null;

            for(int i=0;i<markerList.Count; i++)
            {
                newDataPointMarkerRow = m_markerTable.NewRow();
                setMarkRowByEty(ref newDataPointMarkerRow, markerList[i]);
                m_markerTable.Rows.Add(newDataPointMarkerRow);
            }
            
        }

        public void DeleteByMarkerName(string markerName)
        {

            foreach (DataRow dataRow in m_markerTable.Select("MARKER_NAME = '" + markerName + "'"))
            {
                dataRow.Delete();
            }
            m_markerTable.AcceptChanges();
            m_View.PopulateDataMarkerListDataGridView(1);  
    
        }

        // this func is changed. 
        public void EditMarker (string markerName)
        {
            List<string> ohterMarkerNameList = new List<string>();

            foreach (DataRow marker in m_markerTable.Select("MARKER_NAME <> '" + markerName + "'"))
            {
                ohterMarkerNameList.Add(marker["MARKER_NAME"].ToString());
            }

            foreach (DataRow dataRow in m_markerTable.Select("MARKER_NAME = '" + markerName + "'")) //will only be one row, because we don't allow duplicate name
            {

                IView view = ViewManager.GetInstance().GetView(TrendViewConst.EditMarkerData, m_View.ViewID);

                MarkerDataController ctl = (MarkerDataController)(view.getController());
                ((MarkerData)view).SetFormType(FormType.Edit);
                ctl.SetFormType(FormType.Edit);

                ctl.InitMarkerData(dataRow, ohterMarkerNameList);

                view.ShowView(true);

            }
        }

        public void SaveMarker(object sender, EventArgs e)
        {
            string Function_Name = "SaveMarker";
            try
            {
                IView view = ViewManager.GetInstance().GetView(TrendViewConst.SaveConfigMarker, m_View.ViewID);

                MarkerGroupController ctl = (MarkerGroupController)(view.getController());

                ctl.SetFormType(FormType.Save);

                ctl.InitMarkGrp();
                ((MarkerGroup)(view)).SetFormType(FormType.Save);
                ((MarkerGroup)view).FillConfigNameBox(ctl.GetMarkerGrpNames());
                ((MarkerGroup)view).SetCurrentGrpName(m_grpName);
                view.ShowView(true);

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }
        public void AddMarker(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.AddMarkerData, m_View.ViewID);

            MarkerDataController ctl = (MarkerDataController)(view.getController());
            ((MarkerData)view).SetFormType(FormType.Add);
            ctl.SetFormType(FormType.Add);

            List<string> markerNameList = new List<string>();

            foreach (DataRow dataRow in m_markerTable.Rows)
            {
                markerNameList.Add(dataRow["MARKER_NAME"].ToString());
            }

            ctl.InitMarkerData(markerNameList);

            view.ShowView(true);
        }

        public void FormClose(object sender, EventArgs e)
        {
            KeyValuePair<string, List<EtyMarker>> pair = new KeyValuePair<string, List<EtyMarker>>(m_grpName, ConvertDataTableToList());
            NotifyManager.GetInstance().Send(DataNotificaitonConst.MarkListClosed, m_View.ViewID, pair);
        }
        public List<EtyMarker> ConvertDataTableToList()
        {
            List<EtyMarker> markerList = new List<EtyMarker>();
            foreach (DataRow markerRow in m_markerTable.Rows)
            {
                EtyMarker marker = new EtyMarker();
                SetMarkerEtyByRow(ref marker, markerRow);
                markerList.Add(marker);
            }
            return markerList;
        }
    }
}
