#ifndef DFCONFIG_H_
#define DFCONFIG_H_

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

typedef DFMiniMp3<EspSoftwareSerial::UART, DfMp3NoCallback> DfMp3;

#endif // DFMINIMP3_H_