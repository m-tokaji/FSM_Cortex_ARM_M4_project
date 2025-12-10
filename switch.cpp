#include "switch.h"


// extended state ------------------------------------------------------
  // "basic" state
  // complete this code to achieve the FSM functionality  ++++++++++++++
typedef enum {SW_IDLE, SW_IN, SW_OUT} sw_state_t;
static sw_state_t g_swm_state;
  //  ------------------------------------------------------------------

  // externally reachable objects
  // complete this code to achieve the FSM functionality  ++++++++++++++
bool volatile     gb_swm_can_sleep;
bool gb_swm_msg;
bool gb_swm_long_msg;

  //  ------------------------------------------------------------------

  // hardware resources
  // complete this code to achieve the FSM functionality  ++++++++++++++
static InterruptIn* gp_swm;
static Timeout swm_tmo1s;
static Timeout swm_tmo;


  //  ------------------------------------------------------------------

  // internal objects
  // complete this code to achieve the FSM functionality  ++++++++++++++
static bool gb_swm_initd;
static bool volatile swm_rise_evnt;
 bool volatile swm_fall_evnt;
static bool volatile swm_tmo_evnt;
static int beg;
static int endd;
static bool volatile swm_tmo1s_evnt;

  //  ------------------------------------------------------------------
// end of extended state  ----------------------------------------------

// ISRs ----------------------------------------------------------------
   // complete this code to achieve the FSM functionality  +++++++++++++

static void swm_rise(void)
{
  swm_rise_evnt=true;
  gb_swm_can_sleep=false;
}

static void swm_fall(void)
{
  swm_fall_evnt=true;
  gb_swm_can_sleep=false;
}

static void swm_tmo_isr(void)
{
  swm_tmo_evnt=true;
  gb_swm_can_sleep=false;
}

static void swm_tmo1s_isr(void)
{
  swm_tmo1s_evnt=true;
  gb_swm_can_sleep=false;
}



   //  -----------------------------------------------------------------
// end of ISRs  --------------------------------------------------------

// FSM  ----------------------------------------------------------------
void swm_fsm (void) {
  if (gb_swm_initd) {  // protect against calling swm_fsm() w/o a previous call to swm_init()
    switch (g_swm_state) {
      // complete this code to achieve the FSM functionality  ++++++++++
      case SW_IDLE: swm_rise_evnt=false;
                swm_tmo_evnt=false;
                swm_tmo1s_evnt=false;
              if (swm_fall_evnt)
              {
                swm_fall_evnt=false;
                swm_tmo.attach(swm_tmo_isr,20ms);
                g_swm_state=SW_IN;
              }
              break;
              
      case SW_IN: swm_rise_evnt=false;
              swm_fall_evnt=false;
              swm_tmo1s_evnt=false;
              if(swm_tmo_evnt)
              {
                swm_tmo_evnt=false;
                swm_tmo.detach();
                if(gp_swm->read())
                {
                  g_swm_state=SW_IDLE;
                  break;
                }
                else
                { 
                  swm_tmo1s.detach();
                  swm_tmo1s.attach(swm_tmo1s_isr,1s);
                  g_swm_state=SW_OUT;
                }
                
              }
              break;
              
       case SW_OUT: swm_fall_evnt=false;
                    swm_tmo_evnt=false;
              if(swm_rise_evnt)
              {
                swm_rise_evnt=false;
                gb_swm_msg=true;
                g_swm_state=SW_IDLE;
                    break;
              }
              
              if(swm_tmo1s_evnt)
              {
                swm_tmo1s_evnt=false;
                if(!gp_swm->read())
                  gb_swm_long_msg=true;
                g_swm_state=SW_IDLE;
              }
      //  ----------------------------------------------------------------
    } // switch (swm_state)

  // complete this code to achieve the FSM functionality (can sleep?) ++
    __disable_irq();
    if(!swm_tmo1s_evnt&&!swm_rise_evnt&&!swm_tmo_evnt&&!swm_fall_evnt)
      gb_swm_can_sleep=true;
    __enable_irq();



  //  ------------------------------------------------------------------
  } // if (gb_swm_initd)
}
// end of FSM ----------------------------------------------------------

// initialize FSM machinery  -------------------------------------------
void swm_init (InterruptIn *swm) {
  if (!gb_swm_initd) {
    gb_swm_initd = true;   // protect against multiple calls to swm_init

    // complete this code to achieve the FSM functionality  ++++++++++++

    // initialize state
      g_swm_state=SW_IDLE;
      swm_rise_evnt=false;
      swm_fall_evnt=false;
      swm_tmo_evnt=false;




    // initial actions
    gp_swm = swm;              // save pointers to the pins

    // register ISRs
    gp_swm->rise(swm_rise);
    gp_swm->fall(swm_fall);


    //  ----------------------------------------------------------------
  }
}
// end of FSM initialization  ------------------------------------------
