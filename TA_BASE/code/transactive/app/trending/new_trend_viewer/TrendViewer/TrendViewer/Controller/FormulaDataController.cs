using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using Entity.Trending;
using TrendViewer.Model;
using TrendViewer.View;
using System.Data;
using STEE.ISCS.MulLanguage;
using System.Windows.Forms;

namespace TrendViewer.Controller
{
   public class FormulaDataController : IController
    {
        private EtyFormula m_formula = new EtyFormula();
        private List<string> m_otherFormulaNames = new List<string>();
        private FormulaDataModel m_Model;
        private TrendViewer.View.FormulaData m_View;

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
            m_Model = (FormulaDataModel)model;
            m_View = (TrendViewer.View.FormulaData)view;
        }

        public EtyFormula GetFormula()
        {
            return m_formula;
        }
        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file
        }
        public void InitFormulaData(List<string> formulaNameList) //for "Add"
        {
            if (m_formType == FormType.Add)
            {
                m_otherFormulaNames = formulaNameList;
            }
            else  // for edit
            {
                
            }
        }

        public void InitFormulaData(DataRow formulaRow, List<string> formulaNameList)  // for "Edit"
        {
            m_otherFormulaNames = formulaNameList;

            m_formula.DPLblName = formulaRow["DATA_PT_LBL_NAME"].ToString();
            m_formula.DPType = TrendViewerHelper.convertLineTypeToEnum( formulaRow["DATA_PT_TYPE"].ToString());
            m_formula.DPColor = formulaRow["DATA_PT_COLOR"].ToString();
            m_formula.DPEnabled = TrendViewerHelper.ChangeStrToBool(formulaRow["DATA_PT_ENABLED"].ToString());
            m_formula.DPLblEnabled = TrendViewerHelper.ChangeStrToBool(formulaRow["DATA_PT_LBL_ENABLED"].ToString());
            m_formula.DPEquation = formulaRow["DATA_PT_EQUATION"].ToString();

        }
        public bool FormulaNameValid(string name)
        {
            //check the new added formula's name, make sure it's not the same as any of the list
            foreach (string otherName in m_otherFormulaNames)
            {
                if (name == otherName)
                {
                    return false;
                }
            }
            return true;
        }

        public void AddFormulaData(object sender, EventArgs e)
        {
            string formulaName = m_View.GetFormulaName();

            if (formulaName == "")
            {
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_EmptyName, LanguageHelper.TrendViewer_Msg_EmptyName_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);

                return;
            }

            if (!FormulaNameValid(formulaName))  //means has duplicate names
            {
       
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DuplicateName, LanguageHelper.TrendViewer_Msg_DuplicateName_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);

                return;
            }

            NotifyManager.GetInstance().Send(DataNotificaitonConst.AddFormula, m_View.ViewID, m_View.GetNewFormula());
            m_View.DestroyView();
        }

        public void ModifyFormula(object sender, EventArgs e)
        {
            string formulaName = m_View.GetFormulaName();

            if (formulaName == "")
            {
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_EmptyName, LanguageHelper.TrendViewer_Msg_EmptyName),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (!FormulaNameValid(formulaName))  //means has duplicate names
            {
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DuplicateName, LanguageHelper.TrendViewer_Msg_DuplicateName_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                    MessageBoxButtons.OK, MessageBoxIcon.Error); 
                return;
            }
            KeyValuePair<string, EtyFormula> pair = new KeyValuePair<string, EtyFormula>(m_formula.DPLblName, m_View.GetNewFormula());
            NotifyManager.GetInstance().Send(DataNotificaitonConst.ModifyFormula,m_View.ViewID,  pair);
            m_View.DestroyView();

        }
    }
}
