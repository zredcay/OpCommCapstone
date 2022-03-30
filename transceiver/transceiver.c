// C library headers
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "transceiver.h"
const int DATA_SIZE = 100;  // Only going to be sending chunks of 100 bytes but have buffer size set at 256 just in case
int COUNT = 0;
char message[100];
int n = 0;

int JEFF = 1;
int SOURCE = 0;


// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// Wiring Pi Headers


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
            0    0   TX1
            0    1   TX2
            1    0   TX3
            1    1   TX4
*/

struct thread_args{
    int serial_port;
    int trans_num;
};

int transceiver_select[4][2] =
{
    {0,0},
    {0,1},
    {1,0},
    {1,1}
};

// Send thread function, takes argument of the serial port file descriptor(FD)
void *tx_function(void *vargp)
{
    //wiringPiSetup();      // set up wiring the pins for transceiver selection
    //pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
    //pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33

    // grab passed in arguments
    struct thread_args* arguments = (struct args*) vargp;

    int *serial_port = arguments->serial_port;              // grab FD for serial port
    int trans_num = arguments->trans_num;                   // grab which transceiver to use
    trans_num = trans_num - 1;                              // Passed in transceiver is from 1-8 but for calculation we use 0-7, subtrtact 1 from whatever is passed in
    printf("Trans Num: %i\n",trans_num);

    //clear serial port before start
    tcflush(serial_port, TCIOFLUSH);

    /*
    // transceiver bit selection
    if (trans_num <= 3){
        int input_B = transceiver_select[(trans_num % 7)][0];
        int input_A = transceiver_select[(trans_num % 7)][1];

        pinMode(25, OUTPUT);  // Pin B / GPIO Pin #26 of Pi / Physical Pin 37
        pinMode(24, OUTPUT);  // Pin A / GPIO Pin #19 of Pi / Physical Pin 35

        digitalWrite(25, 0); // B bit selection
        digitalWrite(24, 0); // A bit selection
        digitalWrite(23, 0);       // Allow Mux to operate
        digitalWrite(27, 1);       // Inhibit other Mux
    }else{
        int input_D = transceiver_select[(trans_num - 4)][0];
        int input_C = transceiver_select[(trans_num - 4)][1];

        pinMode(28, OUTPUT);  // Pin D / GPIO Pin #20 of Pi / Physical Pin 38
        pinMode(27, OUTPUT);  // Pin C / GPIO Pin #16 of Pi / Physical Pin 36

        digitalWrite(28, input_D); //D bit selection
        digitalWrite(27, input_C); //c bit selection
        digitalWrite(27, 0);       //Allow Mux to operate
        digitalWrite(23, 1);       //Inhibit other Mux
    }
    */

    char msg[DATA_SIZE];                                            // send message buffer

    clock_t start;
    int elapsed_time = 0;
    int sent_bytes;


    // Write to serial port
    while(1){

        //memset(msg, 0, 100);
        //memset(msg, '$', 100);
        //int sent_bytes_check = scanf("%s",&msg);

        scanf("%s",&msg);
        sent_bytes = write(serial_port, msg, strlen(msg));      // send message
        if (sent_bytes < 0){                                        // check for sending error
            printf("Error Sending\n");
            return 0;
        }


        elapsed_time = 0;

        /*
        do{
            clock_t difference = clock() - start;
            elapsed_time = difference*1000/CLOCKS_PER_SEC;
            if (elapsed_time % 10000 == 0){
                sent_bytes = write(serial_port, msg, strlen(msg));
void *rx_function(void *vargp)
{
            }
        }while( elapsed_time < 30000);
        */

        int result = strcmp("END", msg);                            // send 'END' to close channel
        if (result  == 0){
            break;
        }
        memset(msg, 0, DATA_SIZE);
    }
}

