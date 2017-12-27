using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using System.Xml.Schema;
using System.Net;



namespace ConfigFileGenerator
{
    public partial class Form1 : Form
    {
        private string fileName;
        private string hostName;
        private string dbTagName;
        private static bool isValid;


        // db info found in XML by host name:
        private string service_name;
        private string user_name;
        private string pswd;

        XmlDocument doc;
        XmlNodeList mftInfoNodes;
        XmlNodeList dbConfigNodes;

        public Form1()
        {
            InitializeComponent();
            fileName = "./MFT_DB_Info.xml";
            doc = null;
            service_name = "";
            user_name = "";
            pswd = "";
            hostName = "";
            dbTagName = "";
            mftInfoNodes = null;
            dbConfigNodes = null;
            isValid = true;
        }

        private void button1_Click(object sender, EventArgs e)
        {
           // MessageBox.Show( System.DateTime.Now.ToString());

            button1.Enabled = false;

            if (init())
            {
                if (getDBInfoFromXMLByHostName(hostName))
                {
                    if( generateConfigFile())
                    {
                        MessageBox.Show("Config.ini has been generated successful.");
                        this.Close();
                    }
                }

            }
            MessageBox.Show("Failed.");
            this.Close();
            
        }

        private bool iniXMLfile()
        {
            try
            {
                doc = new XmlDocument();
                doc.Load(fileName);
                mftInfoNodes = doc.GetElementsByTagName("MFT_Info");
                dbConfigNodes = doc.GetElementsByTagName("DB_Config");

                if (null == mftInfoNodes || null == dbConfigNodes)
                {
                    // no sufficient data in XML 
                    // show error msg:　no sufficient data in XML , exe failed.
                    return false;
                }
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("Exceptoion caught about XML file:" + ex.Message);
                return false;
            }


           return true;

        }

        //init hostName, 
        private bool initHostName()
        {
            bool res = false;
            try
            {
                // to do : get current host name
                hostName = Dns.GetHostName();
                return true;

            }
            catch (System.Exception ex)
            {
                // show err msg
                return false;
            }
        }

        // get the "MFT_DB_Info.xml" file, initialize some class memeber
        private bool init()
        {
            try
            {
                if (iniXMLfile() && initHostName())
                {
                    return true;
                }
            }
            catch (System.Exception ex)
            {
                //show message: xml not correct. details. exe will exit.
                return false;
            }
            return false;
        }


        private bool getDBInfoFromXMLByHostName(string hostName)
        {
            bool HostNamefound = false;
            string elementName = "";
            XmlNodeList mftInfoDetails = null;
            XmlNodeList dbConfigDetails = null;

            for (int i = 0; i < mftInfoNodes.Count && !HostNamefound; i++)
            {
                mftInfoDetails = mftInfoNodes.Item(i).ChildNodes;

                for (int j = 0; j < mftInfoDetails.Count; j++)
                {
                    elementName = mftInfoDetails.Item(j).Name;
                    if (elementName.CompareTo("Host_Name") == 0 && (mftInfoDetails.Item(j).ChildNodes[0].Value.ToLower()).CompareTo(hostName.ToLower()) == 0)
                    {
                        HostNamefound = true;
                        continue;
                    }

                    if (elementName.CompareTo("DB_Config_Name") == 0 && HostNamefound == true)
                    {
                        dbTagName = mftInfoDetails.Item(j).ChildNodes[0].Value;
                        break;
                    }
                }
            }
            if(HostNamefound ==false)
            {
                MessageBox.Show("Error: Cann't find the current hostname in XML file, current host name is: " + hostName);
                return false;
            }
            bool dbConfigNameFound = false;
            if (HostNamefound == true && null != dbTagName && dbTagName.CompareTo("") != 0)
            {
                // now have gotten dbTagName ,  find dbConfig details
                for (int i = 0; i < dbConfigNodes.Count && !dbConfigNameFound; i++)
                {
                    dbConfigDetails = dbConfigNodes.Item(i).ChildNodes;

                    for (int j = 0; j < dbConfigDetails.Count; j++)
                    {
                        elementName = dbConfigDetails.Item(j).Name;
                        if (elementName.CompareTo("Name") == 0 && dbConfigDetails.Item(j).ChildNodes[0].Value.CompareTo(dbTagName) == 0)
                        {
                            dbConfigNameFound = true;
                        }

                        if (elementName.CompareTo("Service_Name") == 0 && dbConfigNameFound == true)
                        {
                            service_name = dbConfigDetails.Item(j).ChildNodes[0].Value;
                        }
                        else if (elementName.CompareTo("User_ID") == 0 && dbConfigNameFound == true)
                        {
                            user_name = dbConfigDetails.Item(j).ChildNodes[0].Value;
                        }
                        else if (elementName.CompareTo("User_Password") == 0 && dbConfigNameFound == true)
                        {
                            pswd = dbConfigDetails.Item(j).ChildNodes[0].Value;
                        }
                    }
                }
            }

            if(dbConfigNameFound == false)
            {
                MessageBox.Show("Error: Cann't find the DB config detail in the XML file, current DB_Config name is: " + dbTagName);
                return false;
            }
            else if( service_name.CompareTo("")==0 || user_name.CompareTo("")==0 || pswd.CompareTo("")==0)
            {
                MessageBox.Show("Error: The DB config detail is not complete in the XML file, current DB_Config name is: " + dbTagName);
                return false;
            }
            return dbConfigNameFound;
        }

