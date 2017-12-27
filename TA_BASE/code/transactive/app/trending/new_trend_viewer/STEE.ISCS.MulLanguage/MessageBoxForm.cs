using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using STEE.ISCS.Log;

namespace STEE.ISCS.MulLanguage
{
    class MessageBoxForm : Form
    {

        #region const declaration
        //const
        private const int DEFAULT_FORMMAXWIDTH = 500;
        private const int DEFAULT_FROMMAXHEIGHT = 250;

        private const int DEFAULT_FORMMINWIDTH_BUTTON1 = 150;
        private const int DEFAULT_FROMMINHEIGHT_BUTTON1 = 150;

        private const int DEFAULT_FORMMINWIDTH_BUTTON2 = 250;
        private const int DEFAULT_FROMMINHEIGHT_BUTTON2 = 170;

        private const int DEFAULT_FORMMINWIDTH_BUTTON3 = 350;
        private const int DEFAULT_FROMMINHEIGHT_BUTTON3 = 175;

        private const int ICONSIZE = 38;
        private const int ICON_STARTX = 10;
        private const int ICON_STARTY = 25;

        private const int ICON_MESSAGE_PADDING = 15;
        private const int MESSAGEPANEL_BUTTON_PADDING = 75;
        private const int LEFT_PADDING = 12;
        private const int BUTTONS_PADDING = 12;
        private const int BUTTONS_PANEL_HEIGHTPADDING = 10;
        private const int TEXT_FORMWIDTH_PADDING = 100;
        private const int TEXT_FORMHEIGHT_PADDING = 58;


        private const int BUTTON_WIDTH = 80;
        private const int BUTTON_HEIGHT = 25;

        private const int SC_CLOSE = 0xF060;
        private const int MF_BYCOMMAND = 0x0;
        private const int MF_GRAYED = 0x1;
        private const int MF_ENABLED = 0x0;


        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern bool MessageBeep(uint type);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr GetSystemMenu(IntPtr hWnd, bool bRevert);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern bool EnableMenuItem(IntPtr hMenu, uint uIDEnableItem,
        uint uEnable);

        private const string CLASS_NAME = "STEE.ISCS.MulLanguage.MessageBoxDialog";

        #endregion

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        //Controls created dynamically
       // static private MessageBoxDialog m_NewMessageBox = null;
         private System.Windows.Forms.Panel m_MessageIConPanel = null;
         private System.Windows.Forms.Label m_DisplayTextLBL = null;
         private System.Windows.Forms.PictureBox m_IconPic = null;
         private Button m_button1 = null;
         private Button m_button2 = null;
         private Button m_button3 = null;

         //Object s_ObjectLock = new Object();

        //members
        private DialogResult m_ReturnButton = DialogResult.None;
        private Icon m_icon = null;

        /// <summary>
        /// Constructor method - Add Button Caption to stringHelper and initialise component.
        /// </summary>
        public MessageBoxForm()
        {
            MessageBoxButtonString.GetInstance().AddStringHelper();
            InitializeComponent();
        }

        /// <summary>
        /// Cleans the static varaibles of the class. 
        /// Called while disposing the form.
        /// </summary>
        private void CleanUp()
        {
            m_button1 = null;
            m_button2 = null;
            m_button3 = null;
            m_IconPic = null;
            m_DisplayTextLBL = null;
            m_MessageIConPanel = null;
            //m_ReturnButton = DialogResult.None;
            m_icon = null;
            //m_NewMessageBox = null;
        }
        
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            CleanUp();

            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // MessageBoxDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(392, 221);
            this.Font = System.Drawing.SystemFonts.MessageBoxFont;
            this.ForeColor = System.Drawing.SystemColors.WindowText;
            this.MaximizeBox = false;
            this.MinimizeBox = false;            
            this.ResumeLayout(false);
        }

        #endregion

