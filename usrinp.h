#ifndef USRINP_H
#define USRINP_H

#define MAX_COUNT 256

char logstr[64];
int count = MAX_COUNT;
int maxCount = MAX_COUNT;
uint8_t outOnEdgeE = 1;

void printHelp() {
  Serial.println("?    Show this help text.");
  Serial.println(".    Output another set of data.");
  Serial.println(". n  Set output count to 0<=n<=32000");
  Serial.println("d    Dump the RAM.");
  Serial.println("l ?  List the SD card root directory.");
  Serial.println("l 0  Initialize the RAM with NOP.");
  Serial.println("l n  Load program named n into RAM.");
  Serial.println("r    Put CPU in reset state.");
  Serial.println("R    Release CPU from reset state.");
  Serial.println("*    Output at each clock cycle");
  Serial.println("H    Output at raising edge of E.");
  Serial.println("L    Output at falling edge of E.");
  Serial.println("/    Output at any edge of E.");
  Serial.println("tr   Show stored trigger conditions.");
  Serial.println("tr 0 Reset trigger conditions.");
  Serial.println("tr a|d[!] r|w|x 00[00] 00[00] [+|*]");
  Serial.println("     type: a address, d data, ! invert;");
  Serial.println("     access: r read, w write, x don't care;");
  Serial.println("     range: 00[00] from, to;");
  Serial.println("     logic: + or, * and, omit to reset and assign.");
}

bool parseIntValue(const char* str, const char* fmt, int* pvalue, int goodFrom, int goodTo, const char* errstr) {
  bool done = false;
  sscanf(str, fmt, pvalue);
  if ((*pvalue >= goodFrom) && (*pvalue <= goodTo)) {
    done = true;
  }
  else {
    Serial.println(errstr ? errstr : "Value ignored");
  }
  return done;
}

void processUserInput() {
  static String input = "";

  { // Read and buffer charcters as they arrive, until EOL.
    static bool clearinput = false;
    static String bufr = "";
    if (clearinput) {
      bufr = "";
      clearinput = false;
    }
    input = "";
    while (Serial.available()) {
      char c = Serial.read();
      // Anything less than space counts as EOL.
      if (c >= ' ') {
        bufr += c;
      }
      else {
        input = bufr;
        input.trim();
        // On the next call, the input has been processed and
        // the buffer can be cleared.
        clearinput = true;
      }
    }
  }

  if (input != "") {
    sprintf(logstr, "> %s", input.c_str());
    Serial.println(logstr);

    if (input == ".") {
      count = 0;
    }

    else if (input.startsWith(". ")) {
      int value = MAX_COUNT;
      if (parseIntValue(input.c_str(), "%*c %d", &value, 0, 32000, NULL)) {
        sprintf(logstr, "Output count: %d", value);
        Serial.println(logstr);
        maxCount = value;
        count = maxCount;
      }
    }

    else if (input == "?") {
      printHelp();
    }
    else if (input == "d") {
      dumpRam();
    }
    else if (input.startsWith("l ")) {
      char strFileName[13]; // 8.3 file name + 0
      sscanf(input.c_str(), "%*c %12s", strFileName); // 8.3 file name
      if (strcmp(strFileName, "?") == 0) {
        printDirectory(SD.open("/"), 0);
      }
      else if (strcmp(strFileName, "0") == 0) {
        initRam();
      }
      else {
        sprintf(logstr, "Loading '%s'.", strFileName);
        Serial.println(logstr);
        if (!readFileToRAM(strFileName)) {
          sprintf(logstr, "Failed to open '%s'.", strFileName);
          Serial.println(logstr);
        }
      }
    }

    else if (input == "r") {
      resetLo();
    }
    else if (input == "R") {
      resetHi();
      count = 0;
    }

    else if (input == "*") {
      outOnEdgeE = 0;
    }
    else if (input == "H") {
      outOnEdgeE = 1;
    }
    else if (input == "L") {
      outOnEdgeE = 2;
    }
    else if (input == "/") {
      outOnEdgeE = 3;
    }

    else if (input == "tr") {
      printTriggerCond();
    }
    else if (input.startsWith("tr ")) {
      addTriggerCond(input.c_str());
    }

    else {
      sprintf(logstr, "Ignored '%s'", input.c_str());
      Serial.println(logstr);
    }
  }
}

#endif
