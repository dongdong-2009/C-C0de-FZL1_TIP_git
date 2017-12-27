using System;
using System.Collections.Generic;
using System.Text;
using TrendViewer.Model;
using STEE.ISCS.MVC;
using TrendViewer.View;
using System.Windows.Forms;
using STEE.ISCS.MulLanguage;
using DAO.Trending.Common;

namespace TrendViewer.Controller
{
   public class HistDataPointGroupController : IController
    {
       private const string CLASS_NAME = "HistDataPointGroupController";
        private List<string> m_HistDataPointCfgNameList = null;
        private HistDataPointGroupModel m_Model;
        private TrendViewer.View.HistDataPointGroup m_View;

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
            m_Model = (HistDataPointGroupModel)model;
            m_View = (TrendViewer.View.HistDataPointGroup)view;
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file

        }
        public void InitHistDPGrp()
        {
            string Function_Name = "InitHistDPGrp";
            try
            {
                if (null == m_HistDataPointCfgNameList)
                {
                    m_HistDataPointCfgNameList = m_Model.GetAllHistDPGrp();
                }
            }
            catch (Exception ex)
            {
                TrendViewerHelper.HandleEx(ex);
            }



        }
        public List<string> GetHistDataPointGrpNames()
        {
            return m_HistDataPointCfgNameList;
        }

        public void LoadHistDataPointByGrpName(object sender, EventArgs e)  //todo: should we save the selected name in this controller?
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
            NotifyManager.GetInstance().Send(DataNotificaitonConst.SelectHistDPGroupToLoad, m_View.ViewID,grpName);
        }

        public void SaveHistDataPointToGrpName(object sender, EventArgs e)
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
            NotifyManager.GetInstance().Send(DataNotificaitonConst.SaveHistDataPointToGroup, m_View.ViewID,grpName);
        }
    }
}