        /// <summary>
        /// Builds the Messagbox Form creating the controls based on user requirement.
        /// </summary>
        /// <param name="Title">Title of Form</param>
        /// <param name="Distext">Display Text in the form</param>
        /// <param name="Nbuttons">number of buttons to be displayed</param>
        /// <param name="BPicture">true - icon picture control to be displayed in form, false - does not display.</param>
        public void BuildMessageBox(string Title, string Distext, int Nbuttons, bool BPicture)
        {
            string Function_Name = "BuildMessageBox";
            try
            {
                this.Text = Title;
                this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
                this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
                this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
                this.ShowInTaskbar = false;

                m_MessageIConPanel = new System.Windows.Forms.Panel();
                m_MessageIConPanel.BackColor = System.Drawing.SystemColors.ControlLightLight;
                m_MessageIConPanel.Location = new System.Drawing.Point(0, 0);

                m_DisplayTextLBL = new Label();
                m_DisplayTextLBL.AutoSize = true;
                m_DisplayTextLBL.TabIndex = 1;
                m_DisplayTextLBL.Text = Distext;
                m_DisplayTextLBL.Font = new Font("Tahoma", 9, FontStyle.Regular);
                m_MessageIConPanel.Controls.Add(m_DisplayTextLBL);

                if (BPicture)
                {
                    m_IconPic = new PictureBox();
                    m_IconPic.Size = new System.Drawing.Size(ICONSIZE, ICONSIZE);
                    Image imageIcon = new Bitmap(m_icon.ToBitmap(), ICONSIZE, ICONSIZE);
                    m_IconPic.Image = imageIcon;
                    m_MessageIConPanel.Controls.Add(m_IconPic);
                    m_IconPic.Location = new System.Drawing.Point(ICON_STARTX, ICON_STARTY);
                }

                //Adjust the size of the form based on the text size.
                if (Nbuttons == 1)
                {
                    LayoutControls(DEFAULT_FORMMINWIDTH_BUTTON1, DEFAULT_FROMMINHEIGHT_BUTTON1);
                }
                else if (Nbuttons == 2)
                {
                    LayoutControls(DEFAULT_FORMMINWIDTH_BUTTON2, DEFAULT_FROMMINHEIGHT_BUTTON2);
                }
                else
                {
                    LayoutControls(DEFAULT_FORMMINWIDTH_BUTTON3, DEFAULT_FROMMINHEIGHT_BUTTON3);
                }

                //disable close button and menu for multiple button without cancel.
                DisableCloseIfMultipleButtonsAndNoCancelButton();

            }
            catch (Exception exception)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, exception);
            }
        }

        /// <summary>
        /// Disable Close Menu & button of the form if 
        /// number of button is more than one and button doesnot include cancel button.
        /// </summary>
        private void DisableCloseIfMultipleButtonsAndNoCancelButton()
        {
            string Function_Name = "DisableCloseIfMultipleButtonsAndNoCancelButton";

            if (m_button2 != null)
            {
                //always cancel button will be button1 so check that for cancel
                if ((int)m_button1.Tag == (int)DialogResult.Cancel)
                {
                    return;
                }

                //cancel button is not present, Disable
                //close button
                try
                {
                    EnableMenuItem(GetSystemMenu(this.Handle, false), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
                }
                catch (Exception exception)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, exception);
                }

            }
        }

        /// <summary>
        /// Layout Form and Controls based on the display Test Size.
        /// </summary>
        /// <param name="minWidth">MINIMUM WIDTH of form</param>
        /// <param name="minHeight">minimum height of form</param>
        private void LayoutControls(int minWidth, int minHeight)
        {
            int disTxtWidth;
            if (m_IconPic != null)
            {
                disTxtWidth = ICONSIZE + ICON_MESSAGE_PADDING;
            }
            else
            {
                disTxtWidth = LEFT_PADDING;
            }
            int disTxtHeight = ICON_STARTY;
            if (m_DisplayTextLBL.Size.Width <= (minWidth - TEXT_FORMWIDTH_PADDING) && m_DisplayTextLBL.Size.Height <= (minHeight - (MESSAGEPANEL_BUTTON_PADDING + TEXT_FORMHEIGHT_PADDING)))
            {
                this.Width = minWidth;
                this.Height = minHeight;
                disTxtHeight += ICON_MESSAGE_PADDING;
            }
            else if (m_DisplayTextLBL.Size.Width <= DEFAULT_FORMMAXWIDTH && m_DisplayTextLBL.Size.Height <= DEFAULT_FROMMAXHEIGHT)
            {
                int width = m_DisplayTextLBL.Size.Width + TEXT_FORMWIDTH_PADDING;
                int height = Math.Max((minHeight - (MESSAGEPANEL_BUTTON_PADDING + TEXT_FORMHEIGHT_PADDING)), m_DisplayTextLBL.Size.Height);
                height = Math.Max(minHeight, height);
                this.Width = width;
                this.Height = height;
                disTxtHeight += ICON_MESSAGE_PADDING;
            }
            else
            {
                if (m_DisplayTextLBL.Size.Width >= DEFAULT_FORMMAXWIDTH || m_DisplayTextLBL.Size.Height >= DEFAULT_FROMMAXHEIGHT)
                {
                    m_DisplayTextLBL.Size = new System.Drawing.Size(DEFAULT_FORMMAXWIDTH - TEXT_FORMWIDTH_PADDING, DEFAULT_FROMMAXHEIGHT - (MESSAGEPANEL_BUTTON_PADDING + TEXT_FORMHEIGHT_PADDING));

                    m_DisplayTextLBL.AutoSize = false;
                }
                this.Width = DEFAULT_FORMMAXWIDTH;
                this.Height = DEFAULT_FROMMAXHEIGHT;
            }

            m_MessageIConPanel.Size = new System.Drawing.Size(this.Width, this.Height - MESSAGEPANEL_BUTTON_PADDING);

            m_DisplayTextLBL.Size = new System.Drawing.Size(this.Width - TEXT_FORMWIDTH_PADDING, this.Height - (MESSAGEPANEL_BUTTON_PADDING + TEXT_FORMHEIGHT_PADDING));
            m_DisplayTextLBL.AutoSize = false;
            m_DisplayTextLBL.Location = new System.Drawing.Point(disTxtWidth, disTxtHeight);

            this.Controls.Add(m_MessageIConPanel);

            //minimum one button in form will be.
            m_button1.Location = new System.Drawing.Point(m_MessageIConPanel.Width - (m_button1.Width + BUTTONS_PADDING), m_MessageIConPanel.Height + BUTTONS_PANEL_HEIGHTPADDING);
            this.Controls.Add(m_button1);
            
            if (m_button2 != null)
            {
                m_button2.Location = new System.Drawing.Point(m_button1.Location.X - (m_button2.Width + BUTTONS_PADDING), m_MessageIConPanel.Height + BUTTONS_PANEL_HEIGHTPADDING);
                this.Controls.Add(m_button2);
            }

            if (m_button3 != null)
            {
                m_button3.Location = new System.Drawing.Point(m_button2.Location.X - (m_button3.Width + BUTTONS_PADDING), m_MessageIConPanel.Height + BUTTONS_PANEL_HEIGHTPADDING);
                this.Controls.Add(m_button3);
            }
        }
        
        /// <summary>
        /// Create Button in the form with given text and tag.
        /// </summary>
        /// <param name="button">form 's button(either m_button1,m_button2,m_button3) which is created</param>
        /// <param name="text">button text</param>
        /// <param name="tag">button tag equivalent to DialogResult.buttoncaption</param>
        private void ShowButton(Button button, string text, int tag)
        {
            button.Size = new System.Drawing.Size(BUTTON_WIDTH, BUTTON_HEIGHT);
            button.Font = new Font("Tahoma", 8, FontStyle.Regular);
            button.Text = text;
            button.Tag = tag;
            //button.TabIndex = 3;
            button.TabStop = true;
            button.Click += new EventHandler(btn_Click);
        }

        /// <summary>
        /// Calculates the number buttons the current form should have.
        /// </summary>
        /// <param name="buttons">user defined option</param>
        /// <returns>number of the buttons</returns>
        public int CalculateButtons(MessageBoxButtons buttons)
        {
            int nRet = 1;
            switch (buttons)
            {
                case MessageBoxButtons.AbortRetryIgnore:
                case MessageBoxButtons.YesNoCancel:
                    nRet = 3;
                    break;
                case MessageBoxButtons.OKCancel:
                case MessageBoxButtons.RetryCancel:
                case MessageBoxButtons.YesNo:
                    nRet = 2;
                    break;
                case MessageBoxButtons.OK:
                    nRet = 1;
                    break;
                default:
                    nRet = 1;
                    break;
            }
            return nRet;
        }

        /// <summary>
        /// Creates the Message box icon
        /// </summary>
        /// <param name="icon">message box icon</param>
        public void BuildIcon(MessageBoxIcon icon)
        {
            switch (icon)
            {
                case MessageBoxIcon.Question:
                    m_icon = SystemIcons.Question;
                   MessageBeep((uint)MessageBoxIcon.Question);
                    break;
                case MessageBoxIcon.Error:
                    m_icon = SystemIcons.Error;
                    MessageBeep((uint)MessageBoxIcon.Error);
                    break;
                case MessageBoxIcon.Asterisk:
                    m_icon = SystemIcons.Asterisk;
                   MessageBeep((uint)MessageBoxIcon.Asterisk);
                    break;
                case MessageBoxIcon.Exclamation:
                    m_icon = SystemIcons.Exclamation;
                   MessageBeep((uint)MessageBoxIcon.Exclamation);
                    break;
                case MessageBoxIcon.None:
                    MessageBeep((uint)0);
                    break;
                default:
                    System.ComponentModel.InvalidEnumArgumentException invalidArgument = new InvalidEnumArgumentException("Invalid MessageBoxIcon option.");
                    throw invalidArgument;
            }

        }

        /// <summary>
        /// Based on user option, builds the buttons in the messagebox
        /// </summary>
        /// <param name="MButtons">user option</param>
        public void BuildButtons(MessageBoxButtons MButtons)
        {
            switch(MButtons)
            {
                case MessageBoxButtons.AbortRetryIgnore:
                    m_button1 = new Button();
                    m_button2 = new Button();
                    m_button3 = new Button();
                    ShowButton(m_button1,StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_IGNOREBUTTON, MessageBoxButtonString.MSG_IGNORE_BUTTON_EN), (int)DialogResult.Ignore);
                    ShowButton(m_button2,StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_RETRYBUTTON, MessageBoxButtonString.MSG_RETRY_BUTTON_EN), (int)DialogResult.Retry);
                    ShowButton(m_button3,StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_ABORTBUTTON, MessageBoxButtonString.MSG_ABORT_BUTTON_EN), (int)DialogResult.Abort);
                    //default button selection
                    m_button3.Select();
                    m_button1.TabIndex = 3;
                    m_button2.TabIndex = 2;
                    m_button3.TabIndex = 1;
                    break;

                case MessageBoxButtons.OK:
                    m_button1 = new Button();
                    ShowButton(m_button1, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_OKBUTTON, MessageBoxButtonString.MSG_OK_BUTTON_EN), (int)DialogResult.OK);
                    //default button selection
                    m_button1.Select();
                    m_button1.TabIndex = 1;
                    break;

                case MessageBoxButtons.OKCancel:
                    m_button1 = new Button();
                    m_button2 = new Button();
                    ShowButton(m_button1, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_CANCELBUTTON, MessageBoxButtonString.MSG_CANCEL_BUTTON_EN), (int)DialogResult.Cancel);
                    ShowButton(m_button2, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_OKBUTTON, MessageBoxButtonString.MSG_OK_BUTTON_EN), (int)DialogResult.OK);
                    //default button selection
                    m_button2.Select();
                    m_button1.TabIndex = 2;
                    m_button2.TabIndex = 1;
                    break;

                case MessageBoxButtons.RetryCancel:
                    m_button1 = new Button();
                    m_button2 = new Button();
                    ShowButton(m_button1, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_CANCELBUTTON, MessageBoxButtonString.MSG_CANCEL_BUTTON_EN), (int)DialogResult.Cancel);
                    ShowButton(m_button2, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_RETRYBUTTON, MessageBoxButtonString.MSG_RETRY_BUTTON_EN), (int)DialogResult.Retry);
                    //default button selection
                    m_button2.Select();
                    m_button1.TabIndex = 2;
                    m_button2.TabIndex = 1;
                    break;

                case MessageBoxButtons.YesNo:
                    m_button1 = new Button();
                    m_button2 = new Button();
                    ShowButton(m_button1, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_NOBUTTON, MessageBoxButtonString.MSG_NO_BUTTON_EN), (int)DialogResult.No);
                    ShowButton(m_button2, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_YESBUTTON, MessageBoxButtonString.MSG_YES_BUTTON_EN), (int)DialogResult.Yes);
                    //default button selection
                    m_button2.Select();
                    m_button1.TabIndex = 2;
                    m_button2.TabIndex = 1;
                    break;

                case MessageBoxButtons.YesNoCancel:
                    m_button1 = new Button();
                    m_button2 = new Button();
                    m_button3 = new Button();
                    ShowButton(m_button1, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_CANCELBUTTON, MessageBoxButtonString.MSG_CANCEL_BUTTON_EN), (int)DialogResult.Cancel);
                    ShowButton(m_button2, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_NOBUTTON, MessageBoxButtonString.MSG_NO_BUTTON_EN), (int)DialogResult.No);
                    ShowButton(m_button3, StringHelper.GetInstance().getStringValue(MessageBoxButtonString.MESSAGEBOX_YESBUTTON, MessageBoxButtonString.MSG_YES_BUTTON_EN), (int)DialogResult.Yes);
                    //default button selection
                    m_button3.Select();
                    m_button1.TabIndex = 3;
                    m_button2.TabIndex = 2;
                    m_button3.TabIndex = 1;
                    break;

                default:
                    System.ComponentModel.InvalidEnumArgumentException invalidArgument = new InvalidEnumArgumentException("Invalid MessageBoxButtons option.");
                    throw invalidArgument;
            }
        }

        /// <summary>
        /// Based on user option, Selects the specified button as default button.
        /// </summary>
        /// <param name="defaultButton">button to be defautl</param>
        public void SelectDefaultButton(MessageBoxDefaultButton defaultButton)
        {
            switch (defaultButton)
            {
                case MessageBoxDefaultButton.Button1:
                    if (m_button3 != null)
                        m_button3.Select();
                    else if (m_button2 != null)
                        m_button2.Select();
                    else
                        m_button1.Select();
                    break;
                case MessageBoxDefaultButton.Button2:
                    if (m_button3 != null)
                        m_button2.Select();
                    else if (m_button2 != null)
                        m_button1.Select();
                    break;
                case MessageBoxDefaultButton.Button3:
                    if(m_button3 != null)
                        m_button1.Select();
                    break;
                default:
                    System.ComponentModel.InvalidEnumArgumentException invalidArgument = new InvalidEnumArgumentException("Invalid MessageBoxDefaultButton option.");
                    throw invalidArgument;
            }
        }

        /// <summary>
        /// Button Click event handler - 
        /// Closes the form and sets user response in form of DialogResult values.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void btn_Click(object sender, EventArgs e)
        {
                m_ReturnButton = (DialogResult)((Button)sender).Tag;
                this.Dispose();
        }

        public DialogResult GetResultButton()
        {
            return m_ReturnButton;
        }


    }
}