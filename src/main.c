#include <stdio.h>
#include <stdlib.h>
#include "stateMachine.h"
#include "rplidarPi.h"
#include <time.h>
#include <stdbool.h>
#include "transeivers.h"
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <errno.h>
#include "sharedMemory.h"

int math(float Ax, float Ay, float Az, float Gx, float Gy, float Gz, float Mx, float My, float Mz, int trans, float angle)
{



return 0;
}


int main ()
{



	
		
	char message[100];
	int n = 0;
	int test;
	float angle;
	float dist;
	int transceiver;
	int result;	
	struct data ex;
	State NextState = Intialization;
	printf("setting event\n");
	Event NewEvent = Code_Finished_Event;
	clock_t startTime = clock();
	while(clock() < (startTime + 3 * CLOCKS_PER_SEC))
	{
		switch(NextState)
		{
		case Intialization:
        	{
        		 connectRP();
        		//sharedMemory();
            		NewEvent = Code_Finished_Event;
			NextState = CodeFinishedHandler(NextState);
        	}
        	break;
		
		case End:
        	{
        		//printf("case end state\n");
            		if(User_Exit_Event == NewEvent) // code completes safely and exits S
            		{
            			disconnectRP();
            			//printf("Code Finished\n");         		
            		}
            		else if(Should_Not_Get_Here_Event == NewEvent) // uncaught error or bug in code 
            		{
            			disconnectRP();
            			printf("big error stop code now\n");
            			exit(-1);         		
            		}
            		else // unexpected state and event matching 
            		{
            			disconnectRP();
				printf("event not updated or waiting on user\n");
            			exit(-1);   
			}
            		
        	}
        	break;
        	
		
		case Discovery:
		{
			
			if(Code_Finished_Event == NewEvent)
		    	{
			    	ex = rplidarPi(test);// transceiver is the tranciever number
			    	transceiver = ex.trans;
			    	 angle = ex.angle;
			    	 dist = ex.dist;
			    	printf("return value transceievr %i and angle %f and distance %f\n", transceiver, angle, dist);
			    	if(angle == 0 ) // the transceiver was not found
			    	{
			    		NewEvent = Bad_Data_Event;
			    		NextState = BadDataHandler(NextState);
			    	}
			    	else if ( angle < 360 && angle > 0) // in a given transcieer 1 - 8
			    	{
			    		NewEvent = Code_Finished_Event;
			    		NextState = CodeFinishedHandler(NextState);
			    	}
			    	else // transceiver should not give a number higher than 8 
			    	{
					NewEvent = Should_Not_Get_Here_Event;
					NextState = ShouldNotGetHandler(NextState);
			    	}
			    	startTime = clock();
				
			    }
			
			    
		}
		break;
		
		case Alternate_Discovery:
        	{
        		printf("in alternatre discovery\n");
        		
            		if(Bad_Data_Event == NewEvent)
            		{
            			if(true)
            			{
            				NewEvent = Code_Finished_Event;
            				NextState = CodeFinishedHandler(NextState);
            			
            			}
            			else 
            			{
            				NewEvent = Bad_Data_Event;
            				NextState = BadDataHandler(NextState);
            			}
                		startTime = clock();
            		}
            		else 
            		{
            			NewEvent = Should_Not_Get_Here_Event;
            			NextState = ShouldNotGetHandler(NextState);
            		}
            		
        	}
        	break;
        	
        	case Maintenance:
        	{
        		
            		if(Code_Finished_Event == NewEvent)
            		{
            			result = trans();
            			printf("Result = %i\n", result);
            			if (result == 0) // result is good data
            			{
            				NewEvent = Code_Finished_Event;
            				NextState = CodeFinishedHandler(NextState);
            			}
            			else if(result == 1) // result is bad data
            			{
            				NewEvent = Bad_Data_Event;
            				NextState = BadDataHandler(NextState);
            			}
            			else if(result == 2) //reulst is end of file
            			{
            				NewEvent = User_Exit_Event;
            				NextState = UserExitHandler(NextState);
            			}
            			else // broken return
            			{
            				NewEvent = Should_Not_Get_Here_Event;
            				NextState = ShouldNotGetHandler(NextState);
            			}
            			startTime = clock();
            		}
            		else
            		{
				NewEvent = Should_Not_Get_Here_Event;
				startTime = clock();
			}
            		
        	}
        	break;
        	
        	
        	case Recovery:
        	{
        		
            		if(Bad_Data_Event == NewEvent)
            		{
            			if(true)
            			{
            				NewEvent = Code_Finished_Event;
            				NextState = CodeFinishedHandler(NextState);
            			
            			}
            			else 
            			{
            				NewEvent = Bad_Data_Event;
            				NextState = BadDataHandler(NextState);
            			}
                		startTime = clock();
            		}
            		if(Loss_of_LOS_Event == NewEvent)
            		{
            			
            			if(true)
            			{
            				NewEvent = Code_Finished_Event;
            				NextState = CodeFinishedHandler(NextState);
            			
            			}
            			else 
            			{
            				NewEvent = Bad_Data_Event;
            				NextState = BadDataHandler(NextState);
            			}
                		startTime = clock();
            		}
            		
        	}
        	break;
        	}
	}

return 0;
}
