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

char rec_msg[8];  // Buffer for reading entire message packet to be returned to state machine

int readCounter; // counter used for populating rec_msg
FILE *fp;

char *filename;
int num_packet;             // variable for # of packets being sent
int writeCounter;

#endif
