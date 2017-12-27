using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.Remoting.Messaging;

namespace DAO.TimeTable.Common
{
    public class SessionStore
    {
        private string m_CurrentSessionID = "CurrentSessionConnectionId" ;
        private List<string> m_connectionStringIDs = null;

        public SessionStore()
        {
            m_connectionStringIDs = new List<string>();
        }

        ~SessionStore()
        {
            m_connectionStringIDs = null;
        }

        public void Store(string connectionString, Object value)
        {
            if (!m_connectionStringIDs.Contains(connectionString))
            {
                m_connectionStringIDs.Add(connectionString);
            }
            CallContext.SetData(connectionString, value);            
        }

        public void StoreCurrentSession(Object value)
        {
            CallContext.SetData(m_CurrentSessionID, value);
        }

        public DatabaseSession GetSession(string connectionString)
        {
            return CallContext.GetData(connectionString) as DatabaseSession;
        }

        public DatabaseSession GetCurrentSession()
        {
            return CallContext.GetData(m_CurrentSessionID) as DatabaseSession;
        }

        public void Dispose(string connectionString)
        {
            foreach(var item in m_connectionStringIDs)
            {
                CallContext.SetData(item, null);
            }
            CallContext.SetData(m_CurrentSessionID, null);            
        }
         
    }
}
