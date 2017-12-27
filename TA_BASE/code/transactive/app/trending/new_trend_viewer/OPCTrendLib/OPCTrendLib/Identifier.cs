namespace OPCTrendLib
{
    using System;

    internal abstract class Identifier : Operand
    {
        private string _name;

        public Identifier(string name)
        {
            this._name = name;
        }

        public string Name
        {
            get
            {
                return this._name;
            }
            set
            {
                this._name = value;
            }
        }
    }
}
