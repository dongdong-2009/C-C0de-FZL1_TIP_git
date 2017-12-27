using System;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Diagnostics;

using STEE.ISCS.Log;
using STEE.ISCS.OPCProxy;
using DAO.Trending;
using Entity.Trending;

namespace OPCDataLogger
{
    /// <summary>
    /// This class is responsible for loading the configured datapoints for logging and 
    /// connect to OPC Server and start logging using OPCLoggerQuene class.
    /// </summary>
    class OPCDataPointManager
    {
        private const string CLASS_NAME = "OPCDataLogger.OPCDataPointManager";

        private static OPCDataPointManager m_instance = null;
        private static Object s_Singleton = new Object();
        private static Object m_sharedVarLock = new Object();
        private Object m_sharedThreadObjLock = new Object();

        //DatapointNameToItsDetails dictionary
        private Dictionary<string, OPCDPGrpDetails> m_DataPointdic;        
        CheckConfigVars m_checkConfigVars = null;
        private int m_OPCLogInterval = 500; //(milliseconds)//60;//seconds
        private OPCLoggerQuene m_loggerQuene = null;
        // Server 1 Properties       
        private string m_opcSrv1Name = "";
        public readonly string m_localConnectionString = "";
        public OPCLoggerWriteQuene m_writeQuene = null;
        //private Process m_process = null;
        private string m_processID = "";

        public delegate void VoidEventHandler();
        public event VoidEventHandler ExitApplicationEvent;

        private const string CONFIGFOLDER = "./";
        private const string OPCSVR1HOST = "127.0.0.1";
        private const int MINUTES_UNIT = 60;
        private const int SECONDS_UNIT = 60;

