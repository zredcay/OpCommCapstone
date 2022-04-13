#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#include <semaphore.h>

#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// Wiring Pi Headers
#include <wiringPi.h>

#include "stateMachine.h"
#include "rplidarPi.h"
#include "sharedMemory.h"
#include "transceiver.h"



int convertTransceiver(int trans)
{
	int newtrans;

	if( trans == 0)
	{
		newtrans = 1;
	}
	else if (trans == 1)
	{
		newtrans = 0;
	}
	else if (trans == 2)
	{
		newtrans = 7;
	}
	else if (trans == 3)
	{
		newtrans = 6;
	}
	else if (trans == 4)
	{
		newtrans = 5;
	}
	else if (trans == 5)
	{
		newtrans = 4;
	}
	else if (trans == 6)
	{
		newtrans = 3;
	}
	else if (trans == 7)
	{
		newtrans = 2;
	}


	return newtrans;
}
struct mainData trans_select(float Ax, float Ay, float Az, float Mx, float My, float Mz, float timer, struct mainData ex)
{ // uses imu and discovery data to calculate the new transicever

	struct mainData data = ex;
	printf(" angle %f distance %f transciever %d Velocity %f %f\n", data.angle, data.dist, data.trans, data.Vx, data.Vy);
	float PI  = 3.14159265;
	 float val = PI / 180.0;

	float Vfx = data.Vx + (Ax * timer);
	float Vfy = data.Vy + (Ay * timer);
	float Xo = data.dist*cos(data.angle*val);
	float Yo = data.dist*sin(data.angle*val);
	float Xf = Xo + .5*(Vfx + data.Vx)*timer;
	float Yf = Yo + .5*(Vfy + data.Vy)*timer;

	printf("Vfx %f Vfy %f Xo %f Yo %f Xf %f Yf %f \n", Vfx, Vfy, Xo, Yo, Xf, Yf);


	data.angle = (atan(Yf/Xf))/val;
	if(Xf < 0 )
	{
		data.angle = data.angle + 180;
	}
	if(Yf < 0 && Xf > 0)
	{
		data.angle = data.angle + 360;
	}
	data.dist = sqrt((Xf * Xf) + (Yf * Yf));

	data.trans = convertTransceiver(data.angle/45);

	data.Vx = Vfx;
	data.Vy = Vfy;



	printf(" angle %f distance %f transciever %d Velocity %f %f\n",  data.angle, data.dist, data.trans, data.Vx, data.Vy);


return data;
}

int openFile(int flag)
{
    if (flag == 1) {

        // set the file name of the text file that will be read and sent
        char *filename = "rec_data.txt";

        // struct for determining the size of a file
        struct stat st;

        //set file pointer and open file for writing
        fp = fopen(filename, "w");

        // if file can not be read, end
        if (fp == NULL) {
            printf("Error: Could not open file\n");
            return 1;
        }
    }
    if (flag == 0) {

        // set the file name of the text file that will be read and sent
        printf("open serial port\n");
        char *filename = "file_to_be_sent.txt";

        // struct for determining the size of a file
        printf("open serial port\n");
        struct stat st;

        num_packet = 0;
        writeCounter = 0; // write counter keeps track of how many packets are sent

        printf("open file\n");
        //set file pointer and open file for reading
        fp = fopen(filename, "r");

        if (fp < 0) {
            printf("Error: Could not open file\n");
            return 1;
        }

        // get size of file being sent
        long int size;
        if (stat(filename, &st) == 0) {
            size = st.st_size;
            printf("Size of File: %i\n", size);
        }

        // determine number of packets needed to be sent with 7 data bytes per packet
        if (size % 7 == 0) {
            num_packet = size / 7 - 1;
        } else {
            num_packet = size / 7;
        }
        printf("NUM OF PACKETS: %i\n", num_packet);
    }
}


