/*
 * TRC_File.c
 *
 *  Created on: 13.10.2011
 *      Author: schelor0
 */

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "GEN.h"
#include "DBG.h"
#include "TRC.h"


/**
 * @short Create a new Trace File
 *
 * @param pFilename
 * @param startLogging, TRUE if logging start, FALSE if not, can be started later with TRC_File_SetLogging
 * @return TRC_File handle
 */
TRC_File_t* TRC_File_Create(const char *pFilename, BOOL startLogging)
{
  TRC_File_t *fileTrace = malloc(sizeof(TRC_File_t));
  DBG_ASSERT(fileTrace);

  fileTrace->doLog=startLogging;
  fileTrace->file= fopen(pFilename, "w");
  fileTrace->pFilename=pFilename;

  return fileTrace;
}

/**
 * @short set logging on/off
 *
 * suspend/resume logging
 *
 * @param fileTrc TRC_File handle
 * @param logging
 */
void TRC_File_SetLogging(TRC_File_t* fileTrc, BOOL logging)
{
  DBG_ASSERT(fileTrc);
  fileTrc->doLog=logging;
}

/**
 * @short End logging
 * @param fileTrc
 */
void TRC_File_Close(TRC_File_t* fileTrc)
{
  DBG_ASSERT(fileTrc);
  fclose(fileTrc->file);
  free(fileTrc);
}

/**
 * @short LOG Messag to the trace file
 *
 * @param fileTrc
 * @param fmt
 */
void TRC_File_Print(TRC_File_t* fileTrc, const char *fmt, ...)
{
  if(fileTrc)
  {
    va_list args;

    if(fileTrc->doLog)
    {
      if(fileTrc->file)
      {
        // --- Variable Argumentenliste an vsprintf weitergeben
        va_start(args, fmt);
        vfprintf(fileTrc->file, fmt, args);
        va_end(args);
        fflush(fileTrc->file);
      }
    }
  }
}

/**
 * Macro that can be used to ease the use of TRC_File_Print
 */
#define TRC_FILE_PRINT(trc, c...) {TRC_File_Print(trc, ##c);}

