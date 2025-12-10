#include "display.h"
#include "to_7seg.h"

typedef enum {Disp_on, Disp_off} disp_state_t;
static disp_state_t disp_state;

bool           gb_display_on_msg;          // start display
 bool           gb_display_off_msg;         // stop display
bool           gb_display_update_msg; 
uint16_t       g_display_segs;
bool           gb_display_brightness_msg;
uint8_t        g_display_brightness; 
bool volatile  gb_display_can_sleep;

static bool r_disp_on;
static bool disp_initd;
static int r_disp;
static int l_disp;
static int pulse_width;

static PwmOut* dsl;
static PwmOut* dsr;
static BusOut* seven_seg;

void display_fsm(void)
{
  if(disp_initd)
  {
  switch (disp_state)
  {
    case Disp_on: 
    
      if(gb_display_off_msg)
      {
        gb_display_off_msg=false;
        dsr->pulsewidth_us(0);
        dsl->pulsewidth_us(0);
        disp_state=Disp_off;
      }
      break;

    case Disp_off:
      

    
    if(gb_display_on_msg)
    {
      gb_display_on_msg=false;
      
      if(gb_display_update_msg)
      {
        gb_display_update_msg=false;

        l_disp=g_display_segs>>8;
        r_disp=g_display_segs&127;
      
    }
      
    if(gb_display_brightness_msg)
    {
      gb_display_brightness_msg=false;
      pulse_width=g_display_brightness/2.5;
    }
    
    if(r_disp_on)
    {
      r_disp_on=false;
      if(l_disp<16)
      *seven_seg=to_7seg(l_disp);
      else 
        *seven_seg=l_disp;
      dsl->pulsewidth_us(pulse_width);
      disp_state=Disp_on;
      break;
    }
    else
    {r_disp_on=true;
      if(r_disp<16)
      *seven_seg=to_7seg(r_disp);
      else
        *seven_seg=r_disp;
      dsr->pulsewidth_us(pulse_width);
      disp_state=Disp_on;
    }
  }
    
  }
  __disable_irq();
  if(!gb_display_on_msg&&!gb_display_off_msg&&!gb_display_brightness_msg&&!gb_display_update_msg)
    gb_display_can_sleep=true;
  __enable_irq();
}
  }

void display_init(PwmOut *dl, PwmOut *dr, BusOut *seven)
{
  if(!disp_initd)
  {
    disp_initd=true;
    
    disp_state=Disp_on;
    r_disp_on=false;
    
    dsl=dl;
    dsr=dr;
    seven_seg=seven;
    
    dsl->period_us(40);
    dsr->period_us(40);
    
  }
}
