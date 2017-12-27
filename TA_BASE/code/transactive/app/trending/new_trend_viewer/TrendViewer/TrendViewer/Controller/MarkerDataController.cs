using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using Entity.Trending;
using TrendViewer.Model;
using TrendViewer.View;
using System.Data;
using System.Windows.Forms;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.Controller
{
    public class MarkerDataController:IController
    {
        private EtyMarker m_marker = new EtyMarker();
        private List<string> m_otherMarkerNames = new List<string>();
        private MarkerDataModel m_Model;
        private TrendViewer.View.MarkerData m_View;

        FormType m_formType = FormType.Load;


        public void SetFormType(FormType type)
        {
            m_formType = type;
        }
        public FormType GetFormType()
        {
            return m_formType;
        }

        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (MarkerDataModel)model;
            m_View = (TrendViewer.View.MarkerData)view;
        }

        public EtyMarker GetMarker()
        {
            return m_marker;
        }
        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file
        }
        public void InitMarkerData( List<string> markerNameList) //for "Add"
        {
            if(m_formType == FormType.Add)
            {
                m_otherMarkerNames = markerNameList;
            }
            else  // for edit
            {
                
            }
        }

        public void InitMarkerData(DataRow markerRow, List<string> markerNameList)  // for "Edit"
        {
            m_otherMarkerNames = markerNameList;

            m_marker.MarkerName = markerRow["MARKER_NAME"].ToString();
            if (TrendViewerHelper.isNumeric(markerRow["MARKER_BCOLOR"].ToString(), System.Globalization.NumberStyles.Number))
            {
                m_marker.MarkerBColor = markerRow["MARKER_BCOLOR"].ToString();
            }
            if (TrendViewerHelper.isNumeric(markerRow["MARKER_FCOLOR"].ToString(), System.Globalization.NumberStyles.Number))
            {
                m_marker.MarkerFColor = markerRow["MARKER_FCOLOR"].ToString();
            }
            m_marker.MarkerEnabled = TrendViewerHelper.ChangeStrToBool(markerRow["MARKER_ENABLED"].ToString());

            m_marker.MarkerWidth = Convert.ToDouble(markerRow["MARKER_WIDTH"].ToString());
            m_marker.MarkerValue = Convert.ToDouble(markerRow["MARKER_VALUE"].ToString());
        }
        public bool MarkerNameValid(string name)
        {
            //check the new added marker's name, make sure it's not the same as any of the list
            foreach (string otherName in m_otherMarkerNames)
            {
                if (name == otherName)
                {
                    return false;
                }
            }
            return true;
        }

        public void AddMarkerData(object sender, EventArgs e)
        {
            string markerName = m_View.GetMarkerName();
            
            if ( markerName == "") return;


            if (!MarkerNameValid(markerName))  //means has duplicate names
            {
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DuplicateName, LanguageHelper.TrendViewer_Msg_DuplicateName_EN),
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                
                return;
            }

            NotifyManager.GetInstance().Send(DataNotificaitonConst.AddMarker, m_View.ViewID, m_View.GetNewMarker());

            m_View.DestroyView();
        }

        public void ModifyMarker(object sender, EventArgs e)
        {
            string markerName = m_View.GetMarkerName();

            if (markerName == "") return;

            if (!MarkerNameValid(markerName))  //means has duplicate names
            {
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DuplicateName, LanguageHelper.TrendViewer_Msg_DuplicateName_EN), 
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            KeyValuePair<string, EtyMarker> pair = new KeyValuePair<string, EtyMarker>(m_marker.MarkerName, m_View.GetNewMarker());
            NotifyManager.GetInstance().Send(DataNotificaitonConst.ModifyMarker, m_View.ViewID, pair);
            m_View.DestroyView();
        }
    }
}