// Recieve thread function, takes argument of the serial port file descriptor(FD)
void *rx_function(void *vargp)
{
    //wiringPiSetup();      // set up wiring the pins for transceiver selection

    // grab passed in arguments
    struct thread_args* arguments = (struct args*) vargp;

    int *serial_port = arguments->serial_port;              // grab FD for serial port
    int trans_num = arguments->trans_num;                   // grab which transceiver to use
    trans_num = trans_num - 1;

    // transceiver bit selection
    int input_B = transceiver_select[(trans_num % 7)][0];
    int input_A = transceiver_select[(trans_num % 7)][1];

    /*
    pinMode(25,OUTPUT);  // Pin B0
    pinMode(24, OUTPUT); //Pin A0

    digitalWrite(25,0); //B selection
    digitalWrite(24,0); //A selection
    */

    // Track how many bytes are sent
    int num_bytes = 0;
    int n = 0;

    char read_buf [100];  // Read Buffer

    // Read from serial port
    while(1){

        printf("Trying to read\n");

        num_bytes = read(serial_port, &read_buf, DATA_SIZE);

        if (num_bytes < 0){
            printf("Error reading: %s", strerror(errno));
            return 1;
        }

        if (num_bytes > 0){
            printf("\n");
            printf("Read %i bytes\n",num_bytes);
            COUNT = COUNT + num_bytes;
            printf("Message Recieved: %s\n", read_buf);
            printf("Total of %i bytes sent\n",COUNT);
        }

        int result = strcmp("END", read_buf);
        if (result  == 0){
            printf("\n*** END CHANNEL MESSAGE RECIEVED ***\n");
            break;
        }

        // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
        if (n < 0) {
            printf("Error reading: %s", strerror(errno));
            return 1;
        }

        /*
        clock_t start;
        int elapsed_time = 0;

        do{
            clock_t difference = clock() - start;
            elapsed_time = difference*1000/CLOCKS_PER_SEC;
            printf("Timer test\n");
        }while( elapsed_time < 3000);
        */

        // Print out the recieved message
        if (n>0){
            printf("\n");
            printf("RECEIVED: %s \n",read_buf);
            //printf("SIZE: %i\n",n);
        }
        memset(read_buf, 0, DATA_SIZE);

        //usleep(1);
        tcflush(serial_port, TCIOFLUSH);

    }
}

int jeff_maintenance_routine_read(int transceiver, int port)
{
    /*
    wiringPiSetup();      // set up wiring the pins for transceiver selection
    pinMode(22, OUTPUT);  // RST Pin / GPIO Pin #6 of Pi / Physical Pin 31
    pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
    pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33
    */

    int trans_num = transceiver;                   // grab which transceiver to use
    int serial_port = port;                        // grab FD for serial port
    trans_num = trans_num - 1;                     // Passed in transceiver is from 1-8 but for calculation we use 0-7, subtrtact 1 from whatever is passed in
    //printf("Trans Num: %i\n",trans_num);
    int status = 0;

    //clear serial port before start
    //tcflush(serial_port, TCIOFLUSH);

    /*
    // transceiver bit selection
    if (trans_num <= 3){
        int input_B = transceiver_select[(trans_num % 7)][0];
        int input_A = transceiver_select[(trans_num % 7)][1];

        pinMode(25, OUTPUT);  // Pin B / GPIO Pin #26 of Pi / Physical Pin 37
        pinMode(24, OUTPUT);  // Pin A / GPIO Pin #19 of Pi / Physical Pin 35

        digitalWrite(25, input_B); // B bit selection
        digitalWrite(24, input_A); // A bit selection
        digitalWrite(23, 0);       // Allow Mux to operate
        digitalWrite(27, 1);       // Inhibit other Mux
    }else{
        int input_D = transceiver_select[(trans_num - 4)][0];
        int input_C = transceiver_select[(trans_num - 4)][1];

        pinMode(28, OUTPUT);  // Pin D / GPIO Pin #20 of Pi / Physical Pin 38
        pinMode(27, OUTPUT);  // Pin C / GPIO Pin #16 of Pi / Physical Pin 36

        digitalWrite(28, input_D); //D bit selection
        digitalWrite(27, input_C); //c bit selection
        digitalWrite(27, 0);       //Allow Mux to operate
        digitalWrite(23, 1);       //Inhibit other Mux
    }
    */

    int num_bytes = 0;

    char read_buf[DATA_SIZE];  // Read Buffer

    clock_t start = clock();
    int elapsed_time = 0;

    do{
        clock_t difference = clock() - start;
        elapsed_time = difference*1000/CLOCKS_PER_SEC;
        //printf("Elasped time: %i\n",elapsed_time);

        if((num_bytes = read(serial_port, &read_buf, DATA_SIZE)) == 0){
            //printf("NOTHING\n");
        }

        if (num_bytes < 0){
            printf("Error reading: %s", strerror(errno));
            return 1;
        }

        if (num_bytes > 0){
            //printf("\n");
            //printf("Read %i bytes\n",num_bytes);
            COUNT = COUNT + num_bytes;
            //printf("Message Recieved: %s\n", read_buf);
            printf("%s\n",read_buf);
            message[n] = read_buf[0];
            n++;
            //printf("Total of %i bytes sent\n",COUNT);
            //printf("SUCCESS\n");
            status = 1;
            //break;
        }
    }while(elapsed_time < 500);

    memset(read_buf, 0, DATA_SIZE);
    //usleep(1);
    //tcflush(serial_port, TCIOFLUSH);

    return status;
}

