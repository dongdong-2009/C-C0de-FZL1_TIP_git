CREATE OR REPLACE FUNCTION verifyMsgsSent(simulator IN NUMBER) RETURN NUMBER IS v_ret NUMBER;

 CURSOR cu_Suppliers (simuID NUMBER) IS
 SELECT *
 FROM NST_SIMULATORCONFIG nst_sc
 WHERE nst_sc.simulatorID = simuID AND nst_sc.handlerFactory = 'CommsSupplier';

 msgsCount NUMBER;
 supplierCount NUMBER;

BEGIN
	 v_ret := 1;
	 DBMS_OUTPUT.put_line ('***Verification of sent messages for supplier ' || simulator || '***');
	 SELECT COUNT(simulatorID) INTO supplierCount FROM NST_SIMULATORCONFIG nst_sc
	 WHERE  nst_sc.simulatorID = simulator AND nst_sc.handlerFactory = 'CommsSupplier';
	 IF 0 = supplierCount OR supplierCount IS NULL  THEN
	  	v_ret := -5;
		RETURN v_ret;
 	 END IF;
	 FOR rec IN cu_Suppliers (simulator) LOOP
	 	 SELECT COUNT(supplierid) INTO msgsCount FROM NST_MESSAGESSENT nst_ms
		 WHERE  nst_ms.SUPPLIERID =  rec.simulatorID AND nst_ms.MSGCONTEXTNAME = rec.MSGCONTEXTNAME
		        AND nst_ms.MSGTYPENAME = rec.MSGTYPENAME;
		 IF msgsCount != rec.numSteps THEN
		  	v_ret := -1;
			DBMS_OUTPUT.put_line ('The number of messages sent [' || msgsCount /
			']  do not match with the configuration [' || rec.numSteps || ']');
			RETURN v_ret;
	 	 END IF;
	 END LOOP;
	 DBMS_OUTPUT.put_line ('All the messages for supplier ' || simulator || ' have been sent.');
	 RETURN v_ret;

EXCEPTION
WHEN OTHERS THEN
RETURN -1;
END;
/

