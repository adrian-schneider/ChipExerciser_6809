#ifndef RAMFNC_H
#define RAMFNC_H

#define RAMSIZE 256
#define RAMADDRESSMASK 0x00ff

uint8_t ram[RAMSIZE];

void initRam() {
  for (int i = 0; i < sizeof(ram); i++) {
    ram[i] = 0x12;
  }
}

void loadPgm() {
  uint8_t pgm[] = {0x86, 0x00, 0xb7, 0x10, 0x80, 0x12, 0x4c, 0xb7, 0x10, 0x80, 0x7e, 0x10, 0x05};
  memcpy(ram, pgm, sizeof(pgm));
}

void dumpRam() {
  char str[8];
  Serial.println("      0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F");
  for (int i = 0; i < sizeof(ram); i++) {
    if ((i & 0x0f) == 0) {
      sprintf(str, "%04X: ", i);
      Serial.print(str);
    }
    char spc[3] = "  ";
    spc[1] = ((i & 0x07) == 0x07) ? ' ' : 0;
    sprintf(str, "%02X%s", ram[i], spc);
    Serial.print(str);
    if ((i & 0x0f) == 0x0f) {
      Serial.println();
    }
  }
}

#endif
