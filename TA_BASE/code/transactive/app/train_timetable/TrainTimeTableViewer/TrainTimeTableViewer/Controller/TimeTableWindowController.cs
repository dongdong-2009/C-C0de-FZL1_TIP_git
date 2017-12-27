using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrainTimeTableViewer.View;
using TrainTimeTableViewer.Model;
using TrainTimeTable;
using STEE.ISCS.MVC;
using NET_ISCS_Bridge;

namespace TrainTimeTableViewer.Controller
{
    class TimeTableWindowController : IController
    {
        private const string CLASS_NAME = "TimeTableWindowController";

        private TimeTableWindow  m_View;
        private TimeTableWindowModel m_Model;

        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (TrainTimeTableViewer.Model.TimeTableWindowModel)model;
            m_View = (TrainTimeTableViewer.View.TimeTableWindow)view;

        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            return;
        }

        public void ShowTimeTable(int TrainServiceId, bool Planned, DateTime Date)
        {
            string FUNCTION_NAME = "ShowTimeTable";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function Entered.");

            m_View.Initialise(Convert.ToString(TrainServiceId),Planned);
            List<TrainTimeTableData> TimeTableList = m_Model.GetTrainTimeTableData(Planned, TrainServiceId, Date);
            m_View.AddTrainTimings(TimeTableList);
           
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), 
                EDebugLevelManaged.DebugInfo, "Function Exited.");
           

        }

    }
}
