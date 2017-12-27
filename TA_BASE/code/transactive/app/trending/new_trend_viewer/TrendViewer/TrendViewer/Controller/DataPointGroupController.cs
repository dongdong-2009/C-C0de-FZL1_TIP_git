using System;
using System.Collections.Generic;
using System.Text;
using TrendViewer.Model;
using STEE.ISCS.MVC;
using TrendViewer.View;
using System.Windows.Forms;
using STEE.ISCS.Log;
using STEE.ISCS.MulLanguage;
using DAO.Trending.Common;

namespace TrendViewer.Controller
{
   public class DataPointGroupController : IController
    {
        private const string CLASS_NAME = "DataPointGroupController";
        private List<string> m_DataPointCfgNameList = null;
        private DataPointGroupModel m_Model;
        private TrendViewer.View.DataPointGroup m_View;

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
            m_Model = (DataPointGroupModel)model;
            m_View = (TrendViewer.View.DataPointGroup)view;
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file

        }
        public void InitDPGrp()
        {
            string Function_Name = "InitDPGrp";
            try
            {
                if (null == m_DataPointCfgNameList)
                {
                    m_DataPointCfgNameList = m_Model.GetAllDPGrp();
                }
            }
            catch(Exception ex)
            {
                TrendViewerHelper.HandleEx(ex);
            }


        }
        public List<string> GetDataPointGrpNames()
        {
            return m_DataPointCfgNameList;
        }

        public void LoadDataPointByGrpName(object sender, EventArgs e)  //todo: should we save the selected name in this controller?
        {
            string grpName = m_View.GetConfigName();
            if (grpName == "")
            {
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoConfigToLoad, LanguageHelper.TrendViewer_Msg_NoConfigToLoad_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN), 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);

                return;
            }
            m_View.DestroyView();
            NotifyManager.GetInstance().Send(DataNotificaitonConst.SelectDPGroupToLoad, m_View.ViewID,grpName);
        }

        public void SaveDataPointToGrpName(object sender, EventArgs e)
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
            NotifyManager.GetInstance().Send(DataNotificaitonConst.SaveDataPointToGroup, m_View.ViewID,grpName);
        }
    }
}
