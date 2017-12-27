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
using DAO.Trending.Common;

namespace TrendViewer.Controller
{
    public class FormulaListController : IController
    {
        private const string CLASS_NAME = "FormulaListController";
        private DataTable m_FormulaTable = null;
        private FormulaListModel m_Model;
        private TrendViewer.View.FormulaList m_View;
        private string m_grpName = "";

        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (FormulaListModel)model;
            m_View = (TrendViewer.View.FormulaList)view;
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file

            if (notifyObj.Name != m_View.ViewID) return;

            //will only handle the notification when the ViewID is matching.
            if (notifyObj.Type == DataNotificaitonConst.SelectFormulaGroupToLoad)
            {
                SetFormulaTable(m_Model.GetFormulaListByGrp((string)(notifyObj.Body)));
                m_grpName = (string)(notifyObj.Body);
                m_View.PopulateDataPointListDataGridView(1);
            }
            if (notifyObj.Type == DataNotificaitonConst.SaveFormulaToGroup)
            {
                m_grpName = (string)(notifyObj.Body);
                SaveFormulaListToGroup((string)(notifyObj.Body));
            }
            if (notifyObj.Type == DataNotificaitonConst.AddFormula)
            {
                AddFormulaToList((EtyFormula)(notifyObj.Body));

            }
            if (notifyObj.Type == DataNotificaitonConst.ModifyFormula)
            {
                KeyValuePair<string, EtyFormula> pair = (KeyValuePair<string, EtyFormula>)(notifyObj.Body);
                ModifyFormula(pair.Key, pair.Value);
            }

        }

