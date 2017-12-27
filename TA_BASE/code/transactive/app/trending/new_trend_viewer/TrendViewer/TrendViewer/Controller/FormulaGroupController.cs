using System;
using System.Collections.Generic;
using System.Text;
using TrendViewer.Model;
using STEE.ISCS.MVC;
using TrendViewer.View;
using DAO.Trending.Common;
using STEE.ISCS.MulLanguage;
using System.Windows.Forms;

namespace TrendViewer.Controller
{
   public class FormulaGroupController : IController
    {
       private const string CLASS_NAME = "FormulaGroupController";
        private List<string> m_formulaCfgNameList = null;
        private FormulaGroupModel m_Model;
        private TrendViewer.View.FormulaGroup m_View;

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
            m_Model = (FormulaGroupModel)model;
            m_View = (TrendViewer.View.FormulaGroup)view;
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file

        }
        public void InitFormulaGrp()
        {
            string Function_Name = "InitFormulaGrp";
            try
            {
                if (null == m_formulaCfgNameList)
                {
                    m_formulaCfgNameList = m_Model.GetAllFormulaGrp();
                }
            }
            catch (Exception ex)
            {
                TrendViewerHelper.HandleEx(ex);
            }

        }
        public List<string> GetFormulaGrpNames()
        {
            return m_formulaCfgNameList;
        }

        public void LoadFormulaByGrpName(object sender, EventArgs e)  //todo: should we save the selected name in this controller?
        {
            string grpName = m_View.GetConfigName();

            if (grpName == "")
            {
                m_View.ShowErrMsgNoCfgName();
                return;
            }

            NotifyManager.GetInstance().Send(DataNotificaitonConst.SelectFormulaGroupToLoad, m_View.ViewID,grpName);
            m_View.DestroyView();
        }

        public void SaveFormulaToGrpName(object sender, EventArgs e)
        {
            string grpName = m_View.GetConfigName();

            if (grpName == "")
            {
                m_View.ShowErrMsgNoCfgName();
                return;
            }
            NotifyManager.GetInstance().Send(DataNotificaitonConst.SaveFormulaToGroup, m_View.ViewID,grpName);
            m_View.DestroyView();
        }
    }
}
