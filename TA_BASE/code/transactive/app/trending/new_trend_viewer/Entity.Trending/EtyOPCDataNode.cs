using System;
using System.Collections.Generic;
using System.Text;

namespace Entity.Trending
{
    public class EtyOPCDataNode
    {
        double m_OPCDataNodeId;
        double m_OPCParentDataNodeId;
        string m_OPCDataNodeName = null;
        string m_OPCDataNodeDesc = null;
        string m_OPCDataNodeHost = null;
        bool m_disbaled = false;
        string m_OPCDataNodeServer = null;

        public double OPCDataNodeId
        {
            get { return m_OPCDataNodeId; }
            set { m_OPCDataNodeId = value; }
        }

        public double OPCParentNodeId
        {
            get { return m_OPCParentDataNodeId; }
            set { m_OPCParentDataNodeId = value; }
        }

        public string OPCDataNodeName
        {
            get { return m_OPCDataNodeName; }
            set { m_OPCDataNodeName = value; }
        }

        public string OPCDataNodeDesc
        {
            get { return m_OPCDataNodeDesc; }
            set { m_OPCDataNodeDesc = value; }
        }

        public string OPCDataNodeHost
        {
            get { return m_OPCDataNodeHost; }
            set { m_OPCDataNodeHost = value; }
        }

        public string OPCDataNodeServer
        {
            get { return m_OPCDataNodeServer; }
            set { m_OPCDataNodeServer = value; }
        }

        public bool Disabled
        {
            get { return m_disbaled; }
            set { m_disbaled = value; }
        }
    }   
}
