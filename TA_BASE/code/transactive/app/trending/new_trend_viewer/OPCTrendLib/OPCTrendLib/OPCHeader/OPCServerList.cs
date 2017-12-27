namespace OPCTrendLib.OPCHeader
{
    using System;
    using System.Runtime.InteropServices;

    [ComVisible(true)]
    public class OPCServerList
    {
        private object enumObject = null;
        private IEnumGUID interfaceEnumGUID = null;
        private IOPCServerList interfaceServerList = null;
        private object OPCObjList = null;

        public void Dispose()
        {
            int num;
            this.interfaceEnumGUID = null;
            if (this.enumObject != null)
            {
                num = Marshal.ReleaseComObject(this.enumObject);
                this.enumObject = null;
            }
            this.interfaceServerList = null;
            if (this.OPCObjList != null)
            {
                num = Marshal.ReleaseComObject(this.OPCObjList);
                this.OPCObjList = null;
            }
        }

        ~OPCServerList()
        {
            this.Dispose();
        }

        public void ListAllServer(Guid catid, out OPCServers[] serverslist)
        {
            serverslist = null;
            this.Dispose();
            Guid clsid = new Guid("13486D51-4821-11D2-A494-3CB306C10000");
            Type typeFromCLSID = Type.GetTypeFromCLSID(clsid);
            this.OPCObjList = Activator.CreateInstance(typeFromCLSID);
            this.interfaceServerList = (IOPCServerList) this.OPCObjList;
            if (this.interfaceServerList == null)
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            this.interfaceServerList.EnumClassesOfCategories(1, ref catid, 0, ref catid, out this.enumObject);
            if (this.enumObject == null)
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            this.interfaceEnumGUID = (IEnumGUID) this.enumObject;
            if (this.interfaceEnumGUID == null)
            {
                Marshal.ThrowExceptionForHR(-2147467260);
            }
            int celt = 300;
            IntPtr rgelt = Marshal.AllocCoTaskMem(celt * 0x10);
            int pceltFetched = 0;
            this.interfaceEnumGUID.Next(celt, rgelt, out pceltFetched);
            if (pceltFetched < 1)
            {
                Marshal.FreeCoTaskMem(rgelt);
            }
            else
            {
                serverslist = new OPCServers[pceltFetched];
                byte[] destination = new byte[0x10];
                int num3 = (int) rgelt;
                for (int i = 0; i < pceltFetched; i++)
                {
                    serverslist[i] = new OPCServers();
                    Marshal.Copy((IntPtr) num3, destination, 0, 0x10);
                    serverslist[i].classID = new Guid(destination);
                    this.interfaceServerList.GetClassDetails(ref serverslist[i].classID, out serverslist[i].programID, out serverslist[i].serverName);
                    num3 += 0x10;
                }
                Marshal.FreeCoTaskMem(rgelt);
                this.Dispose();
            }
        }

        public void ListAllServerData(out OPCServers[] serverslist)
        {
            this.ListAllServer(new Guid("63D5F432-CFE4-11d1-B2C8-0060083BA1FB"), out serverslist);
        }
    }
}
