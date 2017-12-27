using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using STEE.ISCS.MVC;
using TrainTimeTable;
using DAO.TimeTable;
using NET_ISCS_Bridge;

namespace TrainTimeTableViewer.Model
{
    class TimeTableWindowModel : IModel
    {
        private const string CLASS_NAME = "TimeTableWindowModel";

        public List<TrainTimeTableData> GetTrainTimeTableData(bool plannedData, int TrainServiceId, DateTime Date)
        {
            string FUNCTION_NAME = "GetTrainTimeTableData";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function Entered.");

            TrainTimeTableDAO timetableDAO = TrainTimeTableDAO.GetInstance();

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
               EDebugLevelManaged.DebugInfo, "Getting Data For TrainServiceId" + TrainServiceId.ToString() + ", Date " + Date.ToShortDateString());

            return timetableDAO.GetTrainTimeTableData(plannedData, TrainServiceId, Date);

         
        }

    }
}
