namespace OPCTrendLib
{
    using System;
    using System.Collections;
    using System.Runtime.InteropServices;

    internal static class BinaryHelper
    {
        public static Hashtable typePriority = new Hashtable();

        static BinaryHelper()
        {
            typePriority.Add(typeof(char), 20);
            typePriority.Add(typeof(sbyte), 30);
            typePriority.Add(typeof(byte), 40);
            typePriority.Add(typeof(short), 50);
            typePriority.Add(typeof(ushort), 60);
            typePriority.Add(typeof(int), 70);
            typePriority.Add(typeof(uint), 80);
            typePriority.Add(typeof(long), 90);
            typePriority.Add(typeof(ulong), 100);
            typePriority.Add(typeof(float), 200);
            typePriority.Add(typeof(double), 300);
            typePriority.Add(typeof(decimal), 400);
            typePriority.Add(typeof(string), 500);
        }

        public static bool ComparePrimitiveType(Type type1, Type type2, out int result)
        {
            result = 0;
            if ((type1 == null) || (type2 == null))
            {
                return false;
            }
            if (!typePriority.ContainsKey(type1))
            {
                return false;
            }
            if (!typePriority.ContainsKey(type2))
            {
                return false;
            }
            if (type1 != type2)
            {
                int num = (int) typePriority[type1];
                int num2 = (int) typePriority[type2];
                result = num.CompareTo(num2);
            }
            return true;
        }

        public static Type GetType(object o)
        {
            if (o == null)
            {
                return null;
            }
            return o.GetType();
        }

        public static bool IsPrimitiveType(Type type)
        {
            if (type == null)
            {
                return false;
            }
            return typePriority.ContainsKey(type);
        }
    }
}
