using System;
using System.Collections.Generic;
using System.Text;

namespace STEE.ISCS.MVC
{
    /// <summary>
    /// Observer interface
    /// It is used to register with NotifyManager
    /// </summary>
    interface IObserver
    {
        /// <summary>
        /// Callback method for getting message notification from NotifyManager
        /// </summary>
        /// <param name="notifyObj">Notification message object</param>
        void NotifyObserver(NotifyObject notifyObj);
    }
}
