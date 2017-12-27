using System;
using System.Collections.Generic;
using System.Text;

namespace STEE.ISCS.MVC
{
    /// <summary>
    /// NotifyManager is used to broadcast message.
    /// All controllers are subscriber by default. 
    /// </summary>
    public class NotifyManager
    {
    
        private NotifyManager()
        {
            m_ObserverMap = new Dictionary<IObserver, IObserver>();
        }
        
	    /// <summary>
	    /// implement a singleton pattern
	    /// </summary>
	    /// <returns></returns>
        public static NotifyManager GetInstance()
        {
            if (m_instance==null)
            {
                m_instance=new NotifyManager();
            }
            return m_instance;
          

        }
	    
        /// <summary>
        /// Register Observer
        /// </summary>
        /// <param name="observer">observer</param>
		internal void RegisterObserver (IObserver observer)
        {
            if (!m_ObserverMap.ContainsKey(observer))
            {
                m_ObserverMap.Add(observer, observer);
            }  

        }

        /// <summary>
        /// send a broadcast message in synchronous way
        /// </summary>
        /// <param name="type">message type</param>
        /// <param name="name">message name</param>
        /// <param name="body">message body</param>
        public void Send(string type, string name, object body)
        {

            NotifyObject obj = new NotifyObject();
            obj.Body = body;
            obj.Type = type;
            obj.Name = name;
            foreach (KeyValuePair<IObserver,IObserver> item in m_ObserverMap)
	        {
                item.Value.NotifyObserver(obj);
	        }
        }


        /// <summary>
        /// unregister a observer
        /// </summary>
        /// <param name="observer">observer</param>
        internal void UnregisterObserver(IObserver observer)
        {
            if (m_ObserverMap.ContainsKey(observer))
            {
                m_ObserverMap.Remove(observer);
            }
        }
	 
		
		static NotifyManager  m_instance=null;
		Dictionary<IObserver,IObserver> m_ObserverMap;

	 
    }
}
