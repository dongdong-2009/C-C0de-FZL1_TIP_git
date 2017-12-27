using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using TrendViewer.Model;
using Entity.Trending;
using System.Windows.Forms;
using STEE.ISCS.Log;
using TrendViewer.Common;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.Controller
{
    public class OPCDataSelectorController:IController
    {
        private const string CLASS_NAME = "OPCDataSelectorController";
        private Dictionary<ulong,EtyEntity> m_dataNodeList = null;
        private OPCDataSelectorModel m_Model;
        private TrendViewer.View.OPCDataSelector m_View;


        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (OPCDataSelectorModel)model;
            m_View = (TrendViewer.View.OPCDataSelector)view;
        }


       public Dictionary<ulong,EtyEntity> getDataNodeMap()
        {
            return m_dataNodeList;
        }

         public override void NotifyObserver(NotifyObject notifyObj)
         {
             
         }

        public void InitDataNodeList()
        {
            string Function_Name = "InitDataNodeList";
            try
            {
                if (null == m_dataNodeList)
                {
                    string serverRootName = TrendingHelper.ConfigureFileHelper.GetInstance().OPCServerRootName;
                    m_dataNodeList = m_Model.GetDataNodeListByServerRootName(serverRootName);
                }
            }
            catch (System.Exception ex)
            {
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoDB, LanguageHelper.TrendViewer_Msg_NoDB_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN), 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
            }

        }

        public void opcItemDataGridView_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string Function_Name = "opcItemDataGridView_CellDoubleClick";
            try
            {
                if (e.RowIndex < 0) return;

                EtyEntity dp = m_View.getDPInGridView(e.RowIndex);
                NotifyManager.GetInstance().Send(DataNotificaitonConst.DPSelected, m_View.ViewID, dp);

                m_View.Close();
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        public void AfterSelect(object sender, TreeViewEventArgs e)
        {
            string Function_Name = "AfterSelect";
            try
            {
                TreeNode selectedNode = m_View.getSelectedNode();
                if ( selectedNode.Nodes.Count < 1)
                {
                    Dictionary<ulong, EtyEntity> childNodes = m_Model.GetDataNodeChildrenByPkey((ulong)(selectedNode.Tag));
                    Dictionary<ulong, EtyEntity> childPoints= m_Model.GetDataPointByDNPkey((ulong)(selectedNode.Tag));

                    m_View.LoadDataNodeChildren(ref selectedNode, childNodes);
                    m_View.LoadDataPointToGridView(childPoints);
                }
                else
                {
                    Dictionary<ulong, EtyEntity> childPoints = m_Model.GetDataPointByDNPkey((ulong)(selectedNode.Tag));
                    m_View.LoadDataPointToGridView(childPoints);
                }

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

    }
}
