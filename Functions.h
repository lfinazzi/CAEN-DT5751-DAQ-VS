#ifndef READOUT_TEST_FUNCTIONS
#define READOUT_TEST_FUNCTIONS

#include "keyb.h"
#include "CAENDigitizerType.h"

#include <stdio.h>
#ifdef _WIN32

    #include <time.h>
    #include <sys/timeb.h>
    #include <conio.h>
    #include <process.h>
	#define getch _getch     /* redefine POSIX 'deprecated' */
	#define kbhit _kbhit     /* redefine POSIX 'deprecated' */

#else
    #include <unistd.h>
    #include <stdint.h>   /* C99 compliant compilers: uint64_t */
    #include <ctype.h>    /* toupper() */
    #include <sys/time.h>
#endif

#ifndef MaxNChannels
#define MaxNChannels 4 
#endif

typedef struct
{
    CAEN_DGTZ_ConnectionType LinkType;
    uint32_t VMEBaseAddress;
	uint32_t RecordLength;
	uint32_t ChannelMask;
    int EventAggr;
    CAEN_DGTZ_PulsePolarity_t PulsePolarity;
    CAEN_DGTZ_DPP_AcqMode_t AcqMode;
    CAEN_DGTZ_IOLevel_t IOlev;
    uint16_t DCOffset;
    uint8_t PreTriggerSize;
    uint8_t CoincMode;
} DigitizerParams_t;


/* ###########################################################################
*  Functions
*  ########################################################################### */

/*! \fn      static long get_time()
*   \brief   Get time in milliseconds
*   \return  time in msec */ 
long get_time();

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      int DataConsistencyCheck(uint32_t *buff32, int NumWords)
*   \brief   Do some data consistency check
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */
int DataConsistencyCheck(uint32_t *buff32, int NumWords);

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      PrintInterface()
*   \brief   Print the interface to screen
*   \return  none */
/* --------------------------------------------------------------------------------------------------------- */
void PrintInterface();

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      reverseBytes16(uint16_t v)
*   \brief   Transforms big endian to little endian
*   \return  uint16_t */
/* --------------------------------------------------------------------------------------------------------- */
uint16_t reverseBytes16(uint16_t v);

#endif