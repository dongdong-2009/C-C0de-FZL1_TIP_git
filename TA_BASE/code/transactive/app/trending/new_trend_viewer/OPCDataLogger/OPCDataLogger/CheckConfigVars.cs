using System;
using System.Collections.Generic;
using System.Text;

using DAO.Trending;

namespace OPCDataLogger
{
    /// <summary>
    /// This class acts as model for Config_VAR DAO.
    /// </summary>
    public class CheckConfigVars
    {
        private const string VARIBALE_NAME = "OPC_DT_SAMPLE_FLAG";

        /// <summary>
        /// Updates the OPC_DT_SAMPLE_FLAG variable value to "N".
        /// </summary>
        public void UpdateToDisableOPCDTSmplFlag()
        {
            ConfigVarsDAO.GetInstance().UpdateValue(VARIBALE_NAME, "N");
        }

        /// <summary>
        /// Returns the value of OPC_DT_SAMPLE_FLAG variable name in database.
        /// </summary>
        /// <returns>value</returns>
        public string GetOPCDTSmplFlagValue()
        {
            string result = ConfigVarsDAO.GetInstance().GetVarValue(VARIBALE_NAME);
            if (result.Equals("null"))
                result = "N";
            return result;
        }
    }
}
