using System;
using System.Collections.Generic;
using System.Text;
using DAO.Trending;
using STEE.ISCS.MVC;

namespace TrendViewer.Model
{
    class FormulaGroupModel:IModel
    {
        public List<string> GetAllFormulaGrp()
        {
            FormulaerDAO formulaDAO = new FormulaerDAO(); ;
            return formulaDAO.GetAllformulaGrpNames();
        }
    }
}
