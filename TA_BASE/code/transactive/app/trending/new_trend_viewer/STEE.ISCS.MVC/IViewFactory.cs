using System;
using System.Collections.Generic;
using System.Text;

namespace STEE.ISCS.MVC
{
    /// <summary>
    /// IViewFactory is an interface which is used in the ViewManager class for creating a particular view based on view type and view id.  
    /// There should be a class which implements the interface and responsible for creating view in client application
    /// </summary>
    public interface IViewFactory
    {
        /// <summary>
        /// Create view based on type and id
        /// </summary>
        /// <param name="type">view type</param>
        /// <param name="id">view id</param>
        /// <returns>A particular view instrance or null if it has not the responsibility to create the view;</returns>
        IView  CreateView(String type,String id);
    }
}