        public void ModifyFormula(string FormulaName, EtyFormula formula)
        {
            foreach (DataRow formulaRow in m_FormulaTable.Rows)  //will only have 1 record
            {
                if (FormulaName == formulaRow["DATA_PT_LBL_NAME"].ToString())
                {
                    formulaRow["DATA_PT_LBL_NAME"] = formula.DPLblName;
                    formulaRow["DATA_PT_TYPE"] = formula.DPType;
                    formulaRow["DATA_PT_COLOR"] = formula.DPColor;
                    formulaRow["DATA_PT_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(formula.DPEnabled);
                    formulaRow["DATA_PT_LBL_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(formula.DPLblEnabled);
                    formulaRow["DATA_PT_EQUATION"]  = formula.DPEquation;
                }
            }

            m_View.PopulateDataPointListDataGridView(1);
        }

        public void AddFormulaToList(EtyFormula formula)
        {
            DataRow formulaRow = m_FormulaTable.NewRow();
            formulaRow["DATA_PT_LBL_NAME"] = formula.DPLblName;
            formulaRow["DATA_PT_TYPE"] = TrendViewerHelper.convertLineTypeToDispLan(formula.DPType);
            formulaRow["DATA_PT_COLOR"] = formula.DPColor;
            formulaRow["DATA_PT_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(formula.DPEnabled);
            formulaRow["DATA_PT_LBL_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(formula.DPLblEnabled);
            formulaRow["DATA_PT_EQUATION"] = formula.DPEquation;
            m_FormulaTable.Rows.Add(formulaRow);
            m_View.PopulateDataPointListDataGridView(1);

        }
        public void SetFormulaEtyByRow(ref EtyFormula formula, DataRow formulaRow)
        {
            formula.DPLblName = formulaRow["DATA_PT_LBL_NAME"].ToString();
            formula.DPType = TrendViewerHelper.convertLineTypeToEnum(formulaRow["DATA_PT_TYPE"].ToString());
            formula.DPColor = formulaRow["DATA_PT_COLOR"].ToString();
            formula.DPEnabled = TrendViewerHelper.ChangeStrToBool(formulaRow["DATA_PT_ENABLED"].ToString());
            formula.DPLblEnabled = TrendViewerHelper.ChangeStrToBool(formulaRow["DATA_PT_LBL_ENABLED"].ToString());
            formula.DPEquation = formulaRow["DATA_PT_EQUATION"].ToString();

        }
        public void SaveFormulaListToGroup(string grpName)
        {
            string Function_Name = "SaveFormulaListToGroup";
            List<EtyFormula> formulaList = new List<EtyFormula>();
            foreach (DataRow formulaRow in m_FormulaTable.Rows)
            {
                EtyFormula formula = new EtyFormula();
                SetFormulaEtyByRow(ref formula, formulaRow);

                formulaList.Add(formula);

            }

            try
            {
                m_Model.SaveFormulaListToGroup(formulaList, grpName);
            }
            catch (Exception ex)
            {
                TrendViewerHelper.HandleEx(ex);
            }
        }
        public void initFormulaTable()
        {
            if (m_FormulaTable != null) return;

            m_FormulaTable = new DataTable("TRENDVIEWER_FORMULA_CONFIG");

            m_FormulaTable.Columns.Add("DATA_PT_EQUATION", System.Type.GetType("System.String"));
            m_FormulaTable.Columns.Add("DATA_PT_TYPE", System.Type.GetType("System.String"));
            m_FormulaTable.Columns.Add("DATA_PT_COLOR", System.Type.GetType("System.String"));
            m_FormulaTable.Columns.Add("DATA_PT_ENABLED", System.Type.GetType("System.String"));
            m_FormulaTable.Columns.Add("DATA_PT_LBL_ENABLED", System.Type.GetType("System.String"));
            m_FormulaTable.Columns.Add("DATA_PT_LBL_NAME", System.Type.GetType("System.String"));
        }

        public DataTable getFormulaTable()
        {
            return m_FormulaTable;
        }

        public void LoadFormula(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.LoadConfigFormula, m_View.ViewID);

            FormulaGroupController ctl = (FormulaGroupController)(view.getController());

            ctl.SetFormType(FormType.Load);

            ctl.InitFormulaGrp();
            ((FormulaGroup)(view)).SetFormType(FormType.Load);
            ((FormulaGroup)view).FillConfigNameBox(ctl.GetFormulaGrpNames());
            ((FormulaGroup)view).SetCurrentGrpName(m_grpName);
            view.ShowView(true);
        }
        public void SetGrpName(string grpName)
        {
            m_grpName = grpName;
        }
        public void SetFormulaTable(List<EtyFormula> formulaList)
        {
            m_FormulaTable.Rows.Clear();
            DataRow newDataPointFormulaRow = null;

            for (int i = 0; i < formulaList.Count; i++)
            {
                EtyFormula formula = formulaList[i];
                newDataPointFormulaRow = m_FormulaTable.NewRow();

                newDataPointFormulaRow["DATA_PT_LBL_NAME"] = formula.DPLblName;
                newDataPointFormulaRow["DATA_PT_TYPE"] = formula.DPType;
                newDataPointFormulaRow["DATA_PT_COLOR"] = formula.DPColor;
                newDataPointFormulaRow["DATA_PT_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(formula.DPEnabled);
                newDataPointFormulaRow["DATA_PT_LBL_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(formula.DPLblEnabled);
                newDataPointFormulaRow["DATA_PT_EQUATION"] = formula.DPEquation;

                m_FormulaTable.Rows.Add(newDataPointFormulaRow);
            }

        }

        public void DeleteByFormulaLblName(string formulaName)
        {

            foreach (DataRow dataRow in m_FormulaTable.Select("DATA_PT_LBL_NAME = '" + formulaName + "'"))
            {
                dataRow.Delete();
            }
            m_FormulaTable.AcceptChanges();
            m_View.PopulateDataPointListDataGridView(1); 

        }

        // this func is changed. 
        public void EditFormula(string formulaName)
        {
            List<string> otherFormulaNameList = new List<string>();

            foreach (DataRow Formula in m_FormulaTable.Select("DATA_PT_LBL_NAME <> '" + formulaName + "'"))
            {
                otherFormulaNameList.Add(Formula["DATA_PT_LBL_NAME"].ToString());
            }

            foreach (DataRow dataRow in m_FormulaTable.Select("DATA_PT_LBL_NAME = '" + formulaName + "'")) //will only be one row, because we don't allow duplicate name
            {

                IView view = ViewManager.GetInstance().GetView(TrendViewConst.EditFormulaData, m_View.ViewID);
                ((FormulaData)view).SetFormType(FormType.Edit);
                FormulaDataController ctl = (FormulaDataController)(view.getController());

                ctl.SetFormType(FormType.Edit);

                ctl.InitFormulaData(dataRow, otherFormulaNameList);
                
                view.ShowView(true);

            }
        }

        public void SaveFormula(object sender, EventArgs e)
        {
            string Function_Name = "SaveFormula";
            try
            {
                IView view = ViewManager.GetInstance().GetView(TrendViewConst.LoadConfigFormula, m_View.ViewID);

                FormulaGroupController ctl = (FormulaGroupController)(view.getController());

                ctl.SetFormType(FormType.Save);

                ctl.InitFormulaGrp();
                ((FormulaGroup)(view)).SetFormType(FormType.Save);
                ((FormulaGroup)view).FillConfigNameBox(ctl.GetFormulaGrpNames());
                ((FormulaGroup)view).SetCurrentGrpName(m_grpName);
                view.ShowView(true);


            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }
        public void AddFormula(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.AddFormulaData, m_View.ViewID);

            FormulaDataController ctl = (FormulaDataController)(view.getController());

            ctl.SetFormType(FormType.Add);
            ((FormulaData)(view)).SetFormType(FormType.Add);

            List<string> FormulaNameList = new List<string>();

            foreach (DataRow dataRow in m_FormulaTable.Rows)
            {
                FormulaNameList.Add(dataRow["DATA_PT_LBL_NAME"].ToString());
            }

            ctl.InitFormulaData(FormulaNameList);
            
            view.ShowView(true);

        }

        public void FormClose(object sender, EventArgs e)
        {
            KeyValuePair<string, List<EtyFormula>> pair = new KeyValuePair<string, List<EtyFormula>>(m_grpName, ConvertDataTableToList());
            NotifyManager.GetInstance().Send(DataNotificaitonConst.FormulaListClosed, m_View.ViewID, pair);
        }

        public List<EtyFormula> ConvertDataTableToList()
        {
            List<EtyFormula> formulaList = new List<EtyFormula>();
            foreach (DataRow formulaRow in m_FormulaTable.Rows)
            {
                EtyFormula formula = new EtyFormula();
                SetFormulaEtyByRow(ref formula, formulaRow);
                formulaList.Add(formula);
            }
            return formulaList;
        }

    }
}
