#include <CAENDigitizer.h>
#include <CAENComm.h>

#include <stdio.h>
#include <stdlib.h>

//#define MANUAL_BUFFER_SETTING   0
// The following define must be set to the actual number of connected boards
#define MAXNB 1

// NB: the following define MUST specify the ACTUAL max allowed number of board's channels
// it is needed for consistency inside the CAENDigitizer's functions used to allocate the memory
#define MaxNChannels 4

// The following define indicates the maximum number of events on a savefile
#define MAXFILEEVENTS 1500000

// Maximum number of files before ending acquisition. Total events = MAXFILEEVENTS * (MAXFILEINDEX + 1)
#define MAXFILEINDEX 0

// config file location
#define DEFAULT_CONFIG_FILE "./cfg.txt"

/* include some useful functions from file Functions.h
you can find this file in the src directory */
#include "Functions.h"
#include "cfg.h"

/* ########################################################################### */
/* MAIN                                                                        */
/* ########################################################################### */
int main(int argc, char* argv[])
{
    /* The following variable is the type returned from most of CAENDigitizer
    library functions and is used to check if there was an error in function
    execution. For example:
    ret = CAEN_DGTZ_some_function(some_args);
    if(ret) printf("Some error"); */
    CAEN_DGTZ_ErrorCode ret = 0;

    char savepath_CH0[200] = "./data/CH0_RUN_";
    char savepath_CH1[200] = "./data/CH1_RUN_";
    char savepath_CH2[200] = "./data/CH2_RUN_";
    char ConfigFileName[200];

    // Buffers to store the data. The memory must be allocated using the appropriate
    // CAENDigitizer API functions (see below), so they must not be initialized here
    // NB: you must use the right type for different DPP analysis (in this case PSD)
    char* buffer = NULL;                                    // readout buffer
    CAEN_DGTZ_DPP_PSD_Event_t* Events[MaxNChannels];  // events buffer
    CAEN_DGTZ_DPP_PSD_Waveforms_t* Waveform = NULL;         // waveforms buffer

    /* The following variables will store the digitizer configuration parameters */
    CAEN_DGTZ_DPP_PSD_Params_t DPPParams[MAXNB];
    DigitizerParams_t Params[MAXNB];

    // to estimate trigger rate
    int TrgCnt[MAXNB][MaxNChannels];

    /* The following variable will be used to get an handler for the digitizer. The
    handler will be used for most of CAENDigitizer functions to identify the board */
    int handle[MAXNB];

    FILE* F_CH0, * F_CH1, * F_CH2, * F_cfg;

    /* Other variables */
    int i, b, ch, ev;
    int Quit = 0;
    int AcqRun = 0;
    uint32_t AllocatedSize, BufferSize;
    int Nb = 0;
    int MajorNumber;
    uint64_t CurrentTime, PrevRateTime, ElapsedTime;
    uint32_t NumEvents[MaxNChannels];
    uint32_t temp = 0;
    CAEN_DGTZ_BoardInfo_t BoardInfo;
    uint32_t extras;
    uint16_t ETT = 0; uint16_t fineTT = 0;

    uint32_t evtCounter_CH0 = 0;
    uint32_t evtCounter_CH1 = 0;
    uint32_t evtCounter_CH2 = 0;
    int fileCounter = 1;
    char saveName_CH0[250] = { 0 };
    char saveName_CH1[250] = { 0 };
    char saveName_CH2[250] = { 0 };

    // opens first save files for execution
    sprintf(saveName_CH0, "%s%d.bin", savepath_CH0, 0);
    sprintf(saveName_CH1, "%s%d.bin", savepath_CH1, 0);
    sprintf(saveName_CH2, "%s%d.bin", savepath_CH2, 0);
    F_CH0 = fopen(saveName_CH0, "wb");
    F_CH1 = fopen(saveName_CH1, "wb");
    F_CH2 = fopen(saveName_CH2, "wb");

    if ((F_CH0 && F_CH1 && F_CH2) != NULL)
        printf("output files successfully created\n");
    else
        goto QuitProgram;

    /* *************************************************************************************** */
    /* Set Parameters                                                                          */
    /* *************************************************************************************** */
    memset(&Params, 0, MAXNB * sizeof(DigitizerParams_t));
    memset(&DPPParams, 0, MAXNB * sizeof(CAEN_DGTZ_DPP_PSD_Params_t));

    /* Load config file */
    strcpy(ConfigFileName, DEFAULT_CONFIG_FILE);
    printf("Opening Configuration File\n");
    F_cfg = fopen(ConfigFileName, "r");

    if (F_cfg == NULL) {
        printf("Couldn't open config file!\n\n");
        goto QuitProgram;
    }
    LoadConfigFile(F_cfg, Params, DPPParams);
    fclose(F_cfg);


    /* *************************************************************************************** */
    /* Open the digitizer and read board information                                           */
    /* *************************************************************************************** */
    /* The following function is used to open the digitizer with the given connection parameters
    and get the handler to it */
    for (b = 0; b < MAXNB; b++) {

        /* The following is for b boards connected via b USB direct links
        in this case you must set Params[b].LinkType = CAEN_DGTZ_USB and Params[b].VMEBaseAddress = 0 */
        ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, 0, 0, Params[b].VMEBaseAddress, &handle[b]);

        if (ret) {
            printf("Can't open digitizer\n");
            goto QuitProgram;
        }

        /* Once we have the handler to the digitizer, we use it to call the other functions */
        ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);
        if (ret) {
            printf("Can't read board info\n");
            goto QuitProgram;
        }
        printf("\nConnected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo.ModelName, b);
        printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
        printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

        // Check firmware revision (only DPP firmware can be used with this Demo) */
        sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
        if (MajorNumber != 131 && MajorNumber != 132 && MajorNumber != 136) {
            printf("This digitizer has not a DPP-PSD firmware\n");
            goto QuitProgram;
        }

    }

    /* *************************************************************************************** */
    /* Program the digitizer (see function ProgramDigitizer)                                   */
    /* *************************************************************************************** */
    for (b = 0; b < MAXNB; b++) {
        ret = ProgramDigitizer(handle[b], Params[b], DPPParams[b]);
        if (ret) {
            printf("Failed to program the digitizer\n");
            goto QuitProgram;
        }
    }

    /* WARNING: The mallocs MUST be done after the digitizer programming,
    because the following functions needs to know the digitizer configuration
    to allocate the right memory amount */
    /* Allocate memory for the readout buffer */
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle[0], &buffer, &AllocatedSize);
    /* Allocate memory for the events */
    ret |= CAEN_DGTZ_MallocDPPEvents(handle[0], Events, &AllocatedSize);
    /* Allocate memory for the waveforms */
    ret |= CAEN_DGTZ_MallocDPPWaveforms(handle[0], &Waveform, &AllocatedSize);
    if (ret) {
        printf("Can't allocate memory buffers\n");
        goto QuitProgram;
    }


    /* *************************************************************************************** */
    /* Readout Loop                                                                            */
    /* *************************************************************************************** */
    // Clear Histograms and counters
    for (b = 0; b < MAXNB; b++) {
        for (ch = 0; ch < MaxNChannels; ch++) {
            // Initialize trigger count array
            TrgCnt[b][ch] = 0;
        }
    }
    PrevRateTime = get_time();
    AcqRun = 0;
    PrintInterface();
    printf("Type a command: ");
    while (!Quit) {

        // Check keyboard
        if (kbhit()) {
            char c;
            c = getch();
            if (c == 'q')
                Quit = 1;
            else if (c == 't')
                for (b = 0; b < MAXNB; b++)
                    ret = CAEN_DGTZ_SendSWtrigger(handle[b]); /* Send a software trigger to each board */
            else if (c == 'r') {
                for (b = 0; b < MAXNB; b++) {
                    CAEN_DGTZ_SWStopAcquisition(handle[b]);
                    printf("Restarted\n");
                    CAEN_DGTZ_ClearData(handle[b]);
                    CAEN_DGTZ_SWStartAcquisition(handle[b]);
                }
            }
            else if (c == 's') {
                for (b = 0; b < MAXNB; b++) {
                    // Start Acquisition
                    // NB: the acquisition for each board starts when the following line is executed
                    // so in general the acquisition does NOT starts syncronously for different boards
                    CAEN_DGTZ_SWStartAcquisition(handle[b]);
                    printf("Acquisition Started for Board %d\n", b);
                }
                AcqRun = 1;
            }
            else if (c == 'S') {
                for (b = 0; b < MAXNB; b++) {
                    // Stop Acquisition
                    CAEN_DGTZ_SWStopAcquisition(handle[b]);
                    printf("Acquisition Stopped for Board %d\n", b);
                    fclose(F_CH0);
                    fclose(F_CH1);
                }
                AcqRun = 0;
            }
            else if (c == 'T') {
                for (b = 0; b < MAXNB; b++) {
                    printf("\n");
                    for (ch = 0; ch < MaxNChannels; ch++) {
                        // Read ADC temperature
                        CAEN_DGTZ_ReadTemperature(handle[b], ch, &temp);
                        printf("Ch %d  ADC temperature: %d %cC\n", ch, temp, 248);
                    }
                }
                printf("Type a command: ");
            }
            else if (c == 'C') {
                for (b = 0; b < MAXNB; b++)
                    CAEN_DGTZ_Calibrate(handle[b]);
                printf("\nADC calibration ready\n");
                printf("Type a command: ");

            }
        }
        if (!AcqRun) {
            Sleep(1);
            continue;
        }

        /* Calculate throughput and trigger rate (every second) */
        CurrentTime = get_time();
        ElapsedTime = CurrentTime - PrevRateTime; /* milliseconds */
        if (ElapsedTime > 1000) {
            int clr_sys = system(CLEARSCR);
            PrintInterface();
            printf("Readout Rate=%.2f MB\n", (float)Nb / ((float)ElapsedTime * 1048.576f));
            for (b = 0; b < MAXNB; b++) {
                printf("\nBoard %d:\n", b);
                for (i = 0; i < MaxNChannels; i++) {
                    if (TrgCnt[b][i] > 0)
                        printf("\tCh %d:\tTrgRate=%.2f KHz\t%\n", b * 8 + i, (float)TrgCnt[b][i] / (float)ElapsedTime);
                    else
                        printf("\tCh %d:\tNo Data\n", i);
                    TrgCnt[b][i] = 0;
                }
            }
            Nb = 0;
            PrevRateTime = CurrentTime;
            printf("\n\n");
        }

        /* Read data from the boards */
        for (b = 0; b < MAXNB; b++) {
            /* Read data from the board */
            ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
            if (ret) {
                printf("Readout Error\n");
                goto QuitProgram;
            }
            if (BufferSize == 0)
                continue;

            Nb += BufferSize;

            ret = DataConsistencyCheck((uint32_t*)buffer, BufferSize / 4);
            ret |= CAEN_DGTZ_GetDPPEvents(handle[b], buffer, BufferSize, Events, NumEvents);
            if (ret) {
                printf("Data Error: %d\n", ret);
                goto QuitProgram;
            }

            /* Analyze data */
            for (ch = 0; ch < MaxNChannels; ch++) {
                if (!(Params[b].ChannelMask & (1 << ch)))
                    continue;

                /* Iterate over events */
                for (ev = 0; ev < NumEvents[ch]; ev++) {
                    TrgCnt[b][ch]++;

                    uint8_t size = 0;
                    uint16_t* WaveLine;
                    //uint8_t *DigitalWaveLine;
                    CAEN_DGTZ_DecodeDPPWaveforms(handle[b], &Events[ch][ev], Waveform);

                    size = (uint8_t)(Waveform->Ns); // Number of samples
                    WaveLine = Waveform->Trace1; // for DPP-PSD, first analog trace is ALWAYS the Input Signal

                    /* Reads Extended and Fine TimeTags */
                    extras = Events[ch][ev].Extras;
                    fineTT = extras & 0x3FF;
                    ETT = (extras & 0xFFFF0000) >> 16;

                    // Binary write to file

                    /*  SAVED FOR EACH EVENT
                     *
                     *  Extended timetag - 2 bytes
                     *  timetag - 4 bytes
                     *  Fine timetag - 2 bytes
                     *  charge short - 2 bytes
                     *  charge long - 2 bytes
                     *  baseline - 2 bytes
                     *  pileup rejection flag - 2 bytes
                     *  all waveform samples (if any) - 2 bytes per sample
                     *
                     *
                     */

                    if (ch == 0) {
                        //fwrite(&size, sizeof(uint8_t), 1, F_CH0);
                        fwrite(&ETT, sizeof(uint16_t), 1, F_CH0);
                        fwrite(&Events[ch][ev].TimeTag, sizeof(uint32_t), 1, F_CH0);
                        fwrite(&fineTT, sizeof(uint16_t), 1, F_CH0);
                        fwrite(&Events[ch][ev].ChargeShort, sizeof(uint16_t), 1, F_CH0);
                        fwrite(&Events[ch][ev].ChargeLong, sizeof(uint16_t), 1, F_CH0);
                        fwrite(&Events[ch][ev].Baseline, sizeof(uint16_t), 1, F_CH0);
                        fwrite(&Events[ch][ev].Pur, sizeof(uint16_t), 1, F_CH0);
                        
                        
                        if (Params[b].AcqMode != CAEN_DGTZ_DPP_ACQ_MODE_List) {	// if in mixed mode
                            for (int i = 0; i < size; i++) {
                                fwrite(&WaveLine[i], sizeof(int16_t), 1, F_CH0);
                            }
                        }
                        
                    }

                    if (ch == 1) {
                        //fwrite(&size, sizeof(uint8_t), 1, F_CH1);
                        fwrite(&ETT, sizeof(uint16_t), 1, F_CH1);  // ETT
                        fwrite(&Events[ch][ev].TimeTag, sizeof(uint32_t), 1, F_CH1);
                        fwrite(&fineTT, sizeof(uint16_t), 1, F_CH1);    // fine TT
                        fwrite(&Events[ch][ev].ChargeShort, sizeof(uint16_t), 1, F_CH1);
                        fwrite(&Events[ch][ev].ChargeLong, sizeof(uint16_t), 1, F_CH1);
                        fwrite(&Events[ch][ev].Baseline, sizeof(uint16_t), 1, F_CH1);
                        fwrite(&Events[ch][ev].Pur, sizeof(uint16_t), 1, F_CH1);

                        
                        if (Params[b].AcqMode != CAEN_DGTZ_DPP_ACQ_MODE_List) {	// if in mixed mode
                            for (int i = 0; i < size; i++) {
                                fwrite(&WaveLine[i], sizeof(int16_t), 1, F_CH1);
                            }
                        }
                        
                    }

                    if (ch == 2) {
                        //fwrite(&size, sizeof(uint8_t), 1, F_CH2);
                        fwrite(&ETT, sizeof(uint16_t), 1, F_CH2);  // ETT
                        fwrite(&Events[ch][ev].TimeTag, sizeof(uint32_t), 1, F_CH2);
                        fwrite(&fineTT, sizeof(uint16_t), 1, F_CH2);    // fine TT
                        fwrite(&Events[ch][ev].ChargeShort, sizeof(uint16_t), 1, F_CH2);
                        fwrite(&Events[ch][ev].ChargeLong, sizeof(uint16_t), 1, F_CH2);
                        fwrite(&Events[ch][ev].Baseline, sizeof(uint16_t), 1, F_CH2);
                        fwrite(&Events[ch][ev].Pur, sizeof(uint16_t), 1, F_CH2);
                        
                        if (Params[b].AcqMode != CAEN_DGTZ_DPP_ACQ_MODE_List) {	// if in mixed mode
                            for (int i = 0; i < size; i++) {
                                fwrite(&WaveLine[i], sizeof(int16_t), 1, F_CH2);
                            }
                        }
                        
                    }


                } // loop on events
            } // loop on channels

            evtCounter_CH0 += NumEvents[0];
            evtCounter_CH1 += NumEvents[1];
            evtCounter_CH2 += NumEvents[1];

            // if current event counter saved is more than MAXFILEEVENTS, open another file
            if ((evtCounter_CH0 > MAXFILEEVENTS) || (evtCounter_CH1 > MAXFILEEVENTS) || (evtCounter_CH2 > MAXFILEEVENTS)) {
                
                // binary
                //fwrite(&evtCounter_CH0, sizeof(uint32_t), 1, F_CH0);
                //fwrite(&evtCounter_CH1, sizeof(uint32_t), 1, F_CH1);
                //fwrite(&evtCounter_CH2, sizeof(uint32_t), 1, F_CH2);

                evtCounter_CH0 = 0;
                evtCounter_CH1 = 0;
                evtCounter_CH2 = 0;
                fclose(F_CH0);
                fclose(F_CH1);
                fclose(F_CH2);

                if (fileCounter > MAXFILEINDEX) {
                    printf("Maximum amount of files reached. Acquisition finished!\n");
                    goto QuitProgram;
                }

                // opens first save files for execution
                sprintf(saveName_CH0, "%s%d.bin", savepath_CH0, fileCounter);
                sprintf(saveName_CH1, "%s%d.bin", savepath_CH1, fileCounter);
                sprintf(saveName_CH2, "%s%d.bin", savepath_CH2, fileCounter);
                F_CH0 = fopen(saveName_CH0, "wb");
                F_CH1 = fopen(saveName_CH1, "wb");
                F_CH2 = fopen(saveName_CH2, "wb");
                fileCounter++;
            }
        } // loop on boards
    } // End of readout loop


QuitProgram:
    /* stop the acquisition, close the device and free the buffers */

    for (b = 0; b < MAXNB; b++) {
        CAEN_DGTZ_SWStopAcquisition(handle[b]);
        CAEN_DGTZ_CloseDigitizer(handle[b]);
    }
    CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    CAEN_DGTZ_FreeDPPEvents(handle[0], Events);
    CAEN_DGTZ_FreeDPPWaveforms(handle[0], Waveform);

    printf("\nPress a key to close window.\n");
    getch();

    exit(EXIT_SUCCESS);

    return 0;
}
    
