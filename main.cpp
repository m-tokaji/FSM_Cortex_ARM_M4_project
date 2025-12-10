#include "mbed.h"
#include "pinout.h"
#include "hardware.h"
#include "to_7seg.h"
#include "range_finder.h"
#include "switch.h"
#include "display.h"
#include "control.h"

bool encurso=false;
// mux stuff
static Ticker         g_mux_tick;
static bool volatile  gb_mux_evnt;
static bool volatile  gb_tmo1;
static Timeout tmo1;

// range_finder automatic start
static Ticker         g_meas_tick;
static bool volatile  gb_meas_evnt;

static void meas_isr (void) {
  gb_meas_evnt = true;
}

int main (void) {
  // the 4 MSB of this variable hold the symbol to be displayed at the
  //  left display, the 4 LSB that to be displayed at the right one
  hw_init();



  // initialize the range finder FSM
  rf_init(&g_trg, &g_ech);
  control_init(&g_lled, &g_rled, &g_mled, &g_voltage, &g_sr);
  // initialize the middle switch FSM
  swm_init(&g_swm);
  // complete this code  +++++++++++++++++++++++++++++++++++++++++++++++
 display_init(&g_dsl, &g_dsr, &g_seven_seg);
  //  ------------------------------------------------------------------

  for (;;) {
    // the range finder FSM
    rf_fsm();
    // the middle switch FSM
    swm_fsm();
    // complete this code  +++++++++++++++++++++++++++++++++++++++++++++
    control_fsm();
    //  ----------------------------------------------------------------
    display_fsm();

    // complete this code  (switch long & short pulsation actions) +++++


    
    // sleep
    __disable_irq();
    if (!gb_meas_evnt && !gb_rf_done_msg && !gb_mux_evnt
        && !gb_rf_start_msg && gb_rf_can_sleep
        && !gb_swm_msg && !gb_swm_long_msg && gb_swm_can_sleep&&gb_display_can_sleep&&gb_control_can_sleep) {
      __WFI();
    }
    __enable_irq();
  } // forever
} // main()

