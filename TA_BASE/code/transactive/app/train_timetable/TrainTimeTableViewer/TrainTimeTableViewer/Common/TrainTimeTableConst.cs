using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrainTimeTableViewer.Common
{
    class TrainTimeTableConst
    {
        public const string About = "About";

        public const string TrainTimeTableView = "TrainTimeTableView";

        public const string TimeTableWindowView  = "TimeTableWindow";

        public const int MAJOR_GRID_LINES_MAX_NUM = 15;

        public const int MINOR_INTERVAL_NUM = 5;

        public const int INSTANCE_MAX_NUM = 8;

        //the number of days earlier than today that can be chosen in date options
        public const int NUM_EARLIER_DAYS = 32;

        //default number of rows to be displayed in traintimetable window
        public const int NUM_ROWS_TRAINTIMETABLE_WINDOW = 15;

        public const string LOG_FILE_PATH_PRERIX = "../logs/Log_TrainTimetable_0";

        public const string SYSTEM_EVENT_NAME_PREFIX = "Global\\TrainTimetable_0";

        public const int TRAINTIMETABLE_VIEWER_APP_TYPE = 102;

    }
}
