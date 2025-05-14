#include "keyb.h"
#include "Functions.h"

//#include <stdio.h>
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

/* ###########################################################################
*  Functions
*  ########################################################################### */
/*! \fn      static long get_time()
*   \brief   Get time in milliseconds
*   \return  time in msec */ 
long get_time()
{
    long time_ms;
#ifdef _WIN32
    struct _timeb timebuffer;
    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
    return time_ms;
}

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      int DataConsistencyCheck(uint32_t *buff32, int NumWords)
*   \brief   Do some data consistency check
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */
int DataConsistencyCheck(uint32_t *buff32, int NumWords)
{
    int pnt = 0;
    uint32_t EventSize;

    if (NumWords == 0)
        return 0;

    // Check for events integrity
    do {
        EventSize = buff32[pnt] & 0x0FFFFFFF;
        pnt += EventSize;  // Jump to next event
    } while (pnt < NumWords);
    if (pnt != NumWords) {
        printf("Data Error: Event truncation\n");
        return -1;
    }
    return 0;
}

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      PrintInterface()
*   \brief   Print the interface to screen
*   \return  none */
/* --------------------------------------------------------------------------------------------------------- */
void PrintInterface() {
	printf("\ns ) Start acquisition\n");
	printf("S ) Stop acquisition\n");
	printf("r ) Restart acquisition\n");
	printf("T ) Read ADC temperature\n");
	printf("C ) Start ADC calibration\n");
	printf("q ) Quit\n");
	printf("t ) Send a software trigger\n");
}

uint16_t reverseBytes16(uint16_t v){
    return ((v & 0xFF) << 8 | (v & 0xFF00) >> 8);
}