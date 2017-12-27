namespace OPCTrendLib
{
    using System;

    internal class Context
    {
        internal int endPos;
        internal int startPos;
        internal Token token;
        internal object value;

        public Context()
        {
        }

        public Context(int startPos)
        {
            this.startPos = startPos;
            this.token = Token.None;
        }

        public Context(Token token, int startPos)
        {
            this.startPos = startPos;
            this.token = token;
        }

        public override string ToString()
        {
            if (this.value == null)
            {
                return this.token.ToString();
            }
            return (this.token.ToString() + ":" + this.value.ToString());
        }

        internal string StringValue
        {
            get
            {
                return (string) this.value;
            }
        }
    }
}
