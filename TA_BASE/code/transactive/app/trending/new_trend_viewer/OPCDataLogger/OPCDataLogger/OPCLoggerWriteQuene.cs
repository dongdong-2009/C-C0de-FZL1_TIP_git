using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

using Entity.Trending;
using DAO.Trending;
using STEE.ISCS.Log;

namespace OPCDataLogger
{
    /// <summary>
    /// This class is responsible for writing TrendLogs in quene into database.
    /// </summary>
    class OPCLoggerWriteQuene
    {
        private const string CLASS_NAME = "OPCDataLogger.OPCLoggerWriteQuene";
        private Queue<EtyTrendLog> m_writeQuene = null;
        private const int m_maxCapacity = 100000;
        private bool m_terminate = false;
        private Object m_ObjectLock = new Object();
        private AutoResetEvent m_addItemSignal = null;
        private bool m_dbDisconnected = false;
        private Thread m_thread = null;

        /// <summary>
        /// Constructor
        /// </summary>
        public OPCLoggerWriteQuene()
        {
            string Function_Name = "OPCLoggerWriteQuene";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            m_writeQuene = new Queue<EtyTrendLog>();
            m_addItemSignal = new AutoResetEvent(false);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }


        /// <summary>
        /// Destructor method
        /// </summary>
        ~OPCLoggerWriteQuene()
        {
            string Function_Name = "~OPCLoggerWriteQuene";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            ClearQuene();
            m_writeQuene = null;
            m_addItemSignal = null;
            m_thread = null;
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }


        /// <summary>
        /// Clears the Write Quene items.
        /// </summary>
        private void ClearQuene()
        {
            string Function_Name = "ClearQuene";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            
            m_writeQuene.Clear();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }


        /// <summary>
        /// This method is responisble for saving logs into database.
        /// </summary>
        public void Run()
        {
            string Function_Name = "Run";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            //store current thread instance
            m_thread = Thread.CurrentThread;

            while (true)
            {
                //wait for new item to be added to quene/ termination of thread
                m_addItemSignal.WaitOne();
                if (IsTerminated())
                {
                    break;
                }

                //if there is any item in queue to process
                if (!IsQueneEmpty())
                {
                    //copy to local quene
                    Queue<EtyTrendLog> tempQuene = null;
                    lock (m_ObjectLock)
                    {
                        tempQuene = new Queue<EtyTrendLog>(m_writeQuene.ToArray());
                        m_writeQuene.Clear();
                    }

                    while (tempQuene != null && tempQuene.Count != 0)
                    {
                        if (CheckDatabaseConnection())
                        {
                            EtyTrendLog etyTrendLog = tempQuene.Dequeue();
                            //save into database
                            if (!TrendLogDAO.GetInstance().InsertTrendViewerLog(etyTrendLog))
                            {

                                m_dbDisconnected = true;
                                //check whether insert SQL failed due to database Connection failure                   
                                if (!CheckDatabaseConnection())
                                {
                                    //insert back to quene
                                    var Items = tempQuene.ToArray();
                                    tempQuene.Clear();
                                    tempQuene.Enqueue(etyTrendLog);
                                    foreach (var item in Items)
                                    {
                                        tempQuene.Enqueue(item);
                                    }
                                }
                                //due to some other error, ignore this item
                            }
                        }
                    }

                }
            }

            // time to end the thread
            Thread.CurrentThread.Abort();
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// checks whether Quene is empty or not
        /// </summary>
        /// <returns></returns>
        private bool IsQueneEmpty()
        {
            bool emtpyBool = false;
            lock (m_ObjectLock)
            {
                emtpyBool = (m_writeQuene.Count == 0 ? true : false);
            }
            return emtpyBool;
        }

        /// <summary>
        /// This method is responsibel for terminating the thread.
        /// </summary>
        public void TerminateAndWait()
        {
            string Function_Name = "TerminateAndWait";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            lock (m_ObjectLock)
            {
                m_terminate = true;
                m_addItemSignal.Set();              
            }

            if (m_thread != null)
            {
                m_thread.Join();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Checks the thread Terminate Flag value
        /// </summary>
        /// <returns></returns>
        public bool IsTerminated()
        {
            bool retVal = false;
            lock (m_ObjectLock)
            {
                retVal = m_terminate;
            }
            return retVal;
        }

        /// <summary>
        /// This method is reponsible for inserting new item into quene and 
        /// signalling the thread to process the item.
        /// </summary>
        /// <param name="etyTrendLog"></param>
        public void PushToQuene(EtyTrendLog etyTrendLog)
        {
            string Function_Name = "PushToQuene";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            lock (m_ObjectLock)
            {
                if (m_writeQuene.Count > m_maxCapacity)
                {
                    LogHelper.Debug(CLASS_NAME, Function_Name, "Maximum capacity of the queue is reached so removing the first item in queue");
                    //this case is very rare.. loss of logs
                    m_writeQuene.Dequeue();
                }
                m_writeQuene.Enqueue(etyTrendLog);
                //m_containsDataInQuene = true;
                m_addItemSignal.Set();
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// This method tries to reconnect to database if it got disconnected.
        /// </summary>
        /// <returns></returns>
        private bool CheckDatabaseConnection()
        {
            string Function_Name = "CheckDatabaseConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            if (m_dbDisconnected)
            {
                LogHelper.Debug(CLASS_NAME, Function_Name, "Reconnecting to Oracle");
                if (DBConnection.getInstance().OpenConnection(OPCDataPointManager.GetInstance().m_localConnectionString))
                {
                    m_dbDisconnected = false;
                }
                else
                    return false;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return true;

        }

    }
}
