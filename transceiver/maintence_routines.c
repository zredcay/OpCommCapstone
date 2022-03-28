int source_maintenance_routine_send(int transceiver, char **data, int port)
{
    /*
    wiringPiSetup();      // set up wiring the pins for transceiver selection
    pinMode(22, OUTPUT);  // RST Pin / GPIO Pin #6 of Pi / Physical Pin 31
    pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
    pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33
    */

    int trans_num = transceiver;            // grab which transceiver to use
    char **msg = data;                      // send message buffer
    int serial_port = port;                 // grab FD for serial port
    trans_num = trans_num - 1;              // Passed in transceiver is from 1-8 but for calculation we use 0-7, subtrtact 1 from whatever is passed in
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

    // Write to serial port
    int sent_bytes = write(serial_port, msg, strlen(msg));      // send message
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
    }while(elapsed_time < 3000);

    memset(read_buf, 0, DATA_SIZE);
    usleep(1);
    tcflush(serial_port, TCIOFLUSH);

    return status;
}

int jeff_maintenance_routine_read(int transceiver, int port)
{

    wiringPiSetup();      // set up wiring the pins for transceiver selection
    pinMode(22, OUTPUT);  // RST Pin / GPIO Pin #6 of Pi / Physical Pin 31
    pinMode(27, OUTPUT);  // INH Pin for 4-7 / GPIO Pin #16 of Pi / Physical Pin 36
    pinMode(23, OUTPUT);  // INH Pin for 0-3 / GPIO Pin #13 of Pi / Physical Pin 33

    int serial_port = port;              // grab FD for serial port
    int trans_num = transceiver;                   // grab which transceiver to use
    trans_num = trans_num - 1;                              // Passed in transceiver is from 1-8 but for calculation we use 0-7, subtrtact 1 from whatever is passed in
    //printf("Trans Num: %i\n",trans_num);
    int status = 0;

    //clear serial port before start
    tcflush(serial_port, TCIOFLUSH);


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
    }while(elapsed_time < 3000);

    memset(read_buf, 0, DATA_SIZE);
    usleep(1);
    tcflush(serial_port, TCIOFLUSH);

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
    if (sent_bytes < 0){                                                  // check for sending error
        printf("Error Sending\n");
    }else{
        status = 1;
    }

    return status;
}
