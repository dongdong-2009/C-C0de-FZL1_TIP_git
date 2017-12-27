namespace OPCTrendLib.OPCData
{
    using System;
    using System.Text;

    public class OPCItemState
    {
        public object DataValue;
        public int Error;
        public int HandleClient;
        public short Quality;
        public long TimeStamp;

        public override string ToString()
        {
            StringBuilder builder = new StringBuilder("OPCIST: ", 0x100);
            builder.AppendFormat("error=0x{0:x} hclt=0x{1:x}", this.Error, this.HandleClient);
            if (this.Error == 0)
            {
                builder.AppendFormat(" val={0} time={1} qual=", this.DataValue, this.TimeStamp);
                builder.Append(OpcGroup.QualityToString(this.Quality));
            }
            return builder.ToString();
        }
    }
}
