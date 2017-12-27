using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;

namespace TrendViewer.Controller
{
   public class ChangeTrendTitleController:IController
    {
        /// <summary>
        /// include 3 strings: trend title, X axis title, Y axis title
        /// </summary>
        string[] m_titles = new string[3];

        private TrendViewer.View.EditChartTitle m_View;


        public override void NotifyObserver(NotifyObject notifyObj)
        {
            if (notifyObj.Name != m_View.ViewID) return;

            //will only handle the notification when the ViewID is matching.

        }
        public override void AttachCallBack(IModel model, IView view)
        {
            m_View = (TrendViewer.View.EditChartTitle)view;
        }

        /// <summary>
        /// init 3 titles
        /// </summary>
        /// <param name="titles">trend title, X axis title, Y axis title</param>
        public void InitData(string[] titles)
        {
            if (titles.Length<3)  return;
            else
            {
                m_titles[0] = titles[0];
                m_titles[1] = titles[1];
                m_titles[2] = titles[2];
            }
        }

        /// <summary>
        /// return 3 titles
        /// </summary>
        /// <returns></returns>
        public string[] GetTitles()
        {
            return m_titles;

        }
        /// <summary>
        /// EventHandler when user click "OK" on EditChartTitle form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void ModifyTitles(object sender, EventArgs e)
        {
            string[] titles = m_View.GetTitles();
            m_titles[0] = titles[0];
            m_titles[1] = titles[1];
            m_titles[2] = titles[2];

            NotifyManager.GetInstance().Send(DataNotificaitonConst.ModifyTitles, m_View.ViewID, m_titles);
            m_View.DestroyView();
        }
    }
}
