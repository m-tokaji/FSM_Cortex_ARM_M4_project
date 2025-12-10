#include "control.h"
#include "switch.h"
#include "display.h"
#include "range_finder.h"
#include "hardware.h"

typedef enum {OFF, MENU, Li, LE, di} control_t;
static control_t control_state;

static bool gb_control_initd;
static Ticker         g_mux_tick;
static bool volatile  gb_mux_evnt;
static int offon=0;
static Timeout tmoled;
static bool volatile  gb_tmoled_evnt;
static int pos;
static Ticker         g_meas_tick;
static bool volatile  gb_meas_evnt;
uint8_t   disp = 0;
static Timeout tmoledm;
static bool volatile  gb_tmoledm_evnt;
bool volatile  gb_control_can_sleep;
static char buff;
static bool volatile gb_write;
char msg[3];

static DigitalOut* rled;
static DigitalOut* lled;
static DigitalOut* mled;
static AnalogIn* voltage;
static BufferedSerial* sr;

static void mux_isr (void) {
  gb_mux_evnt = true;
}

static void tmoled_isr(void)
{
  gb_tmoled_evnt=true;
}
static void wr_isr(void)
{
  if(sr->writable())
      gb_write=true;
}
static void meas_isr(void)
{
  gb_meas_evnt=true;
}

static void tmoledm_isr(void)
{
  gb_tmoledm_evnt=true;
}



void control_init(DigitalOut* l,DigitalOut* r, DigitalOut* m, AnalogIn* v, BufferedSerial* s )
{
  rled=r;
  lled=l;
  mled=m;
  voltage=v;
  sr=s;
  sr->set_baud(230400);
  sr->set_format(8, sr->None, 2);
  sr->sigio(wr_isr);
  control_state=OFF;
  gb_control_initd=true;
  
}

