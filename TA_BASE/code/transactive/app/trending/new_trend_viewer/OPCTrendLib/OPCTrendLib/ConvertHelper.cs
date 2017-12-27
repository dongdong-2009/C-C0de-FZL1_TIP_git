namespace OPCTrendLib
{
    using System;
    using System.ComponentModel;

    internal static class ConvertHelper
    {
        public static object ChangeType(object value, Type conversionType)
        {
            object obj2 = null;
            try
            {
                obj2 = Convert.ChangeType(value, conversionType);
            }
            catch
            {
                if (conversionType == null)
                {
                    throw new ArgumentNullException("conversionType");
                }
                if (value == null)
                {
                    if (conversionType.IsValueType)
                    {
                        throw new InvalidCastException("Cannot Cast Null To ValueType");
                    }
                    return null;
                }
                TypeConverter converter = TypeDescriptor.GetConverter(value);
                if (converter == null)
                {
                    throw new InvalidCastException("Cannot Cast Null To  Target Type");
                }
                if (!converter.CanConvertTo(conversionType))
                {
                    throw new InvalidCastException("Cannot Cast Null To  Target Type");
                }
                return converter.ConvertTo(value, conversionType);
            }
            return obj2;
        }

        public static string ToString(object value)
        {
            object obj2 = null;
            try
            {
                obj2 = Convert.ChangeType(value, typeof(string));
            }
            catch
            {
                if (value == null)
                {
                    return string.Empty;
                }
                if (value is DBNull)
                {
                    return string.Empty;
                }
                TypeConverter converter = TypeDescriptor.GetConverter(value);
                if (converter == null)
                {
                    throw new InvalidCastException("Cannot Cast Null To  Target Type");
                }
                if (!converter.CanConvertTo(typeof(string)))
                {
                    throw new InvalidCastException("Cannot Cast Null To  Target Type");
                }
                return converter.ConvertToString(value);
            }
            return (string) obj2;
        }
    }
}
