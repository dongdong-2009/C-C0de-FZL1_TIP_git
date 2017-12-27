using System;
using System.Collections.Generic;
using System.Text;
using Entity.Trending;
using DAO.Trending;
using STEE.ISCS.MVC;

namespace TrendViewer.Model
{
    public class FormulaListModel:IModel
    {
        public List<EtyFormula> GetFormulaListByGrp(string grpName)
        {
            FormulaerDAO formulaDAO = new FormulaerDAO();
            return formulaDAO.GetFormulasByCfgName(grpName);
        }

        public void SaveFormulaListToGroup(List<EtyFormula> formulaList, string grpName)
        {
            FormulaerDAO formulaDAO = new FormulaerDAO();

            //do it in a transaction:
            formulaDAO.SaveFormulaListToGrp(formulaList, grpName);

            //formulaDAO.DeleteAllFormulaInGrp(grpName);
            //formulaDAO.InsertFormulaListToGrp(formulaList, grpName);

        }
    }
}
