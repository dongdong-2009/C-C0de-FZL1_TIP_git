namespace OPCTrendLib
{
    using System;

    internal class Executor : IExecutor
    {
        private int _currentIndex;
        private ExprNode _node;
        private OPCTrendLib.Result _result;
        private IExecutor[] _subVisitors;
        private object _tag;

        protected void EndVisit(OPCTrendLib.Result result)
        {
            this._result = result;
            this._subVisitors = null;
        }

        public virtual ExecuteAction Execute(IExecutorContext context)
        {
            this._currentIndex++;
            if (this._currentIndex == this._node.OperandCount)
            {
                OPCTrendLib.Result[] argArray = new OPCTrendLib.Result[this._node.OperandCount];
                for (int i = 0; i < this._node.OperandCount; i++)
                {
                    argArray[i] = this._subVisitors[i].Result;
                }
                this.EndVisit(this._node.Expression.Eval((Evaluator) context, argArray));
                return ExecuteAction.End;
            }
            if (this._currentIndex > this._node.OperandCount)
            {
                throw new InvalidOperationException();
            }
            ExprNode node = (ExprNode) this._node.Operands[this._currentIndex];
            this._subVisitors[this._currentIndex] = context.CreateExecutor(node);
            context.Push(this._subVisitors[this._currentIndex]);
            return ExecuteAction.Continue;
        }

        public virtual void Initialize(IExecutorContext context, ExprNode node)
        {
            this._node = node;
            this.Reset(context);
        }

        public virtual void Reset(IExecutorContext context)
        {
            this._subVisitors = new IExecutor[this._node.OperandCount];
            this._currentIndex = -1;
        }

        protected int CurrentIndex
        {
            get
            {
                return this._currentIndex;
            }
            set
            {
                this._currentIndex = value;
            }
        }

        public ExprNode Node
        {
            get
            {
                return this._node;
            }
        }

        public OPCTrendLib.Result Result
        {
            get
            {
                return this._result;
            }
        }

        protected IExecutor[] SubVisitors
        {
            get
            {
                return this._subVisitors;
            }
        }

        public object Tag
        {
            get
            {
                return this._tag;
            }
            set
            {
                this._tag = value;
            }
        }
    }
}
