using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using STEE.ISCS.MVC;
using NET_ISCS_Bridge;
using TrainTimeTableViewer.View;
using TrainTimeTableViewer.Controller;
using TrainTimeTableViewer.Common;
using DAO.TimeTable;

using System.Threading;



namespace TrainTimeTableViewer
{
    public class TrainTimeTableApp : TransactiveNetApp
    {
        private const string CLASS_NAME = "TrainTimeTableViewer.TrainTimeTableApp";

        private const string EXCLUDE_STATION_ID = "Exclude_StationId";

        public override void CheckCommandLine()
        {

        }

        public override Form CreateMainForm()
        {
            string FUNCTION_NAME = ".CreateMainForm";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "CreateMainForm");
            ConfigureFileHelper.GetInstance().init();

            ViewManager.GetInstance().RegisterViewFactory(new TimeTableViewFactory());
            IView view = ViewManager.GetInstance().GetView(TrainTimeTableConst.TrainTimeTableView);  //viewID is "" 
            Form frm = (Form)view;


            return frm;
        }

        public override void EntityChanged()
        {

        }

        public override int DutyChanged()
        {
            return 1;//todo
        }

        public override void ServerStateChange(bool ServerState)
        {

        }

        public override void PreApplicationClose()
        {
            TrainTimeTableViewer.View.TrainTimeTableView frm = (TrainTimeTableViewer.View.TrainTimeTableView)m_pMainFrm;
            frm.TrainTimeTableView_FormClosed();
        }

        public override void SetMainFrmType()
        {
            m_assemblytype = typeof(TrainTimeTableViewer.View.TrainTimeTableView);
        }

        public override void onInitGenericGUICompleted()
        {
            string FUNCTION_NAME = "onInitGenericGUICompleted";
            string exclude_stationId = getGuiEntityParameterValue(EXCLUDE_STATION_ID);

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Exclude_stationId - " + exclude_stationId);
            TrainTimeTableViewer.View.TrainTimeTableView frm = (TrainTimeTableViewer.View.TrainTimeTableView)m_pMainFrm;
            frm.SetParameters(exclude_stationId);
        }
    }
}
