using System;
using System.Collections.Generic;
using System.Text;

using Entity.Trending;
using STEE.ISCS.Log;
using DAO.Trending.Common;
using DAO.Trending.Helper;

namespace DAO.Trending
{
    /// <summary>
    /// This class is responsible for accessing table TRENDVIEWER_FORMULA_CONFIG
    /// </summary>
    public class FormulaerDAO
    {
        private const string CLASS_NAME = "FormulaerDAO";

        /// <summary>
        /// Get all formula configuration names
        /// </summary>
        /// <returns></returns>
        public List<string> GetAllformulaGrpNames()
        {
            string Function_Name = "GetAllformulaGrpNames";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            List<string> formulaGrpNames = new List<string>();

            string localSQL = " SELECT DISTINCT CONFIG_NAME FROM TRENDVIEWER_FORMULA_CONFIG order by config_name ";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(0))
                        formulaGrpNames.Add(DAOHelper.convert8859P1ToGB2312(drReader.GetString(0).ToString()));
                }
            }
            catch (System.Exception ex)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
            }

            if (drReader != null)
            {
                drReader.Close();
                drReader.Dispose();
                //SimpleDatabase.GetInstance().CloseCurrentSession();
            }


            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return formulaGrpNames;

        }

        public bool SaveFormulaListToGrp(List<EtyFormula> formulaList, string grpName)
        {
            SimpleDatabase.GetInstance().BeginTransaction();
            if (DeleteAllFormulaInGrp(grpName))
            {
                if (InsertFormulaListToGrp(formulaList, grpName))
                {
                    SimpleDatabase.GetInstance().CommitTransaction();
                    return true;
                }
            }

            SimpleDatabase.GetInstance().RollbackTransaction();
            return false;
        }

        /// <summary>
        /// insert a new formula configuration to table TRENDVIEWER_FORMULA_CONFIG
        /// </summary>
        /// <param name="formulaList">the formula list included in the configuration</param>
        /// <param name="grpName">the configuration name</param>
        public bool InsertFormulaListToGrp(List<EtyFormula> formulaList, string grpName)
        {
            string Function_Name = "InsertFormulaListToGrp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool executeOK = false;

            string localSQL = " INSERT INTO TRENDVIEWER_FORMULA_CONFIG(CONFIG_NAME,DATA_PT_EQUATION,DATA_PT_TYPE, " +
                        " DATA_PT_COLOR,DATA_PT_ENABLED,DATA_PT_LBL_ENABLED,DATA_PT_LBL_NAME) " +
                        " VALUES( '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(grpName) + "'";
            foreach (EtyFormula formula in formulaList)
            {
                executeOK = SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL
                            + ",'" + formula.DPEquation.Replace("'", "''") + "'"
                            + ",'" + DAOHelper.ConvertLineTypeToDBString(formula.DPType) + "'"
                            + ",'" + formula.DPColor + "'"
                            + ",'" + DAOHelper.ChangeBoolToStr(formula.DPEnabled) + "'"
                            + ",'" + DAOHelper.ChangeBoolToStr(formula.DPLblEnabled) + "'"
                            + ",'" + DAOHelper.convertEscapeStringAndGB2312To8859P1(formula.DPLblName) + "'"
                            + " ) ");
                if (executeOK == false)
                {
                    break;
                }
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return executeOK;
        }
        /// <summary>
        /// delete formula configuration by the configuration name.
        /// </summary>
        /// <param name="grpName">the formula configuration name</param>
        public bool DeleteAllFormulaInGrp(string grpName)
        {
            string Function_Name = "DeleteAllFormulaInGrp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool executeOK=false;

            string localSQL = " DELETE FROM TRENDVIEWER_FORMULA_CONFIG WHERE CONFIG_NAME = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(grpName) + "'";
            executeOK = SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return executeOK;
        }

        /// <summary>
        /// get all formulas by a configuration name.
        /// </summary>
        /// <param name="configName">formula configuration name</param>
        /// <returns>fomula list</returns>
        public List<EtyFormula> GetFormulasByCfgName(string configName)
        {
            string Function_Name = "GetFormulasByCfgName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            List<EtyFormula> formulaList = new List<EtyFormula>();

            string localSQL = " SELECT DATA_PT_EQUATION,DATA_PT_TYPE,DATA_PT_COLOR,DATA_PT_ENABLED,DATA_PT_LBL_ENABLED,DATA_PT_LBL_NAME " +
                       " FROM TRENDVIEWER_FORMULA_CONFIG WHERE CONFIG_NAME = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(configName) + "'";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);

            EtyFormula newEtyFormula = null;

            try
            {
                while (drReader != null && drReader.Read())
                {
                    newEtyFormula = new EtyFormula();

                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_EQUATION")))
                        newEtyFormula.DPEquation = drReader["DATA_PT_EQUATION"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_TYPE")))
                        newEtyFormula.DPType = DAOHelper.ConvertLineTypeToEnum(drReader["DATA_PT_TYPE"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_COLOR")))
                        newEtyFormula.DPColor = drReader["DATA_PT_COLOR"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_ENABLED")))
                        newEtyFormula.DPEnabled = DAOHelper.ChangeStrToBool(drReader["DATA_PT_ENABLED"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_LBL_ENABLED")))
                        newEtyFormula.DPLblEnabled = DAOHelper.ChangeStrToBool(drReader["DATA_PT_LBL_ENABLED"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_LBL_NAME")))
                        newEtyFormula.DPLblName = DAOHelper.convert8859P1ToGB2312(drReader["DATA_PT_LBL_NAME"].ToString());

                    formulaList.Add(newEtyFormula);
                }
            }
            catch (System.Exception ex)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
            }

            if (drReader != null)
            {
                drReader.Close();
                drReader.Dispose();
                //SimpleDatabase.GetInstance().CloseCurrentSession();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return formulaList;
        }
    }
}
