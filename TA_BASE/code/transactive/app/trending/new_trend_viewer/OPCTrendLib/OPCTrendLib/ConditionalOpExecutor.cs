namespace OPCTrendLib
{
    using System;

    internal class ConditionalOpExecutor : Executor
    {
        public override ExecuteAction Execute(IExecutorContext context)
        {
            base.CurrentIndex++;
            if (base.CurrentIndex == 0)
            {
                base.SubVisitors[base.CurrentIndex] = context.CreateExecutor((ExprNode) base.Node.Operands[base.CurrentIndex]);
                context.Push(base.SubVisitors[base.CurrentIndex]);
                return ExecuteAction.Continue;
            }
            if (base.CurrentIndex == 1)
            {
                Result result = base.SubVisitors[0].Result;
                int num = -1;
                if ((bool) result.Value)
                {
                    num = 1;
                }
                else
                {
                    num = 2;
                }
                base.SubVisitors[1] = context.CreateExecutor((ExprNode) base.Node.Operands[num]);
                context.Push(base.SubVisitors[1]);
                return ExecuteAction.Continue;
            }
            base.EndVisit(base.SubVisitors[1].Result);
            return ExecuteAction.End;
        }

        public override void Reset(IExecutorContext context)
        {
            base.Reset(context);
        }
    }
}
