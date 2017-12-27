using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DAO.Trending;

namespace DAO.Trending.Helper
{
    public class LocationKeyHelper
    {
        private const string CLASS_NAME = "LocationKeyHelper";

        private static LocationKeyHelper m_instance = null;

        private bool m_inited = false;
        private bool m_isOCC = false;
        private string m_HostName = "";
        private ulong m_LocationKey = 0;

        public static LocationKeyHelper GetInstance()
        {
            if (m_instance == null)
            {
                m_instance = new LocationKeyHelper();
            }
            return m_instance;

        }

        public void init()
        {
            if(!m_inited)
            {
                InitHostName();
                InitLocationKeyByHostName();
                InitIsOCC();
                m_inited = true;
            }
        }

        private void InitIsOCC()
        {
            //if the location key eqals to OCC location key ,return true; else return false
            EntityDAO entityDAO = new EntityDAO();
            ulong occLocKey = entityDAO.GetLocationKeyByEtyName(DAOHelper.OCC_LOCATIONNAME);
            if (occLocKey == m_LocationKey)
                m_isOCC = true;
            else
                m_isOCC = false;

        }

        private void InitHostName()
        {
            string Function_Name = "InitHostName";
            try
            {
                // get current host name
                m_HostName  = System.Net.Dns.GetHostName();
                //m_HostName = "dt19-mft-01";  //"occ-mft-01";  // just for test  "dt19-mft-01";//

                STEE.ISCS.Log.LogHelper.Info(CLASS_NAME, Function_Name, "Current host name is: " + m_HostName);

            }
            catch (System.Exception ex)
            {
                m_HostName = "";
                STEE.ISCS.Log.LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
                STEE.ISCS.Log.LogHelper.Info(CLASS_NAME, Function_Name, "Can't get current host name");
            }
        }

        private void InitLocationKeyByHostName()
        {
            string Function_Name = "InitLocationKeyByHostName";
            if(m_HostName!="")
            {
                EntityDAO entityDAO = new EntityDAO();
                m_LocationKey = entityDAO.GetLocationKeyByEtyName(m_HostName);
                STEE.ISCS.Log.LogHelper.Info(CLASS_NAME, Function_Name, "Current location key is: " + m_LocationKey);
            }
            else
            {
                STEE.ISCS.Log.LogHelper.Error(CLASS_NAME, Function_Name, "Can't find Location Key because host name is empty.");
            }
        }


        public bool IsOCC
        {
            get { return m_isOCC; }
            set { m_isOCC = value; }
        }


        public string HostName
        {
            get { return m_HostName; }
            set { m_HostName = value; }
        }

      
        public ulong LocationKey
        {
            get { return m_LocationKey; }
            set { m_LocationKey = value; }
        }
    }
}
