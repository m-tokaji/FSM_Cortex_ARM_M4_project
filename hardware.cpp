#include "hardware.h"

// seven segment display anodes
// when in a int8_t, they are 0b-GFEDCBA
BusOut      g_seven_seg(SGA_PIN, SGB_PIN, SGC_PIN, SGD_PIN,
                        SGE_PIN, SGF_PIN, SGG_PIN);

// display cathodes
PwmOut  g_dsl(DSL_PIN);
PwmOut  g_dsr(DSR_PIN);
DigitalOut g_rled(LDR_PIN);
DigitalOut g_lled(LDL_PIN);
DigitalOut g_mled(LDM_PIN);
AnalogIn g_voltage (LIT_PIN);
BufferedSerial g_sr(USBTX, USBRX);

// hc-sr04 range finder
DigitalOut  g_trg(TRG_PIN);
InterruptIn g_ech(ECH_PIN);

// swm
InterruptIn g_swm(SWM_PIN);

// hardware initialization
void hw_init (void) {
  g_ech.mode(PullDown);
  g_swm.mode(PullUp);
}
