using System;
using System.Collections.Generic;
using System.Text;
using TrendViewer.Model;
using STEE.ISCS.MVC;
using TrendViewer.View;
using System.Windows.Forms;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.Controller
{
    public class MarkerGroupController:IController
    {
        private const string CLASS_NAME = "MarkerGroupController";
        private List<string> m_markerCfgNameList = null;
        private MarkerGroupModel m_Model;
        private TrendViewer.View.MarkerGroup m_View;

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
            m_Model = (MarkerGroupModel)model;
            m_View = (TrendViewer.View.MarkerGroup)view;
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file

        }
        public void InitMarkGrp()
        {
            string Function_Name = "InitMarkGrp";
            try
            {
                if (null == m_markerCfgNameList)
                {
                    m_markerCfgNameList = m_Model.GetAllMarkGrp();
                }
            }
            catch (Exception ex)
            {
                TrendViewerHelper.HandleEx(ex);
            }


            
        }
        public List<string> GetMarkerGrpNames()
        {
            return m_markerCfgNameList;
        }

        public void LoadMarkerByGrpName(object sender, EventArgs e)  //todo: should we save the selected name in this controller?
        {
            string grpName = m_View.GetConfigName();
            if (grpName == "")
            {
                MessageBoxDialog.Show(
                   StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_SelectConfigForLoad, LanguageHelper.TrendViewer_Msg_SelectConfigForLoad_EN),
                   StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                   MessageBoxButtons.OK, MessageBoxIcon.Error);
                
                return;
            }

            m_View.DestroyView();
            NotifyManager.GetInstance().Send(DataNotificaitonConst.SelectMarkGroupToLoad, m_View.ViewID, grpName );
        }

        public void SaveMarkerToGrpName(object sender, EventArgs e)
        {
            string grpName = m_View.GetConfigName();
            if (grpName == "")
            {
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoConfigToSave, LanguageHelper.TrendViewer_Msg_NoConfigToSave_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            m_View.DestroyView();
            NotifyManager.GetInstance().Send(DataNotificaitonConst.SaveMarkerToGroup, m_View.ViewID,grpName);
        }
    }
}