int jeff_maintenance_routine_send(int transceiver, char **data, int port)
{
    /*
    wiringPiSetup();      // set up wiring the pins for transceiver selection
    pinMode(22, OUTPUT);  // RST Pin / GPIO Pin #6 of Pi / Physical Pin 31
    pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
    pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33
    */

    int trans_num = transceiver;         // grab which transceiver to use
    char **msg = data;                   // send message buffer
    int serial_port = port;              // grab FD for serial port
    trans_num = trans_num - 1;           // Passed in transceiver is from 1-8 but for calculation we use 0-7, subtrtact 1 from whatever is passed in
    int status = 0;

    //clear serial port before start
    tcflush(serial_port, TCIOFLUSH);

    /*
    // transceiver bit selection
    if (trans_num <= 3){
        int input_B = transceiver_select[(trans_num % 7)][0];
        int input_A = transceiver_select[(trans_num % 7)][1];

        pinMode(25, OUTPUT);  // Pin B / GPIO Pin #26 of Pi / Physical Pin 37
        pinMode(24, OUTPUT);  // Pin A / GPIO Pin #19 of Pi / Physical Pin 35

        digitalWrite(25, input_B); // B bit selection
        digitalWrite(24, input_A); // A bit selection
        digitalWrite(23, 0);       // Allow Mux to operate
        digitalWrite(27, 1);       // Inhibit other Mux
    }else{
        int input_D = transceiver_select[(trans_num - 4)][0];
        int input_C = transceiver_select[(trans_num - 4)][1];

        pinMode(28, OUTPUT);  // Pin D / GPIO Pin #20 of Pi / Physical Pin 38
        pinMode(27, OUTPUT);  // Pin C / GPIO Pin #16 of Pi / Physical Pin 36

        digitalWrite(28, input_D); //D bit selection
        digitalWrite(27, input_C); //c bit selection
        digitalWrite(27, 0);       //Allow Mux to operate
        digitalWrite(23, 1);       //Inhibit other Mux
    }
    */

    int sent_bytes = write(serial_port, msg, strlen(msg));      // send message
    if (sent_bytes < 0){                                        // check for sending error
        printf("Error Sending\n");
    }else{
        status = 1;
    }

    return status;
}

