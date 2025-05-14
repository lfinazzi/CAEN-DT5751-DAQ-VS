#include "CAENDigitizerType.h"
#include "CAENDigitizer.h"
#include "Functions.h"

#ifndef MaxNChannels
#define MaxNChannels 4 
#endif

//#include <cstdio>

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      LoadConfigFile(FILE* f_ini, CAEN_DGTZ_Params_t *Params, CAEN_DGTZ_DPP_PSD_Params_t *DPPParams)
*   \brief   Load configuration file settings to Parameter structures for DGTZ configuration
*   \return  none
/* --------------------------------------------------------------------------------------------------------- */
void LoadConfigFile(FILE* f_ini, DigitizerParams_t *Params, CAEN_DGTZ_DPP_PSD_Params_t *DPPParams);

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPPParamsPHA_t DPPParams)
*   \brief   Program the registers of the digitizer with the relevant parameters
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */
int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PSD_Params_t DPPParams);