using System;
using System.Collections.Generic;
using System.Text;

namespace STEE.ISCS.MVC
{
    /// <summary>
    /// Notify object is used to carry out notification message content.
    /// </summary>
    public class NotifyObject
    {
         
         object m_Body;  //message body
         string m_Name;  //message name
		 string m_Type;  //message type

         public Object Body
         {
	        get { return m_Body;}
	        set { m_Body = value;}
         }
         public string  Name
         {
             get { return m_Name; }
             set { m_Name = value; }
         }
         public string Type
         {
             get { return m_Type; }
             set { m_Type = value; }
         }    
		
    }
}
