CREATE OR REPLACE FUNCTION verifyMsgsReceived(simulator IN NUMBER) RETURN NUMBER IS v_ret NUMBER;

CURSOR cu_Consumer (simuID NUMBER) IS
 SELECT *
 FROM NST_SIMULATORCONFIG nst_sc
 WHERE nst_sc.simulatorID = simuID AND
 	   nst_sc.handlerFactory = 'CommsConsumer' AND
	   nst_sc.enabled = 1;

 CURSOR cu_MsgRecs (simuID NUMBER) IS
 Select *
 From NST_MESSAGESRECEIVED nst_msRecv
 WHERE nst_msRecv.CONSUMERID = simuID;

 msgsCount NUMBER;
 msgsReceived NUMBER;
 consumerCount NUMBER;

BEGIN
	 v_ret := 1;
	 DBMS_OUTPUT.put_line ('***Verification of received messaged for consumer ' || simulator || '***');
	 SELECT COUNT(simulatorID) INTO consumerCount FROM NST_SIMULATORCONFIG nst_sc
	 WHERE  nst_sc.simulatorID = simulator AND
	 		nst_sc.handlerFactory = 'CommsConsumer' AND
			nst_sc.enabled = 1;
	 IF 0 = consumerCount OR consumerCount IS NULL  THEN
	  	v_ret := -1;
		DBMS_OUTPUT.put_line ('Consumer ' || simulator || ' does not exist');
		RETURN v_ret;
 	 END IF;

	 FOR rec IN cu_Consumer (simulator) LOOP
	     IF rec.msgTypeName = '*' THEN
		 /*Calculate the expected message count from NST_SIMULATORCONFIG
		 verify with the NST_MESSAGERECEIVED table if that count matches.
		 */
		   select sum(numSteps) INTO msgsCount FROM NST_SIMULATORCONFIG nst_sc
		          WHERE nst_sc.handlerFactory = 'CommsSupplier' AND
				  		nst_sc.srcLocationKey = rec.srcLocationKey AND
						nst_sc.NOTIFICATIONHOST = rec.NotificationHost AND 
				        nst_sc.enabled = 1;
		   select count(consumerID) INTO msgsReceived FROM NST_MESSAGESRECEIVED nst_msRecv
		   		  WHERE nst_msRecv.CONSUMERID = simulator;
		   IF msgsReceived != msgsCount THEN
		      v_ret := -1;
			  DBMS_OUTPUT.put_line ('Number of msgs received [' || msgsReceived
			                        || '] do not match with expected [' || msgsCount || '] msgs');
			  RETURN v_ret;
		   END IF;
		 ELSE
		    select sum(numSteps) INTO msgsCount FROM NST_SIMULATORCONFIG nst_sc
		          WHERE nst_sc.handlerFactory = 'CommsSupplier' AND
				  nst_sc.MSGTYPENAME = rec.MSGTYPENAME AND
				  nst_sc.srcLocationKey = rec.srcLocationKey AND
				  nst_sc.NOTIFICATIONHOST = rec.NotificationHost AND 
				  nst_sc.enabled = 1;
		   select count(consumerID) INTO msgsReceived FROM NST_MESSAGESRECEIVED nst_msRecv
		   		  WHERE nst_msRecv.CONSUMERID = simulator;
		 	 IF msgsCount != msgsReceived THEN
		  	 	v_ret := -1;
			    DBMS_OUTPUT.put_line ('Number of msgs received [' || msgsReceived
			                          || '] do not match with expected [' || msgsCount || '] msgs');

			    RETURN v_ret;
			 END IF;
	 	 END IF;
	 END LOOP;

	 FOR msgsrec IN cu_MsgRecs(simulator) LOOP
	 	 IF msgsrec.srcSeqNum != msgsrec.expectedSeqNum THEN
		    DBMS_OUTPUT.put_line ('The srcSeqNum [' || msgsrec.srcSeqNum || '] ' ||
			                      'does not match with expectedSeqNum [' ||  msgsrec.expectedSeqNum ||
								  '] for this consumer ' || simulator);
		 	RETURN -1;
		 END IF;
	 END LOOP;
	 DBMS_OUTPUT.put_line ('All the messages for consumer ' || simulator || ' have been received.');
	 RETURN v_ret;
 EXCEPTION
 WHEN OTHERS THEN
 RETURN -1;
END;
/

