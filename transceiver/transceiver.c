// C library headers
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "transceiver.h"
const int DATA_SIZE = 256;  // Only going to be sending chunks of 100 bytes but have buffer size set at 256 just in case
int COUNT = 0;

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
    wiringPiSetup();      // set up wiring the pins for transceiver selection
    pinMode(22, OUTPUT);  // RST Pin / GPIO Pin #6 of Pi / Physical Pin 31
    pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
    pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33

    // grab passed in arguments
    struct thread_args* arguments = (struct args*) vargp;

    int *serial_port = arguments->serial_port;              // grab FD for serial port
    int trans_num = arguments->trans_num;                   // grab which transceiver to use
    trans_num = trans_num - 1;                              // Passed in transceiver is from 1-8 but for calculation we use 0-7, subtrtact 1 from whatever is passed in
    printf("Trans Num: %i\n",trans_num);

    //clear serial port before start
    tcflush(serial_port, TCIOFLUSH);

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

    char msg[DATA_SIZE];                                            // send message buffer

    digitalWrite(22,0);  // Set RST LOW
    usleep(0.5);
    digitalWrite(22,1);  // Set RST Active HIGH

    // Write to serial port
    while(1){
        // Reset transceiver
        //digitalWrite(22,0);  // Set RST LOW
        //usleep(0.5);
        //digitalWrite(22,1);  // Set RST Active HIGH

        //memset(msg, 0, DATA_SIZE);
        //memset(msg, '@', DATA_SIZE);
        int sent_bytes_check = scanf("%s",&msg);

        int sent_bytes = write(serial_port, msg, strlen(msg));      // send message
        if (sent_bytes < 0){                                        // check for sending error
            printf("Error Sending\n");
            return 0;
        }

        int result = strcmp("END", msg);                            // send 'END' to close channel
        if (result  == 0){
            break;
        }
    }
}

// Recieve thread function, takes argument of the serial port file descriptor(FD)
void *rx_function(void *vargp)
{
    // grab passed in arguments
    struct thread_args* arguments = (struct args*) vargp;

    int *serial_port = arguments->serial_port;              // grab FD for serial port
    int trans_num = arguments->trans_num;                   // grab which transceiver to use
    trans_num = trans_num - 1;

    // transceiver bit selection
    int input_B = transceiver_select[(trans_num % 7)][0];
    int input_A = transceiver_select[(trans_num % 7)][1];

    pinMode(25,OUTPUT);  // Pin B0
    pinMode(24, OUTPUT); //Pin A0

    digitalWrite(25,0); //B selection
    digitalWrite(24,0); //A selection

    // Track how many bytes are sent
    int num_bytes = 0;
    int n = 0;

    char read_buf [DATA_SIZE];  // Read Buffer

    // Read from serial port
    while(1){
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

        num_bytes = 0;

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

        // Print out the recieved message
        if (n>0){
            printf("\n");
            printf("RECEIVED: %s \n",read_buf);
            //printf("SIZE: %i\n",n);
        }
        memset(read_buf, 0, DATA_SIZE);
        usleep(0.0005);
        tcflush(serial_port, TCIOFLUSH);
        digitalWrite(22,0);  // Set RST Low
        digitalWrite(22,1);  //  Set RST Active High
    }
}

int main() {
    //create threads
    pthread_t thread_tx;
    pthread_t thread_rx;

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

    tty.c_cc[VTIME] = 1;    // Wait for up to 10s (100 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 1000;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    struct thread_args *arguments = malloc(sizeof(struct thread_args));
    arguments->serial_port = serial_port;
    arguments->trans_num = 1;

    // create the send and receive threads, passing in the FD
    pthread_create(&thread_tx, NULL, tx_function, arguments);
    pthread_create(&thread_rx, NULL, rx_function, arguments);

    // wait for the threads to finish
    pthread_join(thread_tx, NULL);
    pthread_join(thread_rx, NULL);

    printf("\n*** CLOSING COMMUNICATION CHANNEL ***\n");

    // close the serial ports
    close(serial_port);

    return 0; // success
}
