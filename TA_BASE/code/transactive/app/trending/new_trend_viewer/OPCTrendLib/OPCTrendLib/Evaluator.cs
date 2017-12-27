namespace OPCTrendLib
{
    using System;

    public class Evaluator : ExecutorContext
    {
        private IVariableHolder _variableHolder;

        protected override IExecutor CreateExecutorCore(ExprNode node)
        {
            Expression expression = node.Expression;
            if (expression is ConditionalOp)
            {
                return new ConditionalOpExecutor();
            }
            if (expression is BooleanAnd)
            {
                return new BooleanAndExecutor();
            }
            if (expression is BooleanOr)
            {
                return new BooleanOrExecutor();
            }
            if (expression is BinaryOp)
            {
                return new BinaryOpExecutor();
            }
            if (expression is Operand)
            {
                return new OperandExecutor();
            }
            return new Executor();
        }

        public object Eval(Tree tree)
        {
            this.Reset();
            Result result = base.Execute(tree.Root);
            this.Done();
            return result.Value;
        }

        public object Eval(string text)
        {
            Tree tree = new Parser().Parse(text);
            return this.Eval(tree);
        }

        public IVariableHolder VariableHolder
        {
            get
            {
                return this._variableHolder;
            }
            set
            {
                this._variableHolder = value;
            }
        }
    }
}
