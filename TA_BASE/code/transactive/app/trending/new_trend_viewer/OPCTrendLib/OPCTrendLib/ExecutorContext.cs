namespace OPCTrendLib
{
    using System;
    using System.Collections;

    public abstract class ExecutorContext : IExecutorContext
    {
        private Stack _stack = new Stack();
        private object _tag;
        private IExecutor _top;

        public virtual IExecutor CreateExecutor(ExprNode node)
        {
            IExecutor executor = this.CreateExecutorCore(node);
            executor.Initialize(this, node);
            return executor;
        }

        protected abstract IExecutor CreateExecutorCore(ExprNode node);
        protected virtual void Done()
        {
        }

        public Result Execute(ExprNode node)
        {
            if ((node == null) || (node.Expression == null))
            {
                return new Result(string.Empty);
            }
            IExecutor executor = this.CreateExecutor(node);
            IExecutor top = executor;
            this.Push(top);
            while (top != null)
            {
                switch (top.Execute(this))
                {
                    case ExecuteAction.End:
                        this.Pop();
                        break;

                    case ExecuteAction.Exit:
                        return executor.Result;
                }
                top = this.Top;
            }
            return executor.Result;
        }

        public virtual void Pop()
        {
            this._stack.Pop();
            this._top = (IExecutor) this._stack.Peek();
        }

        public virtual void Push(IExecutor executor)
        {
            this._top = executor;
            this._stack.Push(this._top);
        }

        protected virtual void Reset()
        {
            this._stack.Clear();
            this._stack.Push(null);
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

        public IExecutor Top
        {
            get
            {
                return this._top;
            }
        }
    }
}
