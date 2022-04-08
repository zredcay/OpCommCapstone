#ifndef _transceiver_h_
#define _transceiver_h_

void *tx_function(void *vargp);

int jeff_maintenance_routine_read(int transceiver, int port);
int jeff_maintenance_routine_send(int transceiver, char data[], int port);
int source_maintenance_routine_send(int transceiver, char data[], int port);
int source_maintenance_routine_read(int transceiver, int port);

struct thread_args{
    int serial_port;
    int trans_num;
};
const int DATA_SIZE = 64;  // Only going to be sending chunks of 100 bytes but have buffer size set at 256 just in case
char rec_msg[8];  // Buffer for reading entire message packet to be returned to state machine
int source_trans = 0;
int jeff_trans = 4;
int readCounter; // counter used for populating rec_msg
FILE *fp;
int transceiver_select[4][2] =
        {
                {0,0},
                {0,1},
                {1,0},
                {1,1}
        };
char *filename;
int num_packet;             // variable for # of packets being sent
int writeCounter;

#endif
