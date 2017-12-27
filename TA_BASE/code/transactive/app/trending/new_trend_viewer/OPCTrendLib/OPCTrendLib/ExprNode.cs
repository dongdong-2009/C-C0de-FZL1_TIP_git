namespace OPCTrendLib
{
    using System;
    using System.Collections;

    public class ExprNode
    {
        private OPCTrendLib.Expression _expression;
        private ArrayList _operands;
        internal ExprNode _parent;

        internal ExprNode(OPCTrendLib.Expression expression)
        {
            this._expression = expression;
        }

        internal void Add(ExprNode node)
        {
            if (node._parent != null)
            {
                throw new ArgumentException();
            }
            node._parent = this;
            this.Operands.Add(node);
        }

        internal void Remove(ExprNode node)
        {
            if (node.Parent != this)
            {
                throw new ArgumentException();
            }
            node._parent = null;
            this.Operands.Remove(node);
        }

        internal ExprNode RemoveAt(int index)
        {
            ExprNode node = (ExprNode) this.Operands[index];
            node._parent = null;
            this.Operands.RemoveAt(index);
            return node;
        }

        internal OPCTrendLib.Expression Expression
        {
            get
            {
                return this._expression;
            }
            set
            {
                this._expression = value;
            }
        }

        internal ExprNode LastOperand
        {
            get
            {
                if (this._operands == null)
                {
                    return null;
                }
                if (this._operands.Count == 0)
                {
                    return null;
                }
                return (ExprNode) this._operands[this._operands.Count - 1];
            }
        }

        internal int OperandCount
        {
            get
            {
                if (this._operands == null)
                {
                    return 0;
                }
                return this._operands.Count;
            }
        }

        internal ArrayList Operands
        {
            get
            {
                if (this._operands == null)
                {
                    this._operands = new ArrayList(2);
                }
                return this._operands;
            }
        }

        internal ExprNode Parent
        {
            get
            {
                return this._parent;
            }
        }
    }
}
