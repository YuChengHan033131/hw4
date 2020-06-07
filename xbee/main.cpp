#include "mbed.h"
#include "mbed_rpc.h"
#include "accelerometer.h"

RawSerial pc(USBTX, USBRX);
RawSerial xbee(D12, D11);

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);
void acceDisplay(Arguments *in, Reply *out);
RPCFunction rpcMove(&acceDisplay, "acceDisplay");
void acceDisplay(Arguments *in, Reply *out){
  float x,y,z;
  accelerometer(x,y,z);
  pc.printf("(%f,%f,%f)",x,y,z);
}
Thread accThread,exT;
Queue exQ;
accThread.start(&sample);
exT.start(callback(&exQ, &EventQueue::dispatch_forever));
float data[4][10];
int dataCount=0;
float tempData[4];
void storeData();
void sample(){
    bool tilt=false;
    Timer _t;
    _t.start();
    while(1){
        accelerometer(x,y,z);
        if((X*x+y*y)>2&&tilt==false){//tilt over 45
            tilt=true;
            Timer _t1;
            _t1.start();
            exQ.call_every(100,storeData);
            while(_t1<=1){}

        }else{
            tilt=false;
        }
        wait(0.05);
    } 
}
void storeData(){
    if(dataCount<10){
        data[4][dataCount]=tempData;
        dataCount++;
    }else{
        printf("data full");
    }
}



int main(){
  pc.baud(9600);

  char xbee_reply[4];

  // XBee setting
  xbee.baud(9600);



  // start
  pc.printf("start\r\n");
  t.start(callback(&queue, &EventQueue::dispatch_forever));

  // Setup a serial interrupt function of receiving data from xbee
  xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
}

void xbee_rx_interrupt(void)
{
  xbee.attach(NULL, Serial::RxIrq); // detach interrupt
  queue.call(&xbee_rx);
}

void xbee_rx(void)
{
  char buf[100] = {0};
  char outbuf[100] = {0};
  while(xbee.readable()){
    for (int i=0; ; i++) {
      char recv = xbee.getc();
      if (recv == '\r') {
        break;
      }
      buf[i] = pc.putc(recv);
    }
    RPC::call(buf, outbuf);
    pc.printf("%s\r\n", outbuf);
    wait(0.1);
  }
  xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt
}

void reply_messange(char *xbee_reply, char *messange){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
    pc.printf("%s\r\n", messange);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *messenger){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  xbee_reply[3] = xbee.getc();
  pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
  xbee_reply[0] = '\0';
  xbee_reply[1] = '\0';
  xbee_reply[2] = '\0';
  xbee_reply[3] = '\0';
}