int source_maintenance_routine_send(int transceiver, char *data, int port)
{
    /*
    wiringPiSetup();      // set up wiring the pins for transceiver selection
    pinMode(22, OUTPUT);  // RST Pin / GPIO Pin #6 of Pi / Physical Pin 31
    pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
    pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33
    */

    int trans_num = transceiver;            // grab which transceiver to use
    //char msg[DATA_SIZE];                      // send message buffer
    printf("SENDING OUT: %s\n",data);
    int serial_port = port;                 // grab FD for serial port
    trans_num = trans_num - 1;              // Passed in transceiver is from 1-8 but for calculation we use 0-7, subtrtact 1 from whatever is passed in
    //printf("Trans Num: %i\n",trans_num);
    int status = 0;

    //clear serial port before start
    //tcflush(serial_port, TCIOFLUSH);

    /*
    // transceiver bit selection
    if (trans_num <= 3){
        int input_B = transceiver_select[(trans_num % 7)][0];
        int input_A = transceiver_select[(trans_num % 7)][1];

        pinMode(25, OUTPUT);  // Pin B / GPIO Pin #26 of Pi / Physical Pin 37
        pinMode(24, OUTPUT);  // Pin A / GPIO Pin #19 of Pi / Physical Pin 35

        digitalWrite(25, input_B); // B bit selection
        digitalWrite(24, input_A); // A bit selection
        digitalWrite(23, 0);       // Allow Mux to operate
        digitalWrite(27, 1);       // Inhibit other Mux
    }else{
        int input_D = transceiver_select[(trans_num - 4)][0];
        int input_C = transceiver_select[(trans_num - 4)][1];

        pinMode(28, OUTPUT);  // Pin D / GPIO Pin #20 of Pi / Physical Pin 38
        pinMode(27, OUTPUT);  // Pin C / GPIO Pin #16 of Pi / Physical Pin 36

        digitalWrite(28, input_D); //D bit selection
        digitalWrite(27, input_C); //c bit selection
        digitalWrite(27, 0);       //Allow Mux to operate
        digitalWrite(23, 1);       //Inhibit other Mux
    }
    */

    // Write to serial port

    int sent_bytes = write(serial_port, data, strlen(data));      // send message
    printf("SENT BYTES: %i\n",sent_bytes);
    if (sent_bytes < 0){                                        // check for sending error
        printf("Error Sending\n");
    }else{
        status = 1;
    }

    return status;
}

int source_maintenance_routine_read(int transceiver, int port)
{
    /*
    wiringPiSetup();      // set up wiring the pins for transceiver selection
    pinMode(22, OUTPUT);  // RST Pin / GPIO Pin #6 of Pi / Physical Pin 31
    pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
    pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33
    */

    int trans_num = transceiver;                   // grab which transceiver to use
    int serial_port = port;                        // grab FD for serial port
    trans_num = trans_num - 1;                     // Passed in transceiver is from 1-8 but for calculation we use 0-7, subtrtact 1 from whatever is passed in
    //printf("Trans Num: %i\n",trans_num);
    int status = 0;

    //clear serial port before start
    tcflush(serial_port, TCIOFLUSH);

    /*
    // transceiver bit selection
    if (trans_num <= 3){
        int input_B = transceiver_select[(trans_num % 7)][0];
        int input_A = transceiver_select[(trans_num % 7)][1];

        pinMode(25, OUTPUT);  // Pin B / GPIO Pin #26 of Pi / Physical Pin 37
        pinMode(24, OUTPUT);  // Pin A / GPIO Pin #19 of Pi / Physical Pin 35

        digitalWrite(25, input_B); // B bit selection
        digitalWrite(24, input_A); // A bit selection
        digitalWrite(23, 0);       // Allow Mux to operate
        digitalWrite(27, 1);       // Inhibit other Mux
    }else{
        int input_D = transceiver_select[(trans_num - 4)][0];
        int input_C = transceiver_select[(trans_num - 4)][1];

        pinMode(28, OUTPUT);  // Pin D / GPIO Pin #20 of Pi / Physical Pin 38
        pinMode(27, OUTPUT);  // Pin C / GPIO Pin #16 of Pi / Physical Pin 36

        digitalWrite(28, input_D); //D bit selection
        digitalWrite(27, input_C); //c bit selection
        digitalWrite(27, 0);       //Allow Mux to operate
        digitalWrite(23, 1);       //Inhibit other Mux
    }
    */

    int num_bytes = 0;

    char read_buf[DATA_SIZE];  // Read Buffer

    clock_t start = clock();
    int elapsed_time = 0;

    do{
        clock_t difference = clock() - start;
        elapsed_time = difference*1000/CLOCKS_PER_SEC;
        //printf("Elasped time: %i\n",elapsed_time);

        if((num_bytes = read(serial_port, &read_buf, DATA_SIZE)) == 0){
            //printf("NOTHING\n");
        }

        if (num_bytes < 0){
            printf("Error reading: %s", strerror(errno));
            return 1;
        }

        if (num_bytes > 0){
            //printf("\n");
            //printf("Read %i bytes\n",num_bytes);
            COUNT = COUNT + num_bytes;
            //printf("Message Recieved: %s\n", read_buf);
            message[n] = read_buf[0];
            n++;
            //printf("Total of %i bytes sent\n",COUNT);
            //printf("SUCCESS\n");
            status = 1;
            //break;
        }
    }while(elapsed_time < 2000);

    memset(read_buf, 0, DATA_SIZE);
    usleep(1);
    tcflush(serial_port, TCIOFLUSH);

    return status;
}

