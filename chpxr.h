#ifndef CHPXR_H
#define CHPXR_H

// Address Bus Low, Port A
#define BIT_AL_A0   0
#define BIT_AL_A1   1
#define BIT_AL_A2   2
#define BIT_AL_A3   3
#define BIT_AL_A4   4
#define BIT_AL_A5   5
#define BIT_AL_A6   6
#define BIT_AL_A7   7

// Address Bus High, Port C
#define BIT_AH_A8   0
#define BIT_AH_A9   1
#define BIT_AH_AA   2
#define BIT_AH_AB   3
#define BIT_AH_AC   4
#define BIT_AH_AD   5
#define BIT_AH_AE   6
#define BIT_AH_AF   7

// Data Bus, Port L
#define BIT_D_D0    0
#define BIT_D_D1    1
#define BIT_D_D2    2
#define BIT_D_D3    3
#define BIT_D_D4    4
#define BIT_D_D5    5
#define BIT_D_D6    6
#define BIT_D_D7    7

// Control Bus 1, Port D7, G0-G2
#define BIT_C1_RST  7
#define BIT_C1_E    0
#define BIT_C1_RW   1

// Control Bus 2, Port B0-B3
#define BIT_C2_CLK  0

#define BITCLR(port, bit)   ((port) &= ~(1 << (bit)))
#define BITSET(port, bit)   ((port) |= (1 << (bit)))
#define ISBITSET(port, bit) ((port) & (1 << (bit)))

#define OBSERVEEDGE_D(t, v, d) t v = 0; t v##_prev = (d);
#define OBSERVEEDGE(t, v) t v = 0; t v##_prev = 0x55;
#define ONRAISINGEDGE(v) if ((v != v##_prev) && v) { v##_prev = v;
#define ONFALLINGEDGE(v) if ((v != v##_prev) && ! v) { v##_prev = v;
#define ONANYEDGE(v) if (v != v##_prev) { v##_prev = v;
#define ONEND }


inline __attribute__ ((always_inline))
void addrAsOutput() {
  DDRA = 0xff;
  DDRC = 0xff;
}

inline __attribute__ ((always_inline))
void addrAsInput() {
  DDRA = 0x00;
  DDRC = 0x00;
}

inline __attribute__ ((always_inline))
void addr(uint16_t addr) {
  PORTA = addr;
  PORTC = addr >> 8;
}

inline __attribute__ ((always_inline))
uint16_t addr() {
  return PINA | (PINC << 8);
}

inline __attribute__ ((always_inline))
void dataAsOutput() {
  DDRL = 0xff;
}

inline __attribute__ ((always_inline))
void dataAsInput() {
  DDRL = 0x00;
}

inline __attribute__ ((always_inline))
void data(uint8_t data) {
  PORTL = data;
}

inline __attribute__ ((always_inline))
uint8_t data() {
  return PINL;
}

inline __attribute__ ((always_inline))
void resetAsOutput() {
  BITSET(DDRD, BIT_C1_RST);
}

inline __attribute__ ((always_inline))
void resetAsInput() {
  BITCLR(DDRD, BIT_C1_RST);
}

inline __attribute__ ((always_inline))
void resetLo() {
  BITCLR(PORTD, BIT_C1_RST);
}

inline __attribute__ ((always_inline))
void resetHi() {
  BITSET(PORTD, BIT_C1_RST);
}

inline __attribute__ ((always_inline))
void reset(uint8_t rst) {
  if (rst) {
    BITSET(PORTD, BIT_C1_RST);
  }
  else {
    BITCLR(PORTD, BIT_C1_RST);
  }
}

inline __attribute__ ((always_inline))
uint8_t reset() {
  return ISBITSET(PIND, BIT_C1_RST);
}

inline __attribute__ ((always_inline))
void clockAsOutput() {
  BITSET(DDRB, BIT_C2_CLK);
}

inline __attribute__ ((always_inline))
void clockAsInput() {
  BITCLR(DDRB, BIT_C2_CLK);
}

inline __attribute__ ((always_inline))
void clockLo() {
  BITCLR(PORTB, BIT_C2_CLK);
}

inline __attribute__ ((always_inline))
void clockHi() {
  BITSET(PORTB, BIT_C2_CLK);
}

inline __attribute__ ((always_inline))
void clock(uint8_t clk) {
  if (clk) {
    BITSET(PORTB, BIT_C2_CLK);
  }
  else {
    BITCLR(PORTB, BIT_C2_CLK);
  }
}

inline __attribute__ ((always_inline))
uint8_t clock() {
  return ISBITSET(PINB, BIT_C2_CLK);
}

inline __attribute__ ((always_inline))
void readwriteAsOutput() {
  BITSET(DDRG, BIT_C1_RW);
}

inline __attribute__ ((always_inline))
void readwriteAsInput() {
  BITCLR(DDRG, BIT_C1_RW);
}

inline __attribute__ ((always_inline))
void readwriteLo() {
  BITCLR(PORTG, BIT_C1_RW);
}

inline __attribute__ ((always_inline))
void readwriteHi() {
  BITSET(PORTG, BIT_C1_RW);
}

inline __attribute__ ((always_inline))
void readwrite(uint8_t rw) {
  if (rw) {
    BITCLR(PORTG, BIT_C1_RW);
  }
  else {
    BITSET(PORTG, BIT_C1_RW);
  }
}

inline __attribute__ ((always_inline))
uint8_t readwrite() {
  return ISBITSET(PING, BIT_C1_RW);
}

inline __attribute__ ((always_inline))
void clkEAsOutput() {
  BITSET(DDRG, BIT_C1_E);
}

inline __attribute__ ((always_inline))
void clkEAsInput() {
  BITCLR(DDRG, BIT_C1_E);
}

inline __attribute__ ((always_inline))
void clkELo() {
  BITCLR(PORTG, BIT_C1_E);
}

inline __attribute__ ((always_inline))
void clkEHi() {
  BITSET(PORTG, BIT_C1_E);
}

inline __attribute__ ((always_inline))
void clkE(uint8_t clk) {
  if (clk) {
    BITCLR(PORTG, BIT_C1_E);
  }
  else {
    BITSET(PORTG, BIT_C1_E);
  }
}

inline __attribute__ ((always_inline))
uint8_t clkE() {
  return ISBITSET(PING, BIT_C1_E);
}

// Reset ports A, C, G, L to inputs and set data to 0x00.
void resetAll() {
  DDRA = 0x00;
  DDRC = 0x00;
  DDRG = 0x00;
  DDRL = 0x00;
  PORTA = 0x00;
  PORTC = 0x00;
  PORTG = 0x00;
  PORTL = 0x00;
}

#endif
