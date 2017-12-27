namespace OPCTrendLib.OPCData
{
    using OPCTrendLib;
    using OPCTrendLib.OPCDataInterface;
    using OPCTrendLib.OPCHeader;
    using System;
    using System.Collections;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;
    using System.Runtime.InteropServices.ComTypes;
    using STEE.ISCS.Log;


    [ComVisible(true)]
    public class OpcServer : IOPCShutdown
    {
        //private UCOMIConnectionPointContainer cpointcontainer = null;
        private IConnectionPointContainer cpointcontainer = null;
        private IOPCBrowseServerAddressSpace ifBrowse = null;
        private IOPCCommon ifCommon = null;
        private IOPCItemProperties ifItmProps = null;
        private IOPCServer ifServer = null;
        public GeneralFunction localFunction = new GeneralFunction();
        private object OPCserverObj = null;
        private int shutdowncookie = 0;
       // private UCOMIConnectionPoint shutdowncpoint = null;
        private IConnectionPoint shutdowncpoint = null;


        public event ShutdownRequestEventHandler ShutdownRequested;

        public OpcGroup AddGroup(string groupName, bool setActive, int requestedUpdateRate)
        {
            return this.AddGroup(groupName, setActive, requestedUpdateRate, null, null, 0);
        }

        public OpcGroup AddGroup(string groupName, bool setActive, int requestedUpdateRate, int[] biasTime, float[] percentDeadband, int localeID)
        {
            if (this.ifServer == null)
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            OpcGroup group = new OpcGroup(ref this.ifServer, false, groupName, setActive, requestedUpdateRate);
            group.internalAdd(biasTime, percentDeadband, localeID);
            return group;
        }

        private void AdviseIOPCShutdown()
        {
            Type type = typeof(IOPCShutdown);
            Guid gUID = type.GUID;
            this.cpointcontainer.FindConnectionPoint(ref gUID, out this.shutdowncpoint);
            if (this.shutdowncpoint != null)
            {
                this.shutdowncpoint.Advise(this, out this.shutdowncookie);
            }
        }

        public void Browse(OPCBROWSETYPE typ, out ArrayList lst)
        {
            IEnumString str;
            lst = null;
            this.BrowseOPCItemIDs(typ, "", VarEnum.VT_EMPTY, 0, out str);
            if (str != null)
            {
                int num2;
                lst = new ArrayList(500);
                string[] rgelt = new string[100];
                do
                {
                    //int pceltFetched = 0;
                    IntPtr pceltFetchedPtr = new IntPtr(0);
                    num2 = str.Next(100, rgelt, pceltFetchedPtr);
                    int pceltFetched = pceltFetchedPtr.ToInt32();
                    if (pceltFetched > 0)
                    {
                        for (int i = 0; i < pceltFetched; i++)
                        {
                            lst.Add(rgelt[i]);
                        }
                    }
                }
                while (num2 == 0);
                int num4 = Marshal.ReleaseComObject(str);
                str = null;
                lst.TrimToSize();
            }
        }

        public void BrowseAccessPaths(string itemID, out IEnumString stringEnumerator)
        {
            object obj2;
            stringEnumerator = null;
            this.ifBrowse.BrowseAccessPaths(itemID, out obj2);
            stringEnumerator = (IEnumString) obj2;
            obj2 = null;
        }

        public void BrowseOPCItemIDs(OPCBROWSETYPE filterType, string filterCriteria, VarEnum dataTypeFilter, OPCACCESSRIGHTS accessRightsFilter, out IEnumString stringEnumerator)
        {
            object obj2;
            stringEnumerator = null;
            this.ifBrowse.BrowseOPCItemIDs(filterType, filterCriteria, (short) dataTypeFilter, accessRightsFilter, out obj2);
            stringEnumerator = (IEnumString) obj2;
            obj2 = null;
        }

        public void ChangeBrowsePosition(OPCBROWSEDIRECTION direction, string name)
        {
            this.ifBrowse.ChangeBrowsePosition(direction, name);
        }

        public void Connect(string clsidOPCserver)
        {
            this.Disconnect();
            Type typeFromProgID = Type.GetTypeFromProgID(clsidOPCserver);
            if (typeFromProgID == null)
            {
                Marshal.ThrowExceptionForHR(-1073479663);
            }
            this.OPCserverObj = Activator.CreateInstance(typeFromProgID);
            this.ifServer = (IOPCServer) this.OPCserverObj;
            if (this.ifServer == null)
            {
                Marshal.ThrowExceptionForHR(-2147220992);
            }
            this.ifCommon = (IOPCCommon) this.OPCserverObj;
            this.ifBrowse = (IOPCBrowseServerAddressSpace) this.ifServer;
            this.ifItmProps = (IOPCItemProperties) this.ifServer;
            this.cpointcontainer = (IConnectionPointContainer) this.OPCserverObj;
            this.AdviseIOPCShutdown();
        }

        public void Disconnect()
        {
            int num;
            try
            {
                if (this.shutdowncpoint != null)
                {
                    if (this.shutdowncookie != 0)
                    {
                        try
                        {
                            this.shutdowncpoint.Unadvise(this.shutdowncookie);
                        }
                        catch (Exception exception)
                        {
                            LogHelper.Error("OPCTrendLib.OPCData.OPCServer", "Disconnect", exception);
                        }
                        this.shutdowncookie = 0;
                    }
                    num = Marshal.ReleaseComObject(this.shutdowncpoint);
                    this.shutdowncpoint = null;
                }
                this.cpointcontainer = null;
                this.ifBrowse = null;
                this.ifItmProps = null;
                this.ifCommon = null;
                this.ifServer = null;
                if (this.OPCserverObj != null)
                {
                    num = Marshal.ReleaseComObject(this.OPCserverObj);
                    this.OPCserverObj = null;
                }
            }
            catch (Exception exception)
            {
                LogHelper.Error("OPCTrendLib.OPCData.OPCServer", "Disconnect", exception);
            }
            finally
            {
                if (this.shutdowncpoint != null)
                {
                    this.shutdowncpoint = null;
                }
                if (this.OPCserverObj != null)
                {
                    this.OPCserverObj = null;
                }
            }
        }

        ~OpcServer()
        {
            this.Disconnect();
        }

        public string GetErrorString(int errorCode, int localeID)
        {
            string str;
            this.ifServer.GetErrorString(errorCode, localeID, out str);
            return str;
        }

        public string GetItemID(string itemDataID)
        {
            string str;
            this.ifBrowse.GetItemID(itemDataID, out str);
            return str;
        }

        public bool GetItemProperties(string itemID, int[] propertyIDs, out OPCPropertyData[] propertiesData)
        {
            IntPtr ptr;
            IntPtr ptr2;
            propertiesData = null;
            int length = propertyIDs.Length;
            if (length < 1)
            {
                return false;
            }
            int hresultcode = this.ifItmProps.GetItemProperties(itemID, length, propertyIDs, out ptr, out ptr2);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            int num3 = (int) ptr;
            int num4 = (int) ptr2;
            if ((num3 == 0) || (num4 == 0))
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            propertiesData = new OPCPropertyData[length];
            for (int i = 0; i < length; i++)
            {
                propertiesData[i] = new OPCPropertyData();
                propertiesData[i].PropertyID = propertyIDs[i];
                propertiesData[i].Error = Marshal.ReadInt32((IntPtr) num4);
                num4 += 4;
                if (propertiesData[i].Error == 0)
                {
                    propertiesData[i].Data = Marshal.GetObjectForNativeVariant((IntPtr) num3);
                    DUMMY_VARIANT.VariantClear((IntPtr) num3);
                }
                else
                {
                    propertiesData[i].Data = null;
                }
                num3 += DUMMY_VARIANT.ConstSize;
            }
            Marshal.FreeCoTaskMem(ptr);
            Marshal.FreeCoTaskMem(ptr2);
            return (hresultcode == 0);
        }

        public void GetLocaleID(out int lcid)
        {
            this.ifCommon.GetLocaleID(out lcid);
        }

        public OpcGroup GetPublicGroup(string groupName)
        {
            if (this.ifServer == null)
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            OpcGroup group = new OpcGroup(ref this.ifServer, true, groupName, false, 0x3e8);
            group.internalAdd(null, null, 0);
            return group;
        }

        public void GetStatus(out SERVERSTATUS serverStatus)
        {
            this.ifServer.GetStatus(out serverStatus);
        }

        public bool LookupItemIDs(string itemID, int[] propertyIDs, out OPCPropertyItem[] propertyItems)
        {
            IntPtr ptr;
            IntPtr ptr2;
            propertyItems = null;
            int length = propertyIDs.Length;
            if (length < 1)
            {
                return false;
            }
            int hresultcode = this.ifItmProps.LookupItemIDs(itemID, length, propertyIDs, out ptr2, out ptr);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            int num3 = (int) ptr2;
            int num4 = (int) ptr;
            if ((num3 == 0) || (num4 == 0))
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            propertyItems = new OPCPropertyItem[length];
            for (int i = 0; i < length; i++)
            {
                propertyItems[i] = new OPCPropertyItem();
                propertyItems[i].PropertyID = propertyIDs[i];
                propertyItems[i].Error = Marshal.ReadInt32((IntPtr) num4);
                num4 += 4;
                if (propertyItems[i].Error == 0)
                {
                    IntPtr ptr3 = (IntPtr) Marshal.ReadInt32((IntPtr) num3);
                    propertyItems[i].newItemID = Marshal.PtrToStringUni(ptr3);
                    Marshal.FreeCoTaskMem(ptr3);
                }
                else
                {
                    propertyItems[i].newItemID = null;
                }
                num3 += 4;
            }
            Marshal.FreeCoTaskMem(ptr2);
            Marshal.FreeCoTaskMem(ptr);
            return (hresultcode == 0);
        }

        void IOPCShutdown.ShutdownRequest(string shutdownReason)
        {
            ShutdownRequestEventArgs e = new ShutdownRequestEventArgs(shutdownReason);
            if (this.ShutdownRequested != null)
            {
                this.ShutdownRequested(this, e);
            }
        }

        public void QueryAvailableLocaleIDs(out int[] lcids)
        {
            int num;
            IntPtr ptr;
            lcids = null;
            int hresultcode = this.ifCommon.QueryAvailableLocaleIDs(out num, out ptr);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            if (((int) ptr) != 0)
            {
                if (num < 1)
                {
                    Marshal.FreeCoTaskMem(ptr);
                }
                else
                {
                    lcids = new int[num];
                    Marshal.Copy(ptr, lcids, 0, num);
                    Marshal.FreeCoTaskMem(ptr);
                }
            }
        }

        public void QueryAvailableProperties(string itemID, out OPCProperty[] opcProperties)
        {
            IntPtr ptr;
            IntPtr ptr2;
            IntPtr ptr3;
            opcProperties = null;
            int dwCount = 0;
            this.ifItmProps.QueryAvailableProperties(itemID, out dwCount, out ptr, out ptr2, out ptr3);
            if ((dwCount != 0) && (dwCount <= 0x2710))
            {
                int num2 = (int) ptr;
                int num3 = (int) ptr2;
                int num4 = (int) ptr3;
                if (((num2 == 0) || (num3 == 0)) || (num4 == 0))
                {
                    Marshal.ThrowExceptionForHR(-2147467260);
                }
                opcProperties = new OPCProperty[dwCount];
                for (int i = 0; i < dwCount; i++)
                {
                    opcProperties[i] = new OPCProperty();
                    opcProperties[i].PropertyID = Marshal.ReadInt32((IntPtr) num2);
                    num2 += 4;
                    IntPtr ptr4 = (IntPtr) Marshal.ReadInt32((IntPtr) num3);
                    num3 += 4;
                    opcProperties[i].Description = Marshal.PtrToStringUni(ptr4);
                    Marshal.FreeCoTaskMem(ptr4);
                    opcProperties[i].DataType = (VarEnum) Marshal.ReadInt16((IntPtr) num4);
                    num4 += 2;
                }
                Marshal.FreeCoTaskMem(ptr);
                Marshal.FreeCoTaskMem(ptr2);
                Marshal.FreeCoTaskMem(ptr3);
            }
        }

        public OPCNAMESPACETYPE QueryOrganization()
        {
            OPCNAMESPACETYPE opcnamespacetype;
            this.ifBrowse.QueryOrganization(out opcnamespacetype);
            return opcnamespacetype;
        }

        public void SetClientName(string name)
        {
            this.ifCommon.SetClientName(name);
        }

        public void SetLocaleID(int lcid)
        {
            this.ifCommon.SetLocaleID(lcid);
        }
    }
}
