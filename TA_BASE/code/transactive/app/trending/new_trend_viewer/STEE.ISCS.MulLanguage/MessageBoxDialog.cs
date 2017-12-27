using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace STEE.ISCS.MulLanguage
{
    public class MessageBoxDialog
    {
        /// <summary>
        /// Displays a message box with specified text.
        /// </summary>
        /// <param name="text">The text to display in the message box.</param>
        /// <returns>One of the System.Windows.Forms.DialogResult values.</returns>
        public static DialogResult Show(string text)
        {
           /*
            MessageBoxForm m_NewMessageBox = new MessageBoxForm();
            m_NewMessageBox.BuildButtons(MessageBoxButtons.OK);
            m_NewMessageBox.BuildMessageBox("", text, 1, false);
            m_NewMessageBox.ShowDialog();
            return m_NewMessageBox.GetResultButton();
            */
            //using standard MessgaeBox
            return MessageBox.Show(text);            
        }

        /// <summary>
        /// Displays a message box in front of the specified object and with the specified
        /// text.
        /// </summary>
        /// <param name="owner">An implementation of System.Windows.Forms.IWin32Window that will own the
        ///     modal dialog box.</param>
        /// <param name="text">The text to display in the message box.</param>
        /// <returns>One of the System.Windows.Forms.DialogResult values.</returns>
        public static DialogResult Show(IWin32Window owner, string text)
        {
            //using standard MessgaeBox
            return MessageBox.Show(owner,text);     
        }
        
        /// <summary>
        /// Displays a message box with specified text and caption.
        /// </summary>
        /// <param name="text">The text to display in the message box.</param>
        /// <param name="caption">The text to display in the title bar of the message box.</param>
        /// <returns>One of the System.Windows.Forms.DialogResult values.</returns>
        public static DialogResult Show(string text, string caption)
        {
            /*
            MessageBoxForm m_NewMessageBox = new MessageBoxForm();
            m_NewMessageBox.BuildButtons(MessageBoxButtons.OK);
            m_NewMessageBox.BuildMessageBox(caption, text, 1, false);
            m_NewMessageBox.ShowDialog();
            return m_NewMessageBox.GetResultButton();
            */

            //using standard MessgaBox
            return MessageBox.Show(text, caption);
        }

        /// <summary>
        /// Displays a message box in front of the specified object and with the specified
        /// text and caption.
        /// </summary>
        /// <param name="owner">An implementation of System.Windows.Forms.IWin32Window that will own the
        ///     modal dialog box.</param>
        /// <param name="text">The text to display in the message box.</param>
        /// <param name="caption">The text to display in the title bar of the message box.</param>
        /// <returns>One of the System.Windows.Forms.DialogResult values.</returns>
        public static DialogResult Show(IWin32Window owner, string text, string caption)
        {
            //using standard MessgaBox
            return MessageBox.Show(owner,text, caption);
        }

        /// <summary>
        /// Displays a message box with specified text, caption, and buttons.
        /// </summary>
        /// <param name="text">The text to display in the message box.</param>
        /// <param name="caption">The text to display in the title bar of the message box.</param>
        /// <param name="buttons">One of the System.Windows.Forms.MessageBoxButtons values that specifies 
        /// which  buttons to display in the message box.</param>
        /// <returns>One of the System.Windows.Forms.DialogResult values.</returns>
        public static DialogResult Show(string text, string caption, MessageBoxButtons buttons)
        {
            /*
            MessageBoxForm m_NewMessageBox = new MessageBoxForm();
            int nbuttons = m_NewMessageBox.CalculateButtons(buttons);
            m_NewMessageBox.BuildButtons(buttons);
            m_NewMessageBox.BuildMessageBox(caption, text, nbuttons, false);
            m_NewMessageBox.ShowDialog();
            return m_NewMessageBox.GetResultButton();
            */
            //using standard MessgaBox
            return MessageBox.Show(text, caption,buttons);
        }

        public static DialogResult Show(IWin32Window owner, string text, string caption, MessageBoxButtons buttons)
        {
            //using standard MessgaBox
            return MessageBox.Show(owner,text, caption, buttons);
        }


        /// <summary>
        ///  Displays a message box with specified text, caption, buttons, and icon.
        /// </summary>
        /// <param name="text">The text to display in the message box.</param>
        /// <param name="caption">The text to display in the title bar of the message box.</param>
        /// <param name="buttons">One of the System.Windows.Forms.MessageBoxButtons values that specifies
        /// which buttons to display in the message box.</param>
        /// <param name="icon">One of the System.Windows.Forms.MessageBoxIcon values that specifies 
        /// which  icon to display in the message box.</param>
        /// <returns>One of the System.Windows.Forms.DialogResult values.</returns>
        public static DialogResult Show(string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon)
        {
            /*
            MessageBoxForm m_NewMessageBox = new MessageBoxForm();
            int nbuttons = m_NewMessageBox.CalculateButtons(buttons);
            m_NewMessageBox.BuildIcon(icon);
            m_NewMessageBox.BuildButtons(buttons);
            m_NewMessageBox.BuildMessageBox(caption, text, nbuttons, true);
            m_NewMessageBox.ShowDialog();
            return m_NewMessageBox.GetResultButton();
            */
            //using standard MessgaBox
            return MessageBox.Show(text, caption,buttons,icon);
        }

        public static DialogResult Show(IWin32Window owner, string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon)
        {
            /*
            MessageBoxForm m_NewMessageBox = new MessageBoxForm();
            int nbuttons = m_NewMessageBox.CalculateButtons(buttons);
            m_NewMessageBox.BuildIcon(icon);
            m_NewMessageBox.BuildButtons(buttons);
            m_NewMessageBox.BuildMessageBox(caption, text, nbuttons, true);
            m_NewMessageBox.ShowDialog(owner);
            return m_NewMessageBox.GetResultButton();
             */
            //using standard MessgaBox
            return MessageBox.Show(owner,text, caption,buttons,icon);
        }

        /// <summary>
        /// Displays a message box with the specified text, caption, buttons, icon, and
        /// default button.
        /// </summary>
        /// <param name="text">The text to display in the message box.</param>
        /// <param name="caption">The text to display in the title bar of the message box.</param>
        /// <param name="buttons">One of the System.Windows.Forms.MessageBoxButtons values that specifies
        /// which buttons to display in the message box.</param>
        /// <param name="icon">One of the System.Windows.Forms.MessageBoxIcon values that specifies 
        /// which  icon to display in the message box.</param>
        /// <param name="defaultButton">One of the System.Windows.Forms.MessageBoxDefaultButton values that specifies
        ///     the default button for the message box.</param>
        /// <returns>One of the System.Windows.Forms.DialogResult values.</returns>
        public static DialogResult Show(string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon, MessageBoxDefaultButton defaultButton)
        {
           /*
            MessageBoxForm m_NewMessageBox = new MessageBoxForm();
            int nbuttons = m_NewMessageBox.CalculateButtons(buttons);
            m_NewMessageBox.BuildIcon(icon);
            m_NewMessageBox.BuildButtons(buttons);
            m_NewMessageBox.BuildMessageBox(caption, text, nbuttons, true);
            m_NewMessageBox.SelectDefaultButton(defaultButton);
            m_NewMessageBox.ShowDialog();
            return m_NewMessageBox.GetResultButton();
            */
            //using standard MessgaBox
            return MessageBox.Show(text, caption,buttons,icon,defaultButton);
        }

        public static DialogResult Show(IWin32Window owner, string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon, MessageBoxDefaultButton defaultButton)
        {
            //using standard MessgaBox
            return MessageBox.Show(owner, text, caption, buttons, icon, defaultButton);
        }

        public static DialogResult Show(string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon, MessageBoxDefaultButton defaultButton, MessageBoxOptions options)
        {
            //using standard MessgaBox
            return MessageBox.Show(text, caption, buttons, icon, defaultButton,options);
        }

        public static DialogResult Show(IWin32Window owner, string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon, MessageBoxDefaultButton defaultButton, MessageBoxOptions options)
        {
            //using standard MessgaBox
            return MessageBox.Show(owner, text, caption, buttons, icon, defaultButton, options);
        }
    }
}
