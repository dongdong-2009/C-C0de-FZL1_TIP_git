using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;

using TrendViewer.Controller;
using TrendViewer.Model;

namespace TrendViewer.View
{
    class TrendingViewFactory: IViewFactory
    {
        #region IViewFactory Members

        public IView CreateView(string type, string id)
        {
            IView view = null;
            IController controller = null;
            IModel model = null;
            switch (type)
            {
                case  TrendViewConst.About:
                    view= new About();
                    view.ViewType = TrendViewConst.About; 
                    break;
                case TrendViewConst.TrendView:
                    view= new TrendView();
                    view.ViewType = TrendViewConst.TrendView; 
                    controller=new TrendViewController();
                    model = new TrendViewModel();
                    controller.Attach(model, view);
                    break;

                // marker related:
                case TrendViewConst.MarkerList:
                    view = new MarkerList();
                    view.ViewType = TrendViewConst.MarkerList;
                    controller = new MarkerListController();
                    model = new MarkerListModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.LoadConfigMarker:
                    view = new MarkerGroup();
                    controller = new MarkerGroupController();
                    model = new MarkerGroupModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.SaveConfigMarker:
                    view = new MarkerGroup();
                    controller = new MarkerGroupController();  
                    model = new MarkerGroupModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.AddMarkerData:
                    view = new MarkerData();
                    controller = new MarkerDataController();
                    model = new MarkerDataModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.EditMarkerData:
                    view = new MarkerData();
                    controller = new MarkerDataController();
                    model = new MarkerDataModel();
                    controller.Attach(model, view);
                    break;
                // formula related:
                case TrendViewConst.FormulaList:
                    view = new FormulaList();
                    view.ViewType = TrendViewConst.FormulaList;
                    controller = new FormulaListController();
                    model = new FormulaListModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.LoadConfigFormula:
                    view = new FormulaGroup();
                    controller = new FormulaGroupController();
                    model = new FormulaGroupModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.SaveConfigFormula:
                    view = new FormulaGroup();
                    controller = new FormulaGroupController();
                    model = new FormulaGroupModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.AddFormulaData:
                    view = new FormulaData();
                    controller = new FormulaDataController();
                    model = new FormulaDataModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.EditFormulaData:
                    view = new FormulaData();
                    controller = new FormulaDataController();
                    model = new FormulaDataModel();
                    controller.Attach(model, view);
                    break;

                // DP related:
                case TrendViewConst.DataPointList:
                    view = new DataPointList();
                    view.ViewType = TrendViewConst.DataPointList;
                    controller = new DataPointListController();
                    model = new DataPointListModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.LoadConfigDataPoint:
                    view = new DataPointGroup();
                    controller = new DataPointGroupController();
                    model = new DataPointGroupModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.SaveConfigDataPoint:
                    view = new DataPointGroup();
                    controller = new DataPointGroupController();
                    model = new DataPointGroupModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.AddDataPointData:
                    view = new DataPointData();
                    controller = new DataPointDataController();
                    model = new DataPointDataModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.EditDataPointData:
                    view = new DataPointData();
                    controller = new DataPointDataController();
                    model = new DataPointDataModel();
                    controller.Attach(model, view);
                    break;

                //Hist DP related:
                case TrendViewConst.HistDataPointList:
                    view = new HistDataPointList();
                    view.ViewType = TrendViewConst.HistDataPointList;
                    controller = new HistDataPointListController();
                    model = new HistDataPointListModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.LoadConfigHistDataPoint:
                    view = new HistDataPointGroup();
                    controller = new HistDataPointGroupController();
                    model = new HistDataPointGroupModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.SaveConfigHistDataPoint:
                    view = new HistDataPointGroup();
                    controller = new HistDataPointGroupController();
                    model = new HistDataPointGroupModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.AddHistDataPointData:
                    view = new HistDataPointData();
                    controller = new HistDataPointDataController();
                    model = new HistDataPointDataModel();
                    controller.Attach(model, view);
                    break;
                case TrendViewConst.EditHistDataPointData:
                    view = new HistDataPointData();
                    controller = new HistDataPointDataController();
                    model = new HistDataPointDataModel();
                    controller.Attach(model, view);
                    break;

                case TrendViewConst.OPCDataSelector:
                    view = new OPCDataSelector();
                    controller = new OPCDataSelectorController();
                    model = new OPCDataSelectorModel();
                    controller.Attach(model, view);
                    break;

                case TrendViewConst.ChangeTrendTitle:
                    view = new EditChartTitle();
                    controller = new ChangeTrendTitleController();
                    model = null;
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
