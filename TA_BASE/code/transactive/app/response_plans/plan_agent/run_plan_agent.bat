@echo off
plan_agent_copy\PlanAgent --entity-name=PlanAgent --db-connection=TRANSACT:GREGG:GREGG --operation-mode=Control --debug-level=DEBUG --notify-hosts=jupiter:2912 --debug-max-files=1000
REM plan_agent_copy\PlanAgent --entity-name=PlanAgent --db-connection=TRANSACT:GREGG:GREGG --operation-mode=Control --debug-level=DEBUG --notify-hosts=jupiter
