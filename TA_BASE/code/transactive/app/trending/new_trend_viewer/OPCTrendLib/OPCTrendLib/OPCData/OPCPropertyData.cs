namespace OPCTrendLib.OPCData
{
    using System;

    public class OPCPropertyData
    {
        public object Data;
        public int Error;
        public int PropertyID;

        public override string ToString()
        {
            if (this.Error == 0)
            {
                return string.Concat(new object[] { "ID:", this.PropertyID, " Data:", this.Data.ToString() });
            }
            return string.Concat(new object[] { "ID:", this.PropertyID, " Error:", this.Error.ToString() });
        }
    }
}
