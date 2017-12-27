using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

using STEE.ISCS.Log;

using OPCTrendLib.OPCHeader;
using OPCTrendLib.OPCDataInterface;
using OPCTrendLib.OPCData;

using System.Runtime.InteropServices;

namespace STEE.ISCS.OPCProxy
{
    /// <summary>
    /// This class wraps functions which are used to subscribe OPC data change notification.
    /// It also provide data cache for these subscribed OPC data point.
    /// </summary>
    public class OPCServerProxy
    {
        private const string CLASS_NAME = "STEE.ISCS.OPCProxy";

        private Object m_opcDataDicObj = new Object();
        //dictionary<handleClient,OPCDataItem>
        private Dictionary<int, OPCDataItem> m_opcDataDic;
        //dictionary<DataPointName,handleClient>
        private Dictionary<string, int> m_opcDataIndexDic;
        // List of observer for notifying the shutdown event.
        private List<IOPCObserver> m_observers;
        //List of Datapoints failed while Add it to OPC group
        //mainly due to UNKNOWNITEMID error code.
        private List<string> m_TotryQuene;

        private static Object s_Singleton = new Object();
        private static OPCServerProxy s_OPCServerProxy;

        private OpcServer m_OPCServer = null;
        private OpcGroup  m_OPCGroup  = null;
        private string m_OPCSrvName = "";
        private string m_OPCGroupName = "";
        private int m_OPCDataSeqNum = 0;
        private bool m_needToReCreateGrp2Srv = false;
        private bool m_opcSrvConnectFlag = false;

        private const int UPDATEGROUPRATE = 500;
        private const int OPC_GOODQUALITY=192;
        private const int OPC_NOTCONNECTEDQUALITY = 8;
        public static string OPC_NOTCONNECTED = "NOTCONNECTED";
        public static string OPC_BLANK_DATAVALUE = "BLANK";

        private DateTime? m_ShutdownEventTime = null;

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public static OPCServerProxy GetInstance()
        {
            if (s_OPCServerProxy != null)  return s_OPCServerProxy;
            lock(s_Singleton)
            {
                if (s_OPCServerProxy == null)
                {
                    LogHelper.Debug("OPCServerProxy Singleton instance created!");
                    s_OPCServerProxy = new OPCServerProxy();
                }
            }
            return s_OPCServerProxy;
        }

        /// <summary>
        /// 
        /// </summary>
        private OPCServerProxy()
        {
            m_opcDataDic = new Dictionary<int, OPCDataItem>();
            m_opcDataIndexDic = new Dictionary<string, int>();
            m_observers = new List<IOPCObserver>();
            m_TotryQuene = new List<string>();            
        }       

