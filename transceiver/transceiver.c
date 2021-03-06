// C library headers
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "transceiver.h"
const int DATA_SIZE = 64;  // Only going to be sending chunks of 100 bytes but have buffer size set at 256 just in case
char rec_msg[8];        // Buffer for reading entire message packet to be returned to state machine
char prev_msg[8] = "00000000";
int n = 0;                 // counter used for populating rec_msg


// Flags for setting which robot you are using for testing
int JEFF = 1;
int SOURCE = 0;
int transceiver_select[4][2] =
        {
                {0,0},
                {0,1},
                {1,0},
                {1,1}
        };
// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// Wiring Pi Headers
#include <wiringPi.h>

// *****************************************************************************************************
// IMPORTANT NOTES:
// RST Pin must be held at HIGH
// It appears that if a wire is unplugged while the program is running, the transceiver gets tripped up
// by reseting the RST pin by unplugging it for 2 sec. seems to fix the problem
//
// The TX of the Pi goes to the RX of the Mux
// The TX of the Mux goes to the RX of the transceiver
// *****************************************************************************************************

/*
*********MUX BIT SELECTION********************
      INH  B/D  A/C
            0    0   TX0
            0    1   TX1
            1    0   TX2
            1    1   TX3
*/

int jeff_maintenance_routine_read(int transceiver, int port)
{

    int trans_num = transceiver;                   // grab which transceiver to use
    int serial_port = port;                        // grab FD for serial port
    int status = 0;                                // status variable for determining if data was sent correctly

    //clear serial port before start
    //tcflush(serial_port, TCIOFLUSH);

    // transceiver bit selection
    if (trans_num <= 3){
        int input_B = transceiver_select[(trans_num)][0];
        int input_A = transceiver_select[(trans_num)][1];

        digitalWrite(24, input_B); // B bit selection
        digitalWrite(25, input_A); // A bit selection
        digitalWrite(23, 0);       // Allow Mux to operate
        digitalWrite(27, 1);       // Inhibit other Mux
    }else{
        int input_D = transceiver_select[(trans_num - 4)][0];
        int input_C = transceiver_select[(trans_num - 4)][1];

        digitalWrite(28, input_D); // D bit selection
        digitalWrite(29, input_C); // C bit selection
        digitalWrite(27, 0);       // Allow Mux to operate
        digitalWrite(23, 1);       // Inhibit other Mux
    }


    int num_bytes = 0;

    char read_buf[DATA_SIZE];  // Read Buffer

    clock_t start = clock();
    int elapsed_time = 0;

    do{
        clock_t difference = clock() - start;
        elapsed_time = difference*1000/CLOCKS_PER_SEC;

        num_bytes = read(serial_port, &read_buf, 8);

        if (num_bytes < 0){
            printf("Error reading: %s", strerror(errno));
            return 1;
        }

        if (num_bytes > 0){
            while(num_bytes > 0){
                rec_msg[readCounter] = read_buf[0];
                readCounter++;
                //printf("SUCCESS\n");
                num_bytes = read(serial_port, &read_buf, 1);
                status = 1;
            }
        }
    }while(elapsed_time < 200);
    // timer runs for 0.1 s

    return status;
}

int jeff_maintenance_routine_send(int transceiver, char data[], int port)
{

    int trans_num = transceiver;         // grab which transceiver to use
    int serial_port = port;              // grab FD for serial port
    int status = 0;                      // status variable for determining if data was sent correctly

    //clear serial port before start
    tcflush(serial_port, TCIOFLUSH);

    // transceiver bit selection
    if (trans_num <= 3){
        int input_B = transceiver_select[(trans_num)][0];
        int input_A = transceiver_select[(trans_num)][1];

        digitalWrite(24, input_B); // B bit selection
        digitalWrite(25, input_A); // A bit selection
        digitalWrite(23, 0);       // Allow Mux to operate
        digitalWrite(27, 1);       // Inhibit other Mux
    }else{
        int input_D = transceiver_select[(trans_num - 4)][0];
        int input_C = transceiver_select[(trans_num - 4)][1];

        digitalWrite(28, input_D); // D bit selection
        digitalWrite(29, input_C); // C bit selection
        digitalWrite(27, 0);       // Allow Mux to operate
        digitalWrite(23, 1);       // Inhibit other Mux

    }

    int sent_bytes = write(serial_port, data, 8);      // send message

    // check for sending error
    if (sent_bytes < 0){
        printf("Error Sending\n");
    }else{
        status = 1;
    }

    return status;
}

