using System;
using System.Collections.Generic;
using System.Text;
 

namespace STEE.ISCS.MVC
{
    /// <summary>
    /// The ViewManager provides functions to retrieve views and make sure that there is only one instance for a particular view.
    /// Before using this class, a view factory should be register first.
    /// </summary>
    public class ViewManager
    {
        private ViewManager()
        {
            m_ViewMap=new Dictionary<string, IView>();
            m_ViewFactoryMap = new Dictionary<IViewFactory, IViewFactory>();
        }
        
        /// <summary>
        /// implement a singleton pattern
        /// </summary>
        /// <returns></returns>
		public static ViewManager GetInstance()
        {
            if (m_instance==null)
            {
                m_instance=new ViewManager();
            }
            return m_instance;
        }


        /// <summary>
        /// get view by type and "" as name
        /// </summary>
        /// <param name="type">view type</param>
        /// <returns></returns>
        public IView GetView(string type)
        {
            return GetView(type, "");
        }
		
        /// <summary>
        /// get view by type and name
        /// </summary>
        /// <param name="type">view type</param>
        /// <param name="id">view name</param>
        /// <returns></returns>
        public  IView  GetView(string type, string id)
        {
            string realID=type + id;	//get real view id;
            if (m_ViewMap.ContainsKey(realID))
            {
                //if it exist, return the existing view.
                return  m_ViewMap[realID];
            }
            else
            {               
                //if it does not exist, create a new one.
                return createView(type,id);
            }
               
        }

		/// <summary>
		/// Register a view
		/// </summary>
		/// <param name="view">a view</param>
        
		public void RegisterView(IView view)
        {
            string realID=view.ViewType + view.ViewID;
            if (!m_ViewMap.ContainsKey(realID))
            {
                 m_ViewMap.Add(realID,view);
            }           
		    
        }


        /// <summary>
        /// unregister a view
        /// </summary>
        /// <param name="view">a view</param>
		public void UnregisterView(IView view)
        {
             string realID=view.ViewType + view.ViewID;
		    if (m_ViewMap.ContainsKey(realID))
            {
                 m_ViewMap.Remove(realID);
            } 
           
        }

        /// <summary>
        /// register a view factory.
        /// </summary>
        /// <param name="viewFactory">a view factory</param>
        public void RegisterViewFactory(IViewFactory viewFactory)
        {

            if (!m_ViewFactoryMap.ContainsKey(viewFactory))
            {
                m_ViewFactoryMap.Add(viewFactory, viewFactory);
            }   

        }

        /// <summary>
        /// create a view
        /// </summary>
        /// <param name="type">view type</param>
        /// <param name="id">view id</param>
        /// <returns>a particular view or null</returns>
	    private IView createView(string type, string id)
        {        
		    foreach(KeyValuePair<IViewFactory, IViewFactory> pair in m_ViewFactoryMap)
            {
			     IView v= (pair.Value).CreateView(type, id);
                 if (v != null)
			     {
				     RegisterView(v);
                     return v;
			     }
		    }
		    return null;
        }
        	    
        Dictionary<string, IView> m_ViewMap;  //view is identified by view plus id
        Dictionary<IViewFactory, IViewFactory>  m_ViewFactoryMap;  //view factories		 

		static ViewManager  m_instance;
    }
}
