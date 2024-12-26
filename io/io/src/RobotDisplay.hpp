#ifndef ROBOTDISPLAY_HPP_
#define ROBOTDISPLAY_HPP_

#include <Arduino.h>
#include "config.h"
#include <TFT_eSPI.h>
#include <AnimatedGIF.h>
#include <SD.h>

#define DMA_BUF 0

class RobotDisplay
{
public:
  RobotDisplay(TFT_eSPI &tft);
  void playGif(const char *gifPath);
  bool update();

  static int gifSeek(GIFFILE *pFile, int iPosition);
  static void* gifOpen(const char *szFilename, int32_t *pFileSize);
  static void gifClose(void *pFile);
  static int32_t fileRead(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
  static void gifDraw(GIFDRAW *pDraw);
  uint16_t usTemp[1][BUFFER_SIZE];
private:
  TFT_eSPI &tft;
  File gifFile;
  AnimatedGIF gif;
};

#endif // ROBOTDISPLAY_HPP_