void control_fsm(void)
{
  if(gb_control_initd)
  {
    switch (control_state)
    {
      case OFF: 
        gb_mux_evnt=false;
        gb_tmoledm_evnt=false;
        gb_meas_evnt=false;
      if(gb_tmoled_evnt)
      {
        gb_tmoled_evnt=false;
        lled->write(0);
        rled->write(0);
      }
      
      gb_display_off_msg=true;
      
      if(gb_swm_long_msg)
      {
        
        gb_swm_long_msg=false;
        gb_display_update_msg=true;
        g_display_segs=(56<<8)|16;
        gb_display_brightness_msg=true;
        g_display_brightness=100;
        g_mux_tick.attach(mux_isr, 4ms);
        lled->write(1);
        tmoled.attach(tmoled_isr, 50ms);
        pos=0;
        printf("On\n");
        control_state=MENU;
      }
      break;
      case MENU:
         gb_tmoledm_evnt=false;
        gb_meas_evnt=false;
        if(gb_tmoled_evnt)
      {
        gb_tmoled_evnt=false;
        lled->write(0);
        rled->write(0);
      }
      

            if (gb_mux_evnt) {
                      gb_mux_evnt=false;
                        if(offon){
                                    gb_display_off_msg=true;
                                    offon=0;
                                  }
                            else
                                  {
                                    gb_display_on_msg=true;
                                    offon=1;
                                  }
                                      }
            if(gb_swm_msg)
            {
              gb_swm_msg=false;
              tmoled.attach(tmoled_isr, 50ms);
              rled->write(1);
              switch(pos)
              {
                case 0: pos=1;
                gb_display_update_msg=true;
        g_display_segs=(94<<8)|16;//di
                break;
                case 1: pos=2;
                gb_display_update_msg=true;
        g_display_segs=(56<<8)|14;//LE
                break;
                case 2: pos=3;
                gb_display_update_msg=true;
        g_display_segs=15;//OF
                break;
                case 3: pos=0;
                gb_display_update_msg=true;
        g_display_segs=(56<<8)|16;//Li
                
                        
              }
            }
            
            if(gb_swm_long_msg)
            {
              gb_swm_long_msg=false;
              lled->write(1);
              tmoled.attach(tmoled_isr, 50ms);
              switch(pos)
              {
                case 0: 
                  g_meas_tick.attach(meas_isr, 100ms);
                printf("=>Li\n");
                  control_state=Li;
                break;
                case 1: g_meas_tick.attach(meas_isr, 100ms);
                printf("=>di\n");
                  control_state=di;
                break;
                case 2: gb_display_update_msg=true;
                  g_display_segs=(64<<8)|64;
                printf("=>LE\n");
                  g_meas_tick.attach(meas_isr, 100ms);
                  control_state=LE;
                break;
                case 3:control_state=OFF;
                printf("Off\n");
                
              }
            }
            break;
            
      case Li: 
       gb_tmoledm_evnt=false;
      
      if(gb_tmoled_evnt)
      {
        gb_tmoled_evnt=false;
        lled->write(0);
        rled->write(0);
      }
       if (gb_mux_evnt) {
                      gb_mux_evnt=false;
                        if(offon){
                                    gb_display_off_msg=true;
                                    offon=0;
                                  }
                            else
                                  {
                                    gb_display_on_msg=true;
                                    offon=1;
                                  }
                                      }
      if(gb_swm_long_msg)
      {
        gb_swm_long_msg=false;
        lled->write(1);
        tmoled.attach(tmoled_isr, 50ms);
        g_meas_tick.detach();
        gb_display_update_msg=true;
        g_display_segs=(56<<8)|16;//Li
        printf("<=Li\n");
        control_state=MENU;
        break;
      }
      
      if(gb_swm_msg)
      {
        gb_swm_msg=false;
        rled->write(1);
        tmoled.attach(tmoled_isr, 50ms);
      }
      
      if(gb_meas_evnt)
      {
        gb_meas_evnt=false;
        gb_display_update_msg=true;
        disp=voltage->read_u16()*100/65535;
        printf("    %i\n",disp);
        g_display_segs=(disp/10<<8)|disp%10;
        gb_display_brightness_msg=true;
        g_display_brightness=disp;
      }
      break;
       case di: 
          gb_tmoledm_evnt=false;
       if(gb_tmoled_evnt)
      {
        gb_tmoled_evnt=false;
        lled->write(0);
        rled->write(0);
      }
       if (gb_mux_evnt) {
                      gb_mux_evnt=false;
                        if(offon){
                                    gb_display_off_msg=true;
                                    offon=0;
                                  }
                            else
                                  {
                                    gb_display_on_msg=true;
                                    offon=1;
                                  }
                                      }
      if(gb_swm_long_msg)
      {
        gb_swm_long_msg=false;
        lled->write(1);
        tmoled.attach(tmoled_isr, 50ms);
        g_meas_tick.detach();
                        gb_display_update_msg=true;
        g_display_segs=(94<<8)|16;//di
        printf("<=di\n");
        control_state=MENU;
        break;
      }
      
      if(gb_swm_msg)
      {
        gb_swm_msg=false;
        rled->write(1);
        tmoled.attach(tmoled_isr, 50ms);
      }
      
       if (gb_meas_evnt) {
      gb_meas_evnt = false;
      gb_rf_start_msg = true;
    }
       
    if (gb_rf_done_msg) {
      gb_rf_done_msg = false;
      gb_display_brightness_msg=true;
      gb_display_update_msg=true;
      if (g_rf_range_cm > 99) {
        g_display_brightness=99;
        disp = 0xBB;  // --
      } else if (-1 == g_rf_range_cm) {
       disp = 0xEC;  // Er
      } else {
        g_display_brightness=g_rf_range_cm;
       disp= (g_rf_range_cm / 10) << 4 | (g_rf_range_cm % 10);
      }
      if(disp==0xBB)
        printf("    --\n");
      else if (disp==0xEC)
        printf("    Er\n");
      else printf("    %i\n", g_rf_range_cm);
      g_display_segs=((disp>>4)<<8)|disp&15;
    }
    
    break;
    
                case LE: 
                if(gb_tmoled_evnt)
      {
        gb_tmoled_evnt=false;
        lled->write(0);
        rled->write(0);
      }
       if (gb_mux_evnt) {
                      gb_mux_evnt=false;
                        if(offon){
                                    gb_display_off_msg=true;
                                    offon=0;
                                  }
                            else
                                  {
                                    gb_display_on_msg=true;
                                    offon=1;
                                  }
                                      }
      if(gb_swm_long_msg)
      {
        gb_swm_long_msg=false;
        lled->write(1);
        tmoled.attach(tmoled_isr, 50ms);
        g_meas_tick.detach();
        mled->write(0);
                        gb_display_update_msg=true;
        g_display_segs=(56<<8)|14;//LE
        printf("<=LE\n");
        control_state=MENU;
        break;
      }
      
      if(gb_swm_msg)
      {
        gb_swm_msg=false;
        rled->write(1);
        tmoled.attach(tmoled_isr, 50ms);
      }
      
      if(gb_meas_evnt)
      {
        gb_meas_evnt=false;
        tmoledm.attach(tmoledm_isr, 50ms);
        mled->write(1);
      }
      
      if(gb_tmoledm_evnt)
      {
        gb_tmoledm_evnt=false;
        mled->write(0);
      }
      
      
      
        
    }//switch
    __disable_irq();
    if(!gb_mux_evnt&&!gb_tmoled_evnt&&!gb_meas_evnt&&!gb_tmoledm_evnt)
      gb_control_can_sleep=true;
    __enable_irq();
  }
  
  
}