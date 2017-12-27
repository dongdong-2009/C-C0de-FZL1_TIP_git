namespace Jyc.Expr
{
    using System;
    using System.CodeDom.Compiler;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Globalization;
    using System.Resources;
    using System.Runtime.CompilerServices;

    [GeneratedCode("System.Resources.Tools.StronglyTypedResourceBuilder", "2.0.0.0"), DebuggerNonUserCode, CompilerGenerated]
    internal class SR
    {
        private static CultureInfo resourceCulture;
        private static System.Resources.ResourceManager resourceMan;

        internal SR()
        {
        }

        [EditorBrowsable(EditorBrowsableState.Advanced)]
        internal static CultureInfo Culture
        {
            get
            {
                return resourceCulture;
            }
            set
            {
                resourceCulture = value;
            }
        }

        internal static string ErrorFormat
        {
            get
            {
                return ResourceManager.GetString("ErrorFormat", resourceCulture);
            }
        }

        internal static string InternalError
        {
            get
            {
                return ResourceManager.GetString("InternalError", resourceCulture);
            }
        }

        internal static string NoParenBefore
        {
            get
            {
                return ResourceManager.GetString("NoParenBefore", resourceCulture);
            }
        }

        internal static string NotHexChar
        {
            get
            {
                return ResourceManager.GetString("NotHexChar", resourceCulture);
            }
        }

        internal static string NotSupportGlobalFunction
        {
            get
            {
                return ResourceManager.GetString("NotSupportGlobalFunction", resourceCulture);
            }
        }

        internal static string ParenNotMatch
        {
            get
            {
                return ResourceManager.GetString("ParenNotMatch", resourceCulture);
            }
        }

        [EditorBrowsable(EditorBrowsableState.Advanced)]
        internal static System.Resources.ResourceManager ResourceManager
        {
            get
            {
                if (object.ReferenceEquals(resourceMan, null))
                {
                    System.Resources.ResourceManager manager = new System.Resources.ResourceManager("Jyc.Expr.SR", typeof(Jyc.Expr.SR).Assembly);
                    resourceMan = manager;
                }
                return resourceMan;
            }
        }

        internal static string UnrecogniseChar
        {
            get
            {
                return ResourceManager.GetString("UnrecogniseChar", resourceCulture);
            }
        }

        internal static string VariableNotExist
        {
            get
            {
                return ResourceManager.GetString("VariableNotExist", resourceCulture);
            }
        }
    }
}
