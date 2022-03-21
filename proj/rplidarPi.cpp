#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "sl_lidar.h" 
#include "sl_lidar_driver.h"
#include "rplidarPi.h"
#include <unistd.h>

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif
static inline void delay(sl_word_size_t ms){
    while (ms>=1000){
        usleep(1000*1000);
        ms-=1000;
    };
    if (ms!=0)
        usleep(ms*1000);
}
using namespace sl;



bool checkSLAMTECLIDARHealth(ILidarDriver * drv)
{
    sl_result     op_result;
    sl_lidar_response_device_health_t healthinfo;

    op_result = drv->getHealth(healthinfo);
    if (SL_IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        printf("SLAMTEC Lidar health status : %d\n", healthinfo.status);
        if (healthinfo.status == SL_LIDAR_STATUS_ERROR) {
            fprintf(stderr, "Error, slamtec lidar internal error detected. Please reboot the device to retry.\n");
            // enable the following code if you want slamtec lidar to be reboot by software
            // drv->reset();
            return false;
        } else {
            return true;
        }

    } else {
        fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
        return false;
    }
}


bool ctrl_c_pressed;
void ctrlc(int)
{
    ctrl_c_pressed = true;
}


int rplidarPi(int test)
{
    const char * opt_is_channel = NULL; 
    const char * opt_channel = NULL;
    const char * opt_channel_param_first = NULL;
    sl_u32         opt_channel_param_second = 115200;
    sl_result     op_result;
	int          opt_channel_type = CHANNEL_TYPE_SERIALPORT;

    bool useArgcBaudrate = false;
    int tran = 0;
    int  smallest = 0;
    ///  Create a communication channel instance
    IChannel* _channel;
    bool connectSuccess = false;
    
     sl_lidar_response_device_info_t devinfo;
    // create the driver instance
    ILidarDriver * drv = *createLidarDriver();

    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(-2);
    }
    
    opt_channel_param_first = "/dev/ttyUSB0";
     _channel = (*createSerialPortChannel(opt_channel_param_first, opt_channel_param_second));
            if (SL_IS_OK((drv)->connect(_channel))) {
                op_result = drv->getDeviceInfo(devinfo);

                if (SL_IS_OK(op_result)) 
                {
	                connectSuccess = true;
	                printf("Model: %d, Firmware Version: %d.%d, Hardware Version: %d\n",
            		devinfo.model,
            		devinfo.firmware_version >> 8, devinfo.firmware_version & 0xffu,
            		devinfo.hardware_version);
                }
                else{
                	fprintf(stderr, "Failed to get device information from LIDAR %08x\r\n", op_result);
                    delete drv;
			drv = NULL;
                }
                
            }
            else
            {
            	fprintf(stderr, "Failed to connect to LIDAR %08x\r\n", op_result);
            }
   
     // check health...
    if (!checkSLAMTECLIDARHealth(drv)) {
        goto on_finished;
    }


    signal(SIGINT, ctrlc);
    
	if(opt_channel_type == CHANNEL_TYPE_SERIALPORT)
        drv->setMotorSpeed();
    // start scan...
    drv->startScan(0,1);

    // fetech result and print it out...
    //Isabelle editing code for readability 
    float avgDist[36][2];
    
    
    
    for(int runtime = 1; runtime < 7; runtime++) {
        sl_lidar_response_measurement_node_hq_t nodes[8192];
        size_t   count = _countof(nodes);

        op_result = drv->grabScanDataHq(nodes, count);

        if (SL_IS_OK(op_result)) {
            drv->ascendScanData(nodes, count);
            for (int pos = 0; pos < (int)count ; pos = pos +30) {
            //my code
            if((nodes[pos].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT) > 30){
            	avgDist[pos/30][0] = (nodes[pos].angle_z_q14 * 90.f) / 16384.f;
            	avgDist[pos/30][1] = nodes[pos].dist_mm_q2/4.0f;
            }
            //end of my code
                /*printf("%s theta: %03.2f Dist: %08.2f Q: %d \n", 
                    (nodes[pos].flag & SL_LIDAR_RESP_HQ_FLAG_SYNCBIT) ?"S ":"  ", 
                    (nodes[pos].angle_z_q14 * 90.f) / 16384.f,
                    nodes[pos].dist_mm_q2/4.0f,
                    nodes[pos].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);
                    */
            }
        }

        if (ctrl_c_pressed){ 
            break;
        }
    }
    smallest = avgDist[0][1];
	for(int i = 0; i < 36; i++){
		printf("theta: %03.2f Dist: %08.2f \n", avgDist[i][0], avgDist[i][1]);
		if(avgDist[i][1] < smallest && avgDist[i][1] != 0){
			smallest = avgDist[i][1];
			tran = avgDist[i][0];
		}
	}
	tran = tran/45;
	printf("transciever : %i \n", tran);


    drv->stop();
	delay(200);
	if(opt_channel_type == CHANNEL_TYPE_SERIALPORT)
        drv->setMotorSpeed(0);

    /// Delete Lidar Driver and channel Instance
on_finished:
    if(drv) {
        delete drv;
        drv = NULL;
    }
    delete _channel;
    return tran;
}
