using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using STEE.ISCS.MVC;

namespace STEE.ISCS.GenericGUI
{
    /// <summary>
    /// The class provides a basic implementation of form. All form should be inherited from this class. 
    /// It inherits IView interface and has integrated with the ISCS MVC framework.
    /// In the future, some more features can be introduced to this class according to function requirement.
    /// </summary>
    public partial class BaseForm : Form, IView
    {
        
        public BaseForm()
        {
            InitializeComponent();
            //register the view in ViewManager
            //ViewManager.GetInstance().RegisterView(this);
        }

        #region IView Members

        private String m_ViewID;

        public String ViewID
        {
            get { return m_ViewID; }
            set { m_ViewID = value; }
        }


        private String m_ViewType;

        public String ViewType
        {
            get { return m_ViewType; }
            set { m_ViewType = value; }
        }       


        public virtual void PutObject(string objectName,object obj)
        {
             
        }

        public void DestroyView()
        {
            
            this.Dispose();
        }

        public void HideView()
        {
            this.Hide();
        }

        public void ShowView(bool showModule)
        {
            if (showModule)
            {
                this.ShowDialog();
            }
            else
            {
                this.Show();
            }
        }
       
        public void AttachController(IController controller)
        {
            m_controller = controller;
        }

        #endregion


        private void BaseForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            //unregister the view 
            ViewManager.GetInstance().UnregisterView(this);
            if (m_controller!=null)
            {
                //De-attach the controller bound with the view.
                m_controller.Deattach();
            }
            m_controller = null;
        }

        private void BaseForm_Load(object sender, EventArgs e)
        {
            //multiple language support, set caption of controls based on the requirement
            TranslateCaption();
            //attach its event with the controller
            AttachListener(m_controller);            
        }

        
        /// <summary>
        /// It provides a default implementation
        /// All child form should override this function.
        /// </summary>
        /// <param name="controller">attached controller</param>
        public virtual void AttachListener(IController controller)
        {

        }

        /// <summary>
        /// Get the bound controller with the view.
        /// It provide a way to get the bound controller.
        /// </summary>
        /// <returns>Controller</returns>
        public IController getController()
        {
            return m_controller;
        }


        /// <summary>
        /// It provides a default implementation for supporting multiple languages.
        /// All child form should override this function.
        /// </summary>
        public virtual void TranslateCaption()
        {

        }

        private IController m_controller = null;
    }
}