int source_maintenance_routine_send(int transceiver, char data[], int port)
{

    int trans_num = transceiver;            // grab which transceiver to use
    int serial_port = port;                 // grab FD for serial port
    int status = 0;                         // status variable that is returned determining if data was sent correctly

    // transceiver bit selection
    if (trans_num <= 3){
        int input_B = transceiver_select[(trans_num)][0];
        int input_A = transceiver_select[(trans_num)][1];

        digitalWrite(24, input_B); // B bit selection
        digitalWrite(25, input_A); // A bit selection
        digitalWrite(23, 0);       // Allow Mux to operate
        digitalWrite(27, 1);       // Inhibit other Mux
    }else{
        int input_D = transceiver_select[(trans_num - 4)][0];
        int input_C = transceiver_select[(trans_num - 4)][1];

        digitalWrite(28, input_D); //D bit selection
        digitalWrite(29, input_C); //c bit selection
        digitalWrite(27, 0);       //Allow Mux to operate
        digitalWrite(23, 1);       //Inhibit other Mux
    }

    // Write to serial port
    int sent_bytes = write(serial_port, data, DATA_SIZE);               // send message

    if (sent_bytes < 0){                                        // check for sending error
        printf("Error Sending\n");
    }else{
        status = 1;
    }

    return status;
}

