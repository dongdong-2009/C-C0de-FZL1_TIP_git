namespace OPCTrendLib.OPCData
{
    using OPCTrendLib.OPCHeader;
    using System;
    using System.Runtime.InteropServices;

    public class OPCProperty
    {
        public VarEnum DataType;
        public string Description;
        public int PropertyID;

        public override string ToString()
        {
            return string.Concat(new object[] { "ID:", this.PropertyID, " '", this.Description, "' T:", DUMMY_VARIANT.VarEnumToString(this.DataType) });
        }
    }
}
