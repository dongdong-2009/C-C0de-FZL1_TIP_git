namespace OPCTrendLib
{
    using Oracle.DataAccess.Client;
    using System;
    using System.Data;
    using System.Data.SqlClient;
    using System.Globalization;
    using System.IO;
    using System.Net;
    using System.Net.Sockets;
    using System.Runtime.InteropServices;
    using System.Text;

    public class GeneralFunction
    {
        public string configFolder = "./";
        public string localConnectionString = "";
        public string localHostName = Dns.GetHostName().Trim();
        public string sqlConnectionString = "";
        private StringBuilder tempStringBuilder = null;

        public GeneralFunction()
        {
            this.localConnectionString = "Data Source = " + this.GetINIDataString("DATABASE_SERVER", "SERVICE_NAME", "", 0xff, this.configFolder + "config.ini") + ";User Id = " + this.GetINIDataString("DATABASE_SERVER", "USER_ID", "", 0xff, this.configFolder + "config.ini") + "; Password = " + this.GetINIDataString("DATABASE_SERVER", "USER_PASSWORD", "", 0xff, this.configFolder + "config.ini") + ";";
        }

        public void BroadcastPacket(int portNo, string broadCastMessage, string callingModule, DataTable hostIPTable)
        {
            foreach (DataRow row in hostIPTable.Select(""))
            {
                if (row["HOST_IP"].ToString().Trim() != "")
                {
                    try
                    {
                        Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                        IPEndPoint remoteEP = new IPEndPoint(IPAddress.Parse(row["HOST_IP"].ToString().Trim()), portNo);
                        Base64Encoder encoder = new Base64Encoder(Encoding.ASCII.GetBytes(broadCastMessage));
                        byte[] bytes = Encoding.ASCII.GetBytes(encoder.GetEncoded());
                        socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
                        socket.SendTo(bytes, remoteEP);
                        socket.Close();
                    }
                    catch (Exception exception)
                    {
                        Console.WriteLine(exception.ToString());
                    }
                }
            }
        }

        public void BroadcastPacketSingleTarget(int portNo, string broadCastMessage, string callingModule, string destinationIP)
        {
            try
            {
                Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                IPEndPoint remoteEP = new IPEndPoint(IPAddress.Parse(destinationIP), portNo);
                Base64Encoder encoder = new Base64Encoder(Encoding.ASCII.GetBytes(broadCastMessage));
                byte[] bytes = Encoding.ASCII.GetBytes(encoder.GetEncoded());
                socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
                socket.SendTo(bytes, remoteEP);
                socket.Close();
            }
            catch (Exception exception)
            {
                Console.WriteLine(exception.ToString());
            }
        }

        public string ConvertBoolToData(bool localData)
        {
            if (localData)
            {
                return "Y";
            }
            return "N";
        }

        public bool ConvertDataToBool(string localData)
        {
            return (localData.ToUpper() == "Y");
        }

        public void ExecuteOracleNonQuery(string query, OracleConnection localConnection, string callingModule)
        {
            int num = 0;
            bool flag = true;
            while (flag && (num < 3))
            {
                try
                {
                    OracleCommand command = new OracleCommand(query, localConnection);
                    command.CommandType = CommandType.Text;
                    command.ExecuteNonQuery();
                    if (command != null)
                    {
                        command.Dispose();
                    }
                    flag = false;
                }
                catch (Exception exception)
                {
                    this.WriteEventLog(exception.ToString(), callingModule);
                }
                num++;
            }
        }

        public void ExecuteSQLNonQuery(string query, string callingModule)
        {
            try
            {
                SqlConnection connection = new SqlConnection(this.sqlConnectionString);
                connection.Open();
                SqlCommand command = null;
                int num = 0;
                bool flag = true;
                while (flag && (num < 3))
                {
                    try
                    {
                        try
                        {
                            command = new SqlCommand(query, connection);
                            command.CommandType = CommandType.Text;
                            command.ExecuteNonQuery();
                            flag = false;
                        }
                        catch (Exception exception1)
                        {
                            this.WriteEventLog(exception1.ToString(), callingModule);
                        }
                    }
                    finally
                    {
                        if (command != null)
                        {
                            command.Dispose();
                        }
                    }
                    num++;
                }
                if (connection != null)
                {
                    connection.Close();
                    connection.Dispose();
                }
            }
            catch (Exception exception2)
            {
                this.WriteEventLog(exception2.ToString(), callingModule);
            }
        }

        public byte[] GetBinaryFile(string filePath)
        {
            FileStream input = new FileStream(filePath, FileMode.Open, FileAccess.Read);
            BinaryReader reader = new BinaryReader(input);
            byte[] buffer = reader.ReadBytes((int) input.Length);
            reader.Close();
            input.Close();
            return buffer;
        }

        public string[] GetINIData(string entryCategory, string entryKey, string entryDefault, int entrySize, string entryFile)
        {
            this.tempStringBuilder = new StringBuilder(entrySize);
            int num = GetPrivateProfileString(entryCategory, entryKey, entryDefault, this.tempStringBuilder, entrySize, entryFile);
            return this.tempStringBuilder.ToString().Split(new char[] { ',' });
        }

        public string GetINIDataString(string entryCategory, string entryKey, string entryDefault, int entrySize, string entryFile)
        {
            this.tempStringBuilder = new StringBuilder(entrySize);
            int num = GetPrivateProfileString(entryCategory, entryKey, entryDefault, this.tempStringBuilder, entrySize, entryFile);
            return this.tempStringBuilder.ToString().Trim();
        }

        public int GetIntFromQuery(OracleCommand cmdOracle, string callingModule)
        {
            int num = 0;
            OracleDataReader reader = null;
            try
            {
                try
                {
                    reader = cmdOracle.ExecuteReader();
                    if (reader.Read() && !reader.IsDBNull(0))
                    {
                        num = Convert.ToInt32(reader.GetValue(0).ToString());
                    }
                    return num;
                }
                catch (Exception exception)
                {
                    this.WriteEventLog(exception.ToString(), callingModule);
                }
            }
            finally
            {
                if (reader != null)
                {
                    reader.Close();
                    reader.Dispose();
                }
            }
            return num;
        }

        public int GetIntFromQuery(string cmdQuery, OracleConnection localConnection, string callingModule)
        {
            int num = 0;
            OracleCommand command = new OracleCommand(cmdQuery, localConnection);
            OracleDataReader reader = null;
            try
            {
                try
                {
                    reader = command.ExecuteReader();
                    if (reader.Read() && !reader.IsDBNull(0))
                    {
                        num = Convert.ToInt32(reader.GetValue(0).ToString());
                    }
                    return num;
                }
                catch (Exception exception)
                {
                    this.WriteEventLog(exception.ToString(), callingModule);
                }
            }
            finally
            {
                if (command != null)
                {
                    command.Dispose();
                }
                if (reader != null)
                {
                    reader.Dispose();
                }
            }
            return num;
        }

        [DllImport("kernel32")]
        public static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);
        public string GetSingleStringFromQuery(OracleCommand cmdOracle, string callingModule)
        {
            string str = "";
            OracleDataReader reader = null;
            try
            {
                try
                {
                    reader = cmdOracle.ExecuteReader();
                    if (reader.Read() && !reader.IsDBNull(0))
                    {
                        str = reader.GetOracleString(0).Value.Trim();
                    }
                    return str;
                }
                catch (Exception exception)
                {
                    this.WriteEventLog(exception.ToString(), callingModule);
                }
            }
            finally
            {
                if (reader != null)
                {
                    reader.Close();
                    reader.Dispose();
                }
            }
            return str;
        }

        public string GetSingleStringFromQuery(string cmdQuery, OracleConnection localConnection, string callingModule)
        {
            string str = "";
            OracleCommand command = new OracleCommand(cmdQuery, localConnection);
            OracleDataReader reader = null;
            try
            {
                try
                {
                    reader = command.ExecuteReader();
                    if (reader.Read() && !reader.IsDBNull(0))
                    {
                        str = reader.GetString(0);
                    }
                    return str;
                }
                catch (Exception exception)
                {
                    this.WriteEventLog(exception.ToString(), callingModule);
                }
            }
            finally
            {
                if (command != null)
                {
                    command.Dispose();
                }
                if (reader != null)
                {
                    reader.Close();
                    reader.Dispose();
                }
            }
            return str;
        }

        public DataRow GetSQLQuery(string localSQL, OracleConnection localConnection)
        {
            DataTable dataTable = new DataTable();
            using (null)
            {
                try
                {
                    new OracleDataAdapter(localSQL, localConnection).Fill(dataTable);
                }
                catch (Exception exception)
                {
                    Console.WriteLine(exception.ToString());
                }
            }
            if (dataTable.Rows.Count > 0)
            {
                return dataTable.Rows[0];
            }
            return null;
        }

        public DataSet GetSQLQuery(string localSQL, string localDataSetName, OracleConnection localConnection)
        {
            DataSet dataSet = new DataSet(localDataSetName);
            using (null)
            {
                try
                {
                    new OracleDataAdapter(localSQL, localConnection).Fill(dataSet);
                }
                catch (Exception exception)
                {
                    Console.WriteLine(exception.ToString());
                }
            }
            return dataSet;
        }

        public string GetStringFromQuery(string cmdQuery, OracleConnection localConnection, string callingModule)
        {
            string str = "";
            OracleCommand command = new OracleCommand(cmdQuery, localConnection);
            OracleDataReader reader = null;
            try
            {
                try
                {
                    reader = command.ExecuteReader();
                    while (reader.Read())
                    {
                        if (!reader.IsDBNull(0))
                        {
                            str = str + reader.GetString(0) + ",";
                        }
                    }
                }
                catch (Exception exception)
                {
                    this.WriteEventLog(exception.ToString(), callingModule);
                }
            }
            finally
            {
                if (command != null)
                {
                    command.Dispose();
                }
                if (reader != null)
                {
                    reader.Close();
                    reader.Dispose();
                }
            }
            if ((str.Trim() != "") && ((str.LastIndexOf(",") + 1) == str.Length))
            {
                str = str.Substring(0, str.Length - 1);
            }
            return str;
        }

        public DataTable GetTccHostIP(OracleConnection localConnection, string callingModule)
        {
            string cmdText = "";
            DataTable table = new DataTable("TCC_CLIENT");
            table.Columns.Add("HOST_IP", Type.GetType("System.String"));
            int num = 0;
            bool flag = true;
            while (flag && (num < 3))
            {
                try
                {
                    cmdText = " SELECT DISTINCT HOST_IP FROM TCC_CLIENT ORDER BY HOST_IP";
                    OracleCommand command = new OracleCommand(cmdText, localConnection);
                    command.CommandType = CommandType.Text;
                    OracleDataReader reader = command.ExecuteReader();
                    DataRow row = null;
                    table.Rows.Clear();
                    while (reader.Read())
                    {
                        if (!reader.IsDBNull(0))
                        {
                            row = table.NewRow();
                            row["HOST_IP"] = reader.GetOracleString(0).Value.ToString();
                            table.Rows.Add(row);
                            table.AcceptChanges();
                        }
                    }
                    if (reader != null)
                    {
                        reader.Close();
                        reader.Dispose();
                    }
                    if (command != null)
                    {
                        command.Dispose();
                    }
                    flag = false;
                }
                catch (Exception exception)
                {
                    this.WriteEventLog(exception.ToString(), callingModule);
                }
                num++;
            }
            return table;
        }

        public bool IsBoolean(string val)
        {
            bool flag;
            return bool.TryParse(val, out flag);
        }

        public bool IsNumeric(string val, NumberStyles NumberStyle)
        {
            double num;
            return double.TryParse(val, NumberStyle, CultureInfo.CurrentCulture, out num);
        }

        public string PreparePagingStatement(string queryString, int lowerRecord, int upperRecord)
        {
            queryString = " SELECT * FROM ( SELECT /*+ FIRST_ROWS(n) */ a.*, ROWNUM rnum  FROM ( " + queryString + " ) a ) WHERE ROWNUM <= " + upperRecord.ToString() + " AND RNUM >= " + lowerRecord.ToString();
            return queryString;
        }

        public void WriteEventLog(string messageContent, string moduleName)
        {
            try
            {
                StreamWriter writer = System.IO.File.AppendText(this.configFolder + "logs/" + DateTime.Now.ToString("yyyyMMdd") + ".txt");
                writer.WriteLine(moduleName + " [" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + "]" + messageContent);
                writer.Close();
            }
            catch (Exception exception)
            {
                Console.WriteLine(exception.ToString());
            }
        }
    }
}