        private bool generateConfigFile()
        {
            DirectoryInfo dir = new DirectoryInfo("./");
            FileInfo[] files = dir.GetFiles("config.ini");
            if (files.Length > 0)
            {
                // there is an exiting "config.ini" . need to delete it first:
                // to do: show msg : There is ... will delete it to generate a new one. go on? yes/no
                if (MessageBox.Show("There is an existing config.ini. Will delete it to generate a new one. Go on?", "System Message", MessageBoxButtons.YesNo, MessageBoxIcon.Question).ToString() == "Yes")
                {
                    //delete the old file
                    File.Delete("./config.ini");
                }
                else  //user chose no
                {
                    MessageBox.Show("The process has been cancled. ");
                    return false;
                }

            }

            try
            {
                FileInfo f = new FileInfo("./config.ini");
                StreamWriter w = f.CreateText();
                w.WriteLine("[DATABASE_SERVER]");
                w.WriteLine("USER_ID=" + user_name);
                w.WriteLine("USER_PASSWORD=" + pswd);
                w.WriteLine("SERVICE_NAME=" + service_name);
                w.WriteLine("");
                w.WriteLine("[OPC_CLIENT]");
                w.WriteLine("SERVER1_NAME=TransActiveDataSource");
                w.WriteLine("SERVER_ROOT_NAME=Scada");
                w.WriteLine("LINE_THICKNESS=10");
                w.WriteLine("ABOUT_MSG=\"TrendViewer V 01.27.13.50\"");
                w.WriteLine("");
                w.WriteLine("[OPC_DT_LOG]");
                w.WriteLine("SERVER1_NAME=TransActiveDataSource");
                w.WriteLine("SERVER1_ENTRY_TAG=OPCBridge");
                w.WriteLine("SERVER1_EXCLUDE_TAG=OPCBridge");
                w.WriteLine("TIME_INTERVAL=60");
                w.WriteLine("");
                w.WriteLine("[DIRECTORY]");
                w.WriteLine("DEFAULT_FOLDER=C:\\");

                w.Flush();
                w.Close();
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("Exception:"+ ex.Message);
                return false;
            }
            
            return true;

        }

//         public static bool ValidationSchema(string filename)
//         {
//             XmlTextReader r = new XmlTextReader(filename);
//             XmlValidatingReader v = new XmlValidatingReader(r);
//             v.ValidationType = ValidationType.Schema;
//             v.ValidationEventHandler += new ValidationEventHandler(MyValidationEventHandler);
//             while (v.Read())
//             {
//                 // Can add code here to process the content.
//             }
//             v.Close();
//             return isValid;
//         }
// 
//         /// <summary>
//         /// 验证处理
//         /// </summary>
//         /// <param name="sender"></param>
//         /// <param name="args"></param>
//         private static void MyValidationEventHandler(object sender, ValidationEventArgs args)
//         {
//             if (args.Severity == XmlSeverityType.Warning)
//             {
//                 WriteErrorLogs("Validation Warning:   " + args.Message);
//             }
//             else
//             {
//                 WriteErrorLogs("Validation Error:   " + args.Message);
//             }
//             isValid = false;
//         }        /// <summary>
//         /// 记录错误日志
//         /// </summary>
//         /// <param name="ss"></param>
//         private static void WriteErrorLogs(string ss)
//         {
//             StreamWriter sw = null;
//             try
//             {
//                 sw = new StreamWriter("./xml_error.txt", true, Encoding.UTF8);
//                 sw.WriteLine("[" + DateTime.Now.ToString() + "]" + ss);
//                 sw.Flush();
//                 sw.Close();
//             }
//             catch { sw.Close(); }
//         }


    }

}

