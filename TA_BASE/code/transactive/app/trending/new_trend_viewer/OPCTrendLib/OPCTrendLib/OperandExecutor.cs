namespace OPCTrendLib
{
    using System;

    internal class OperandExecutor : Executor
    {
        public override ExecuteAction Execute(IExecutorContext context)
        {
            base.EndVisit(base.Node.Expression.Eval((Evaluator) context, null));
            return ExecuteAction.End;
        }

        public override void Reset(IExecutorContext context)
        {
        }
    }
}
