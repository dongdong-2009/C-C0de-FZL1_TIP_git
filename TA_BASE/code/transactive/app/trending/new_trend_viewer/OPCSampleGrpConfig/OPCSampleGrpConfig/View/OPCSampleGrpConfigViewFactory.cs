using System;
using System.Collections.Generic;
using System.Text;


using STEE.ISCS.MVC;
using OPCSampleGrpConfig.Controller;
using OPCSampleGrpConfig.Model;

namespace OPCSampleGrpConfig.View
{
    public class OPCSampleGrpConfigViewFactory: IViewFactory
    {
        #region IViewFactory Members

        public IView CreateView(string type, string id)
        {
            IView view = null;
            IController controller = null;
            IModel model = null;
            switch (type)
            {
                case OPCSampleGrpConfigStart.OPCSAMPLEGRPCONFIGSTARTFRM:
                    view = new OPCSampleGrpConfigStart();
                    view.ViewType = OPCSampleGrpConfigStart.OPCSAMPLEGRPCONFIGSTARTFRM;
                    controller = new OPCSampleGrpConfigStartController();
                    model = new OPCSampleGrpConfigStartModel();
                    controller.Attach(model, view);
                    break;

                case OPCDataSelector.OPCDATASELECTORFRM:
                    view = new OPCDataSelector();
                    view.ViewType = OPCDataSelector.OPCDATASELECTORFRM;
                    controller = new OPCDataSelectorController();
                    model = new OPCDataSelectorModel();
                    controller.Attach(model, view);
                    view.PutObject(OPCDataSelector.OPCSAMPLEGRPDIC, ((OPCDataSelectorModel)model).GetSampleGrps());
                    break;

                default:
                    break;
            }
            return view;
        }

        #endregion
    }
}
