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
#include <semaphore.h>

int math(float Ax, float Ay, float Az, float Gx, float Gy, float Gz, float Mx, float My, float Mz, int trans, float angle)
{
	


return 0;
}


int main ()
{
	int flag = 0;
	char message[100];
	int n = 0;
	int test;
	float Ax, float Ay, float Az, float Gx, float Gy, float Gz, float Mx, float My, float Mz;
	float angle;
	float dist;
	int transceiver;
	int result;	
	
	struct data ex;
	struct shared sharMem;
	struct Memory arr;
	
	
	sem_t* mutex;
	
	
	
	
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
        		sharMem = createMemory(); // creates shared memory
        		mutex = createNamedSem(); // creates named semaphore
			
			clock_t init_bluetooth = clock();
			int elapsed_time = 0;

			do{
				clock_t difference = clock() - init_bluetooth;
				elapsed_time = difference*1000/CLOCKS_PER_SEC;
			}while(elapsed_time < 30000);
			
			
        		arr = sharedMemory(flag, sharMem, mutex); // also maintenance code
        		closeNamedSem(mutex); // goes in end tpo close named semphore
        		closeMemeory(sharMem); // goes in end to close shared memory
        		////************this is maintenance code **************
        		for(int i = 0; i < 36; i++)
     				{
     	  				printf("Server has filled %i to shared memory...\n", arr.data[i]);  
     	  				//use this to convert to the data  
     				}
     			//transceiver = math(Ax, Ay, Az, Gx, Gy, Gz, Mx, My, Mz, transceiver, angle);
     			//************end of maintenance code**********
            		NewEvent = Code_Finished_Event;
			NextState = CodeFinishedHandler(NextState);
        	}
        	break;
		
		case End:
        	{
        		// Closes named semaphore and shared memory before exiting code
        		//closeNamedSem(mutex);
        		//closeMemeory(sharMem);
        		//printf("case end state\n");
            		if(User_Exit_Event == NewEvent) // code completes safely and exits S
            		{
            			//printf("Code Finished\n");         		
            		}
            		else if(Should_Not_Get_Here_Event == NewEvent) // uncaught error or bug in code 
            		{
            			printf("big error stop code now\n");
            			exit(-1);         		
            		}
            		else // unexpected state and event matching 
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
			    	else // angle should not give a number higher than 360
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
            			result = trans(); //result if the data was sent and recived correctly
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
        		//try tans +1 and trans -1 in new recovery code 
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
