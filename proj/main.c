#include <stdio.h>
#include <stdlib.h>
#include "stateMachine.h"
#include "rplidarPi.h"
#include <time.h>
#include <stdbool.h>
#include "transeivers.h"


int main ()
{
	int test;
	int result;	
	State NextState = Discovery;
	printf("setting event\n");
	Event NewEvent = Code_Finished_Event;
	clock_t startTime = clock();
	while(clock() < (startTime + 3 * CLOCKS_PER_SEC))
	{

		switch(NextState)
		{
		case Idle:
        	{
        		printf("case idle state\n");
            		if(User_Exit_Event == NewEvent)
            		{
            			
            			printf("leave code its broken\n");         		
            		}
            		else if(Should_Not_Get_Here_Event == NewEvent)
            		{
            			
            			printf("big error stop code now\n");
            			exit(-1);         		
            		}
            		else if(Code_Finished_Event == NewEvent)
            		{
            			printf("code work as expected\n");
            			exit(0); 
            		}
            		else
            		{
				printf("event not updated or waiting on user\n");
            			exit(-1);   
			}
            		
        	}
        	break;
        	
		
		case Discovery:
		{
			
			if(Code_Finished_Event == NewEvent)
		    	{
			    	result  = rplidarPi(test);
			    	printf("return value %i\n", result);
			    	if(result == 0 )
			    	{
			    		NewEvent = Bad_Data_Event;
			    		NextState = BadDataHandler(NextState);
			    	}
			    	else if ( result < 9 && result > 0)
			    	{
			    		NewEvent = Code_Finished_Event;
			    		NextState = CodeFinishedHandler(NextState);
			    	}
			    	else 
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
            			trans();
            			NewEvent = User_Exit_Event;
            			printf("in pin enter event\n");
                		NextState = Idle;
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
