#include <stdint.h>
#include "chpxr.h"
#include "ramfnc.h"
#include "trigr.h"
#include "sdcard.h"
#include "usrinp.h"

#define INTVECTORSIZE 16
#define INTVECTORADDRESSMASK 0xfff0
#define INTVECTORRANGEMASK 0x000f

uint8_t interruptVector[INTVECTORSIZE] = {
// MS    LS
  0x00, 0x00, // FFF0 FFF1 Reserved
  0x00, 0x00, // FFF2 FFF3 SWI3
  0x00, 0x00, // FFF4 FFF5 SWI2
  0x00, 0x00, // FFF6 FFF7 /FIRQ
  0x00, 0x00, // FFF8 FFF9 /IRQ
  0x00, 0x00, // FFFA FFFB SWI
  0x00, 0x00, // FFFC FFFD /NMI
  0x10, 0x00, // FFFE FFFF /RESET
};

void setup() {
  resetAll();
  // Addres bus low byte.
  // PA0 .. PA7: A0 .. A7
  // Address bus high byte.
  // PC0 .. PC7: A8 .. A15
  addrAsInput();

  // Data bus.
  // PL0 .. PL7: D0 .. D7
  dataAsInput();

  // Control 1.
  // PD7: RST
  resetAsOutput();

  // Control 1.
  // PG0, PG1, PG2: E, R/W, XXX
  // XXX are unused.
  readwriteAsInput();
  clkEAsInput();

  // Control 2.
  // PB0, PB1, PB2, PB3: CLK, XXX, XXX, XXX
  // XXX are unused.
  clockAsOutput();

  // Initialize signals.
  resetLo();
  clock();

  delay(500);

  // LED off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);

  Serial.println();
  Serial.println("    _________________________");
  Serial.println("   /                        /|");
  Serial.println("  _/  6809 Chip Exerciser  //");
  Serial.println(" /________________________//");
  Serial.println(" |U_U_U_U_U_U_U_U_U_U_U_U_/");
  Serial.println("  | | | | | | | | | | | |");
  Serial.println();
  printHelp();
  Serial.println();
  initRam();

  //getCardInfo();
}

OBSERVEEDGE(uint8_t, clk)
OBSERVEEDGE(uint8_t, uclkE)
OBSERVEEDGE(uint8_t, urw)

void loop() {
  static uint8_t udata = 0;
  static uint16_t uaddr = 0;
  static uint16_t ram_address = 0;

  // Toggle the clock to keep the processor running.
  clk = ~clk;
  clock(clk);

  urw = readwrite();
  ONRAISINGEDGE(urw)
    dataAsOutput();
  ONEND
  ONFALLINGEDGE(urw)
    dataAsInput();
  ONEND
  
  udata = data();
  uaddr = addr();
  ram_address = uaddr & RAMADDRESSMASK;

  if ((uaddr & INTVECTORADDRESSMASK) == INTVECTORADDRESSMASK) {
    udata = interruptVector[uaddr & INTVECTORRANGEMASK];
    data(udata);
  }
  else {
    if (urw) { // Read access
      udata = ram[ram_address];
      data(udata);
    }
  }

  uclkE = clkE();
  char cclkE = (uclkE ? '1' : '0');
  char cclk = (clk ? '1' : '0');

  ONRAISINGEDGE(clk)
    cclk = 'H';
  ONEND
  ONFALLINGEDGE(clk)
    cclk = 'L';
  ONEND

  uint8_t edgeE = 0;
  ONRAISINGEDGE(uclkE)
    cclkE = 'H';
    edgeE = 1;

    // Write access on raising edge of E.
    if (!urw) {
      ram[ram_address] = udata;
    }
  ONEND
  ONFALLINGEDGE(uclkE)
    cclkE = 'L';
    edgeE = 2;
  ONEND

  //
  // Processor status output and triggering.
  //
  if (count < maxCount) {
    if (count == 0) {
      Serial.println("count CLK E   R/W ADDR (RAMA) DATA");
      Serial.println("----------------------------------");
    }
    if (!outOnEdgeE || (edgeE & outOnEdgeE)) {
      char crw = (urw ? 'R' : 'W');
      sprintf(logstr, "%04d  %c   %c   %c   %04X (%04X) %02X\n",
        count, cclk, cclkE, crw, uaddr, ram_address, udata);
      Serial.print(logstr);

      if (trigger(urw, uaddr, udata)) {
        Serial.println("---- TR");
        count = maxCount - 1;
      }
    }
    if (count == (maxCount - 1)) {
      Serial.println();
    }

    count++;
  }

  processUserInput();
}
