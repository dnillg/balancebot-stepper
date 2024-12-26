#include "RobotDisplay.hpp"

RobotDisplay* singletonDisplay;

RobotDisplay::RobotDisplay(TFT_eSPI &tft) : tft(tft)
{
    singletonDisplay = this;
    gif.begin(LITTLE_ENDIAN_PIXELS);
}

void RobotDisplay::playGif(const char *gifPath)
{
  Serial.println("Opening GIF...");
  // Open the GIF file
  if (!gif.open(gifPath, gifOpen, gifClose, fileRead, gifSeek, gifDraw))
  {
    Serial.println("Failed to open GIF file!");
    while (1)
      ;
  }
}

bool RobotDisplay::update()
{
  if (gif.playFrame(true, NULL) == 0)
  {
    gif.reset();
    return true;
  }
  return false;
}

// ----------------------------------------------------------------------------
// Callback functions for AnimatedGIF library
// ----------------------------------------------------------------------------

int RobotDisplay::gifSeek(GIFFILE *pFile, int iPosition)
{
  if (iPosition < 0)
    iPosition = 0;
  else if (iPosition >= pFile->iSize)
    iPosition = pFile->iSize - 1;
  pFile->iPos = iPosition;
  ((File*)pFile)->seek(pFile->iPos);
  return iPosition;
}

void* RobotDisplay::gifOpen(const char *szFilename, int32_t *pFileSize)
{
  Serial.println(szFilename);
  singletonDisplay->gifFile = SD.open(szFilename);
  *pFileSize = singletonDisplay->gifFile.size();
  Serial.println("File opened... " + String(*pFileSize));
  return &(singletonDisplay->gifFile);
}

void RobotDisplay::gifClose(void *pFile)
{
  singletonDisplay->gifFile.close();
}

int32_t RobotDisplay::fileRead(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
  int32_t iBytesRead;
  iBytesRead = iLen;
  if ((pFile->iSize - pFile->iPos) < iLen)
    iBytesRead = pFile->iSize - pFile->iPos;
  if (iBytesRead <= 0)
    return 0;

  singletonDisplay->gifFile.seek(pFile->iPos);
  int32_t bytesRead = singletonDisplay->gifFile.read(pBuf, iLen);
  pFile->iPos += iBytesRead;

  return bytesRead;
}

void RobotDisplay::gifDraw(GIFDRAW *pDraw)
{
  uint8_t *s;
  uint16_t *d, *usPalette;
  int x, y, iWidth, iCount;

  // Display bounds chech and cropping
  iWidth = pDraw->iWidth;
  if (iWidth + pDraw->iX > DISPLAY_WIDTH)
    iWidth = DISPLAY_WIDTH - pDraw->iX;
  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y; // current line
  if (y >= DISPLAY_HEIGHT || pDraw->iX >= DISPLAY_WIDTH || iWidth < 1)
    return;

  // Old image disposal
  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) // restore to background color
  {
    for (x = 0; x < iWidth; x++)
    {
      if (s[x] == pDraw->ucTransparent)
        s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }

  // Apply the new pixels to the main image
  if (pDraw->ucHasTransparency) // if transparency used
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    pEnd = s + iWidth;
    x = 0;
    iCount = 0; // count non-transparent pixels
    while (x < iWidth)
    {
      c = ucTransparent - 1;
      d = &singletonDisplay->usTemp[0][0];
      while (c != ucTransparent && s < pEnd && iCount < BUFFER_SIZE )
      {
        c = *s++;
        if (c == ucTransparent) // done, stop
        {
          s--; // back up to treat it like transparent
        }
        else // opaque
        {
          *d++ = usPalette[c];
          iCount++;
        }
      } // while looking for opaque pixels
      if (iCount) // any opaque pixels?
      {
        // DMA would degrtade performance here due to short line segments
        singletonDisplay->tft.startWrite();
        singletonDisplay->tft.setAddrWindow(pDraw->iX + x, y, iCount, 1);
        singletonDisplay->tft.pushPixels(singletonDisplay->usTemp, iCount);
        singletonDisplay->tft.endWrite();
        x += iCount;
        iCount = 0;
      }
      // no, look for a run of transparent pixels
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
          x++;
        else
          s--;
      }
    }
  }
  else
  {
    s = pDraw->pPixels;

    // Unroll the first pass to boost DMA performance
    // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    if (iWidth <= BUFFER_SIZE)
      for (iCount = 0; iCount < iWidth; iCount++) singletonDisplay->usTemp[DMA_BUF][iCount] = usPalette[*s++];
    else
      for (iCount = 0; iCount < BUFFER_SIZE; iCount++) singletonDisplay->usTemp[DMA_BUF][iCount] = usPalette[*s++];

#ifdef USE_DMA // 71.6 fps (ST7796 84.5 fps)
    tft.dmaWait();
    tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
    tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
    dmaBuf = !dmaBuf;
#else // 57.0 fps
    singletonDisplay->tft.startWrite();
    singletonDisplay->tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
    singletonDisplay->tft.pushPixels(&singletonDisplay->usTemp[0][0], iCount);
    singletonDisplay->tft.endWrite();
#endif

    iWidth -= iCount;
    // Loop if pixel buffer smaller than width
    while (iWidth > 0)
    {
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      if (iWidth <= BUFFER_SIZE)
        for (iCount = 0; iCount < iWidth; iCount++) singletonDisplay->usTemp[DMA_BUF][iCount] = usPalette[*s++];
      else
        for (iCount = 0; iCount < BUFFER_SIZE; iCount++) singletonDisplay->usTemp[DMA_BUF][iCount] = usPalette[*s++];

#ifdef USE_DMA
      tft.dmaWait();
      tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
      dmaBuf = !dmaBuf;
#else
      singletonDisplay->tft.startWrite();
      singletonDisplay->tft.pushPixels(&singletonDisplay->usTemp[0][0], iCount);
      singletonDisplay->tft.endWrite();
#endif
      iWidth -= iCount;
    }
  }

  taskYIELD();
}