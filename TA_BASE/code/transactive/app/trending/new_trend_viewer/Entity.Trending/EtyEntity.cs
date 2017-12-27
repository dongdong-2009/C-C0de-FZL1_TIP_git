using System;
using System.Collections.Generic;
using System.Text;

namespace Entity.Trending
{
    public class EtyEntity
    {
        private ulong m_Pkey;
        private string m_Name;
        private string m_Description;

        private string m_DisplayName;


	
//         private ulong m_TypeKey;
//         private ulong  m_ParentKey;

        public ulong Pkey
        {
            get { return m_Pkey; }
            set { m_Pkey = value; }
        }

        public string Name
        {
            get { return m_Name; }
            set { m_Name = value; }
        }

        public string  Description
        {
            get { return m_Description; }
            set { m_Description = value; }
        }

//         public ulong ParentKey
//         {
//             get { return m_ParentKey; }
//             set { m_ParentKey = value; }
//         }
//         
//         public ulong TypeKey
//         {
//             get { return m_TypeKey; }
//             set { m_TypeKey = value; }
//         }

        public string DisplayName
        {
            get { return m_Name; }
            set { m_DisplayName = value; }
        }
	
    }
}