int main () {
    int flag = 0;
    char prev_msg[8] = "00000000";
    float Ax = .2;
    float Ay = .1;
    float Az;
    float Gx;
    float Gy;
    float Gz;
    float Mx;
    float My;
    float Mz;
    float angle = 100;
    float dist = .3;
    int transceiver = 0;
    float mainTimer = .5;
    float Vx = 0;
    float Vy = 0;

    struct lidarData lidar;
    struct shared sharMem;
    struct Memory arr;
    struct mainData data;
    int serial_port;
    sem_t *mutex;

    data.angle = 120;
    data.dist = .2;
    data.trans = 3;
    data.Vx = -.2;
    data.Vy = -.1;

/* // testing whole bluetooth thing
    sharMem = createMemory(); // creates shared memory
    mutex = createNamedSem(); // creates named semaphore
    sleep(5);
    arr = sharedMemory(flag, sharMem, mutex); //recieves the float value from imu
    closeNamedSem(mutex);
    closeMemeory(sharMem);

    lidar = rplidarPi();// transceiver is the tranciever number
    transceiver = lidar.trans;
    angle = lidar.angle;
    dist = lidar.dist;
    printf("return value transceievr %i and angle %f and distance %f\n", transceiver, angle, dist);

	for(int i = 0; i < 4; i++){
	 data = transciever_select(Ax, Ay, Az, Mx, My, Mz, mainTimer, data);
	 }

    exit(-1);
    */
    State NextState = Intialization;
    printf("setting event\n");
    Event NewEvent = Code_Finished_Event;
    clock_t startTime = clock();
    while (clock() < (startTime + 3 * CLOCKS_PER_SEC)) {
        switch (NextState) {
            case Intialization: {

                // WiringPi set up of all used pins for multiplexer communication
                wiringPiSetup();
                pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
                pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33
                pinMode(25, OUTPUT);  // Pin A / GPIO Pin #26 of Pi / Physical Pin 37
                pinMode(24, OUTPUT);  // Pin B / GPIO Pin #19 of Pi / Physical Pin 35
                pinMode(29, OUTPUT);  // Pin C / GPIO Pin #21 of Pi / Physical Pin 40
                pinMode(28, OUTPUT);  // Pin D / GPIO Pin #20 of Pi / Physical Pin 38
                printf("done with wiringPi\n");
                //*****************transcoever code start************************88
                serial_port = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
                int val = fcntl(serial_port, F_GETFL, 0);

                // struct for setting up serial port
                struct termios tty;


                // Read in existing settings, and handle any error
                if (tcgetattr(serial_port, &tty) != 0) {
                    printf("Error %i from tcgetattr tx: %s\n", errno, strerror(errno));
                    return 1;
                }

                tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
                tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
                tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
                tty.c_cflag |= CS8; // 8 bits per byte (most common)
                tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
                tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

                tty.c_lflag &= ~ICANON;
                tty.c_lflag &= ~ECHO; // Disable echo
                tty.c_lflag &= ~ECHOE; // Disable erasure
                tty.c_lflag &= ~ECHONL; // Disable new-line echo
                tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
                tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
                tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

                tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
                tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
                // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
                // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

                tty.c_cc[VTIME] = 0;    // Wait for up to 10s (100 deciseconds), returning as soon as any data is received.
                tty.c_cc[VMIN] = 0;

                // Set in/out baud rate to be 9600
                cfsetispeed(&tty, B115200);
                cfsetospeed(&tty, B115200);

                // Save tty settings, also checking for error
                if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
                    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
                    return 1;
                }
                printf("open serial port\n");
                openFile(flag);
                printf("done initlizing\n");
                //*******************tranceover code done
                //sharMem = createMemory(); // creates shared memory
                //mutex = createNamedSem(); // creates named semaphore

                //*************** time for connection ****************88
               /* clock_t init_bluetooth = clock();
                int elapsed_time = 0;

                do {
                    clock_t difference = clock() - init_bluetooth;
                    elapsed_time = difference * 1000 / CLOCKS_PER_SEC;
                } while (elapsed_time < 30000);
                */

                NewEvent = Code_Finished_Event;
                NextState = CodeFinishedHandler(NextState);
            }
                break;

            case End: {
                // Closes named semaphore and shared memory before exiting code
                fclose(fp);
                close(serial_port);
                closeNamedSem(mutex);
                closeMemeory(sharMem);
                //printf("case end state\n");
                if (User_Exit_Event == NewEvent) // code completes safely and exits S
                {
                    printf("Code Finished\n");
                } else if (Should_Not_Get_Here_Event == NewEvent) // uncaught error or bug in code
                {
                    printf("big error stop code now\n");
                    exit(-1);
                } else // unexpected state and event matching
                {
                    printf("event not updated or waiting on user\n");
                    exit(-1);
                }

            }
                break;

            case Discovery: {

                if (Code_Finished_Event == NewEvent) {
                    //lidar = rplidarPi();// transceiver is the tranciever number
                    transceiver = 0; // lidar.trans;
                    angle = 100; //lidar.angle;
                    dist = 100; //lidar.dist;
                    printf("return value transceievr %i and angle %f and distance %f\n", transceiver, angle, dist);
                    if (angle == 0) // the transceiver was not found
                    {
                        NewEvent = Bad_Data_Event;
                        NextState = BadDataHandler(NextState);
                    } else if (angle < 360 && angle > 0) // in a given transcieer 1 - 8
                    {
                        NewEvent = Code_Finished_Event;
                        NextState = CodeFinishedHandler(NextState);
                    } else // angle should not give a number higher than 360
                    {
                        NewEvent = Should_Not_Get_Here_Event;
                        NextState = ShouldNotGetHandler(NextState);
                    }
                    startTime = clock();

                }


            }
                break;

            case Alternate_Discovery: {
                printf("in alternatre discovery\n");

                if (Bad_Data_Event == NewEvent) {
                    if (true) {
                        NewEvent = Code_Finished_Event;
                        NextState = CodeFinishedHandler(NextState);

                    } else {
                        NewEvent = Bad_Data_Event;
                        NextState = BadDataHandler(NextState);
                    }
                    startTime = clock();
                } else {
                    NewEvent = Should_Not_Get_Here_Event;
                    NextState = ShouldNotGetHandler(NextState);
                }

            }
                break;

            case Maintenance: {
                int status;
                //Jeff
                if (flag == 1) {
                    printf("RUNNING JEFF CODE\n");
                    printf("\n");

                    int end_file = 0;   // status used to check if the end of file marker has been reached
                    int checksum = 0;   // checksum variable

                    // accept data packets until the end of file marker is reached
                    while (end_file == 0) { // ******turn into timer*****

                        // try to read a message from SOURCE
                        status = jeff_maintenance_routine_read(transceiver, serial_port);

                        readCounter = 0;
                        if (status == 0) {
                            //printf("COMMUNICATION TIMEOUT\n");
                        } else if (status == 1) {
                            //printf("COMMUNICATION SUCCESS\n");

                            printf("MESSAGE: %s\n", rec_msg);

                            // checksum calculation
                            int checksum = 0;
                            for (int k = 0; k <= 6; k++) {
                                checksum += (int) rec_msg[k];
                            }
                            checksum = (((checksum % 100) / 10) + ((checksum % 100) % 10)) % 10;

                            // if checksum value rec == checksum value calculated
                            if (checksum + '0' == rec_msg[7]) {

                                // set checksum value in rec_msg buffer to null
                                rec_msg[7] = '\0';

                                int result;

                                if ((result = strcmp(rec_msg, prev_msg)) != 0){
                                    for (int k = 0; k <= 6; k++){
                                        prev_msg[k] = rec_msg[k];
                                    }
                                    fputs(rec_msg, fp);
                                }

                                // check to see if the end of the file has been reached
                                int c = 0;
                                while (c <= 6) {
                                    if (rec_msg[c] == '^') {
                                        //printf("END OF FILE\n");
                                        end_file = 1;
                                        status = 3;
                                    }
                                    c++;
                                }

                                // reset the rec_msg buffer in order to send response
                                memset(rec_msg, '\0', 8);

                                // if the checksums match, send a 0
                                rec_msg[0] = '0';
                            } else {
                                // else send a 1
                                rec_msg[0] = '1';
                                status = 2;
                            }
                        }

                        // send a response to SOURCE with if the data was rec correctly
                        status = jeff_maintenance_routine_send(transceiver, rec_msg, serial_port);
                        if (status == 0) {
                            //printf("ERROR SENDING\n");
                        } else if (status == 1) {
                            //printf("SEND SUCCESSFUL\n");
                        }

                        // reset rec_msg buffer for reading
                        memset(rec_msg, 0, 8);
                    }

                    // close file
                    close(fp);
                }
                //Source
                if (flag == 0) {
                    printf("RUNNING SOURCE CODE\n");
                    printf("\n");

                    char msg[8];        // buffer for sending data

                    // counters used for sending packets

                    int i = 0;
                    int j = 0;

                    while (writeCounter <= num_packet) {
                    /*
                        if (writeCounter % 500 == 0) {
                            printf("PACKETS SENT: %i\n", writeCounter);
                        }
                    */

                        for (i = (writeCounter * 7); i <= ((writeCounter * 7) + 6); i++) {
                            msg[j] = fgetc(fp);
                            j++;
                        }

                        j = 0;

                        // checksum value calculation
                        int checksum = 0;
                        for (int k = 0; k <= 6; k++) {
                            checksum += (int) msg[k];
                        }
                        checksum = (((checksum % 100) / 10) + ((checksum % 100) % 10)) % 10;

                        // store checksum value in msg buffer
                        msg[7] = checksum + '0';

                        // used for testing sending messages
                        //scanf("%s", &msg);

                        // send msg to JEFF
                        status = source_maintenance_routine_send(transceiver, msg, serial_port);

                        // check if msg was sent correctly
                        if (status == 4) {
                            //printf("ERROR SENDING\n");
                        } else if (status == 1) {
                            //printf("SEND SUCCESSFUL\n");
                        }

                        // wait for JEFF response
                        status = source_maintenance_routine_read(transceiver, serial_port);
                        readCounter = 0;
                        if (status == 0) {
                            printf("COMMUNICATION TIMEOUT\n");
                            fseek(fp, -7, SEEK_CUR);
                        } else if (rec_msg[0] == '0') {
                            writeCounter++;
                            printf("COMMUNICATION SUCCESS\n");
                            printf("ENTIRE MESSAGE: %s\n",rec_msg);
                            //int read_bytes = strlen(rec_msg);
                            //printf("READ BYTES: %i\n",read_bytes);
                            status = 1;
                        } else if (rec_msg[0] == '1') {
                            printf("Error: Bad Data\n");
                            fseek(fp, -7, SEEK_CUR);
                            status = 2;
                        }else{
                            printf("ENTIRE MESSAGE: %s\n",rec_msg);
                            printf("STATUS %i\n",status);
                            fseek(fp, -7, SEEK_CUR);
                        }
                        printf("Write Counter: %i\n",writeCounter);
                        printf("FILE POINTER %i\n",ftell(fp));
                    }
                }

                if (Code_Finished_Event == Code_Finished_Event) {

                    printf("status = %i\n", status);
                    if (status == 0) // status is timeout got to recovery
                    {
                        NewEvent = Timeout_Event;
                        NextState = TimeoutEventHandler(NextState);
                    }
                    else if (status == 1) // status is good data ove on
                    {
                        NewEvent = Code_Finished_Event;
                        NextState = CodeFinishedHandler(NextState);
                    }
                    else if (status == 2) //status is bad data
                    {
                        NewEvent = Bad_Data_Event;
                        NextState = BadDataHandler(NextState);
                    }
                    else if (status == 3) //status is End of File
                    {
                        NewEvent = User_Exit_Event;
                        NextState = UserExitHandler(NextState);
                    }
                    else if (status == 4) //Status is something broken
                    {
                        NewEvent = Should_Not_Get_Here_Event;
                        NextState = ShouldNotGetHandler(NextState);
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


                case Recovery: {
                    //try tans +1 and trans -1 in new recovery code
                    if (Bad_Data_Event == NewEvent) {
                        if (true) {
                            NewEvent = Code_Finished_Event;
                            NextState = CodeFinishedHandler(NextState);

                        } else {
                            NewEvent = Bad_Data_Event;
                            NextState = BadDataHandler(NextState);
                        }
                        startTime = clock();
                    }
                    if (Loss_of_LOS_Event == NewEvent) {

                        if (true) {
                            NewEvent = Code_Finished_Event;
                            NextState = CodeFinishedHandler(NextState);

                        } else {
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
