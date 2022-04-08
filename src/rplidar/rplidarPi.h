#ifndef _rplidarPi_h_
#define _rplidarPi_h_

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct lidarData{
		int trans;
		float angle;
		float dist;
		};
	struct lidarData rplidarPi();
	 int connectRP();
	int disconnectRP();
 
#ifdef __cplusplus
} // end extern "C"
#endif


#endif
