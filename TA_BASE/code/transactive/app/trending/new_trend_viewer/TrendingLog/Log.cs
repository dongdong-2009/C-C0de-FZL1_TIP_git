using System;
using System.Collections.Generic;
using System.Text;

using log4net;
using log4net.Config;
using log4net.Repository.Hierarchy;
using log4net.Core;
using log4net.Appender;
using log4net.Layout;

namespace LogForTrend
{
    public class Log
    {
        private static Object s_logLock=new Object();

        private static readonly ILog s_log4net = LogManager.GetLogger(typeof(Log));


        private Log() { }



        //currently, we just config log4net programmatically, but not by config file:
        public static void setLogFile(string file)
        {
            lock(s_logLock)
            {
                //log4net.Config.BasicConfigurator.Configure();
                //log4net.Config.XmlConfigurator.Configure();  

                Hierarchy hierarchy = (Hierarchy)LogManager.GetRepository();
                TraceAppender tracer = new TraceAppender();
                PatternLayout patternLayout = new PatternLayout();
                patternLayout.ConversionPattern = "%d [%t] %-5p %m%n"; ;
                patternLayout.ActivateOptions();
                tracer.Layout = patternLayout;
                tracer.ActivateOptions();
                hierarchy.Root.AddAppender(tracer);
                RollingFileAppender roller = new RollingFileAppender();
                roller.Layout = patternLayout;
                roller.AppendToFile = true;
                roller.RollingStyle = RollingFileAppender.RollingMode.Size;
                roller.MaxSizeRollBackups = 10;
                roller.MaximumFileSize = "1MB";
                roller.StaticLogFileName = true;
                roller.File = file;  //"E:/temp/Log_TrendViewer.txt"; 
                roller.ActivateOptions();
                hierarchy.Root.AddAppender(roller);
                hierarchy.Root.Level = Level.All;
                hierarchy.Configured = true;
            }

        }

        public static void  writeLogInfo (string logMsg)
        {
            lock(s_logLock)
            {
                s_log4net.Info(logMsg);
            }
            
        }

        public static void writeLogDebug(string logMsg)
        {
            lock (s_logLock)
            {
                s_log4net.Debug(logMsg);
            }
        }
        public static void writeLogWarn(string logMsg)
        {
            lock (s_logLock)
            {
                s_log4net.Warn(logMsg);
            }
        }
        public static void writeLogError(string logMsg)
        {
            lock (s_logLock)
            {
                s_log4net.Error(logMsg);
            }
        }
        public static void writeLogFatal(string logMsg)
        {
            lock (s_logLock)
            {
                s_log4net.Fatal(logMsg);
            }
        }

        public static void writeLog(string logMsg, LogLevel level)
        {
            lock (s_logLock)
            {
                if (level == LogLevel.DEBUG)
                {
                    s_log4net.Debug(logMsg);
                    return;
                }
                else if (level == LogLevel.INFO)
                {
                    s_log4net.Info(logMsg);
                    return;
                }
                else if (level == LogLevel.WARN)
                {
                    s_log4net.Warn(logMsg);
                    return;
                }
                else if (level == LogLevel.ERROR)
                {
                    s_log4net.Error(logMsg);
                    return;
                }
                else if (level == LogLevel.FATAL)
                {
                    s_log4net.Fatal(logMsg);
                    return;
                }
            }
        }
    }
}
