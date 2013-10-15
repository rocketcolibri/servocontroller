/**
 * @addtogroup _DBG
 *
 * The module contains the HIstory File (HIF) handling. There
 * are functions to make HIF entries, to display the HIF list
 * and to send the HIF table to the management.
 *
 * _DBG0 contains the interface and the implementation of the
 * functionality as well.
 *
 * @short Debug support functions
 *
 * @{
 */
#ifndef PPG_DBG0_H
#define PPG_DBG0_H


#include "GEN.h"


/******************************************************************************
 * Constants
 *****************************************************************************/
// Reset codes
#define DBG_NO_RESET    0
#define DBG_RESET       1
#define DBG_LOG         2

// Error Codes
#define DBG_ERR0        0
#define DBG_ERR1        1
#define DBG_ERR2        2
#define DBG_ERR3        3
#define DBG_ERR4        4
#define DBG_ERR5        5
#define DBG_ERR6        6
#define DBG_ERR7        7


/******************************************************************************
 * Macros
 *****************************************************************************/

/**
 * Makes an entry in the debug log
 */
#define DBG_MAKE_ENTRY(reset)\
 DBG_ErrorHandler(reset, DBG_ERR1, __FILE__, __LINE__,"-")

/**
 * Checks for condition and resets, if false
 */
#define DBG_ASSERT(cond) do if(!(cond)) {DBG_MAKE_ENTRY(DBG_RESET); } while(0)

/**
 * Checks for condition and complains, if false
 */
#define DBG_ASSERT_NO_RES(cond) do if(!(cond)) {DBG_MAKE_ENTRY(DBG_NO_RESET); } while(0)

/**
 * make entry but without Error_Code (err = 0) and with message string added to HIF entry
 */
#define DBG_MAKE_ENTRY_MSG(reset,msg)\
 DBG_ErrorHandler(reset, DBG_ERR0, __FILE__, __LINE__, msg)

/**
 * make a HIF entry with message using a format string to compose the message
 */
#define DBG_MAKE_ENTRY_FMT(reset, fmt...) { char tmp[128]; \
                                        snprintf (tmp, sizeof(tmp), ##fmt); tmp[127]=0; \
                                        DBG_ErrorHandler(reset, DBG_ERR0, __FILE__, __LINE__, tmp); }

/**
 * log important message.
 */
#define DBG_LOG_ENTRY(msg)\
	DBG_ErrorHandler(DBG_LOG, DBG_ERR0, __FILE__, __LINE__, msg)

/**
 * make a LOG entry with message using a format string to compose the message
 */
#define DBG_LOG_ENTRY_FMT( fmt...) { char tmp[128]; \
                                        snprintf (tmp, sizeof(tmp), ##fmt); tmp[127]=0; \
                                        DBG_ErrorHandler(DBG_LOG, DBG_ERR0, __FILE__, __LINE__, tmp); }

/******************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @short Initialization of Debug module
 *
 * Call this function from the initialization task only.
 *
 * @param  phase: phase A, B or C
 */
extern void DBG_Init();

/**
 * @short Error handler for HIFs
 *
 * Error handler to make entries into the HIF list.
 *
 * @param  reset: 0 no reset, 1 reset, 2 log
 * @param  err_code: error code to be passed and displayed
 * @param  file: filename where error occurred
 * @param  line: line number where error occurred
 * @param  msg: error message to be displayed
 */
extern void DBG_ErrorHandler(UINT8 reset, UINT8 err_code, char *file, UINT16 line,
                      char *msg);
/**
 * @short Performs a controlled reset
 *
 * Switches to BANK1, where the BOOT module is resident and
 * jumps to the reset vector. The tricky part is to set the
 * bank switch to BANK1 without causing a crash, therefore
 * the code is copied into RAM and executed from RAM. All
 * interrupts are disabled during reboot.
 *
 */
extern void DBG_Reset(void);

/**
 * Print backtrace to "/nvd/goulf.backtrace"
 */
extern void DBG_PrintTraceToFile(void);

/** @ } */
#endif
