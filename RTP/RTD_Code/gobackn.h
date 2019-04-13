int init(void);
float jimsrand(void);
void generate_next_arrival(void);
void insertevent( struct event *p);
void stoptimer(int AorB);
void starttimer(int AorB,float increment);
void tolayer3(int AorB,struct pkt packet);
void tolayer5(int AorB,char datasent[]);
int get_next_seqnum(struct windowElement *start);
int windowlen(int AorB);
int calc_checksum(struct pkt *tgt_pkt);
bool pkt_checksum_valid(struct pkt *tgt_pkt);
struct pkt *make_pkt(int seqnum, char data[20]);
void send_pkt(int caller, struct pkt *pkt_to_send);
void send_nack(int caller, struct pkt *pkt_to_nack);
void send_ack(int caller, struct pkt *pkt_to_ack);