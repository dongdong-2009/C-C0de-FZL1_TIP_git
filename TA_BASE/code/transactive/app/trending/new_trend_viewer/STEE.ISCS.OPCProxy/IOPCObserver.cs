using System;
using System.Collections.Generic;
using System.Text;

namespace STEE.ISCS.OPCProxy
{
     public interface IOPCObserver
    {
         //void DataChange(OPCDataItem dataItem);
         void ServerShutdownEvent();
    }
}
