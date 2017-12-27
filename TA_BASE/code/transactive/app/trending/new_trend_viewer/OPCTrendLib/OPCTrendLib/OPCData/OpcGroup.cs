namespace OPCTrendLib.OPCData
{
    using OPCTrendLib.OPCDataInterface;
    using OPCTrendLib.OPCHeader;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;
    using System.Runtime.InteropServices.ComTypes;
    using System.Text;
    using STEE.ISCS.Log;

    public class OpcGroup : IOPCDataCallback
    {
        private int callbackcookie = 0;
        private IConnectionPoint callbackcpoint = null;
        private IConnectionPointContainer cpointcontainer = null;
        private IOPCAsyncIO2 ifAsync = null;
        private IOPCItemMgt ifItems = null;
        private IOPCGroupStateMgt ifMgt = null;
        private IOPCServer ifServer = null;
        private IOPCSyncIO ifSync = null;
        private readonly int sizeOPCITEMDEF;
        private readonly int sizeOPCITEMRESULT;
        private OPCGroupState state;
        private readonly Type typeOPCITEMDEF;
        private readonly Type typeOPCITEMRESULT;

        public event CancelCompleteEventHandler CancelCompleted;

        public event DataChangeEventHandler DataChanged;

        public event ReadCompleteEventHandler ReadCompleted;

        public event WriteCompleteEventHandler WriteCompleted;

        internal OpcGroup(ref IOPCServer ifServerLink, bool isPublic, string groupName, bool setActive, int requestedUpdateRate)
        {
            this.ifServer = ifServerLink;
            this.state.Name = groupName;
            this.state.Public = isPublic;
            this.state.UpdateRate = requestedUpdateRate;
            this.state.Active = setActive;
            this.state.TimeBias = 0;
            this.state.PercentDeadband = 0f;
            this.state.LocaleID = 0;
            this.state.HandleClient = this.GetHashCode();
            this.state.HandleServer = 0;
            this.typeOPCITEMDEF = typeof(OPCITEMDEFintern);
            this.sizeOPCITEMDEF = Marshal.SizeOf(this.typeOPCITEMDEF);
            this.typeOPCITEMRESULT = typeof(OPCITEMRESULTintern);
            this.sizeOPCITEMRESULT = Marshal.SizeOf(this.typeOPCITEMRESULT);
        }

        public bool AddItems(OPCItemDef[] arrDef, out OPCItemResult[] arrRes)
        {
            IntPtr ptr2;
            IntPtr ptr3;
            int num4;
            arrRes = null;
            bool flag = false;
            int length = arrDef.Length;
            IntPtr pItemArray = Marshal.AllocCoTaskMem(length * this.sizeOPCITEMDEF);
            int num2 = (int) pItemArray;
            OPCITEMDEFintern structure = new OPCITEMDEFintern();
            structure.wReserved = 0;
            foreach (OPCItemDef def in arrDef)
            {
                structure.szAccessPath = def.AccessPath;
                structure.szItemID = def.ItemID;
                structure.bActive = def.Active;
                structure.hClient = def.HandleClient;
                structure.vtRequestedDataType = (short) def.RequestedDataType;
                structure.dwBlobSize = 0;
                structure.pBlob = IntPtr.Zero;
                if (def.Blob != null)
                {
                    structure.dwBlobSize = def.Blob.Length;
                    if (structure.dwBlobSize > 0)
                    {
                        flag = true;
                        structure.pBlob = Marshal.AllocCoTaskMem(structure.dwBlobSize);
                        Marshal.Copy(def.Blob, 0, structure.pBlob, structure.dwBlobSize);
                    }
                }
                Marshal.StructureToPtr(structure, (IntPtr) num2, false);
                num2 += this.sizeOPCITEMDEF;
            }
            int hresultcode = this.ifItems.AddItems(length, pItemArray, out ptr2, out ptr3);
            num2 = (int) pItemArray;
            if (flag)
            {
                for (num4 = 0; num4 < length; num4++)
                {
                    IntPtr ptr = (IntPtr) Marshal.ReadInt32((IntPtr) (num2 + 20));
                    if (ptr != IntPtr.Zero)
                    {
                        Marshal.FreeCoTaskMem(ptr);
                    }
                    Marshal.DestroyStructure((IntPtr) num2, this.typeOPCITEMDEF);
                    num2 += this.sizeOPCITEMDEF;
                }
            }
            else
            {
                num4 = 0;
                while (num4 < length)
                {
                    Marshal.DestroyStructure((IntPtr) num2, this.typeOPCITEMDEF);
                    num2 += this.sizeOPCITEMDEF;
                    num4++;
                }
            }
            Marshal.FreeCoTaskMem(pItemArray);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            int num5 = (int) ptr2;
            int num6 = (int) ptr3;
            if ((num5 == 0) || (num6 == 0))
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            arrRes = new OPCItemResult[length];
            for (num4 = 0; num4 < length; num4++)
            {
                arrRes[num4] = new OPCItemResult();
                arrRes[num4].Error = Marshal.ReadInt32((IntPtr) num6);
                if (!HRESULTS.Failed(arrRes[num4].Error))
                {
                    arrRes[num4].HandleServer = Marshal.ReadInt32((IntPtr) num5);
                    arrRes[num4].CanonicalDataType = (VarEnum) Marshal.ReadInt16((IntPtr) (num5 + 4));
                    arrRes[num4].AccessRights = (OPCACCESSRIGHTS) Marshal.ReadInt32((IntPtr) (num5 + 8));
                    int num7 = Marshal.ReadInt32((IntPtr) (num5 + 0x10));
                    if (num7 != 0)
                    {
                        int num8 = Marshal.ReadInt32((IntPtr) (num5 + 12));
                        if (num8 > 0)
                        {
                            arrRes[num4].Blob = new byte[num8];
                            Marshal.Copy((IntPtr) num7, arrRes[num4].Blob, 0, num8);
                        }
                        Marshal.FreeCoTaskMem((IntPtr) num7);
                    }
                    num5 += this.sizeOPCITEMRESULT;
                    num6 += 4;
                }
            }
            Marshal.FreeCoTaskMem(ptr2);
            Marshal.FreeCoTaskMem(ptr3);
            return (hresultcode == 0);
        }

        private void AdviseIOPCDataCallback()
        {
            Type type = typeof(IOPCDataCallback);
            Guid gUID = type.GUID;
            this.cpointcontainer.FindConnectionPoint(ref gUID, out this.callbackcpoint);
            if (this.callbackcpoint != null)
            {
                this.callbackcpoint.Advise(this, out this.callbackcookie);
            }
        }

        public void Cancel2(int cancelID)
        {
            this.ifAsync.Cancel2(cancelID);
        }

        public OpcEnumItemAttributes CreateAttrEnumerator()
        {
            object obj2;
            Type type = typeof(IEnumOPCItemAttributes);
            Guid gUID = type.GUID;
            int hresultcode = this.ifItems.CreateEnumerator(ref gUID, out obj2);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            if ((hresultcode == 1) || (obj2 == null))
            {
                return null;
            }
            IEnumOPCItemAttributes ifEnump = (IEnumOPCItemAttributes) obj2;
            obj2 = null;
            return new OpcEnumItemAttributes(ifEnump);
        }

        public void DeletePublic(bool bForce)
        {
            if (!this.state.Public)
            {
                Marshal.ThrowExceptionForHR(-2147467259);
            }
            IOPCServerPublicGroups ifServer = null;
            ifServer = (IOPCServerPublicGroups) this.ifServer;
            if (ifServer == null)
            {
                Marshal.ThrowExceptionForHR(-2147467262);
            }
            int handleServer = this.state.HandleServer;
            this.Remove(false);
            ifServer.RemovePublicGroup(handleServer, bForce);
            ifServer = null;
        }

        ~OpcGroup()
        {
            this.Remove(false);
        }

        public void GetEnable(out bool isEnabled)
        {
            this.ifAsync.GetEnable(out isEnabled);
        }

        private void getinterfaces()
        {
            this.ifItems = (IOPCItemMgt) this.ifMgt;
            this.ifSync = (IOPCSyncIO) this.ifMgt;
            this.ifAsync = (IOPCAsyncIO2) this.ifMgt;
            this.cpointcontainer = (IConnectionPointContainer) this.ifMgt;
        }

        public void GetStates()
        {
            this.ifMgt.GetState(out this.state.UpdateRate, out this.state.Active, out this.state.Name, out this.state.TimeBias, out this.state.PercentDeadband, out this.state.LocaleID, out this.state.HandleClient, out this.state.HandleServer);
        }

        internal void internalAdd(int[] biasTime, float[] percentDeadband, int localeID)
        {
            object obj2;
            Type type = typeof(IOPCGroupStateMgt);
            Guid gUID = type.GUID;
            if (this.state.Public)
            {
                IOPCServerPublicGroups ifServer = null;
                ifServer = (IOPCServerPublicGroups) this.ifServer;
                if (ifServer == null)
                {
                    Marshal.ThrowExceptionForHR(-2147467262);
                }
                ifServer.GetPublicGroupByName(this.state.Name, ref gUID, out obj2);
                ifServer = null;
            }
            else
            {
                this.ifServer.AddGroup(this.state.Name, this.state.Active, this.state.UpdateRate, this.state.HandleClient, biasTime, percentDeadband, this.state.LocaleID, out this.state.HandleServer, out this.state.UpdateRate, ref gUID, out obj2);
            }
            if (obj2 == null)
            {
                Marshal.ThrowExceptionForHR(-2147467262);
            }
            this.ifMgt = (IOPCGroupStateMgt) obj2;
            obj2 = null;
            this.GetStates();
            this.getinterfaces();
            this.AdviseIOPCDataCallback();
        }

        public void MoveToPublic()
        {
            if (this.state.Public)
            {
                Marshal.ThrowExceptionForHR(-2147467259);
            }
            IOPCPublicGroupStateMgt ifMgt = null;
            ifMgt = (IOPCPublicGroupStateMgt) this.ifMgt;
            if (ifMgt == null)
            {
                Marshal.ThrowExceptionForHR(-2147467262);
            }
            ifMgt.MoveToPublic();
            ifMgt.GetState(out this.state.Public);
            ifMgt = null;
        }

        void IOPCDataCallback.OnCancelComplete(int dwTransid, int hGroup)
        {
            if (hGroup == this.state.HandleClient)
            {
                CancelCompleteEventArgs e = new CancelCompleteEventArgs(dwTransid, hGroup);
                if (this.CancelCompleted != null)
                {
                    this.CancelCompleted(this, e);
                }
            }
        }

        void IOPCDataCallback.OnDataChange(int dwTransid, int hGroup, int hrMasterquality, int hrMastererror, int dwCount, IntPtr phClientItems, IntPtr pvValues, IntPtr pwQualities, IntPtr pftTimeStamps, IntPtr ppErrors)
        {
            if ((dwCount != 0) && (hGroup == this.state.HandleClient))
            {
                int num = dwCount;
                int num2 = (int) phClientItems;
                int num3 = (int) pvValues;
                int num4 = (int) pwQualities;
                int num5 = (int) pftTimeStamps;
                int num6 = (int) ppErrors;
                DataChangeEventArgs e = new DataChangeEventArgs();
                e.transactionID = dwTransid;
                e.groupHandleClient = hGroup;
                e.masterQuality = hrMasterquality;
                e.masterError = hrMastererror;
                e.sts = new OPCItemState[num];
                for (int i = 0; i < num; i++)
                {
                    e.sts[i] = new OPCItemState();
                    e.sts[i].Error = Marshal.ReadInt32((IntPtr) num6);
                    num6 += 4;
                    e.sts[i].HandleClient = Marshal.ReadInt32((IntPtr) num2);
                    num2 += 4;
                    if (HRESULTS.Succeeded(e.sts[i].Error))
                    {
                        if (Marshal.ReadInt16((IntPtr) num3) == 10)
                        {
                            e.sts[i].Error = Marshal.ReadInt32((IntPtr) (num3 + 8));
                        }
                        e.sts[i].DataValue = Marshal.GetObjectForNativeVariant((IntPtr) num3);
                        e.sts[i].Quality = Marshal.ReadInt16((IntPtr) num4);
                        e.sts[i].TimeStamp = Marshal.ReadInt64((IntPtr) num5);
                    }
                    num3 += DUMMY_VARIANT.ConstSize;
                    num4 += 2;
                    num5 += 8;
                }
                if (this.DataChanged != null)
                {
                    this.DataChanged(this, e);
                }
            }
        }

        void IOPCDataCallback.OnReadComplete(int dwTransid, int hGroup, int hrMasterquality, int hrMastererror, int dwCount, IntPtr phClientItems, IntPtr pvValues, IntPtr pwQualities, IntPtr pftTimeStamps, IntPtr ppErrors)
        {
            if ((dwCount != 0) && (hGroup == this.state.HandleClient))
            {
                int num = dwCount;
                int num2 = (int) phClientItems;
                int num3 = (int) pvValues;
                int num4 = (int) pwQualities;
                int num5 = (int) pftTimeStamps;
                int num6 = (int) ppErrors;
                ReadCompleteEventArgs e = new ReadCompleteEventArgs();
                e.transactionID = dwTransid;
                e.groupHandleClient = hGroup;
                e.masterQuality = hrMasterquality;
                e.masterError = hrMastererror;
                e.sts = new OPCItemState[num];
                for (int i = 0; i < num; i++)
                {
                    e.sts[i] = new OPCItemState();
                    e.sts[i].Error = Marshal.ReadInt32((IntPtr) num6);
                    num6 += 4;
                    e.sts[i].HandleClient = Marshal.ReadInt32((IntPtr) num2);
                    num2 += 4;
                    if (HRESULTS.Succeeded(e.sts[i].Error))
                    {
                        if (Marshal.ReadInt16((IntPtr) num3) == 10)
                        {
                            e.sts[i].Error = Marshal.ReadInt32((IntPtr) (num3 + 8));
                        }
                        e.sts[i].DataValue = Marshal.GetObjectForNativeVariant((IntPtr) num3);
                        e.sts[i].Quality = Marshal.ReadInt16((IntPtr) num4);
                        e.sts[i].TimeStamp = Marshal.ReadInt64((IntPtr) num5);
                    }
                    num3 += DUMMY_VARIANT.ConstSize;
                    num4 += 2;
                    num5 += 8;
                }
                if (this.ReadCompleted != null)
                {
                    this.ReadCompleted(this, e);
                }
            }
        }

        void IOPCDataCallback.OnWriteComplete(int dwTransid, int hGroup, int hrMastererr, int dwCount, IntPtr pClienthandles, IntPtr ppErrors)
        {
            if ((dwCount != 0) && (hGroup == this.state.HandleClient))
            {
                int num = dwCount;
                int num2 = (int) pClienthandles;
                int num3 = (int) ppErrors;
                WriteCompleteEventArgs e = new WriteCompleteEventArgs();
                e.transactionID = dwTransid;
                e.groupHandleClient = hGroup;
                e.masterError = hrMastererr;
                e.res = new OPCWriteResult[num];
                for (int i = 0; i < num; i++)
                {
                    e.res[i] = new OPCWriteResult();
                    e.res[i].Error = Marshal.ReadInt32((IntPtr) num3);
                    num3 += 4;
                    e.res[i].HandleClient = Marshal.ReadInt32((IntPtr) num2);
                    num2 += 4;
                }
                if (this.WriteCompleted != null)
                {
                    this.WriteCompleted(this, e);
                }
            }
        }

        public static string QualityToString(short Quality)
        {
            StringBuilder builder = new StringBuilder(0x100);
            OPC_QUALITY_MASTER opc_quality_master = (OPC_QUALITY_MASTER) ((short) (Quality & 0xc0));
            OPC_QUALITY_STATUS opc_quality_status = (OPC_QUALITY_STATUS) ((short) (Quality & 0xfc));
            OPC_QUALITY_LIMIT opc_quality_limit = ((OPC_QUALITY_LIMIT) Quality) & OPC_QUALITY_LIMIT.LIMIT_CONST;
            builder.AppendFormat("{0}+{1}+{2}", opc_quality_master, opc_quality_status, opc_quality_limit);
            return builder.ToString();
        }

        public bool Read(OPCDATASOURCE src, int[] arrHSrv, out OPCItemState[] arrStat)
        {
            IntPtr ptr;
            IntPtr ptr2;
            arrStat = null;
            int length = arrHSrv.Length;
            int hresultcode = this.ifSync.Read(src, length, arrHSrv, out ptr, out ptr2);
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
            arrStat = new OPCItemState[length];
            for (int i = 0; i < length; i++)
            {
                arrStat[i] = new OPCItemState();
                arrStat[i].Error = Marshal.ReadInt32((IntPtr) num3);
                num3 += 4;
                arrStat[i].HandleClient = Marshal.ReadInt32((IntPtr) num4);
                if (HRESULTS.Succeeded(arrStat[i].Error))
                {
                    if (Marshal.ReadInt16((IntPtr) (num4 + 0x10)) == 10)
                    {
                        arrStat[i].Error = Marshal.ReadInt32((IntPtr) (num4 + 0x18));
                    }
                    arrStat[i].TimeStamp = Marshal.ReadInt64((IntPtr) (num4 + 4));
                    arrStat[i].Quality = Marshal.ReadInt16((IntPtr) (num4 + 12));
                    arrStat[i].DataValue = Marshal.GetObjectForNativeVariant((IntPtr) (num4 + 0x10));
                    DUMMY_VARIANT.VariantClear((IntPtr) (num4 + 0x10));
                }
                else
                {
                    arrStat[i].DataValue = null;
                }
                num4 += 0x20;
            }
            Marshal.FreeCoTaskMem(ptr);
            Marshal.FreeCoTaskMem(ptr2);
            return (hresultcode == 0);
        }

        public bool Read(int[] arrHSrv, int transactionID, out int cancelID, out int[] arrErr)
        {
            IntPtr ptr;
            arrErr = null;
            cancelID = 0;
            int length = arrHSrv.Length;
            int hresultcode = this.ifAsync.Read(length, arrHSrv, transactionID, out cancelID, out ptr);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            arrErr = new int[length];
            Marshal.Copy(ptr, arrErr, 0, length);
            Marshal.FreeCoTaskMem(ptr);
            return (hresultcode == 0);
        }

        public void Refresh2(OPCDATASOURCE sourceMode, int transactionID, out int cancelID)
        {
            this.ifAsync.Refresh2(sourceMode, transactionID, out cancelID);
        }

        public void Remove(bool bForce)
        {
            try
            {
                int num;
                if (this.callbackcpoint != null)
                {
                    if (this.callbackcookie != 0)
                    {
                        this.callbackcpoint.Unadvise(this.callbackcookie);
                        this.callbackcookie = 0;
                    }
                    num = Marshal.ReleaseComObject(this.callbackcpoint);
                    this.callbackcpoint = null;
                }
                this.cpointcontainer = null;
                this.ifItems = null;
                this.ifSync = null;
                this.ifAsync = null;
                if (this.ifMgt != null)
                {
                    num = Marshal.ReleaseComObject(this.ifMgt);
                    this.ifMgt = null;
                }
                if (this.ifServer != null)
                {
                    if (!this.state.Public)
                    {
                        this.ifServer.RemoveGroup(this.state.HandleServer, bForce);
                    }
                    this.ifServer = null;
                }
                this.state.HandleServer = 0;
            }
            catch (Exception exception)
            {
                LogHelper.Error("OPCTrendLib.OPCData.OpcGroup", "Remove", exception);
                Console.WriteLine(exception.ToString());
            }
        }

        public bool RemoveItems(int[] arrHSrv, out int[] arrErr)
        {
            IntPtr ptr;
            arrErr = null;
            int length = arrHSrv.Length;
            int hresultcode = this.ifItems.RemoveItems(length, arrHSrv, out ptr);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            arrErr = new int[length];
            Marshal.Copy(ptr, arrErr, 0, length);
            Marshal.FreeCoTaskMem(ptr);
            return (hresultcode == 0);
        }

        public bool SetActiveState(int[] arrHSrv, bool activate, out int[] arrErr)
        {
            IntPtr ptr;
            arrErr = null;
            int length = arrHSrv.Length;
            int hresultcode = this.ifItems.SetActiveState(length, arrHSrv, activate, out ptr);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            arrErr = new int[length];
            Marshal.Copy(ptr, arrErr, 0, length);
            Marshal.FreeCoTaskMem(ptr);
            return (hresultcode == 0);
        }

        public bool SetClientHandles(int[] arrHSrv, int[] arrHClt, out int[] arrErr)
        {
            IntPtr ptr;
            arrErr = null;
            int length = arrHSrv.Length;
            if (length != arrHClt.Length)
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            int hresultcode = this.ifItems.SetClientHandles(length, arrHSrv, arrHClt, out ptr);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            arrErr = new int[length];
            Marshal.Copy(ptr, arrErr, 0, length);
            Marshal.FreeCoTaskMem(ptr);
            return (hresultcode == 0);
        }

        public bool SetDatatypes(int[] arrHSrv, VarEnum[] arrVT, out int[] arrErr)
        {
            IntPtr ptr2;
            arrErr = null;
            int length = arrHSrv.Length;
            if (length != arrVT.Length)
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            IntPtr pRequestedDatatypes = Marshal.AllocCoTaskMem(length * 2);
            int num2 = (int) pRequestedDatatypes;
            foreach (VarEnum enum2 in arrVT)
            {
                Marshal.WriteInt16((IntPtr) num2, (short) enum2);
                num2 += 2;
            }
            int hresultcode = this.ifItems.SetDatatypes(length, arrHSrv, pRequestedDatatypes, out ptr2);
            Marshal.FreeCoTaskMem(pRequestedDatatypes);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            arrErr = new int[length];
            Marshal.Copy(ptr2, arrErr, 0, length);
            Marshal.FreeCoTaskMem(ptr2);
            return (hresultcode == 0);
        }

        public void SetEnable(bool doEnable)
        {
            this.ifAsync.SetEnable(doEnable);
        }

        public void SetName(string newName)
        {
            this.ifMgt.SetName(newName);
            this.state.Name = newName;
        }

        public bool ValidateItems(OPCItemDef[] arrDef, bool blobUpd, out OPCItemResult[] arrRes)
        {
            IntPtr ptr2;
            IntPtr ptr3;
            int num4;
            arrRes = null;
            bool flag = false;
            int length = arrDef.Length;
            IntPtr pItemArray = Marshal.AllocCoTaskMem(length * this.sizeOPCITEMDEF);
            int num2 = (int) pItemArray;
            OPCITEMDEFintern structure = new OPCITEMDEFintern();
            structure.wReserved = 0;
            foreach (OPCItemDef def in arrDef)
            {
                structure.szAccessPath = def.AccessPath;
                structure.szItemID = def.ItemID;
                structure.bActive = def.Active;
                structure.hClient = def.HandleClient;
                structure.vtRequestedDataType = (short) def.RequestedDataType;
                structure.dwBlobSize = 0;
                structure.pBlob = IntPtr.Zero;
                if (def.Blob != null)
                {
                    structure.dwBlobSize = def.Blob.Length;
                    if (structure.dwBlobSize > 0)
                    {
                        flag = true;
                        structure.pBlob = Marshal.AllocCoTaskMem(structure.dwBlobSize);
                        Marshal.Copy(def.Blob, 0, structure.pBlob, structure.dwBlobSize);
                    }
                }
                Marshal.StructureToPtr(structure, (IntPtr) num2, false);
                num2 += this.sizeOPCITEMDEF;
            }
            int hresultcode = this.ifItems.ValidateItems(length, pItemArray, blobUpd, out ptr2, out ptr3);
            num2 = (int) pItemArray;
            if (flag)
            {
                for (num4 = 0; num4 < length; num4++)
                {
                    IntPtr ptr = (IntPtr) Marshal.ReadInt32((IntPtr) (num2 + 20));
                    if (ptr != IntPtr.Zero)
                    {
                        Marshal.FreeCoTaskMem(ptr);
                    }
                    Marshal.DestroyStructure((IntPtr) num2, this.typeOPCITEMDEF);
                    num2 += this.sizeOPCITEMDEF;
                }
            }
            else
            {
                num4 = 0;
                while (num4 < length)
                {
                    Marshal.DestroyStructure((IntPtr) num2, this.typeOPCITEMDEF);
                    num2 += this.sizeOPCITEMDEF;
                    num4++;
                }
            }
            Marshal.FreeCoTaskMem(pItemArray);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            int num5 = (int) ptr2;
            int num6 = (int) ptr3;
            if ((num5 == 0) || (num6 == 0))
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            arrRes = new OPCItemResult[length];
            for (num4 = 0; num4 < length; num4++)
            {
                arrRes[num4] = new OPCItemResult();
                arrRes[num4].Error = Marshal.ReadInt32((IntPtr) num6);
                if (!HRESULTS.Failed(arrRes[num4].Error))
                {
                    arrRes[num4].HandleServer = Marshal.ReadInt32((IntPtr) num5);
                    arrRes[num4].CanonicalDataType = (VarEnum) Marshal.ReadInt16((IntPtr) (num5 + 4));
                    arrRes[num4].AccessRights = (OPCACCESSRIGHTS) Marshal.ReadInt32((IntPtr) (num5 + 8));
                    int num7 = Marshal.ReadInt32((IntPtr) (num5 + 0x10));
                    if (num7 != 0)
                    {
                        int num8 = Marshal.ReadInt32((IntPtr) (num5 + 12));
                        if (num8 > 0)
                        {
                            arrRes[num4].Blob = new byte[num8];
                            Marshal.Copy((IntPtr) num7, arrRes[num4].Blob, 0, num8);
                        }
                        Marshal.FreeCoTaskMem((IntPtr) num7);
                    }
                    num5 += this.sizeOPCITEMRESULT;
                    num6 += 4;
                }
            }
            Marshal.FreeCoTaskMem(ptr2);
            Marshal.FreeCoTaskMem(ptr3);
            return (hresultcode == 0);
        }

        public bool Write(int[] arrHSrv, object[] arrVal, out int[] arrErr)
        {
            IntPtr ptr;
            arrErr = null;
            int length = arrHSrv.Length;
            if (length != arrVal.Length)
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            int hresultcode = this.ifSync.Write(length, arrHSrv, arrVal, out ptr);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            arrErr = new int[length];
            Marshal.Copy(ptr, arrErr, 0, length);
            Marshal.FreeCoTaskMem(ptr);
            return (hresultcode == 0);
        }

        public bool Write(int[] arrHSrv, object[] arrVal, int transactionID, out int cancelID, out int[] arrErr)
        {
            IntPtr ptr;
            arrErr = null;
            cancelID = 0;
            int length = arrHSrv.Length;
            if (length != arrVal.Length)
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            int hresultcode = this.ifAsync.Write(length, arrHSrv, arrVal, transactionID, out cancelID, out ptr);
            if (HRESULTS.Failed(hresultcode))
            {
                Marshal.ThrowExceptionForHR(hresultcode);
            }
            arrErr = new int[length];
            Marshal.Copy(ptr, arrErr, 0, length);
            Marshal.FreeCoTaskMem(ptr);
            return (hresultcode == 0);
        }

        public bool Active
        {
            get
            {
                return this.state.Active;
            }
            set
            {
                this.ifMgt.SetState(null, out this.state.UpdateRate, new bool[] { value }, null, null, null, null);
                this.state.Active = value;
            }
        }

        public int HandleClient
        {
            get
            {
                return this.state.HandleClient;
            }
            set
            {
                this.ifMgt.SetState(null, out this.state.UpdateRate, null, null, null, null, new int[] { value });
                this.state.HandleClient = value;
            }
        }

        public int HandleServer
        {
            get
            {
                return this.state.HandleServer;
            }
        }

        public int LocaleID
        {
            get
            {
                return this.state.LocaleID;
            }
            set
            {
                this.ifMgt.SetState(null, out this.state.UpdateRate, null, null, null, new int[] { value }, null);
                this.state.LocaleID = value;
            }
        }

        public string Name
        {
            get
            {
                return this.state.Name;
            }
            set
            {
                this.SetName(value);
            }
        }

        public float PercentDeadband
        {
            get
            {
                return this.state.PercentDeadband;
            }
            set
            {
                this.ifMgt.SetState(null, out this.state.UpdateRate, null, null, new float[] { value }, null, null);
                this.state.PercentDeadband = value;
            }
        }

        public bool Public
        {
            get
            {
                return this.state.Public;
            }
        }

        public int TimeBias
        {
            get
            {
                return this.state.TimeBias;
            }
            set
            {
                this.ifMgt.SetState(null, out this.state.UpdateRate, null, new int[] { value }, null, null, null);
                this.state.TimeBias = value;
            }
        }

        public int UpdateRate
        {
            get
            {
                return this.state.UpdateRate;
            }
            set
            {
                this.ifMgt.SetState(new int[] { value }, out this.state.UpdateRate, null, null, null, null, null);
            }
        }
    }
}
