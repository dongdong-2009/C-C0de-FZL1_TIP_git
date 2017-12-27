using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

using Entity.Trending;
using DAO.Trending;
using STEE.ISCS.OPCProxy;
using STEE.ISCS.Log;

namespace OPCDataLogger
{
    /// <summary>
    /// This class is responsible for logging the dataPoints realtime value
    /// from OPC Server for specified logger interval in Database.
    /// </summary>
    class OPCLoggerQuene
    {
        private const string CLASS_NAME = "OPCDataLogger.OPCLoggerQuene";

        private Object m_sharedVarLock = new Object();
        private bool m_serviceStarted = false;
        CheckConfigVars m_checkConfigVars = null;
        //private bool m_opcDisconnected = false;
        private bool m_dbDisconnected = false;
        private string m_opcSrvName = "";
        private string m_opcSvrHost = "";
        //private List<EtyTrendLog> m_FailedDataPTQuene = null;
        //private const int QUENECAPACITY = 30000;
        private Thread m_thread = null;

        public OPCLoggerQuene(string ServerName, string HostName)
        {
            string Function_Name = "OPCLoggerQuene";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            m_opcSrvName = ServerName;
            m_opcSvrHost = HostName;
            m_checkConfigVars = new CheckConfigVars();
           // m_FailedDataPTQuene = new List<EtyTrendLog>();
           // m_FailedDataPTQuene.Capacity = QUENECAPACITY;

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        ~OPCLoggerQuene()
        {
            string Function_Name = "~OPCLoggerQuene";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            m_checkConfigVars = null;
            m_thread = null;

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }   

        /// <summary>
        /// Sets the Running status of the thread.
        /// </summary>
        /// <param name="flag"></param>
        public void setServiceRunning(bool flag)
        {
            lock (m_sharedVarLock)
            {
                LogHelper.Info(string.Format("Service status updated to {0}", flag));
                m_serviceStarted = flag;
            }
        }


        /// <summary>
        /// The main function for logging the datapoints value from OPC Server in database.
        /// </summary>
        public void Run()
        {
            string Function_Name = "Run";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            //store current thread instance
            m_thread = Thread.CurrentThread;

            //Initiliase the Datalogger here to avoid different thread COM Object issue.
            OPCDataPointManager.GetInstance().InitializeDataLogger(false);

            //till the thread is running
            while (m_serviceStarted)
            {
                try
                {
                    // Start ID 0001170
                    if (TestOracleConnection() && CheckOPCConnection())
                    {
                        //LogFaileDataPTQuene();

                        // Add Datapoints which failed to be added to OPCGroup since OPCServer didnot intialize at that time properly.
                        OPCServerProxy.GetInstance().AddFailedOPCItems();

                        // End ID 0001170  
                        CheckSampleGroupFlag();

                        OPCDPGrpDetails opcDetails = null;
                        Dictionary<string, OPCDPGrpDetails> opcDataPTsDic = OPCDataPointManager.GetInstance().GetOPCLoggerDataPoints();
                        foreach (KeyValuePair<string, OPCDPGrpDetails> pair in opcDataPTsDic)
                        {
                            try
                            {
                                opcDetails = pair.Value;
                                //bOPCItemQualityNotGood = false;
                                            
                                if (CanLogDataPT(opcDetails))
                                {
                                    DateTime plannedLogTime = System.Convert.ToDateTime(opcDetails.NextSampleTime);                                                      
                                    DateTime currenttime = DateTime.Now;
                                    opcDetails.NextSampleTime = OPCDataPointManager.GetInstance().GetNextSampleTime(plannedLogTime, opcDetails.Interval, false);
                                    string opcvalue = GetOPCValue(opcDetails.DT_PT_Name);
                                    if (opcvalue.Trim() != "")
                                    {
                                        opcDetails.Value = opcvalue;
                                    }
                                    else
                                    {
                                        LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("DataPoint: {0}- DataPoint value: invalid", opcDetails.DT_PT_Name));
                                         //bOPCItemQualityNotGood = true;
                                        continue;
                                    }
                                    string value = "";
                                    double delta = 0;
                                    bool writeFlag = false;

                                    EtyTrendLog etyTrendLog = new EtyTrendLog();
                                    etyTrendLog.Data_PT_Name = opcDetails.DT_PT_Name;
                                    etyTrendLog.Data_PT_Host = m_opcSvrHost;
                                    etyTrendLog.Data_PT_Server = m_opcSrvName;                                   
                                    etyTrendLog.Data_PT_Time = currenttime;

                                    //first time logging
                                    if (opcDetails.OldValue.ToString() == "null")
                                    {
                                        opcDetails.OldValue = value = opcDetails.Value;
                                        etyTrendLog.Data_PT_Value = Convert.ToDouble(value);
                                        writeFlag = true;                                  
                                    }
                                    else
                                    {
                                        // check delta
                                        delta = Math.Abs(Convert.ToDouble(opcDetails.Value .ToString()) -
                                            Convert.ToDouble(opcDetails.OldValue.ToString()));
                                        //log only if the value is >= delta value.
                                        if (delta >= Convert.ToDouble(opcDetails.Delta.ToString()))
                                        {
                                            opcDetails.OldValue = value = opcDetails.Value;
                                            etyTrendLog.Data_PT_Value = Convert.ToDouble(value);
                                            writeFlag = true;
                                        }
                                    }
                                    if (writeFlag)
                                    {
                                        OPCDataPointManager.GetInstance().PushToWriteQuene(etyTrendLog);
                                    }
                                }
                            }
                            catch (Exception localException)
                            {
                                LogHelper.Error(opcDetails.DT_PT_Name.ToString()+" Fail");                               
                                CheckOracleConnection(localException.ToString());
                            }
                        }
                    }
                }
                catch (Exception localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());        
                    CheckOracleConnection(localException.ToString());
                }

                if (m_serviceStarted)
                {
                    Thread.Sleep(new TimeSpan(0, 0, 0, 0, OPCDataPointManager.GetInstance().GetLoggerInterval()));
                }
            }

