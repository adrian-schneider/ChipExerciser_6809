#ifndef SDCARD_H
#define SDCARD_H

#include "src/SD/SD.h"

const int chipSelect = 10;
Sd2Card card;
SdVolume volume;
SdFile root;

void initSD() {
  static bool inited = false;
  if (!inited) {
    if (SD.begin(chipSelect)) {
      Serial.println("SD card found.");
      inited = true;
    }
    else {
      Serial.println("Card failed, or not present");
    }
  }
}

bool getCardInfo() {
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed.");
    return false;
  }
  else {
    Serial.println("Wiring is correct and a card is present.");

    Serial.println();
    Serial.print("Card type:         ");
    switch (card.type()) {
      case SD_CARD_TYPE_SD1:
        Serial.println("SD1");
        break;
      case SD_CARD_TYPE_SD2:
        Serial.println("SD2");
        break;
      case SD_CARD_TYPE_SDHC:
        Serial.println("SDHC");
        break;
      default:
        Serial.println("Unknown");
    }

    // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
    if (!volume.init(card)) {
      Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
      while (1);
    }

    Serial.print("Clusters:          ");
    Serial.println(volume.clusterCount());
    Serial.print("Blocks x Cluster:  ");
    Serial.println(volume.blocksPerCluster());
  
    Serial.print("Total Blocks:      ");
    Serial.println(volume.blocksPerCluster() * volume.clusterCount());
    Serial.println();
  
    // print the type and size of the first FAT-type volume
    uint32_t volumesize;
    Serial.print("Volume type is:    FAT");
    Serial.println(volume.fatType(), DEC);
  
    volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
    volumesize *= volume.clusterCount();       // we'll have a lot of clusters
    volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
    Serial.print("Volume size (Kb):  ");
    Serial.println(volumesize);
    Serial.print("Volume size (Mb):  ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Gb):  ");
    Serial.println((float)volumesize / 1024.0);
  
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);
  
    // list all files in the card with date and size
    root.ls(LS_R | LS_DATE | LS_SIZE);
    root.close();
    return true;
  }
}

void printDirectory(File dir, int numTabs) {
  initSD();
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      //printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

bool readFileToRAM(const char *inFileName) {
  initSD();
  bool done = false;
  File inFile = SD.open(inFileName);
  if (inFile) {
    memset(ram, 0, sizeof(ram));
    int count = 0;
    while(inFile.available() && (count < sizeof(ram))) {
      ram[count++] = inFile.read();
    }
    inFile.close();
    done = true;
  }
  else {
    Serial.println("Failed opening file for reading.");
  }
  return done;
}

#endif
