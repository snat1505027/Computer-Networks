#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: SLIGHTLY MODIFIED
 FROM VERSION 1.1 of J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
       are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
       or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
       (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 1 /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */

#define SEQ_NUM 7
#define WINDOW_SIZE 4
#define A 0
#define B 1
#define TIME_OUT 25
#define BUFFER_SIZE 128

struct msg
{
    char data[20];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */

struct pkt
{
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
};

struct Sender
{
    int send_seq;
    int ack_seq;
    int recv_seq;
    int pck_num;
    struct pkt buffer[WINDOW_SIZE];

    int start_timer;
    int end_timer;
    int now_timer;
    float timer;
    float expire_timer[WINDOW_SIZE];
    int seq_timer;
    int total_seq_timer[WINDOW_SIZE];

    int start_queue;
    int end_queue;
    struct msg qbuffer[2];
} sender;

struct Receiver
{
    int send_seq;
    int ack_seq;
    int recv_seq;
    int pck_num;
    struct pkt buffer[WINDOW_SIZE];

    int start_timer;
    int end_timer;
    int now_timer;
    float timer;
    float expire_timer[WINDOW_SIZE];
    int seq_timer;
    int total_seq_timer[WINDOW_SIZE];

    int start_queue;
    int end_queue;
    struct msg qbuffer[50];
} receiver;

int generate_checksum(struct pkt packet)
{
    //printf("\t\t\tin function\npacket seq: %d\npacket ack: %d\nmessage: %s\n",packet.seqnum,packet.acknum,packet.payload);
    int sum = 0;
    sum += packet.seqnum;
    sum += packet.acknum;
    sum = (sum >> 16) + (sum & 0xffff);
    for (int i = 0; i < 20; i += 2)
    {
        sum += (packet.payload[i] << 8) + packet.payload[i + 1];
        sum = (sum >> 16) + (sum & 0xffff);
    }
    sum = (~sum) & 0xffff;
    return sum;
}

void addQueue(struct msg message, int AorB)
{
    /* bound check */
    if (AorB == 0)
    {
        if (sender.start_queue == sender.end_queue + 1)
        {
            printf("Warning: there is no avaliable space in queue.\n");
            return;
        }
        sender.qbuffer[sender.end_queue] = message;
        sender.end_queue = (sender.end_queue + 1) % WINDOW_SIZE;
    }
    else
    {
        if (receiver.start_queue == receiver.end_queue + 1)
        {
            printf("Warning: there is no avaliable space in queue.\n");
            return;
        }
        receiver.qbuffer[receiver.end_queue] = message;
        receiver.end_queue = (receiver.end_queue + 1) % WINDOW_SIZE;
    }
}

/* get messsage out of queue */
struct msg removeQueue(int AorB)
{
    /* bound check */
    struct msg temp;
    strcpy(temp.data, "");

    if (AorB == 0)
    {
        if (sender.start_queue == sender.end_queue)
        {
            printf("Warning: no packet in queue.\n");
            return temp;
        }
        struct msg message = sender.qbuffer[sender.start_queue];
        sender.start_queue = (sender.start_queue + 1) % WINDOW_SIZE;
        return message;
    }
    else
    {
        if (receiver.start_queue == receiver.end_queue)
        {
            printf("Warning: no packet in queue.\n");
            return temp;
        }
        struct msg message = receiver.qbuffer[receiver.start_queue];
        receiver.start_queue = (receiver.start_queue + 1) % WINDOW_SIZE;
        return message;
    }
}

/********* FUNCTION PROTOTYPES. DEFINED IN THE LATER PART******************/
void starttimer(int AorB, float increment);
void stoptimer(int AorB);
void tolayer3(int AorB, struct pkt packet);
void tolayer5(int AorB, char datasent[20]);

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    if (sender.pck_num >= WINDOW_SIZE)
    {
        printf("  A_output: buffer full. drop the message: %s\n", message.data);
        addQueue(message, A);
    }
    else
    {
        //printf("####\nA sent msg\n");
        struct pkt packet;
        strcpy(packet.payload, message.data);
        packet.seqnum = sender.send_seq;

        //printf("####\nChknum: %d\n",generate_checksum(packet));
        packet.acknum = (sender.recv_seq + WINDOW_SIZE - 1) % WINDOW_SIZE;
        //printf("####\nACK num: %d\n",packet.acknum);
        packet.checksum = generate_checksum(packet);

        sender.buffer[sender.send_seq] = packet;
        sender.send_seq = (sender.send_seq + 1) % WINDOW_SIZE;
        sender.pck_num++;
        tolayer3(A, packet);
        printf("\n#######\nA is going to send the following packet:\nSequence number: %d\nChecksum: %d\nMessage: %s\nThe ACK A is expecting to see is %d\n#######\n", packet.seqnum, packet.checksum, packet.payload, sender.recv_seq);

        //START
        if (sender.start_timer == sender.end_timer + 1)
        {
            printf("######Can't create more timers#####\n");
            return;
        }
        if (sender.now_timer == 0)
        {
            sender.now_timer = 1;
            sender.seq_timer = packet.seqnum;
            starttimer(A, TIME_OUT);
        }
        else
        {
            sender.expire_timer[sender.end_timer] = sender.timer + TIME_OUT;
            sender.total_seq_timer[sender.end_timer] = packet.seqnum;
            sender.end_timer = (sender.end_timer + 1) % WINDOW_SIZE;
        }
    }
}

/* need be completed only for extra credit */
void B_output(struct msg message)
{
    if(receiver.pck_num >= WINDOW_SIZE){
        printf("  B_output: buffer full. drop the message: %s\n", message.data);
        addQueue(message, B);
    }
    else{
        //printf("####\nB received msg\n");
        
        struct pkt packet;
        strcpy(packet.payload,message.data);
        packet.seqnum = receiver.send_seq;
        packet.acknum = (receiver.recv_seq+WINDOW_SIZE-1)%WINDOW_SIZE;
        packet.checksum = generate_checksum(packet);
        
        receiver.buffer[receiver.send_seq] = packet;
        receiver.send_seq = (receiver.send_seq+1)%WINDOW_SIZE;
        receiver.pck_num++;
        tolayer3(B,packet);
        printf("\n#######\nB is going to send the following packet:\nSequence number: %d\nChecksum: %d\nMessage: %s\nThe ACK A is expecting to see is %d\n#######\n",packet.seqnum,packet.checksum,packet.payload,receiver.recv_seq);


        //START
        if(receiver.start_timer == receiver.end_timer+1){
            printf("######Can't create more timers#####\n");
            return;
        }
        if(receiver.now_timer == 0){
            receiver.now_timer = 1;
            receiver.seq_timer = packet.seqnum;
            starttimer(B,TIME_OUT);
        }
        else{
            receiver.expire_timer[receiver.end_timer] = receiver.timer + TIME_OUT;
            receiver.total_seq_timer[receiver.end_timer] = packet.seqnum;
            receiver.end_timer = (receiver.end_timer+1)%WINDOW_SIZE;
        }
    }
}

/* called from layer 3, when a packet arrives for layer 4 */

void A_input(struct pkt packet)
{
    if (sender.recv_seq == packet.seqnum && packet.checksum == generate_checksum(packet))
    {
        //printf("####\nA received ACK\n");
        printf("\n##########\nA has received the ACK packet!\nACK: %d\nCurrent expected ACK: %d\n########\n", packet.acknum, sender.send_seq);

        struct msg message;
        strcpy(message.data, packet.payload);
        tolayer5(A, message.data);
        sender.recv_seq = (sender.recv_seq + 1) % WINDOW_SIZE;

        int flag = 0;
        if ((sender.ack_seq <= packet.acknum && packet.acknum < sender.send_seq) || (sender.send_seq < sender.ack_seq && sender.ack_seq <= packet.acknum) || (packet.acknum < sender.send_seq && sender.send_seq < sender.ack_seq))
            flag = 1;
        else
        {
            flag = 0;
        }

        while (flag)
        {
            sender.pck_num--;
            sender.ack_seq = (sender.ack_seq + 1) % WINDOW_SIZE;

            //STOP
            //stop_multitimer(AorB, expect_ack[AorB]);
            if (sender.now_timer == 0)
            {
                printf("######Can't create more timers#####\n");
                return;
            }
            stoptimer(A);
            sender.now_timer = 0;

            if (sender.start_timer != sender.end_timer)
            {
                sender.now_timer = 1;
                float inc = sender.expire_timer[sender.start_timer] - sender.timer;
                sender.seq_timer = sender.total_seq_timer[sender.start_timer];
                sender.start_timer = (sender.start_timer + 1) % WINDOW_SIZE;
                starttimer(A, inc);
            }

            if ((sender.ack_seq <= packet.acknum && packet.acknum < sender.send_seq) || (sender.send_seq < sender.ack_seq && sender.ack_seq <= packet.acknum) || (packet.acknum < sender.send_seq && sender.send_seq < sender.ack_seq))
                flag = 1;
            else
            {
                flag = 0;
            }
        }

        flag = 1;
        if (sender.start_queue == sender.end_queue)
        {
            flag = 0;
        }
        while (sender.pck_num < WINDOW_SIZE && !(sender.start_queue == sender.end_queue))
        {
            struct msg message = removeQueue(A);

            struct pkt npacket;
            strcpy(npacket.payload, message.data);
            npacket.seqnum = sender.send_seq;
            npacket.acknum = (sender.recv_seq + WINDOW_SIZE - 1) % WINDOW_SIZE;
            npacket.checksum = generate_checksum(npacket);
            sender.buffer[sender.send_seq] = npacket;
            sender.send_seq = (sender.send_seq + 1) % WINDOW_SIZE;
            sender.pck_num++;

            tolayer3(A, npacket);

            //START
            if (sender.start_timer == sender.end_timer + 1)
            {
                printf("######Can't create more timers#####\n");
                return;
            }
            if (sender.now_timer == 0)
            {
                sender.now_timer = 1;
                sender.seq_timer = npacket.seqnum;
                starttimer(A, TIME_OUT);
            }
            else
            {
                sender.expire_timer[sender.end_timer] = sender.timer + TIME_OUT;
                sender.total_seq_timer[sender.end_timer] = npacket.seqnum;
                sender.end_timer = (sender.end_timer + 1) % WINDOW_SIZE;
            }
        }
    }
}

/* called when A's timer goes off */
void A_timerinterrupt(void)
{
    sender.now_timer = 0;
    for (int i = sender.ack_seq; i != sender.send_seq; i=(i+WINDOW_SIZE)%WINDOW_SIZE)
    {
        if (i != sender.ack_seq)
        {

            //STOP
            if (sender.now_timer == 0)
            {
                printf("######Can't create more timers#####\n");
                return;
            }
            stoptimer(A);
            sender.now_timer = 0;

            if (sender.start_timer != sender.end_timer)
            {
                sender.now_timer = 1;
                float inc = sender.expire_timer[sender.start_timer] - sender.timer;
                sender.seq_timer = sender.total_seq_timer[sender.start_timer];
                sender.start_timer = (sender.start_timer + 1) % WINDOW_SIZE;
                starttimer(A, inc);
            }
        }
        tolayer3(A, sender.buffer[i]);

        //START
        if (sender.start_timer == sender.end_timer + 1)
        {
            printf("######Can't create more timers#####\n");
            return;
        }
        if (sender.now_timer == 0)
        {
            sender.now_timer = 1;
            sender.seq_timer = i;
            starttimer(A, TIME_OUT);
        }
        else
        {
            sender.expire_timer[sender.end_timer] = sender.timer + TIME_OUT;
            sender.total_seq_timer[sender.end_timer] = i;
            sender.end_timer = (sender.end_timer + 1) % WINDOW_SIZE;
        }
    }
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(void)
{
    sender.ack_seq = 0;
    sender.send_seq = 0;
    sender.recv_seq = 0;
    sender.pck_num = 0;

    sender.start_timer = 0;
    sender.end_timer = 0;
    sender.now_timer = 0;
    sender.seq_timer = 0;
    sender.timer = 0.0;

    sender.start_queue = 0;
    sender.end_queue = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    //printf("####\nACK num: %d\n",packet.acknum);
    printf("####\nin B received \npacket seq: %d\nexpected seq: %d\npkt chksum: %d\ngenerated chksum: %d\nmessage: %s\n", packet.seqnum, receiver.recv_seq, packet.checksum, generate_checksum(packet), packet.payload);
    if (receiver.recv_seq == packet.seqnum && packet.checksum == generate_checksum(packet))
    {
        //printf("####\nB received ACK\n");
        struct msg message;
        strcpy(message.data, packet.payload);
        tolayer5(B, message.data);
        receiver.recv_seq = (receiver.recv_seq + 1) % WINDOW_SIZE;

        int flag = 0;
        
        if (((receiver.ack_seq <= packet.acknum) && (packet.acknum < receiver.send_seq)) || ((receiver.send_seq < receiver.ack_seq) && (receiver.ack_seq <= packet.acknum)) || ((packet.acknum < receiver.send_seq) && (receiver.send_seq < receiver.ack_seq)))
        {
            flag = 1;
        }
        else
        {
            flag = 0;
        }

        printf("\t\t*****************receiver ack seq: %d , packet ack num: %d , receiver send seq: %d\n",receiver.ack_seq,packet.acknum,receiver.send_seq);
        printf("\t\t************there before while 1 flag %d*******************\n",flag);
        while (flag)
        {
            printf("\t\t\there in while 1\n");
            receiver.pck_num--;
            receiver.ack_seq = (receiver.ack_seq + 1) % WINDOW_SIZE;

            //STOP

            if (receiver.now_timer == 0)
            {
                printf("######Can't create more timers#####\n");
                return;
            }
            stoptimer(B);
            sender.now_timer = 0;

            if (receiver.start_timer != receiver.end_timer)
            {
                receiver.now_timer = 1;
                float inc = receiver.expire_timer[receiver.start_timer] - receiver.timer;
                receiver.seq_timer = receiver.total_seq_timer[receiver.start_timer];
                receiver.start_timer = (receiver.start_timer + 1) % WINDOW_SIZE;
                starttimer(B, inc);
            }

            int flag = 0;
            if (((receiver.ack_seq <= packet.acknum) && (packet.acknum < receiver.send_seq)) || ((receiver.send_seq < receiver.ack_seq) && (receiver.ack_seq <= packet.acknum)) || ((packet.acknum < receiver.send_seq) && (receiver.send_seq < receiver.ack_seq)))
            {
                flag = 1;
            }
            else
            {
                flag = 0;
            }
        }

        /*flag = 1;
        if(receiver.start_queue == receiver.end_queue){
            flag = 0;
        }*/
        while (receiver.pck_num < WINDOW_SIZE && !(receiver.start_queue == receiver.end_queue))
        {
            struct msg message = removeQueue(A);
            printf("\t\t\there in while 2\n");

            struct pkt npacket;
            strcpy(npacket.payload, message.data);
            npacket.seqnum = receiver.send_seq;
            npacket.acknum = (receiver.recv_seq + WINDOW_SIZE - 1) % WINDOW_SIZE;
            npacket.checksum = generate_checksum(npacket);
            receiver.buffer[receiver.send_seq] = npacket;
            receiver.send_seq = (receiver.send_seq + 1) % WINDOW_SIZE;
            receiver.pck_num++;

            tolayer3(B, npacket);

            if (receiver.start_timer == receiver.end_timer + 1)
            {
                printf("######Can't create more timers#####\n");
                return;
            }
            if (receiver.now_timer == 0)
            {
                receiver.now_timer = 1;
                receiver.seq_timer = npacket.seqnum;
                starttimer(B, TIME_OUT);
            }
            else
            {
                receiver.expire_timer[receiver.end_timer] = receiver.timer + TIME_OUT;
                receiver.total_seq_timer[receiver.end_timer] = npacket.seqnum;
                receiver.end_timer = (receiver.end_timer + 1) % WINDOW_SIZE;
            }
        }
    }
}

/* called when B's timer goes off */
void B_timerinterrupt(void)
{
    printf("  B_timerinterrupt: B doesn't have a timer. ignore.\n");
    receiver.now_timer = 0;
    printf("\t\t\t******here 1*******\n");
    for (int i = receiver.ack_seq; i != receiver.send_seq; i=(i+WINDOW_SIZE)%WINDOW_SIZE)
    {
    printf("\t\t\t******here 2*******\n");
    
        if (i != receiver.ack_seq)
        {
            //STOP
            printf("\t\t\t******here 3*******\n");
    

            if (receiver.now_timer == 0)
            {
                printf("######Can't create more timers#####\n");
                return;
            }
            stoptimer(B);
            receiver.now_timer = 0;
            printf("\t\t\t******here 4*******\n");
    
            if (receiver.start_timer != receiver.end_timer)
            {
                receiver.now_timer = 1;
                float inc = receiver.expire_timer[receiver.start_timer] - receiver.timer;
                receiver.seq_timer = receiver.total_seq_timer[receiver.start_timer];
                receiver.start_timer = (receiver.start_timer + 1) % WINDOW_SIZE;
                starttimer(B, inc);
            }
        }
        printf("\t\t\t******here buff*******\n");
    
        tolayer3(B, receiver.buffer[i]);

        printf("\t\t\t******here 5*******\n");
    
        //START
        if (receiver.start_timer == receiver.end_timer + 1)
        {
            printf("######Can't create more timers#####\n");
            return;
        }
        printf("\t\t\t******here 6*******\n");
    
        if (receiver.now_timer == 0)
        {
            receiver.now_timer = 1;
            receiver.seq_timer = i;
            starttimer(B, TIME_OUT);
        }
        else
        {
            receiver.expire_timer[receiver.end_timer] = receiver.timer + TIME_OUT;
            receiver.total_seq_timer[receiver.end_timer] = i;
            receiver.end_timer = (receiver.end_timer + 1) % WINDOW_SIZE;
        }
    }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(void)
{
    receiver.ack_seq = 0;
    receiver.send_seq = 0;
    receiver.recv_seq = 0;
    receiver.pck_num = 0;

    receiver.start_timer = 0;
    receiver.end_timer = 0;
    receiver.now_timer = 0;
    receiver.seq_timer = 0;
    receiver.timer = 0.0;

    receiver.start_queue = 0;
    receiver.end_queue = 0;
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
    - emulates the tranmission and delivery (possibly with bit-level corruption
        and packet loss) of packets across the layer 3/4 interface
    - handles the starting/stopping of a timer, and generates timer
        interrupts (resulting in calling students timer handler).
    - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event
{
    float evtime;       /* event time */
    int evtype;         /* event type code */
    int eventity;       /* entity where event occurs */
    struct pkt *pktptr; /* ptr to packet (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};
struct event *evlist = NULL; /* the event list */

/* possible events: */
#define TIMER_INTERRUPT 0
#define FROM_LAYER5 1
#define FROM_LAYER3 2

#define OFF 0
#define ON 1

int TRACE = 1;   /* for my debugging */
int nsim = 0;    /* number of messages from 5 to 4 so far */
int nsimmax = 0; /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;    /* probability that a packet is dropped  */
float corruptprob; /* probability that one bit is packet is flipped */
float lambda;      /* arrival rate of messages from layer 5 */
int ntolayer3;     /* number sent into layer 3 */
int nlost;         /* number lost in media */
int ncorrupt;      /* number corrupted by media*/

void init();
void generate_next_arrival(void);
void insertevent(struct event *p);

int main()
{
    struct event *eventptr;
    struct msg msg2give;
    struct pkt pkt2give;

    int i, j;
    char c;

    init();
    A_init();
    B_init();

    while (1)
    {
        eventptr = evlist; /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next; /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2)
        {
            printf("\nEVENT time: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", timerinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer5 ");
            else
                printf(", fromlayer3 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time = eventptr->evtime; /* update time to next event time */
        if (eventptr->evtype == FROM_LAYER5)
        {
            if (nsim < nsimmax)
            {
                if (nsim + 1 < nsimmax)
                    generate_next_arrival(); /* set up future arrival */
                /* fill in msg to give with string of same letter */
                j = nsim % 26;
                for (i = 0; i < 20; i++)
                    msg2give.data[i] = 97 + j;
                msg2give.data[19] = 0;
                if (TRACE > 2)
                {
                    printf("          MAINLOOP: data given to student: ");
                    for (i = 0; i < 20; i++)
                        printf("%c", msg2give.data[i]);
                    printf("\n");
                }
                nsim++;
                if (eventptr->eventity == A)
                    A_output(msg2give);
                else
                    B_output(msg2give);
            }
        }
        else if (eventptr->evtype == FROM_LAYER3)
        {
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i = 0; i < 20; i++)
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
            if (eventptr->eventity == A) /* deliver packet by calling */
                A_input(pkt2give);       /* appropriate entity */
            else
                B_input(pkt2give);
            free(eventptr->pktptr); /* free the memory for packet */
        }
        else if (eventptr->evtype == TIMER_INTERRUPT)
        {
            if (eventptr->eventity == A)
                A_timerinterrupt();
            else
                B_timerinterrupt();
        }
        else
        {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(eventptr);
    }

terminate:
    printf(
        " Simulator terminated at time %f\n after sending %d msgs from layer5\n",
        time, nsim);
}

void init() /* initialize the simulator */
{
    int i;
    float sum, avg;
    float jimsrand();

    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    printf("Enter the number of messages to simulate: ");
    scanf("%d", &nsimmax);
    printf("Enter  packet loss probability [enter 0.0 for no loss]:");
    scanf("%f", &lossprob);
    printf("Enter packet corruption probability [0.0 for no corruption]:");
    scanf("%f", &corruptprob);
    printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
    scanf("%f", &lambda);
    printf("Enter TRACE:");
    scanf("%d", &TRACE);

    srand(9999); /* init random number generator */
    sum = 0.0;   /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75)
    {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(1);
    }

    ntolayer3 = 0;
    nlost = 0;
    ncorrupt = 0;

    time = 0.0;              /* initialize time to 0.0 */
    generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand(void)
{
    double mmm = RAND_MAX;
    float x;          /* individual students may need to change mmm */
    x = rand() / mmm; /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival(void)
{
    double x, log(), ceil();
    struct event *evptr;
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + x;
    evptr->evtype = FROM_LAYER5;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}

void insertevent(struct event *p)
{
    struct event *q, *qold;

    if (TRACE > 2)
    {
        printf("            INSERTEVENT: time is %lf\n", time);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }
    q = evlist;    /* q points to header of list in which p struct inserted */
    if (q == NULL) /* list is empty */
    {
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else
    {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL) /* end of list */
        {
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        }
        else if (q == evlist) /* front of list */
        {
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        }
        else /* middle of list */
        {
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

void printevlist(void)
{
    struct event *q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next)
    {
        printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype,
               q->eventity);
    }
    printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(int AorB /* A or B is trying to stop timer */)
{
    struct event *q, *qold;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping timer at %f\n", time);
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;        /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist) /* front of list - there must be event after */
            {
                q->next->prev = NULL;
                evlist = q->next;
            }
            else /* middle of list */
            {
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(int AorB /* A or B is trying to start timer */, float increment)
{
    struct event *q;
    struct event *evptr;

    if (TRACE > 2)
        printf("          START TIMER: starting timer at %f\n", time);
    /* be nice: check to see if timer is already started, if so, then  warn */
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }

    /* create future event for when timer goes off */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    insertevent(evptr);
}

/************************** TOLAYER3 ***************/
void tolayer3(int AorB, struct pkt packet)
{
    struct pkt *mypktptr;
    struct event *evptr, *q;
    float lastime, x;
    int i;

    ntolayer3++;

    /* simulate losses: */
    if (jimsrand() < lossprob)
    {
        nlost++;
        if (TRACE > 0)
            printf("          TOLAYER3: packet being lost\n");
        return;
    }

    /* make a copy of the packet student just gave me since he/she may decide */
    /* to do something with the packet after we return back to him/her */
    mypktptr = (struct pkt *)malloc(sizeof(struct pkt));
    mypktptr->seqnum = packet.seqnum;
    mypktptr->acknum = packet.acknum;
    mypktptr->checksum = packet.checksum;
    for (i = 0; i < 20; i++)
        mypktptr->payload[i] = packet.payload[i];
    if (TRACE > 2)
    {
        printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
               mypktptr->acknum, mypktptr->checksum);
        for (i = 0; i < 20; i++)
            printf("%c", mypktptr->payload[i]);
        printf("\n");
    }

    /* create future event for arrival of packet at the other side */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtype = FROM_LAYER3;      /* packet will pop out from layer3 */
    evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
    evptr->pktptr = mypktptr;         /* save ptr to my copy of packet */
    /* finally, compute the arrival time of packet at the other end.
       medium can not reorder, so make sure packet arrives between 1 and 10
       time units after the latest arrival time of packets
       currently in the medium on their way to the destination */
    lastime = time;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_LAYER3 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();

    /* simulate corruption: */
    if (jimsrand() < corruptprob)
    {
        ncorrupt++;
        if ((x = jimsrand()) < .75)
            mypktptr->payload[0] = 'Z'; /* corrupt payload */
        else if (x < .875)
            mypktptr->seqnum = 999999;
        else
            mypktptr->acknum = 999999;
        if (TRACE > 0)
            printf("          TOLAYER3: packet being corrupted\n");
    }

    if (TRACE > 2)
        printf("          TOLAYER3: scheduling arrival on other side\n");
    insertevent(evptr);
}

void tolayer5(int AorB, char datasent[20])
{
    int i;
    if (TRACE > 2)
    {
        printf("          TOLAYER5: data received: ");
        for (i = 0; i < 20; i++)
            printf("%c", datasent[i]);
        printf("\n");
    }
}
