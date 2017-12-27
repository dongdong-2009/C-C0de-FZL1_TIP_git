namespace OPCTrendLib
{
    using System;
    using System.Collections;
    using System.IO;

    public class Tree
    {
        internal bool _isStart;
        private Stack _nodeStack;
        internal ExprNode _root = new ExprNode(null);
        internal ExprNode _top;

        public Tree()
        {
            this._top = this._root;
            this._nodeStack = new Stack();
            this._isStart = true;
        }

        internal void AddOperand(Operand expr)
        {
            this._isStart = false;
            ExprNode node = new ExprNode(expr);
            this._top.Add(node);
        }

        internal void AddOperator(Operator expr)
        {
            this._isStart = false;
            ExprNode node = new ExprNode(expr);
            ExprNode lastOperand = this._top.LastOperand;
            if (this._top.Expression == null)
            {
                this._top.Expression = expr;
            }
            else
            {
                Operator expression = this._top.Expression as Operator;
                if (expression == null)
                {
                    throw ParserException.InternalError();
                }
                if (expr.Priority > expression.Priority)
                {
                    if (lastOperand == null)
                    {
                        throw ParserException.InternalError();
                    }
                    this._top.Remove(lastOperand);
                    node.Add(lastOperand);
                    this._top.Add(node);
                    this._top = node;
                }
                else if (this._top.Parent != null)
                {
                    node.Add(this._root);
                    this._root = node;
                    this._top = node;
                }
                else
                {
                    node.Add(this._top);
                    this._top = node;
                    this._root = this._top;
                }
            }
        }

        internal void Complete()
        {
            if (this._nodeStack.Count != 0)
            {
                StackNode node = (StackNode) this._nodeStack.Pop();
                throw ParserException.ParenNotMatch(node.charPushed);
            }
            this._nodeStack = null;
            if (this._root.Expression == null)
            {
                if (this._root.OperandCount == 0)
                {
                    this._root = null;
                }
                else
                {
                    if (this._root.OperandCount > 1)
                    {
                        throw ParserException.InternalError();
                    }
                    this._root = (ExprNode) this._root.Operands[0];
                    this._root._parent = null;
                }
            }
        }

        internal void Pop(char ch)
        {
            this._isStart = false;
            if (this._nodeStack.Count == 0)
            {
                if (ch == ',')
                {
                    throw ParserException.NoParenBefore("'( or ']'");
                }
                throw ParserException.NoParenBefore(ch.ToString());
            }
            StackNode node = (StackNode) this._nodeStack.Pop();
            if (((ch != ',') && (node.charPushed != ',')) && (ch != node.charPushed))
            {
                throw ParserException.ParenNotMatch(ch);
            }
            if (this._root.Expression == null)
            {
                if (this._root.OperandCount > 0)
                {
                    foreach (ExprNode node2 in this._root.Operands)
                    {
                        node2._parent = node.top;
                    }
                    node.top.Operands.AddRange(this._root.Operands);
                }
            }
            else
            {
                node.top.Add(this._root);
            }
            this._root = node.root;
            this._top = node.top;
        }

        public void Print(TextWriter w)
        {
            if ((this._root != null) && (this._root.Expression != null))
            {
                PrintNode(this._root, w, 0);
            }
        }

        private static void PrintIndent(TextWriter w, int indent)
        {
            for (int i = 0; i < indent; i++)
            {
                w.Write("  ");
            }
        }

        private static void PrintNode(ExprNode node, TextWriter w, int indent)
        {
            PrintIndent(w, indent);
            w.Write(node.Expression.ToString());
            if (node.Expression is Operator)
            {
                w.Write(':');
            }
            w.WriteLine();
            if (node.OperandCount != 0)
            {
                foreach (ExprNode node2 in node.Operands)
                {
                    PrintNode(node2, w, indent + 1);
                }
            }
        }

        internal void Push(char ch)
        {
            StackNode node = new StackNode();
            node.root = this._root;
            node.top = this._top;
            node.charPushed = ch;
            this._nodeStack.Push(node);
            this._root = new ExprNode(null);
            this._top = this._root;
            this._isStart = true;
        }

        internal ExprNode Root
        {
            get
            {
                return this._root;
            }
            set
            {
                this._root = value;
            }
        }

        internal class StackNode
        {
            internal char charPushed;
            internal ExprNode root;
            internal ExprNode top;
        }
    }
}