        public static OPCDataPointManager GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_instance == null)
            {
                lock (s_Singleton)
                {
                    m_instance = new OPCDataPointManager();
                    LogHelper.Info(CLASS_NAME, Function_Name, "Create Instance");
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return m_instance;
        }

        public static void ReleaseInstance()
        {
            string Function_Name = "ReleaseInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            if (m_instance != null)
            {
                lock (s_Singleton)
                {
                    m_instance.ClearDataPoints();
                    OPCServerProxy.ReleaseInstance();
                    DBConnection.ReleaseInstance();                  
                    m_instance = null;
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        public void ClearDataPoints()
        {
            string Function_Name = "ClearDataPoints";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            lock (m_sharedVarLock)
            {
                m_DataPointdic.Clear();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        private OPCDataPointManager()
        {
            string Function_Name = "OPCDataPointManager";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            m_DataPointdic = new Dictionary<string, OPCDPGrpDetails>();
            m_checkConfigVars = new CheckConfigVars();

            m_opcSrv1Name = ConfigureFileHelper.GetInstance().OPCDTSever1Name;
            //string tempString = ConfigureFileHelper.GetInstance().DTTimeInteval;
            string tempString = ConfigureFileHelper.GetInstance().TimeIntevalMS;
            if (isNumeric(tempString, System.Globalization.NumberStyles.Integer))
                m_OPCLogInterval = int.Parse(tempString);

            m_loggerQuene = new OPCLoggerQuene(m_opcSrv1Name, OPCSVR1HOST);

            m_localConnectionString = ConfigureFileHelper.GetInstance().ConnectionString;

            m_writeQuene = new OPCLoggerWriteQuene();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        ~OPCDataPointManager()
        {
            string Function_Name = "~OPCDataPointManager";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            m_DataPointdic = null;
            m_checkConfigVars = null;
            m_loggerQuene = null;
            m_writeQuene = null;
   
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Initialisez the datalogger and starts logging in separate thread.
        /// </summary>
        /// <param name="frmprocess"></param>
        public void InitializeOPCSetting( string processID)
        {
            string Function_Name = "InitializeOPCSetting";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            try
            {
                //m_process = frmprocess;
                m_processID = processID;
                //InitializeDataLogger();

                lock (m_sharedThreadObjLock)
                {
                    //main Logger Thread
                    ThreadStart threadStartFunction = new ThreadStart(m_loggerQuene.Run);
                    Thread workerThread = new Thread(threadStartFunction);
                    //workerThread.SetApartmentState(ApartmentState.STA);
                    // start the thread
                    workerThread.Start();

                    //WriteThread
                    ThreadStart writeThreadFunction = new ThreadStart(m_writeQuene.Run);
                    Thread writeWorkerThread = new Thread(writeThreadFunction);
                    writeWorkerThread.Start();
                }
               
                LogHelper.Info(CLASS_NAME, Function_Name, "OPC Data Logger is ready");
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Connects to OPC Server and Database.
        /// Loads the configured datapoints and connect it to OPC Server.
        /// </summary>
        /// <param name="bReInitializing">true - application is reInitializing the logger DataPoints, false - intializing datapoints after application startup</param>
        public void InitializeDataLogger(bool bReInitializing)
        {
            string Function_Name = "InitializeDataLogger";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            try
            {
                lock (m_sharedVarLock)
                {
                    //check oracle connection
                    bool bConnected = DBConnection.getInstance().OpenConnection(m_localConnectionString);
                    if (!bConnected)
                    {
                        LogHelper.Error("No Connection to Oracle Database");
                        if (!bReInitializing)
                        {
                           //m_process.Kill(); 
                            ExitApplicationEvent();
                            return;
                        }
                    }

                    LoadDataPointTimeStamp();

                    LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Number of DataPoints to be logged = {0}", m_DataPointdic.Count));

                    if (!InitializeAndConnectOPCServer())
                    {
                        LogHelper.Error("No Connection to OPC Server");
                        if (!bReInitializing)
                        {
                            //m_process.Kill();   
                            ExitApplicationEvent();
                            return;
                        }
                        else
                        {
                            ConnectDPToOPC();
                        }
                    }

                    //update the config_var table after loading datapoints.
                    m_checkConfigVars.UpdateToDisableOPCDTSmplFlag();                    
                }

                lock(m_sharedThreadObjLock)
                {
                    m_loggerQuene.setServiceRunning(true);
                }

                Thread.Sleep(1000);
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Loads the Enabled Datapoints of the logger servername.
        /// </summary>
        public void LoadDataPointTimeStamp()
        {
            string Function_Name = "LoadDataPointTimeStamp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            //get unique Datapoint of this servername and host which is enabled in both table - opc_dt_pt & opc_sample_grp
            List<EtyOPCDataPoint> etyDataPointList = OPCNodeNDataPointDAO.GetInstance().GetUniqueEnabledOPCDataPoint(OPCSVR1HOST, m_opcSrv1Name);

            //get all Sample Groups
            List<EtyOPCSampleGroup> etySampleGrpList = OPCSampleGroupDAO.GetInstance().GetAllEnabledSampleGrp();
            foreach (EtyOPCDataPoint etyDP in etyDataPointList)
            {
                OPCDPGrpDetails opcDPGrp = new OPCDPGrpDetails();
                opcDPGrp.DT_PT_Name = etyDP.OPCDataPointName;
                EtyOPCSampleGroup etySampleGrp = etySampleGrpList.Find(delegate(EtyOPCSampleGroup obj) { return obj.SampleGrpID == etyDP.OPCSampleGrpId; });
                //add only if the sample group is also enabled.
                if (etySampleGrp != null)
                {
                    opcDPGrp.Delta = etySampleGrp.DeltaValue;
                    double interval = etySampleGrp.Interval;
                    if (etySampleGrp.IntervalType == DAOHelper.HOUR)
                    {
                        interval = etySampleGrp.Interval * MINUTES_UNIT * SECONDS_UNIT;
                    }
                    else if (etySampleGrp.IntervalType == DAOHelper.MINITE)
                    {
                        interval = etySampleGrp.Interval * SECONDS_UNIT;
                    }
                    opcDPGrp.Interval = interval;
                    if (etySampleGrp.StartTime.Trim() != "")
                    {
                        DateTime currentTime = System.DateTime.Now;
                        DateTime startDatetime = new DateTime(currentTime.Year, currentTime.Month, currentTime.Day, int.Parse(etySampleGrp.StartTime.Substring(0, 2)), int.Parse(etySampleGrp.StartTime.Substring(3, 2)), 0);
           
                        //calculate the next time for logging.
                        opcDPGrp.NextSampleTime = GetNextSampleTime(startDatetime, interval, true);
                    }
                    m_DataPointdic.Add(etyDP.OPCDataPointName, opcDPGrp);
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Connects to OPC server and connects the loadded datapoints.
        /// </summary>
        /// <returns></returns>
        public bool InitializeAndConnectOPCServer()
        {
            string Function_Name = "InitializeAndConnectOPCServer";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_opcSrv1Name.Trim() != "" && OPCSVR1HOST.Trim() != "")
            {
                OPCServerProxy.GetInstance().InitializeServer(m_opcSrv1Name, m_processID);
            }

            if (OPCServerProxy.GetInstance().ReconnectAndAddToOPC())
            {
                ConnectDPToOPC();
            }
            else
            {
                LogHelper.Error("No Connection to OPC Server");
                return false;
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return true;
        }

        /// <summary>
        /// Calculates the next log time based on given inputs.
        /// </summary>
        /// <param name="startTime">start time</param>
        /// <param name="intervalInSecs">interval in seconds</param>
        /// <param name="firstCall">true - first calculation for datapoints</param>
        /// <returns></returns>
        public DateTime GetNextSampleTime(DateTime startTime, double intervalInSecs, bool firstCall)
        {
            string Function_Name = "GetNextSampleTime";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            DateTime currentTime = System.DateTime.Now;
            currentTime = new DateTime(currentTime.Year, currentTime.Month, currentTime.Day, currentTime.Hour, currentTime.Minute, currentTime.Second, 0);
            DateTime nextSampleTime = startTime;
            if (!firstCall)
            {
                nextSampleTime = startTime.AddSeconds(intervalInSecs);
            }
           
            //if the start time is passed in current day, set next time 
            if (DateTime.Compare(currentTime, startTime) > 0)
            {
             //  nextSampleTime = tempDateTime.AddDays(1);
                TimeSpan diff = (currentTime - startTime);
                int remainder =(int) (diff.TotalSeconds % intervalInSecs);
                if(remainder == 0)
                {
                    if (firstCall)
                    {
                        nextSampleTime = currentTime;
                    }
                    else
                    {
                        nextSampleTime = currentTime.AddSeconds(intervalInSecs);
                    }
                }
                else
                {
                    nextSampleTime = currentTime.AddSeconds(intervalInSecs - remainder);                   
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return nextSampleTime;   
        }


        /// <summary>
        /// Connects the configured datapoints to the OPC Group in OPC Server.
        /// </summary>
        public void ConnectDPToOPC()
        {
            string Function_Name = "ConnectDPToOPC";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            try
            {

                foreach (KeyValuePair<string, OPCDPGrpDetails> pair in m_DataPointdic)
                {
                    OPCServerProxy.GetInstance().AddOPCItem(pair.Key);
                }            

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");

        }

        /// <summary>
        /// Stops logging thread.
        /// </summary>
        public void TerminateAndWait()
        {
            string Function_Name = "TerminateAndWait";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            lock (m_sharedThreadObjLock)
            {
                    m_loggerQuene.TerminateAndWait();                          
                    m_writeQuene.TerminateAndWait();                                 
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }     

        /// <summary>
        /// Gets logger configured DataPoints.
        /// </summary>
        /// <returns></returns>
        public Dictionary<string, OPCDPGrpDetails> GetOPCLoggerDataPoints()
        {
            Dictionary<string, OPCDPGrpDetails> datapointList = new Dictionary<string, OPCDPGrpDetails>();
            lock (m_sharedVarLock)
            {
                datapointList = m_DataPointdic;
            }
            return datapointList;
        }

        /// <summary>
        /// Parses the value as numeric.
        /// </summary>
        /// <param name="val"></param>
        /// <param name="NumberStyle"></param>
        /// <returns></returns>
        public bool isNumeric(string val, System.Globalization.NumberStyles NumberStyle)
        {
            Double result;
            return Double.TryParse(val, NumberStyle, System.Globalization.CultureInfo.CurrentCulture, out result);
        }

        /// <summary>
        /// Parses the value as boolean.
        /// </summary>
        /// <param name="val"></param>
        /// <returns></returns>
        public bool isBoolean(string val)
        {
            bool result;
            return Boolean.TryParse(val, out result);
        }

        public int GetLoggerInterval()
        {
            int retval;
            lock (m_sharedVarLock)
            {
                retval = m_OPCLogInterval;
            }
            return retval;
        }

        public void PushToWriteQuene(EtyTrendLog etyTrendLog)
        {
            lock (m_sharedThreadObjLock)
            {
                m_writeQuene.PushToQuene(etyTrendLog);
            }
        }

    }
}
