#ifndef TRIGR_H
#define TRIGR_H

#define TC_TYPE_NONE    0 // empty condition
#define TC_TYPE_DATA    1 // data condition
#define TC_TYPE_ADDRESS 2 // address condition

#define TC_RW_WRITE     0 // write access
#define TC_RW_READ      1 // read access
#define TC_RW_DONTCARE  3 // don't care access

#define TC_LOGIC_ASSIGN 0 // assign
#define TC_LOGIC_OR     1 // assign with or
#define TC_LOGIC_AND    2 // assign with and

struct triggerCond_t {
  uint8_t type  : 3; // 0: none, 1:data, 2:address
  uint8_t neg   : 1; // 0: no-negate, 1: negate
  uint8_t rw    : 2; // 00: write, 01: read, 11:don't care
  uint8_t logic : 2; // 00:none, 01:or, 10:and
  union {
    uint8_t dataFrom;
    uint16_t addrFrom;
  };
  union {
    uint8_t dataTo;
    uint16_t addrTo;
  };
};

#define TRIGGERCOUNT 4
struct triggerCond_t triggerCond[TRIGGERCOUNT];

bool testTriggerCond(uint8_t rw, uint16_t addr, uint8_t data, struct triggerCond_t tc) {
  bool cond = false;
  if (tc.type == TC_TYPE_DATA) {
    cond = (tc.dataFrom <= data) && (data <= tc.dataTo);
  }
  else if (tc.type == TC_TYPE_ADDRESS) {
    cond = (tc.addrFrom <= addr) && (addr <= tc.addrTo);
  }

  if (tc.rw != TC_RW_DONTCARE) {
    cond = cond && (!tc.rw == !rw);
    char str[16];
    sprintf(str, "-- %02x %02x", tc.rw, rw);
    Serial.println(str);
  }

  if (tc.neg) {
    cond = !cond;
  }
  return cond;
}

void resetTriggerCond() {
  memset(triggerCond, 0, sizeof(triggerCond));
}

void printTriggerCond() {
  char str[64];
  Serial.println("Trigger conditions:");
  for (int i = 0; i < TRIGGERCOUNT; i++) {
    if (triggerCond[i].type != TC_TYPE_NONE) {
      char type = (triggerCond[i].type == TC_TYPE_DATA ? 'd' : 'a');
      char logic = (triggerCond[i].logic == TC_LOGIC_ASSIGN ? '='
        : (triggerCond[i].logic == TC_LOGIC_AND ? '*' : '+'));
      char neg = (triggerCond[i].neg ? '!' : ' ');
      char rw = (triggerCond[i].rw == TC_RW_READ ? 'r' :
        (triggerCond[i].rw == TC_RW_WRITE ? 'w' : 'x'));
      if (triggerCond[i].type == TC_TYPE_DATA) {
        sprintf(str, "%d: %c%c %c %04X %04X %c", i, type, neg, rw, triggerCond[i].dataFrom, triggerCond[i].dataTo, logic);
      }
      else {
        sprintf(str, "%d: %c%c %c %04X %04X %c", i, type, neg, rw, triggerCond[i].addrFrom, triggerCond[i].addrTo, logic);
      }
      Serial.println(str);
    }
  }
}

bool addTriggerCond(const char *fmt) {
  bool done = false;
  char type = '?';
  char neg = '?';
  char rw = '?';
  uint16_t rnfrom, rnto;
  char logic = '?';
  //printf("-- -- Adding from:'%s'\n", fmt);
  sscanf(fmt, "%*s %c%c %c %hx %hx %c", &type, &neg, &rw, &rnfrom, &rnto, &logic);
  //printf("-- -- t:'%c'n:'%c' rw:'%c' rf:'%hx' rt:'%hx' l:'%c'\n",
  //  type, neg, rw, rnfrom, rnto, logic);
  if (type == '0') {
    Serial.println("Reset all triggers.");
    resetTriggerCond();
  }
  else {
    if (logic == '?') {
      Serial.println("Reset all triggers.");
      resetTriggerCond();
    }
    bool done = false;
    for (int i = 0; (i < TRIGGERCOUNT) && !done; i++) {
      if (triggerCond[i].type == TC_TYPE_NONE) {
        if (type == 'd') {
          //Serial_println("Type data.");
          triggerCond[i].type = TC_TYPE_DATA;
        }
        else if (type == 'a') {
          //Serial_println("Type address.");
          triggerCond[i].type = TC_TYPE_ADDRESS;
        }
        triggerCond[i].addrFrom = rnfrom;
        triggerCond[i].addrTo = rnto;
        triggerCond[i].neg = (neg == '!');
        triggerCond[i].rw = (rw == 'r' ? TC_RW_READ :
          (rw == 'w' ? TC_RW_WRITE : TC_RW_DONTCARE));
        triggerCond[i].logic = (logic == '*' ? TC_LOGIC_AND :
          (logic == '+' ? TC_LOGIC_OR : TC_LOGIC_ASSIGN));
        done = true;
        //Serial_println("Added trigger condition.");
        printTriggerCond();
      }
    }
  }
  return done;
}

bool trigger(uint8_t rw, uint16_t addr, uint8_t data) {
  bool cond = false;
  for (int i = 0; i < TRIGGERCOUNT; i++) {
    if (triggerCond[i].type != TC_TYPE_NONE) {
      if (triggerCond[i].logic == TC_LOGIC_ASSIGN) {
        cond = testTriggerCond(rw, addr, data, triggerCond[i]);
      }
      else if (triggerCond[i].logic == TC_LOGIC_OR) {
        cond = cond || testTriggerCond(rw, addr, data, triggerCond[i]);
      }
      else if (triggerCond[i].logic == TC_LOGIC_AND) {
        cond = cond && testTriggerCond(rw, addr, data, triggerCond[i]);
      }
    }
  }
  return cond;
}

#endif
