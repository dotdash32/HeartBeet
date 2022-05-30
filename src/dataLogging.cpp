/* dataLogging.cpp
 * 
 * Description: Module for logging HRV data as subject-specific CSV file on a microSD card.
 */
#include "dataLogging.h"
#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN 4 // chip-select line for SD card - pin 4

// VARIABLES
File outfile; // CSV file to write to
String headers = "SubjectID,Time (ms),HRV (ms)";
bool logging = false; // True if we should currently be logging data
int subjectID = 0; // Keeps track of the current subject ID
unsigned long t0 = 0; // Marks the start time of the subject's recording

// Private function prototypes
String formatFilename(int id);

// FUNCTION DEFINITIONS
bool setupSD(void) {
  bool returnVal = true;

  Serial.print("Initializing SD... ");
  // Initialize SD module for data logging
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD initialization failed!");
    returnVal = false;
  } else {
    Serial.println("done.");
  }
  
  return returnVal;
}

bool startLogging(void) {
  bool returnVal = false;

  // Check if we are logging, tell user
  if (logging) {
    Serial.println("Already logging.");

  } else {
    String filename = formatFilename(subjectID); // ensure 8.3 filename
    outfile = SD.open(filename, FILE_WRITE);

    // Verify the file is open
    if (outfile) {
      Serial.println("File " + filename + " ready");
    } else {
      Serial.println("Error opening " + filename);
    }

    // Add header line
    outfile.println(headers);

    logging = true;
    t0 = millis(); // Mark start of session
    
    Serial.println("Logging started.");
    returnVal = true;
  }

  return returnVal;
}

bool stopLogging(void) {
  bool returnVal = false;
  
  if (!logging) {
    Serial.println("Not logging; nothing to stop.");
    
  } else {
    Serial.println("Closing " + String(outfile.name()));
    outfile.close();
    logging = false;
    
    Serial.println("Logging stopped.");
    subjectID++; // Increment for next subject file
    returnVal = true;
  }
  
  return returnVal;
}

bool logData(unsigned long t, float HRV) {
  Serial.print("Writing... ");
  bool returnVal = false;

  // Ensure that the output file exists before writing
  if (outfile) {
    outfile.print(subjectID);
    outfile.print(",");
    outfile.print(t - t0); // Store the relative time in session
    outfile.print(",");
    outfile.println(HRV);

    returnVal = true;
  }
  Serial.println("done.");
  
  return returnVal;
}

void printDirectory(File dir, int numTabs) {
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
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    
    entry.close();
  }
}

void deleteDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    
    Serial.println("Removing " + String(entry.name()));
    
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    
    entry.close();
    SD.remove(entry.name());
  }
}

void readDirectory(File dir, int numTabs) {
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
      printDirectory(entry, numTabs + 1);
    } else {
      // print contents of file
      Serial.println("");
      while (entry.available()) {
        Serial.write(entry.read());
      }
    }
    Serial.println("--- EOF ---");
    entry.close();
  }
}

bool isLogging(void) {
  return logging;
}

// PRIVATE FUNCTIONS
String formatFilename(int id) {
  String idStr = String(id);
  String filename = "";
  
  for (int i=0; i < (8 - idStr.length()); i++) {
    filename += "0";
  }
  filename += idStr;
  filename += ".csv";

  return filename;
}