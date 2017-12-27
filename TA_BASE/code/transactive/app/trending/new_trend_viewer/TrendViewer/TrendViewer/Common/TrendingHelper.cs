using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.Log;
using STEE.ISCS.MulLanguage;
using Entity.Trending;
using System.Windows.Forms;

namespace TrendViewer.View
{
   public class TrendViewerHelper
    {
        private const string CLASS_NAME = "TrendViewerHelper";
        const string FORMULA_PRE = "F-";  //the prefix for formula type series name
        const string HIST_PRE = "H-";    // the prefix for historical type series name

        // return series name by the trending chart type(formula, historical, realtime) and the datapoint name.
        // it will be the inputname added by some prefix.
        public static string getDisplayName(ETrendingChartType trendingCType, string inputName)
        {
            string dispNmae;
            if (trendingCType == ETrendingChartType.Formula)   // for formula type
            {
                dispNmae = FORMULA_PRE + inputName;
                return dispNmae;
            }
            else if (trendingCType == ETrendingChartType.Historical) // for historical type
            {
                dispNmae = HIST_PRE + inputName;
                return dispNmae;
            }else                         // for real time type, currently we don't add any prefix to the name. 
                                          // This is because we suppose the datapoint name will never be started with "H-" or "F-"
            {
                dispNmae = inputName;   
                return dispNmae;
            }

        }

        // return the trending chart type (formula, historical, realtime) by the series name
        public static ETrendingChartType getETrendingChartType(string displayName)
        {
            if(displayName.StartsWith(FORMULA_PRE))
            {
                return ETrendingChartType.Formula;
            }
            else if (displayName.StartsWith(HIST_PRE))
            {
                return ETrendingChartType.Historical;
            }else
            {
                return ETrendingChartType.RealTime;
            }
        }

       //handle exceptions ( DatabaseConnectionException )
       public static void HandleEx(Exception ex)
       {
           string Function_Name = "HandleEx";

           STEE.ISCS.Log.LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());

           if (ex is DAO.Trending.Common.DatabaseConnectionException) //for DatabaseConnectionException
           {
               MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoDB2, LanguageHelper.TrendViewer_Msg_NoDB2_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
           }
           else //for other exceptions:
           {
               MessageBoxDialog.Show(
                  ex.ToString(),
                   StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                  MessageBoxButtons.OK, MessageBoxIcon.Error);
           }
       }

        /// <summary>
        /// convert LineType from enum to english string
        /// </summary>
        /// <param name="linetype">LineType enum</param>
        /// <returns></returns>
        public static string ConvertLineTypeFromEnumToEn(LineType linetype)
        {
            if(linetype == LineType.Line)
            {
                return LanguageHelper.TrendViewer_LintTypeLine_EN;
            }
            if(linetype == LineType.Spline)
            {
                return LanguageHelper.TrendViewer_LintTypeSpline_EN;
            }
            return LanguageHelper.TrendViewer_LintTypeLine_EN;
        }

        /// <summary>
        /// convert LineType from enum to string in current language.
        /// </summary>
        /// <param name="lineType">LineType enum</param>
        /// <returns></returns>
        public static string convertLineTypeToDispLan(LineType lineType)
        {
            if (lineType == LineType.Line )
            {
                return StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeLine, LanguageHelper.TrendViewer_LintTypeLine_EN);
            }
            else if (lineType == LineType.Spline )
            {
                return StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeSpline, LanguageHelper.TrendViewer_LintTypeSpline_EN);
            }

            return StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeLine, LanguageHelper.TrendViewer_LintTypeLine_EN);
        }

        /// <summary>
        /// convert LineType from current display string to enum
        /// </summary>
        /// <param name="lineType">current display string for line type(in current language)</param>
        /// <returns></returns>
        public static LineType convertLineTypeToEnum(string lineType)
        {
            if (lineType.Equals(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeLine, LanguageHelper.TrendViewer_LintTypeLine_EN)))
            {
                return LineType.Line;
            }
            else if (lineType.Equals(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_LintTypeSpline, LanguageHelper.TrendViewer_LintTypeSpline_EN)))
            {
                return LineType.Spline;
            }
            return LineType.Line;
        }


        public static bool isNumeric(string val, System.Globalization.NumberStyles NumberStyle)
        {
            Double result;
            return Double.TryParse(val, NumberStyle, System.Globalization.CultureInfo.CurrentCulture, out result);
        }


        public static bool ChangeStrToBool(string str)
        {
            if ("Y" == str)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public static string ChangeBoolToStr (bool b)
        {
            if(b== true)
            {
                return "Y";
            }
            else
            {
                return "N";
            }
        }

        public static string CheckValidVariables(string statement)
        {
            string Function_Name = "CheckValidVariables";
            if (statement.Trim() == "") return "";
            if (statement.IndexOf("::") < 0) return "";

            string variableList = "";

            try
            {
                string[] variableArray = statement.Split(new string[] { "::" }, StringSplitOptions.None);

                if (variableArray.Length % 2 == 0) return "";

                for (int i = 0; i < variableArray.Length; i++)
                {
                    if (i % 2 == 1)
                    {
                        if (variableArray[i].Length > 0 && variableArray[i].LastIndexOf('\n') == -1)
                        {
                            if (variableList.IndexOf(variableArray[i]) < 0)
                                variableList = variableList + variableArray[i] + ",";
                        }
                    }
                }

                if (variableList.Length < 1) return "";

                variableList = variableList.Substring(0, variableList.Length - 1);


            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            return variableList;

        }

        public static bool isBoolean(string val)
        {
            bool result;
            return Boolean.TryParse(val, out result);
        }

    }
   public enum ETrendingChartType
    {
        Formula,
        Historical,
        RealTime
    }


    public enum FormType
    {
        Save,
        Load,
        Add,
        Edit
    }


    /// <summary>
    /// NotMixed: Historical mode, 
    /// Mixed: Mixed mode
    /// </summary>
    public enum DrawHistoryMode
    {
        NotMixed,
        Mixed
    }


}
