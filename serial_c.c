// C library headers
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "header.h"
const int DATA_SIZE = 512;

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// *****************************************************************************************************
// IMPORTANT NOTES:
// RST Pin must be held at HIGH
// It appears that if a wire is unplugged while the program is running, the IR sensors get tripped up
// by reseting the RST pin by unplugging it for 2 sec. seems to fix the problem
// *****************************************************************************************************

// Send thread function, takes argument of the serial port file descriptor(FD)
void *tx_function(void *vargp)
{
    // grab FD passed in
    int *serial_port = (int *)vargp;

    tcflush(serial_port, TCIOFLUSH);

    // Write to serial port
    while(1){
        char msg[DATA_SIZE];                                              // send message buffer
        //memset(msg, 0, DATA_SIZE);
        //memset(msg, '@', DATA_SIZE);
        int sent_bytes_check = scanf("%s",&msg);
        //printf("SEND: ");

        //printf("sent bytes: %i\n",sent_bytes_check);

        int sent_bytes = write(serial_port, msg, strlen(msg));    // send message
        if (sent_bytes < 0){                                        // check for sending error
            printf("Error Sending\n");
            return 0;
        }

        int result = strcmp("END", msg);                            // send 'END' to close channel
        if (result  == 0){
            break;
        }

        memset(msg, 0, DATA_SIZE);
        //sleep(1);
    }
}

// Recieve thread function, takes argument of the serial port file descriptor(FD)
void *rx_function(void *vargp)
{
    // grab FD passed in
    int *serial_port = (int *)vargp;

    // Read from serial port
    while(1){

        // Allocate memory for read buffer
        char read_buf [DATA_SIZE];

        // Read each character that comes in to the serial port
        int i = 0;
        char c;
        int n = 0;
        int num_bytes = 0;

        num_bytes = read(serial_port, &read_buf, DATA_SIZE);

        if (num_bytes < 0){
            printf("Error reading: %s", strerror(errno));
            return 1;
        }

        if (num_bytes > 0){
            printf("\n");
            printf("Read %i bytes\n",num_bytes);
            printf("Message Recieved: %s\n", read_buf);
        }

        int result = strcmp("END", read_buf);
        if (result  == 0){
            printf("\n*** END CHANNEL MESSAGE RECIEVED ***\n");
            break;
        }

        /*do
        {
            n = read(serial_port, (void*) &c, 1);
            if (n > 0){
                read_buf[i++] = c;
            }
        }
        while(c != '\r' && i < (data_size - 1) && n > 0);

        int result = strcmp("END", read_buf);
        if (result  == 0){
            printf("\n*** END CHANNEL MESSAGE RECIEVED ***\n");
            break;
        }

        // add a final empty character to the end of the read buffer
        read_buf[i] = '\0';
        */



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

        // flush serial port or else unwanted data comes through
        //printf("Trying to read");
        memset(read_buf, 0, DATA_SIZE);
        usleep(1);
        tcflush(serial_port, TCIOFLUSH);
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
    tty.c_cc[VMIN] = 100;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    // create the send and receive threads, passing in the FD
    pthread_create(&thread_tx, NULL, tx_function, (void*) serial_port);
    pthread_create(&thread_rx, NULL, rx_function, (void*) serial_port);

    // wait for the threads to finish
    pthread_join(thread_tx, NULL);
    pthread_join(thread_rx, NULL);

    printf("\n*** CLOSING COMMUNICATION CHANNEL ***\n");

    // close the serial ports
    close(serial_port);

    return 0; // success
}
