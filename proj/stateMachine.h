

typedef enum 
{
    Discovery,
    Alternate_Discovery,
    Maintenance,
    Recovery,
    Idle,
}State;

typedef enum
{
    Code_Finished_Event,
    Bad_Data_Event,
    Loss_of_LOS_Event,
    User_Exit_Event,
    Should_Not_Get_Here_Event
} Event;

State CodeFinishedHandler(State thisState)
{
	if(thisState == Discovery || thisState == Alternate_Discovery)
	{
		return Maintenance;
	}
	else if(thisState == Recovery)
	{
		return Discovery;
	}
	else 
	{
		return Idle;
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
		return Recovery;
	}
	else  if(thisState == Recovery)
	{
		return Discovery;
	}
	else
	{
		return Idle;
	}
}

State ShouldNotGetHandler(State thisState){

	return Idle;
}






