namespace OPCTrendLib.OPCData
{
    using OPCTrendLib.OPCDataInterface;
    using OPCTrendLib.OPCHeader;
    using System;
    using System.Runtime.InteropServices;

    public class OpcEnumItemAttributes
    {
        private IEnumOPCItemAttributes ifEnum;

        internal OpcEnumItemAttributes(IEnumOPCItemAttributes ifEnump)
        {
            this.ifEnum = ifEnump;
        }

        public void Dispose()
        {
            if (this.ifEnum != null)
            {
                int num = Marshal.ReleaseComObject(this.ifEnum);
                this.ifEnum = null;
            }
        }

        ~OpcEnumItemAttributes()
        {
            this.Dispose();
        }

        public void Next(int enumcountmax, out OPCItemAttributes[] attributes)
        {
            IntPtr ptr;
            int num;
            attributes = null;
            this.ifEnum.Next(enumcountmax, out ptr, out num);
            int num2 = (int) ptr;
            if (((num2 != 0) && (num > 0)) && (num <= enumcountmax))
            {
                attributes = new OPCItemAttributes[num];
                for (int i = 0; i < num; i++)
                {
                    attributes[i] = new OPCItemAttributes();
                    IntPtr ptr2 = (IntPtr) Marshal.ReadInt32((IntPtr) num2);
                    attributes[i].AccessPath = Marshal.PtrToStringUni(ptr2);
                    Marshal.FreeCoTaskMem(ptr2);
                    ptr2 = (IntPtr) Marshal.ReadInt32((IntPtr) (num2 + 4));
                    attributes[i].ItemID = Marshal.PtrToStringUni(ptr2);
                    Marshal.FreeCoTaskMem(ptr2);
                    attributes[i].Active = Marshal.ReadInt32((IntPtr) (num2 + 8)) != 0;
                    attributes[i].HandleClient = Marshal.ReadInt32((IntPtr) (num2 + 12));
                    attributes[i].HandleServer = Marshal.ReadInt32((IntPtr) (num2 + 0x10));
                    attributes[i].AccessRights = (OPCACCESSRIGHTS) Marshal.ReadInt32((IntPtr) (num2 + 20));
                    attributes[i].RequestedDataType = (VarEnum) Marshal.ReadInt16((IntPtr) (num2 + 0x20));
                    attributes[i].CanonicalDataType = (VarEnum) Marshal.ReadInt16((IntPtr) (num2 + 0x22));
                    attributes[i].EUType = (OPCEUTYPE) Marshal.ReadInt32((IntPtr) (num2 + 0x24));
                    attributes[i].EUInfo = Marshal.GetObjectForNativeVariant((IntPtr) (num2 + 40));
                    DUMMY_VARIANT.VariantClear((IntPtr) (num2 + 40));
                    int num4 = Marshal.ReadInt32((IntPtr) (num2 + 0x1c));
                    if (num4 != 0)
                    {
                        int length = Marshal.ReadInt32((IntPtr) (num2 + 0x18));
                        if (length > 0)
                        {
                            attributes[i].Blob = new byte[length];
                            Marshal.Copy((IntPtr) num4, attributes[i].Blob, 0, length);
                        }
                        Marshal.FreeCoTaskMem((IntPtr) num4);
                    }
                    num2 += 0x38;
                }
                Marshal.FreeCoTaskMem(ptr);
            }
        }

        public void Reset()
        {
            this.ifEnum.Reset();
        }

        public void Skip(int celt)
        {
            this.ifEnum.Skip(celt);
        }
    }
}
