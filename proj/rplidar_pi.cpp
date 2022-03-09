#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "sl_lidar.h" 
#include "sl_lidar_driver.h"
//#include "libsl_lidar_sdk.a"

#include <unistd.h>
using namespace sl;

int read(int argc, char* argv)
{
    const char * opt_is_channel = NULL; 
    const char * opt_channel = NULL;
    const char * opt_channel_param_first = NULL;
    sl_u32         opt_channel_param_second = 115200;
    sl_result     op_result;


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
   
    // TODO
	printf("transeiver 4");
    /// Delete Lidar Driver and channel Instance
     delete drv;
    delete _channel;
}
