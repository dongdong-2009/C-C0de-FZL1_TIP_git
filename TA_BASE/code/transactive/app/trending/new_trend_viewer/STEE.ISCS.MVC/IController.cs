using System;
using System.Collections.Generic;
using System.Text;

namespace STEE.ISCS.MVC
{
    /// <summary>
    /// It is a abstract class and provides a basic implementation of a controller of the ISCS MVC pattern.
    /// IController is an abstract class and includes basic functions to attach a view and a model.  
    /// An observer pattern is applied in the class, it is an observer which should subscriber 
    /// notification message that is sent from the NotifyManager. 
    /// The method ¡®NotifyObserver¡¯ is used to get callback from the NotifyManager
    /// </summary>
    public abstract class IController : IObserver, IDisposable
    {

        private bool m_disposed=false;
        private IModel m_Model = null;
        private IView m_View = null;

        public IController()
        {
            //register to NotifyManager
            NotifyManager.GetInstance().RegisterObserver(this);
        }
        

        /// <summary>
        /// Attach to a model and a View
        /// </summary>
        /// <param name="model">a model</param>
        /// <param name="view">a view</param>
        public void  Attach(IModel model, IView view)
        {
             m_Model=model;
             m_View=view;
             if (view !=null )
             {
                 view.AttachController(this);
             }
             AttachCallBack(model, view);
        }


        /// <summary>
        /// AttachCallBack provides a chance that transforms  IModel and IView to a particular type of Model and View for child controller.
        /// </summary>
        /// <param name="model">model</param>
        /// <param name="view">view</param>
        public virtual void AttachCallBack(IModel model, IView view)
        {

        }

        /// <summary>
        /// De-attach view, model
        /// </summary>
        public void   Deattach()
        {
            m_View=null;
            m_Model=null;
            //unregister from NotifyManager
            NotifyManager.GetInstance().UnregisterObserver(this);
        }

        #region IObserver Members

        /// <summary>
        /// This function is used to receive notification message
        /// </summary>
        /// <param name="notifyObj">Notification message object</param>
        public abstract void NotifyObserver(NotifyObject notifyObj);
         

        #endregion

        #region IDisposable Members

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool isDisposing)
        {
            if (!m_disposed)
            {
                if (isDisposing)
                {
                    //NotifyManager.GetInstance().UnregisterObserver(this);
                }
                m_disposed = true;
            }
        }

        #endregion
    }
}
