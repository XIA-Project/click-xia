#ifndef CLICK_COUNTERFLOOD_HH
#define CLICK_COUNTERFLOOD_HH
#include <click/element.hh>
#include <click/glue.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include <click/etheraddress.hh>
#include <click/vector.hh>
#include <click/hashmap.hh>
#include <click/dequeue.hh>
#include <elements/grid/linktable.hh>
#include <elements/grid/arptable.hh>
#include <elements/grid/sr/path.hh>
#include "counterflood.hh"
#include <elements/wifi/rxstats.hh>
CLICK_DECLS

/*
 * =c
 * CounterFlood(ETHTYPE eth, IP ip, BCAST_IP ip, ETH eth, COUNT int, 
 *              MAX_DELAY int, 
 *              [DEBUG bool], [HISTORY int]);
 * =d
 * DSR-inspired end-to-end ad-hoc routing protocol.
 * Input 0: ethernet packets from dev
 * Input 1: ip packets from higher layer 
 * Output 0: ethernet packets to device (protocol)
 * Output 1: ip packets to higher layer
 *
 * =over 8
 *
 * =item COUNT
 * 
 * count of x indicates don't forward if you've recieved x packets
 * Count of 0 indicates always forward
 * Count of 1 indicates never forward; like a local broadcast
 * 
 * 
 * =item MAX_DELAY
 *
 * max time to wait after 1st packet rx to forward packet. default is 750
 *
 * =item HISTORY
 *
 * number of sequence numbers to remember. default is 100
 *
 *
 *
 *
 */


class CounterFlood : public Element {
 public:
  
  CounterFlood();
  ~CounterFlood();
  
  const char *class_name() const		{ return "CounterFlood"; }
  const char *processing() const		{ return PUSH; }
  int initialize(ErrorHandler *);
  CounterFlood *clone() const;
  int configure(Vector<String> &conf, ErrorHandler *errh);


  static String static_print_debug(Element *f, void *);
  static int static_write_debug(const String &arg, Element *e,
				void *, ErrorHandler *errh); 

  static int static_write_count(const String &arg, Element *e,
				void *, ErrorHandler *errh); 

  static String static_print_stats(Element *e, void *);
  String print_stats();
  static String static_print_count(Element *e, void *);
  String print_count();

  static String static_print_packets(Element *e, void *);
  String print_packets();

  void push(int, Packet *);

  void add_handlers();
private:


  class Broadcast {
  public:
    uint32_t _seq;
    bool _originated; /* this node started the bcast */
    Packet *_p;
    int _num_rx;
    struct timeval _first_rx;
    bool _forwarded;
    bool _actually_sent;
    Timer *t;
    struct timeval _to_send;

    void del_timer() {
      if (t) {
	t->unschedule();
	delete t;
	t = NULL;
      }
    }
  };


  DEQueue<Broadcast> _packets;

  IPAddress _ip;    // My IP address.
  EtherAddress _en; // My ethernet address.
  uint32_t _et;     // This protocol's ethertype
  IPAddress _bcast_ip;

  EtherAddress _bcast;

  bool _debug;


  int _packets_originated;
  int _packets_tx;
  int _packets_rx;

  int _count;
  int _max_delay_ms;

  int _history;

  void forward(Broadcast *bcast);
  void forward_hook();
  void trim_packets();
  static void static_forward_hook(Timer *, void *e) { 
    ((CounterFlood *) e)->forward_hook(); 
  }
};


CLICK_ENDDECLS
#endif
