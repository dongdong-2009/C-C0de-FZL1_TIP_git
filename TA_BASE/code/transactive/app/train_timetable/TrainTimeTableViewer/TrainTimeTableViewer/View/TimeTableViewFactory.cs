using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using TrainTimeTableViewer.Controller;
using TrainTimeTableViewer.Model;
using TrainTimeTableViewer.Common;


namespace TrainTimeTableViewer.View
{
    class TimeTableViewFactory :IViewFactory
    {
        
        #region IViewFactory Members

        public IView CreateView(string type, string id)
        {
            IView view = null;
            IController controller = null;
            IModel model = null;
            switch (type)
            {
                case TrainTimeTableConst.TrainTimeTableView:
                    view= new TrainTimeTableView();
                    view.ViewType = TrainTimeTableConst.TrainTimeTableView;
                    controller = new TimeTableViewController();
                    model = new TimeTableViewModel();
                    controller.Attach(model, view);
                    break;
                case TrainTimeTableConst.TimeTableWindowView:
                    view = new TimeTableWindow();
                    view.ViewType = TrainTimeTableConst.TimeTableWindowView;
                    controller = new TimeTableWindowController();
                    model = new TimeTableWindowModel();
                    controller.Attach(model, view);
                    break;
                default:
                    break;
            }
            view.ViewID = id;
            return view;
        }

        #endregion
    }
}