            // time to end the thread
            Thread.CurrentThread.Abort();
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        public void TerminateAndWait()
        {
            string Function_Name = "TerminateAndWait";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            setServiceRunning(false);
            if (m_thread != null)
            {
                m_thread.Join();
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Method callled when there is an exception while logging.
        /// Check is exception is due to oracle connection and set disconnection flag.
        /// </summary>
        /// <param name="exceptionString"></param>
        private void CheckOracleConnection(string exceptionString)
        {
            string Function_Name = "CheckOracleConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            try
            {
                if (exceptionString.Trim() == "")
                {
                    m_dbDisconnected = false;
                }
                else
                {
                    if (m_dbDisconnected) return;
                    m_dbDisconnected = true;
                    LogHelper.Error(exceptionString);
                    LogHelper.Error("Oracle Connection Fail");
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");

        }

        /// <summary>
        /// Reconnects to oracle database if the disconnection flag is set.
        /// </summary>
        /// <returns>true - if database connection succeed, false - failed</returns>
        private bool TestOracleConnection()
        {
            string Function_Name = "TestOracleConnection";
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
        
        /// <summary>
        /// Checks the Config_vars variable value. Based on value,
        /// Restarts logging by reloading datapoints.
        /// </summary>
        private void CheckSampleGroupFlag()
        {
            string Function_Name = "CheckSampleGroupFlag";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            try
            {
                if (!m_dbDisconnected)
                {
                    string resetFlag = m_checkConfigVars.GetOPCDTSmplFlagValue();
                    if (resetFlag == "Y")
                    {
                        LogHelper.Info(CLASS_NAME, Function_Name, "Reset Flag Set");
                        try
                        {
                            OPCServerProxy.GetInstance().RemoveAllOPCItem();
                        }
                        catch (Exception Ex)
                        {
                            LogHelper.Error(CLASS_NAME, Function_Name, Ex);
                            //OPCBridge service is shutdown/crashed.
                        }
                        OPCDataPointManager.GetInstance().ClearDataPoints();
                        Thread.Sleep(1000);
                        OPCDataPointManager.GetInstance().InitializeDataLogger(true);
                    }
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
                CheckOracleConnection(localException.ToString());
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Reconnects to OPC Server if the disconnection flag is set.
        /// </summary>
        /// <returns></returns>
        private bool CheckOPCConnection()
        {
            string Function_Name = "CheckOPCConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (/*m_opcDisconnected || */!(OPCServerProxy.GetInstance().IsOPCServerConnected()))
            {
                LogHelper.Debug(CLASS_NAME, Function_Name, "Reconnecting to OPC Server");
                bool result = OPCServerProxy.GetInstance().ReconnectAndAddToOPC();
               // if(result)
               //     m_opcDisconnected = false;
                return result;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return true;
        }

        /// <summary>
        /// Checks whether the datapoints next time is >= current time.
        /// </summary>
        /// <param name="opcDPGrp">OPCDP Grp Details</param>
        /// <returns>true - can log, false - cannot log</returns>
        private bool CanLogDataPT(OPCDPGrpDetails opcDPGrp)
        {
            string Function_Name = "CanLogDataPT";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            DateTime timeStamp = DateTime.Now;
            timeStamp = new DateTime(timeStamp.Year, timeStamp.Month, timeStamp.Day, timeStamp.Hour, timeStamp.Minute, timeStamp.Second, 0);
            if (opcDPGrp.NextSampleTime == null)
            {
                opcDPGrp.NextSampleTime = timeStamp;
                return true;
            }
            else
            {
                if (DateTime.Compare(timeStamp, System.Convert.ToDateTime(opcDPGrp.NextSampleTime)) >= 0)
                {
                    return true;
                }
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return false;

        }

        /*private void LogFaileDataPTQuene()
        {
            if (m_FailedDataPTQuene.Count != 0)
            {
                foreach (EtyTrendLog etyLog in m_FailedDataPTQuene)
                {
                    if (!TrendLogDAO.GetInstance().InsertTrendViewerLog(etyLog))
                    {
                        m_dbDisconnected = true;
                        break;
                    }
                    m_FailedDataPTQuene.Remove(etyLog);
                }
            }
        }

        private void AddToQuene(EtyTrendLog etyLog)
        {
            if (m_FailedDataPTQuene.Count != QUENECAPACITY)
            {
                m_FailedDataPTQuene.Add(etyLog);
            }
            else
            {
                m_FailedDataPTQuene.RemoveAt(0);
                m_FailedDataPTQuene.Add(etyLog);
            }
        }*/

        /// <summary>
        /// Gets the curretn DataPoint value from OPC server.
        /// </summary>
        /// <param name="dataPointname"></param>
        /// <returns></returns>
        private string GetOPCValue(string dataPointname)
        {
            string Function_Name = "GetOPCValue";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            string dpValue = "";
            object dpValueObj = OPCServerProxy.GetInstance().GetDataItemValue(dataPointname);
            //check if opc server is connect and quality is good.
            if (dpValueObj != null && dpValueObj.ToString() != "BLANK" && dpValueObj.ToString() != "NOTCONNECTED")
            {
                if (OPCDataPointManager.GetInstance().isNumeric(dpValueObj.ToString(), System.Globalization.NumberStyles.Number))
                {
                    dpValue = dpValueObj.ToString();
                }
                else if (OPCDataPointManager.GetInstance().isBoolean(dpValueObj.ToString()))
                {
                    dpValue = "0";
                    if (Convert.ToBoolean(dpValueObj.ToString()))
                        dpValue = "1";
                }
                else
                {
                    dpValue = "0";
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return dpValue;

        }

    }
}