int main() {
    //create threads
    //pthread_t thread_tx;
    //pthread_t thread_rx;

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

    //struct thread_args *arguments = malloc(sizeof(struct thread_args));
    //arguments->serial_port = serial_port;
    //arguments->trans_num = 1;

    // create the send and receive threads, passing in the FD
    //pthread_create(&thread_tx, NULL, tx_function, arguments);
    //pthread_create(&thread_rx, NULL, rx_function, arguments);

    // wait for the threads to finish
    //pthread_join(thread_tx, NULL);
    //pthread_join(thread_rx, NULL);

    int status_read, status_send;
    // ******** JEFF TESTING METHODS *********
    if (JEFF == 1){
        printf("RUNNING JEFF CODE\n");
        printf("\n");
        while(1){
            status_read = jeff_maintenance_routine_read(0,serial_port);
            n = 0;
            if (status_read == 0){
                //printf("COMMUNICATION TIMEOUT\n");
            }else if(status_read == 1){
                //printf("COMMUNICATION SUCCESS\n");
                printf("\n");
                printf("ENTIRE MESSAGE: %s\n",message);
            }else if(status_read == 3){
                //printf("BAD DATA\n");
            }

            status_send = jeff_maintenance_routine_send(0,message,serial_port);
            if (status_send == 0){
                //printf("ERROR SENDING\n");
            }else if(status_send == 1){
                //printf("SEND SUCCESSFUL\n");
            }
            memset(message,0,100);
        }
    }

    // ******** SOURCE TESTING METHODS *********
    if (SOURCE == 1){
        printf("RUNNING SOURCE CODE\n");
        printf("\n");
        while(1){

            char msg[DATA_SIZE];
            scanf("%s",&msg);
            char *data_location = msg;

            status_send = source_maintenance_routine_send(0,data_location,serial_port);

            if (status_send == 0){
                //printf("ERROR SENDING\n");
            }else if(status_send == 1){
                printf("SEND SUCCESSFUL\n");
            }

            status_read = source_maintenance_routine_read(0,serial_port);
            if (status_read == 0){
                printf("COMMUNICATION TIMEOUT\n");
            }else if(status_read == 1){
                //printf("COMMUNICATION SUCCESS\n");
                printf("ENTIRE MESSAGE: %s\n",message);
                int read_bytes = strlen(message);
                printf("READ BYTES: %i",read_bytes);
            }else if(status_read == 3){
                printf("BAD DATA\n");
            }
            printf("\n");
            n = 0;
            memset(message,0,100);
        }
    }
    printf("\n*** CLOSING COMMUNICATION CHANNEL ***\n");

    // close the serial ports
    close(serial_port);

    return 0; // success
}
