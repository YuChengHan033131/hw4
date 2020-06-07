#include "mbed.h"
#include "mbed_rpc.h"
#include "accelerometer.h"

RawSerial pc(USBTX, USBRX);
RawSerial xbee(D12, D11);

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t,acc;
DigitalOut blue(LED_BLUE);
Timer _t;

void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);
void state(Arguments *in, Reply *out);
RPCFunction rpc(&state, "state");
int dataCount=0;
void state(Arguments *in, Reply *out){
  pc.printf("\r\n%d\r\n%f\r",dataCount,_t.read());
  dataCount=0;
}
//float data[4][10];
void outputAcc(float x,float y,float z,float t){
  dataCount++;
}
void sample(){
    bool tilt=false;
    float x,y,z;
    _t.start();
    accelerometer(x,y,z);
    while(1){
        if((x*x+y*y)>0.5&&tilt==false){//tilt over 45
          tilt=true;
          blue=!tilt;
          for(int i=0;i<10;i++){
            outputAcc(x,y,z,_t.read());
            wait(0.1);
            accelerometer(x,y,z);
          }
        }else{
          if((x*x+y*y)<0.5){
            tilt=false;
            blue=!tilt;
          }
          outputAcc(x,y,z,_t.read());
          wait(0.5);
          accelerometer(x,y,z);
        }
    } 
}



int main(){
  pc.baud(9600);

  char xbee_reply[4];

  // XBee setting
  xbee.baud(9600);



  // start
  t.start(callback(&queue, &EventQueue::dispatch_forever));
  acc.start(sample);
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