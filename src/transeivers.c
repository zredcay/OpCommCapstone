// C library headers
#include <stdio.h>
#include <string.h>
#include <pthread.h>

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


int trans() {

   time_t t;
   int min = 0;
   int max = 3;
 ;
   
   /* Intializes random number generator */
   srand((unsigned) time(&t));

   
   return((rand() % (max - min +1)) + min);
   

   }
