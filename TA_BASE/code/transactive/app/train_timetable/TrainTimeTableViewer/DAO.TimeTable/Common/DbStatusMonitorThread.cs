using System;
using System.Collections.Generic;
using System.Threading;
using System.Text;
using DAO.TimeTable.Helper;

namespace DAO.TimeTable.Common
{
    internal class DbStatusMonitorThread
    {
        private Thread m_thread = null;
        private List<string> m_items = null;
        //delegate to update status
        private AutoResetEvent m_addItemSignal = null;
        public delegate void VoidEventHandler(string connectionString, DBStatus dbStatus);
        public event VoidEventHandler UpdateStatusHandler;
        private bool m_terminate = false;
        private Object m_ObjectLock = new Object();

        public DbStatusMonitorThread()
        {
            m_items = new List<string>();
            m_addItemSignal = new AutoResetEvent(false);
        }

        public void Run()
        {
            //store current thread instance
            m_thread = Thread.CurrentThread;
            while (true)
            {
                //wait for new item to be added to list/ termination of thread
                m_addItemSignal.WaitOne();
                if (IsTerminated())
                {
                    break;
                }
                List<string> localList = new List<string>();
                lock (m_ObjectLock)
                {
                    foreach (var item in m_items)
                    {
                        localList.Add(item);
                    }
                    m_items.Clear();
                }
                
                for (int i = 0; i < localList.Count;i++ )
                {
                    Thread workerThread = new Thread(new ParameterizedThreadStart(ThreadMemberFunction));
                    workerThread.Start(localList[i]);
                    localList.Remove(localList[i]);
                }
            }
        }

        public void ThreadMemberFunction(object item)
        {
            string connectionString = (string)item;
            bool bConnected = false;
            while (!bConnected)
            {
                if (IsTerminated())
                {
                    break;
                }
                bConnected = SimpleDatabase.GetInstance().IsDatabaseConnectable(connectionString);
            }
            if (bConnected)
                UpdateStatusHandler(connectionString,DBStatus.DB_ONLINE);
        }

        public void AddItem(string connectionString)
        {
            lock (m_ObjectLock)
            {
                m_items.Add(connectionString);
                m_addItemSignal.Set();
            }
        }

        public void TerminateThread()
        {
            lock (m_ObjectLock)
            {
                m_terminate = true;
                m_addItemSignal.Set();
            }

            if (m_thread != null)
            {
                m_thread.Join();
            }

        }

        public bool IsTerminated()
        {
            bool retVal = false;
            lock (m_ObjectLock)
            {
                retVal = m_terminate;
            }
            return retVal;
        }
    }
}