        /// <summary>
        /// initialize OPC server and group
        /// </summary>
        /// <param name="serverName">OPC Server Name</param>
        /// <param name="groupName">OPC Group name under the OPC server. 
        /// Here groupName is trendViewer Process ID in string</param>
        /// 
        public void InitializeServer(string serverName,string groupName)
        {
            string Function_Name = "InitializeServer";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            lock (m_opcDataDicObj)
            {
                try
                {
                    m_OPCSrvName = serverName;
                    m_OPCGroupName = groupName;

                    //Fix - Remove opcGroup and Disconnect server to avoid GC Fianlizer thread call these methods
                    try
                    {
                        if (m_OPCGroup != null)
                        {
                            m_OPCGroup.Remove(false);
                            m_OPCGroup = null;
                        }

                        if (m_OPCServer != null)
                        {
                            m_OPCServer.Disconnect();
                            m_OPCServer = null;
                        }
                    }
                    catch (Exception localException)
                    {
                       LogHelper.Error(CLASS_NAME, Function_Name, localException);
                    }
                    
                    m_OPCServer = new OpcServer();

                    LogHelper.Trace(CLASS_NAME, Function_Name, "Before Connect");
                    m_OPCServer.Connect(serverName);
                    LogHelper.Trace(CLASS_NAME, Function_Name, "After Connect");

                    Thread.Sleep(200);
                    m_OPCServer.SetClientName("DirectOPC " + groupName + serverName);	// set my client name (exe+process no)

                    SERVERSTATUS sts;
                    m_OPCServer.GetStatus(out sts);
                    LogHelper.Info(CLASS_NAME, Function_Name, serverName + ": " + DateTime.FromFileTime(sts.ftStartTime).ToString());
                    LogHelper.Info(CLASS_NAME, Function_Name, serverName + ": " + sts.eServerState.ToString());

                    // add event handler for server shutdown
                    m_OPCServer.ShutdownRequested += new ShutdownRequestEventHandler(this.OPCSrv_ServerShutDown);

                    //Setup opc group
                    m_OPCGroup = m_OPCServer.AddGroup("MFTOPC-Group-" + serverName, true, UPDATEGROUPRATE);
                    m_OPCGroup.DataChanged += new DataChangeEventHandler(this.OPCGrp_DataChange);
                    m_opcSrvConnectFlag = true;
                }
                catch (COMException localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, localException);
                    LogHelper.Info(CLASS_NAME, Function_Name, "Initialize of the OPC Server is not successful");
                }
                catch (Exception localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, localException);
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// This is server shutdown Event Handler function. 
        /// This method is responsible for notifying the event to its observers.
        /// </summary>
        /// <param name="sender">opc server</param>
        /// <param name="e">Shutdown event</param>
        protected void OPCSrv_ServerShutDown(object sender, ShutdownRequestEventArgs e)
        {
            string Function_Name = "opcSrv_ServerShutDown";
            LogHelper.Trace(CLASS_NAME, Function_Name, "OPC server shuts down because:" + e.shutdownReason);
            lock (m_opcDataDicObj)
            {
                  if (m_opcSrvConnectFlag)
                  {
                      m_opcSrvConnectFlag = false;
                      m_ShutdownEventTime = DateTime.Now;
                      LogHelper.Info(CLASS_NAME, Function_Name, string.Format("Shutdown Event Occured at time = {0}", m_ShutdownEventTime.ToString()));
                      NotifyShutDownToObservers();
                  }                  
            }
          LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Notify ShutDown Event to observers
        /// </summary>
        private void NotifyShutDownToObservers()
        {      
            //notifiy the observers
            foreach (IOPCObserver observer in m_observers)
            {
                observer.ServerShutdownEvent();
            }
        }

  
        /// <summary>
        /// It is used to subscribe data change notification from OPC server
        /// </summary>
        /// <param name="sender">sender</param>
        /// <param name="e">inlcude all updated OPC data point</param>
        private void OPCGrp_DataChange(object sender, DataChangeEventArgs e)
       {
           string Function_Name = "opcGrp_DataChange";
           LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
           try
           {
               lock (m_opcDataDicObj)
               {
                   foreach (OPCItemState s in e.sts)
                   {
                       if (m_opcDataDic.ContainsKey(s.HandleClient))
                       {
                           if (HRESULTS.Succeeded(s.Error))
                           {
                               if (s.Quality == OPC_GOODQUALITY)
                               {
                                   LogHelper.Debug(CLASS_NAME, Function_Name, "DataValue = " + s.DataValue.ToString());
                                   m_opcDataDic[s.HandleClient].DataValue = s.DataValue;
                               }
                               else
                               {
                                  // m_needToReCreateGrp2Srv = true; 

                                   if (s.Quality == OPC_NOTCONNECTEDQUALITY)  // means the datapoint Quality is "NOT_CONNECT"
                                   {
                                       m_needToReCreateGrp2Srv = true;  // next time plotting, will re-create group to OpcSrv1.

                                       if (m_opcDataDic[s.HandleClient].DataValue.ToString().CompareTo(OPC_NOTCONNECTED) != 0)  //if last value was not "NOTCONNECTED"
                                       {
                                           LogHelper.Info(CLASS_NAME, Function_Name, "DataPoint:" + m_opcDataDic[s.HandleClient].ID +
                                          " is not connected! ");
                                           m_opcDataDic[s.HandleClient].DataValue = OPC_NOTCONNECTED;

                                       }
                                   }
                                   else
                                   {
                                       LogHelper.Info(CLASS_NAME, Function_Name, "For DataPoint: " + m_opcDataDic[s.HandleClient].ID + ", OPCItemState quality not good, Quality = " + s.Quality);
                                       m_opcDataDic[s.HandleClient].DataValue = OPC_BLANK_DATAVALUE;
                                   }

                               }
                               LogHelper.Debug(CLASS_NAME, Function_Name, "The new value for datapoint[" + m_opcDataDic[s.HandleClient].ID +"] is " + m_opcDataDic[s.HandleClient].DataValue.ToString());
                           }
                       }
                   }
               }
           }
           catch (Exception localException)
           {
               LogHelper.Error(CLASS_NAME, Function_Name, localException);
           }

           LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
       }

        /// <summary>
        /// Checks whether OPC Server is connected or not. If not, try to connect again.
        /// Also checks whether there is need to recreate group and connect OPC Items to it. 
        /// </summary>
        /// <returns>true - OPC Server is connected, false - not connected</returns>
        public bool ReconnectAndAddToOPC()
        {
            string Function_Name = "ReconnectAndAddToOPC";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool bReConnect = false;
            if (!IsOPCServerConnected())
            {
                DateTime currrentTime = DateTime.Now;
                // check elapsed time between shutdown event and now before reconnecting
                //to avoid false shutdown event raised
                if (m_ShutdownEventTime != null)
                {
                    DateTime shutDownTime = (DateTime) m_ShutdownEventTime;
                    TimeSpan elapsedTime = currrentTime - shutDownTime;
                    LogHelper.Info(CLASS_NAME, Function_Name, string.Format("Elapsed time for shutdown = {0}", elapsedTime.TotalSeconds));
                    if (elapsedTime.TotalMilliseconds < 5000)
                    {
                        double sleepTime = 5000 - elapsedTime.TotalMilliseconds;
                        System.Threading.Thread.Sleep((int) sleepTime);
                    }
                    m_ShutdownEventTime = null;
                }               

                InitializeServer(m_OPCSrvName, m_OPCGroupName);
                if (!IsOPCServerConnected())
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, "Cannot able to connect to OPC Server.Function_Exited with return value false.");
                    return false;
                }
                bReConnect = true;
            }
            if (IsNeedToReCreateOPCGrp() && !bReConnect)
            {
                ReleaseOPC();
                InitializeServer(m_OPCSrvName, m_OPCGroupName);
                if (!IsOPCServerConnected() || (GetOPCGrp() == null))
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, "Cannot able to connect to OPC Server.Function_Exited with return value false.");
                    return false;
                }
                bReConnect = true;
            }
            if (bReConnect)
            {
                //reconnect the datapoints to OPC Grp                
                int count = 0;
                List<string> opcDataCopyList = new List<string>();
                lock (m_opcDataDicObj)
                {
                    if ((m_OPCDataSeqNum == 0) && (m_TotryQuene.Count == 0))
                    {
                        //no datapoints to be added
                        return true;
                    }
                    for (count = 0; count < m_OPCDataSeqNum; count++)
                    {
                        OPCDataItem opcDataItemVar = m_opcDataDic[count];
                        opcDataCopyList.Add(opcDataItemVar.ID);
                    }
                    foreach (string opcID in m_TotryQuene)
                    {
                        opcDataCopyList.Add(opcID);
                    }
                }
                ClearDataItems();                
                foreach(string opcItemID in opcDataCopyList)
                {
                    bool ret = AddOPCItem(opcItemID);
                    //if (!ret)
                    //{
                        //todo
                    //}
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return true;
        }

        /// <summary>
        /// Clears the datapoint details
        /// </summary>
        private void ClearDataItems()
        {
            lock (m_opcDataDicObj)
            {
                m_opcDataDic.Clear();
                m_opcDataIndexDic.Clear();
                m_TotryQuene.Clear();
                m_OPCDataSeqNum = 0;               
            }
        }


        /// <summary>
        /// Add unique OPC item to group
        /// </summary>
        /// <param name="opcID">OPC Item name</param>
        /// <returns>false - not added to OPC group, true - added successfully</returns>
        public bool AddOPCItem(string opcID)
        {
            string Function_Name = "AddOPCItem";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered for datapoint {0}", opcID));
            if (opcID.Trim() == "")
            {
                LogHelper.Debug(CLASS_NAME, Function_Name, "New Datapoint name is empty.Function_Exited with return value false.");
                return false;
            }

            int currentSeq =0;
            OPCDataItem opcDataItemVar = null;       
            lock (m_opcDataDicObj)
            {
                if (m_opcDataIndexDic.ContainsKey(opcID))
                {
                    LogHelper.Info(CLASS_NAME, Function_Name, "Already the datapoint is added.Function_Exited");
                    return true;
                }
                currentSeq = m_OPCDataSeqNum++;
                opcDataItemVar = new OPCDataItem(opcID, currentSeq);
                //add to internal map
                m_opcDataIndexDic.Add(opcID, currentSeq);
                m_opcDataDic.Add(currentSeq, opcDataItemVar);
            }

            if (!IsOPCServerConnected())
            {
                LogHelper.Debug(CLASS_NAME, Function_Name, "OPC Server is not connect. Reconnect before adding datapoints.");
                return false;
            }

            try
            {
                LogHelper.Trace(CLASS_NAME, Function_Name, "Connect to OPC Group");
                //connect to OPC Group
                OPCItemDef[] aD = new OPCItemDef[1];
                aD[0] = new OPCItemDef(opcID, true, currentSeq, VarEnum.VT_EMPTY);
                OPCItemResult[] arrRes;
                {
                    lock (m_opcDataDicObj)
                    {
                        m_OPCGroup.AddItems(aD, out arrRes);
                    }
                }
                if (arrRes == null)
                {
                    LogHelper.Info(CLASS_NAME, Function_Name, "OPC Group AddItems() return null(cant not find handle server). Function Exited with return value false.");
                    return false;
                }
                if (arrRes[0].Error != HRESULTS.S_OK)
                {
                    LogHelper.Info(CLASS_NAME, Function_Name, string.Format("OPC Group AddItems() return Error code({0}, not OK) for DataPoint = {1}. Function Exited with return value false.", arrRes[0].Error,opcID));
                    lock (m_opcDataDicObj)
                    {
                        m_OPCDataSeqNum--;                       
                        m_opcDataIndexDic.Remove(opcID);
                        m_opcDataDic.Remove(currentSeq);
                        m_TotryQuene.Add(opcID);
                    }
                    return false;
                }

                opcDataItemVar.HandleServer = arrRes[0].HandleServer;

                OPCACCESSRIGHTS itmAccessRights = arrRes[0].AccessRights;
                TypeCode itmTypeCode = VT2TypeCode(arrRes[0].CanonicalDataType);

                if ((itmAccessRights & OPCACCESSRIGHTS.OPC_READABLE) != 0)
                {
                    int cancelID;
                    lock (m_opcDataDicObj)
                    {
                        m_OPCGroup.Refresh2(OPCDATASOURCE.OPC_DS_DEVICE, 7788, out cancelID);
                    }                   
                }
                else
                {
                    LogHelper.Info(CLASS_NAME, Function_Name, "Can find a handle server, but not have access right.Function Exited with return value false.");
                    return false;
                }

            }
            catch (COMException localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, string.Format("{0} for DataPoint - {1}",localException.ToString(),opcID));
                // if (localException.Message.Contains("The RPC server is unavailable.")) 
				// Fixed issue - related to System Language, to avoid it used Error code
                if (localException.Message.Contains("0x800706BA"))
                {                 
                    lock (m_opcDataDicObj)
                    {
                        m_opcSrvConnectFlag = false;
                    }                   
                }                
                return false;
            }
            catch(Exception exception)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, string.Format("{0} for DataPoint - {1}", exception.ToString(), opcID));               
                return false;
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return true;
        }

        /// <summary>
        ///  This function is responsible for adding those Datapoint to the OPC Group which failed last time.
        /// </summary>
        public void AddFailedOPCItems()
        {
            string Function_Name = "AddFailedOPCItems";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            List<string> ToTryCache = null;
            lock (m_opcDataDicObj)
            {
                if (0 == m_TotryQuene.Count)
                {
                    LogHelper.Trace(CLASS_NAME, Function_Name, "There is no OPC items which failed to be added to OPC group.");
                    return;
                }
                ToTryCache = new List<string>(m_TotryQuene);
                m_TotryQuene.Clear();
            }          

            foreach(string opcId in ToTryCache)
            {
                AddOPCItem(opcId);
            }

            //clear
            ToTryCache.Clear();
            ToTryCache = null;
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }
    
        /// <summary>
        /// Removes OPC Item from OPC Group
        /// </summary>
        /// <param name="opcID">OPC item name</param>
        /// <returns>sucessfully removed or not</returns>
        /// OPCID is the Datapoint Name 
        public bool RemoveOPCItem(string opcID)
        {
            string Function_Name = "RemoveOPCItem";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            lock (m_opcDataDicObj)
            {
                if (!m_opcDataIndexDic.ContainsKey(opcID))
                {
                    LogHelper.Info(CLASS_NAME, Function_Name, "Function_Exited");
                    return true;
                }

                //remove from OPC Group
                int handleClient = m_opcDataIndexDic[opcID];
                if (!m_opcDataDic.ContainsKey(handleClient))
                {
                    //error
                    LogHelper.Info(CLASS_NAME, Function_Name, "Function_Exited");
                    return false;
                }

                OPCDataItem opcDataItemVar = m_opcDataDic[handleClient];
                try
                {

                    int[] serverhandles = new int[1] { opcDataItemVar.HandleServer };
                    int[] remerrors;
                    if (m_OPCGroup != null)
                    {
                        m_OPCGroup.RemoveItems(serverhandles, out remerrors);
                    }

                    //remove from internal map
                    m_opcDataIndexDic.Remove(opcID);
                    m_opcDataDic.Remove(handleClient);
                    m_OPCDataSeqNum--;
                }
                catch (Exception localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, localException);
                    //if (localException.Message.Contains("The RPC server is unavailable."))
					// Fixed issue - related to System Language, to avoid it used Error code
                    if (localException.Message.Contains("0x800706BA"))
                    {
                        //when opc server is crashed.
                       throw localException;
                    }
                    return false;
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
             return true;
        }

        /// <summary>
        /// Remove all registered OPC items
        /// </summary>
        /// <returns>true: successfully; false: failed</returns>
        public bool RemoveAllOPCItem()
        {
            string Function_Name = "RemoveAllOPCItem";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool removeSuccess = true;
            int totalCount = 0;
            int count = 0;
            Dictionary<int, OPCDataItem> opcDataCopy = null;
            lock (m_opcDataDicObj)
            {
                totalCount = m_OPCDataSeqNum;
                opcDataCopy = new Dictionary<int, OPCDataItem>(m_opcDataDic);
            }
            for (; count < totalCount;count++ )
            {
                OPCDataItem opcDataItem = opcDataCopy[count];
                bool ret = true;
                try
                {
                     ret = RemoveOPCItem(opcDataItem.ID);
                }
                catch (Exception exception)
                {
                    //when opcserver crashed
                    ClearDataItems();                    
                    lock (m_opcDataDicObj)
                    {
                        NotifyShutDownToObservers();
                        m_opcSrvConnectFlag = false;
                    }
                    throw exception;
                }
                if (ret != true)
                {
                    removeSuccess = false;
                }
            }
            lock (m_opcDataDicObj)
            {
                if (m_opcDataIndexDic.Count != 0)
                {
                    //todo error
                    //not possible
                }              
            }

            ClearDataItems();
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return removeSuccess;
        }


        /// <summary>
        /// Returns data item value based on OPC Item name
        /// </summary>
        /// <param name="itemID">OPC item name</param>
        /// <returns>value object</returns>
        public Object GetDataItemValue(string itemID)
        {
            if (!m_opcDataIndexDic.ContainsKey(itemID))
            {
                //not found the datapoint item
                return null;
            }
            OPCDataItem opcDataItemVar;
            if (m_opcDataDic.TryGetValue(m_opcDataIndexDic[itemID], out opcDataItemVar))
            {
                return opcDataItemVar.DataValue;
            }
            return null;
        }


        /// <summary>
        /// Release OPC resource
        /// </summary>
        /// <returns></returns>
        public void ReleaseOPC()
        {   
            //release the OPC Group first then OPCServer
            lock (m_opcDataDicObj)
            {
                if (m_OPCGroup != null)
                {
                    m_OPCGroup.DataChanged -= new DataChangeEventHandler(this.OPCGrp_DataChange);
                    m_OPCGroup.Remove(false);
                    m_OPCGroup = null;
                }

                if (m_OPCServer != null)
                {
                    m_OPCServer.ShutdownRequested -= new ShutdownRequestEventHandler(this.OPCSrv_ServerShutDown);
                    m_OPCServer.Disconnect();
                    m_OPCServer = null;
                }
                m_opcSrvConnectFlag = false;
               
            }            
        }

        /// <summary>
        /// Release singleton object
        /// </summary>
        public static void ReleaseInstance()
        {
            if (s_OPCServerProxy == null) return;
            lock (s_Singleton)
            {
                try
                {

                    if (s_OPCServerProxy != null)
                    {
                        s_OPCServerProxy.RemoveAllOPCItem();
                        s_OPCServerProxy.ReleaseOPC();
                        s_OPCServerProxy.m_observers.Clear();
                        s_OPCServerProxy.m_TotryQuene.Clear();
                        //s_OPCServerProxy.m_observers = null;
                        //s_OPCServerProxy.m_TotryQuene = null;
                        //s_OPCServerProxy = null;
                    }
                }
                catch (Exception ex)
                {
                    LogHelper.Error(ex.ToString());
                }
                finally
                {
                    s_OPCServerProxy.m_observers = null;
                    s_OPCServerProxy.m_TotryQuene = null;
                    s_OPCServerProxy = null;
                }
            }
        }

        /// <summary>
        /// Checks whether there is need for recreating OPC Group and its item.
        /// </summary>
        /// <returns></returns>
        private bool IsNeedToReCreateOPCGrp()
        {
            bool ret = false;
            lock (m_opcDataDicObj) 
            {
                ret = m_needToReCreateGrp2Srv;
            }
            return ret;
        }

        /// <summary>
        /// Disables the need to recrete OPC Group and its item.
        /// </summary>
        private void DisbaleReCreateGrp2SrvFlag()
        {
            lock (m_opcDataDicObj) 
            {
                m_needToReCreateGrp2Srv = false;
            }
        }

        /// <summary>
        /// Returns whether Server is connected or not.
        /// </summary>
        /// <returns></returns>
        public bool IsOPCServerConnected()
        {
            string Function_Name = "IsOPCServerConnected";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool ret = false;
            lock (m_opcDataDicObj)
            {
                if (m_opcSrvConnectFlag)
                {
                    SERVERSTATUS sts = new SERVERSTATUS();
                    sts.eServerState = OPCSERVERSTATE.OPC_STATUS_RUNNING;
                    try
                    {
                        m_OPCServer.GetStatus(out sts);

                        if (sts.eServerState != OPCSERVERSTATE.OPC_STATUS_RUNNING)
                        {
                            LogHelper.Info(CLASS_NAME, Function_Name, string.Format("status is {0}", sts.eServerState.ToString()));
                        }
                        //bool statusFlag = (sts.eServerState == OPCSERVERSTATE.OPC_STATUS_RUNNING) ? true : false;
                    }
                    catch (Exception localException)
                    {
                        LogHelper.Error(CLASS_NAME, Function_Name, localException);
                        //check is server is ok?
                        //if (localException.Message.Contains("The RPC server is unavailable."))
						// Fixed issue - related to System Language, to avoid it used Error code
                        if (localException.Message.Contains("0x800706BA"))
                        {
                            m_opcSrvConnectFlag = false;
                        }                        
                    }
                }                
                ret = m_opcSrvConnectFlag;
            }

            if (!ret)
            {
                LogHelper.Debug(CLASS_NAME, Function_Name, "OPC Server status is Down!");
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return ret;
        }

        /// <summary>
        /// Add Observer inorder receive Server Shutdown event notification.
        /// </summary>
        /// <param name="observer"></param>
        public void AddObserver(IOPCObserver observer)
        {
            string Function_Name = "AddObserver";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            lock (m_opcDataDicObj)
            {
                this.m_observers.Add(observer);
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Remove the Observer from its list
        /// </summary>
        /// <param name="observer"></param>
        public void RemoveObserver(IOPCObserver observer)
        {
            string Function_Name = "RemoveObserver";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            lock (m_opcDataDicObj)
            {
                try
                {
                    this.m_observers.Remove(observer);
                }
                catch (Exception execption)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, execption);
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Get current OPC Group.
        /// </summary>
        /// <returns></returns>
        private OpcGroup GetOPCGrp()
        {
            OpcGroup ret = null;
            lock (m_opcDataDicObj)
            {
                ret = m_OPCGroup;
            }
            return ret;
        }

        /// <summary>
        /// Ulitility function for type conversion.
        /// </summary>
        /// <param name="vevt">VarEnum type</param>
        /// <returns>TypeCode type</returns>
        private TypeCode VT2TypeCode(VarEnum vevt)
        {
            switch (vevt)
            {
                case VarEnum.VT_I1:
                    return TypeCode.SByte;
                case VarEnum.VT_I2:
                    return TypeCode.Int16;
                case VarEnum.VT_I4:
                    return TypeCode.Int32;
                case VarEnum.VT_I8:
                    return TypeCode.Int64;

                case VarEnum.VT_UI1:
                    return TypeCode.Byte;
                case VarEnum.VT_UI2:
                    return TypeCode.UInt16;
                case VarEnum.VT_UI4:
                    return TypeCode.UInt32;
                case VarEnum.VT_UI8:
                    return TypeCode.UInt64;

                case VarEnum.VT_R4:
                    return TypeCode.Single;
                case VarEnum.VT_R8:
                    return TypeCode.Double;

                case VarEnum.VT_BSTR:
                    return TypeCode.String;
                case VarEnum.VT_BOOL:
                    return TypeCode.Boolean;
                case VarEnum.VT_DATE:
                    return TypeCode.DateTime;
                case VarEnum.VT_DECIMAL:
                    return TypeCode.Decimal;
                case VarEnum.VT_CY:				// not supported
                    return TypeCode.Double;
            }

            return TypeCode.Object;
        }

        
    }
}
