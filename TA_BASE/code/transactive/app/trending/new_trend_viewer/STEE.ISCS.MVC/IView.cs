using System;
using System.Collections.Generic;
using System.Text;

namespace STEE.ISCS.MVC
{
    /// <summary>
    ///  IView is an interface which contains some basic functions for operating view. 
    ///  A particular view is identified by view type and view id
    /// </summary>
    public interface IView
    {

        string ViewID
        {
            get;
            set;
        }

        string ViewType
        {
            get;
            set;
           
        }		 

		void PutObject(String objectName,Object obj);
        //Destroy view
        void DestroyView();
	 
        //Hide view
		void HideView();

        //Show View
        void ShowView(bool showModal);

        //Attach the view to a controller        
        void AttachController(IController controller);

        IController getController();

    }
}
