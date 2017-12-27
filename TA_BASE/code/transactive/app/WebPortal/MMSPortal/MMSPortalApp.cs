using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Text;
using NET_ISCS_Bridge;

namespace MMSPortal
{
    public class MMSPortalApp : TransactiveNetApp
    {
        private const string CLASS_NAME = "MMSPortal.MMSPortalApp";
        private const string START_URL_COLUMN = "StartupURL";
        private const string EXIT_URL_COLUMN = "ExitURL";
        private const string REFRESH_TIME_COLUMN = "AutoRefreshTime";
        private const string GUI_WIDTH_COLUMN = "GUIWidth";
        private const string GUI_HEIGHT_COLUMN = "GUIHeight";
        private const string WINDOW_CAPTION_COLUMN = "WindowCaption";
        private const string ENABLE_MAXIMISE_COLUMN = "EnableMaximise";


        public override void CheckCommandLine()
        {

        }

        public override Form CreateMainForm()
        {
            string FUNCTION_NAME = ".CreateMainForm";
            MMSPortalForm frm = new MMSPortalForm();
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
            MMSPortalForm frm = (MMSPortalForm)m_pMainFrm;
           // frm.Form1_FormClosed();
        }

        public override void SetMainFrmType()
        {
            m_assemblytype = typeof(MMSPortalForm);
        }

        public override void onInitGenericGUICompleted()
        {
            string startUrlvalue = getGuiEntityParameterValue(START_URL_COLUMN);
            string endUrlValue = getGuiEntityParameterValue(EXIT_URL_COLUMN);
            string refreshTime = getGuiEntityParameterValue(REFRESH_TIME_COLUMN);
            string guiWidth = getGuiEntityParameterValue(GUI_WIDTH_COLUMN);
            string guiHeight = getGuiEntityParameterValue(GUI_HEIGHT_COLUMN);
            string windowCaption = getGuiEntityParameterValue(WINDOW_CAPTION_COLUMN);
            int nRefresh = Convert.ToInt16(refreshTime);
            int frmwidth = Convert.ToInt16(guiWidth);
            int frmheight = Convert.ToInt16(guiHeight);
            if (frmwidth == 0 || frmheight == 0)
            {
                frmwidth = 979;
                frmheight = 672;
            }
            int enableMax = Convert.ToInt16(getGuiEntityParameterValue(ENABLE_MAXIMISE_COLUMN));
            bool bMax = (enableMax == 0 ? false : true);
            MMSPortalForm frm = (MMSPortalForm)m_pMainFrm;
            frm.setParameters(startUrlvalue, endUrlValue, nRefresh, frmwidth, frmheight, windowCaption,bMax);       
        }

    }
}