int source_maintenance_routine_read(int transceiver, int port)
{

    int trans_num = transceiver;                   // grab which transceiver to use
    int serial_port = port;                        // grab FD for serial port
    int status = 0;                                // status variable for determining if data was sent correctly

    //clear serial port before start of reading
    tcflush(serial_port, TCIOFLUSH);

    // transceiver bit selection
    if (trans_num <= 3){
        int input_B = transceiver_select[(trans_num)][0];
        int input_A = transceiver_select[(trans_num)][1];

        digitalWrite(24, input_B); // B bit selection
        digitalWrite(25, input_A); // A bit selection
        digitalWrite(23, 0);       // Allow Mux to operate
        digitalWrite(27, 1);       // Inhibit other Mux
    }else{
        int input_D = transceiver_select[(trans_num - 4)][0];
        int input_C = transceiver_select[(trans_num - 4)][1];

        digitalWrite(28, input_D); //D bit selection
        digitalWrite(29, input_C); //c bit selection
        digitalWrite(27, 0);       //Allow Mux to operate
        digitalWrite(23, 1);       //Inhibit other Mux
    }

    int num_bytes = 0;              // used to track number of bytes being read
    char read_buf[DATA_SIZE];       // Read Buffer

    //  timer used to read for set amount of time before returning
    clock_t start = clock();
    int elapsed_time = 0;

    do{
        clock_t difference = clock() - start;
        elapsed_time = difference*1000/CLOCKS_PER_SEC;

        if((num_bytes = read(serial_port, &read_buf, DATA_SIZE)) == 0){
            //printf("NOTHING\n");
        }

        // check is serial port read returned an error
        if (num_bytes < 0){
            printf("Error reading: %s", strerror(errno));
            return 1;
        }

        // if there was data read
        if (num_bytes > 0){
            //printf("Message Recieved: %s\n", read_buf);
            rec_msg[readCounter] = read_buf[0];                       // store the data in the rec_msg buffer
            readCounter++;                                            // increment counter to next buffer position
            status = 1;                                     // data has been read successfully
        }
    }while(elapsed_time < 200);
    // timer runs for 0.1 sec

    // clear the read_buf used in this function and clear the serial port
    memset(read_buf, 0, DATA_SIZE);
    usleep(1);
    tcflush(serial_port, TCIOFLUSH);

    // return status of read function call
    return status;
}
/*
int main() {

    // WiringPi set up of all used pins for multiplexer communication
    wiringPiSetup();
    pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
    pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33
    pinMode(25, OUTPUT);  // Pin A / GPIO Pin #26 of Pi / Physical Pin 37
    pinMode(24, OUTPUT);  // Pin B / GPIO Pin #19 of Pi / Physical Pin 35
    pinMode(29, OUTPUT);  // Pin C / GPIO Pin #21 of Pi / Physical Pin 40
    pinMode(28, OUTPUT);  // Pin D / GPIO Pin #20 of Pi / Physical Pin 38

    int source_trans = 0;
    int jeff_trans = 4;

    // open the serial port
    int serial_port = open("/dev/ttyS0", O_RDWR| O_NOCTTY);
    int val = fcntl(serial_port, F_GETFL, 0);

    // struct for setting up serial port
    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_port, &tty) != 0) {
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
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

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

    int status_read, status_send;
    // ******** JEFF TESTING METHODS *********
    if (JEFF == 1){
        printf("RUNNING JEFF CODE\n");
        printf("\n");

        // set the file name of the text file that will be read and sent
        char *filename = "rec_data.txt";

        // struct for determining the size of a file
        struct stat st;

        //set file pointer and open file for writing
        FILE *fp;
        fp = fopen(filename, "w");

        // if file can not be read, end
        if (fp == NULL){
            printf("Error: Could not open file\n");
            return 1;
        }

        int end_file = 0;   // status used to check if the end of file marker has been reached
        int checksum = 0;   // checksum variable

        // accept data packets until the end of file marker is reached
        while(end_file == 0){

            // try to read a message from SOURCE
            status_read = jeff_maintenance_routine_read(jeff_trans,serial_port);

            readCounter = 0;
            if (status_read == 0){
                //printf("COMMUNICATION TIMEOUT\n");
            }else if(status_read == 1){
                //printf("COMMUNICATION SUCCESS\n");

                //printf("MESSAGE: %s\n",rec_msg);

                // checksum calculation
                int checksum = 0;
                for(int k = 0; k <= 6; k++){
                    checksum += (int) rec_msg[k];
                }
                checksum = (((checksum % 100) / 10) + ((checksum % 100) % 10)) % 10;

                // if checksum value rec == checksum value calculated
                if(checksum + '0' == rec_msg[7]){

                    // set checksum value in rec_msg buffer to null
                    rec_msg[7] = '\0';

                    int result;

                    if ((result = strcmp(rec_msg,prev_msg)) != 0){
                        //printf("PREV MSG: %s\n",prev_msg);
                        //printf("REC MSG: %s\n",rec_msg);
                        //printf("RESULT: %i\n",result);
                        for(int k = 0; k <= 6; k++){
                            prev_msg[k] = rec_msg[k];
                        }
                        // store rest of message in the file
                        fputs(rec_msg, fp);
                    }

                    // check to see if the end of the file has been reached
                    int c = 0;
                    while(c <= 6){
                        if (rec_msg[c] == '^'){
                            //printf("END OF FILE\n");
                            end_file = 1;
                        }
                        c++;
                    }

                    // reset the rec_msg buffer in order to send response
                    memset(rec_msg,'\0',8);

                    // if the checksums match, send a 0
                    rec_msg[0] = '0';
                }else{
                    // else send a 1
                    rec_msg[0] = '1';
                }
            }

            // send a response to SOURCE with if the data was rec correctly
            status_send = jeff_maintenance_routine_send(jeff_trans,rec_msg,serial_port);
            if (status_send == 0){
                //printf("ERROR SENDING\n");
            }else if(status_send == 1){
                //printf("SEND SUCCESSFUL\n");
            }

            // reset rec_msg buffer for reading
            memset(rec_msg,0,8);
        }

        // close file
        close(fp);
    }

    // ******** SOURCE TESTING METHODS *********
    if (SOURCE == 1){
        printf("RUNNING SOURCE CODE\n");
        printf("\n");

        // set the file name of the text file that will be read and sent
        char *filename = "file_to_be_sent.txt";

        // struct for determining the size of a file
        struct stat st;

        //set file pointer and open file for reading
        FILE *fp;
        fp = fopen(filename, "r");

        if (fp == NULL){
            printf("Error: Could not open file\n");
            return 1;
        }

        // get size of file being sent
        long int size;
        if (stat(filename, &st) == 0){
            size = st.st_size;
            printf("Size of File: %i\n",size);
        }

        int num_packet;             // variable for # of packets being sent
        char msg[DATA_SIZE];        // buffer for sending data

        // determine number of packets needed to be sent with 7 data bytes per packet
        if (size % 7 == 0){
            num_packet = size / 7 - 1;
        }else{
            num_packet = size / 7;
        }
        printf("NUM OF PACKETS: %i\n",num_packet);

        // counters used for sending packets
        int i = 0;
        int j = 0;

        while(writeCounter < num_packet){
            if (writeCounter % 500 == 0){
                printf("PACKETS SENT: %i\n",writeCounter);
            }
            for (i = (writeCounter*7); i <= ((writeCounter * 7) + 6); i++){
                msg[j] = fgetc(fp);
                j++;
            }

            j = 0;

            // checksum value calculation
            int checksum = 0;
            for(int k = 0; k <= 6; k++){
                checksum += (int) msg[k];
            }
            checksum = (((checksum % 100) / 10) + ((checksum % 100) % 10)) % 10;

            // store checksum value in msg buffer
            msg[7] = checksum + '0';

            // used for testing sending messages
            scanf("%s",&msg);

            // send msg to JEFF
            status_send = source_maintenance_routine_send(source_trans,msg,serial_port);

            // check if msg was sent correctly
            if (status_send == 0){
                //printf("ERROR SENDING\n");
            }else if(status_send == 1){
                //printf("SEND SUCCESSFUL\n");
            }

            // wait for JEFF response
            status_read = source_maintenance_routine_read(source_trans,serial_port);
            readCounter = 0;
            if (status_read == 0){
                printf("COMMUNICATION TIMEOUT\n");
                fseek(fp,-7,SEEK_CUR);
            }else if(rec_msg[0] == '0'){
                writeCounter++;
                printf("COMMUNICATION SUCCESS\n");
                //printf("ENTIRE MESSAGE: %s\n",rec_msg);
                //int read_bytes = strlen(rec_msg);
                //printf("READ BYTES: %i\n",read_bytes);
            }else if(rec_msg[0] == '1'){
                printf("Error: Bad Data\n");
                fseek(fp,-7,SEEK_CUR);
            }
        }

        // close file
        fclose(fp);
    }
    //printf("%s\n",rec_msg);
    printf("\n*** CLOSING COMMUNICATION CHANNEL ***\n");

    // close the serial ports
    close(serial_port);

    return 0; // success
}
*/
