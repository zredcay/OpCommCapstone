#ifndef _stateMachine_h_
#define _stateMachine_h_


typedef struct mainData{
	int trans;
	float angle;
	float dist;
	float Vx;
	float Vy
};

typedef enum
{
    Discovery,
    Alternate_Discovery,
    Maintenance,
    Recovery,
    End,
    Intialization
}State;

typedef enum
{
    Code_Finished_Event,
    Bad_Data_Event,
    Loss_of_LOS_Event,
    User_Exit_Event,
    Should_Not_Get_Here_Event,
    Timeout_Event
} Event;

State CodeFinishedHandler(State thisState)
{
	if(thisState == Intialization)
	{
		return Discovery;
	}
	else if(thisState == Discovery || thisState == Alternate_Discovery)
	{
		return Maintenance;
	}
	else if(thisState == Recovery)
	{
		return Maintenance;
	}
	else if(thisState == Maintenance)
	{
		return Maintenance;
	}
	else
	{
		return End;
	}

}

State BadDataHandler(State thisState)
{
    	if(thisState == Discovery)
	{
		return Alternate_Discovery;
	}
	else if(thisState == Maintenance)
	{
		return Maintenance;
	}
	else  if(thisState == Recovery)
	{
		return Discovery;
	}
	else
	{
		return End;
	}
}

State ShouldNotGetHandler(State thisState){

	return End;
}

State UserExitHandler(State thisState){

	return End;
}

State TimeoutEventHandler(State thisState){
     if(thisState == Maintenance)
	{
		return Recovery;
	}
	else  if(thisState == Recovery)
	{
		return Discovery;
	}
	else{
	    return Recovery;
	}

}

#endif




