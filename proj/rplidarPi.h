#ifndef _rplidarPi_h_
#define _rplidarPi_h_

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct data{
		int trans;
		float angle;
		float dist;
		};
	struct data rplidarPi(int test);
 
#ifdef __cplusplus
} // end extern "C"
#endif


#endif
