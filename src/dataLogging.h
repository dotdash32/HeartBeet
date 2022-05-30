/* dataLogging.h
 * 
 * Description: Header file for dataLogging module which stores HRV data as a CSV file on a microSD card.
 */

#pragma once

#include <SPI.h>
#include <SD.h>

// Logging
bool startLogging(void);
bool stopLogging(void);
bool logData(unsigned long t, float HRV);
bool isLogging(void);

// SD Management
bool setupSD(void);
void printDirectory(File dir, int numTabs);
void readDirectory(File dir, int numTabs);
void deleteDirectory(File dir, int numTabs);
