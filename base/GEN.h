/**
 * @addtogroup GEN
 *
 * @short Module contains the basic functions, types and definitions of the GOULF software
 *
 * This part are the generic types and functions used by all modules.
 *
 * This module contains functions to get information from the NE list, or to make your own list of
 * NEs with certain properties using the query functions.
 *
 * Functions to calculate a UID-hash value from a UID-path has been added to the
 * module since the Ulaf+ version 6.0.
 *
 * @{
 *
 * @par MAP Purpose
 * A Map is a sorted associative container that associates objects of type Key
 * with objects of type Data. It is also a unique associative container,
 * meaning that no two elements have the same key.
 * @par
 * This module implements a map of pointers.
 * Hence, each map element is a pair of Key/pData.
 * @par
 * In a real application, a map's data is divided into two parts. The
 * 'administrative' part is mainly built by the key/pData-pairs described above.
 * The 'operative' part is a memory aera allocated statically by the user which
 * holds the data administrated by the map.
 * @par
 * With the data passed on creation, a map instance overlays an array of elements
 * over the 'operative' memory aera evaluating a pointer to the base address of
 * each element. This pointers are stored in the pData fields of the map.
 * Insertion is done by storing the key passed by the insertion function to the
 * next free map element's key field. This binds the data aera referenced by the
 * pData pointer of this element to the passed key.
 * A find call for such a Key provides access to the data associated to that key
 * by returning the pData pointer of the found map element.
 * @par
 * To avoid the usage of dynamic memory allocation, the number of map
 * instances and the number of members a map can have are limited to values
 * defined in the CONFIG.H header of a board's firmware. A memory pool will be
 * used to manage the 'administrative' part of maps which allows dynamic creation
 * and deletion of maps at runtime.
 * @par
 * Maps ar task-save i.e. access is semaphore protected.
 *
 * @par MAP Usage
 * The following is an example usage of a Map:
 * @code
 * #include "PPGGEN0.H"
 *
 * // User data, 'operative' part of map data
 * typedef struct
 * {
 *    // ...
 * } USR_NeData_t ;
 *
 * USR_NeData_t USR_NeData[GEN_NOF_NE] ;
 *
 * // Map creation
 * GEN_MapHandle_t hMap = GEN_MapCreate(
 *                        USR_NeData,
 *                        sizeof( USR_NeData ),
 *                        sizeof( USR_NeData_t ),
 *                        GEN_NOF_NE
 *                     ) ;
 *
 * // type wrapper functions, may be replaced by macros
 * USR_NeData_t* USR_FindNeData( UINT16 key, UINT16* const pIndex )
 * {
 *    return (USR_NeData_t*)GEN_MapFind( hMap, key, pIndex ) ;
 * }
 *
 * USR_NeData_t* USR_GetFirst( UINT16* const pIndex )
 * {
 *    return (USR_NeData_t*)GEN_MapGetFirst( hMap, pIndex ) ;
 * }
 *
 * USR_NeData_t* USR_GetNext( UINT16* const pIndex )
 * {
 *    return (USR_NeData_t*)GEN_MapGetNext( hMap, pIndex ) ;
 * }
 *
 * // fill up the map
 * void USR_Init( GEN_InitPhase phase )
 * {
 *    switch (phase)
 *    {
 *       case GEN_INIT_A:
 *          // ...
 *          for (i = 0; i < GEN_NOF_NE; i++)
 *          {
 *             GEN_NeHandle_t key = GEN_GetNeHandleFromIndex( i ) ;
 *             GEN_MapInsert( hMap, key ) ;
 *          }
 *          // ...
 *          break ;
 *       case GEN_INIT_B: ...; break;
 *       case GEN_INIT_C: ...; break;
 *       case GEN_INIT_DONE: ...; break;
 *       case GEN_INIT_STOP: ...; break;
 *       default:
 *          DBG_MAKE_ENTRY( DBG_RESET ) ;
 *          break;
 *     }
 * }
 *
 *
 * // Access the map via keys
 * TGRM_Tec_t USR_TgHandler( GEN_NeHandle_t hNe,
 *                           TGRM_GenericTG_t* inMsg,
 *                           TGRM_GenericTG_t* outMsg )
 * {
 *    UINT16 index ;
 *    // ...
 *    USR_NeData_t* pNeData = USR_FindNeData( hNe, &index ) ;
 *    // ...
 * }
 *
 *
 * // Iterate the map
 * void USR_ueberAlleNes()
 * {
 *    UINT16 index ;
 *    USR_NeData_t* pNeData = USR_GetFirst( &index ) ;
 *    while (pNeData != NULL)
 *    {
 *       // ...
 *       pNeData = USR_GetNext( &index ) ;
 *    } ;
 * }
 * @endcode
 */

#ifndef PPG_GEN0_H
#define PPG_GEN0_H

/** Boolean */
typedef unsigned char BOOL;

typedef unsigned int 	UINT;	/**> INTEGER unsigned (platform specific)*/
typedef  int 			INT;	/**> INTEGER signed (platform specific)*/
typedef unsigned char 	UINT8;	/**> Unsigned INTEGER 8 bits */
typedef unsigned short  UINT16; /**> Unsigned INTEGER 16 bits */
typedef unsigned int 	UINT32; /**> Unsigned INTEGER 32 bits */
typedef unsigned long long int  UINT64; /**> Unsigned INTEGER 64 bits */
typedef signed char   	SINT8; 	/**> Signed INTEGER 8 bits */
typedef signed short  	SINT16; /**> Signed INTEGER 16 bits */
typedef signed int   	SINT32; /**> Signed INTEGER 32 bits */
typedef signed char   	SCHAR; 	/**> Signed character */



// General constants
#undef TRUE
#define TRUE            1
#undef FALSE
#define FALSE           0

// & masks for the nibble extraction from a byte
#define LOW_NIBBLE      0x0F
#define HIGH_NIBBLE     (unsigned char)0xF0
#define NIBBLE_BIT_LEN  4

// & masks for the byte extraction from a word
#define LOW_BYTE        0x00FF
#define HIGH_BYTE       (unsigned int)0xFF00
#define BYTE_BIT_LEN    8

/**
 * Bitmask up to 16-bit values
 */
#define BIT0            0x0001
#define BIT1            0x0002
#define BIT2            0x0004
#define BIT3            0x0008
#define BIT4            0x0010
#define BIT5            0x0020
#define BIT6            0x0040
#define BIT7            0x0080
#define BIT8            0x0100
#define BIT9            0x0200
#define BIT10           0x0400
#define BIT11           0x0800
#define BIT12           0x1000
#define BIT13           0x2000
#define BIT14           0x4000
#define BIT15           0x8000

/**
 * Definitions for unknown values for L7 message fields and HWCF entries
 */
#define UNK8S           (-128)              /**< signed 8 bit */
#define UNK8U           0xFF                /**< unsigned 8 bit */
#define UNK16S          (-32768)            /**< signed 16 bit */
#define UNK16U          0xFFFF              /**< unsigned 16 bit */
#define UNK32S          (-2147483647-1)     /**< signed 32 bit */
#define UNK32U          0xFFFFFFFF          /**< unsigned 32 bit */
#define UNK64U          0xFFFFFFFFFFFFFFFFULL  /**< unsigned 64 bit */

#ifndef NULL
  #define NULL ((void*)0)
#endif

#define LOG_EXOR(a,b) (((a==0)&&(b!=0))||((a!=0)&&(b==0)))
extern UINT32 GEN_GetRTC(void);
#endif
