#include "cfg.h"
#include <math.h>

#ifndef MaxNChannels
#define MaxNChannels 4
#endif

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      LoadConfigFile(FILE* f_ini, CAEN_DGTZ_Params_t *Params, CAEN_DGTZ_DPP_PSD_Params_t *DPPParams)
*   \brief   Load configuration file settings to Parameter structures for DGTZ configuration
*   \return  none
/* --------------------------------------------------------------------------------------------------------- */
void LoadConfigFile(FILE* f_ini, DigitizerParams_t *Params, CAEN_DGTZ_DPP_PSD_Params_t *DPPParams)
{
    char setting[256];
    int ch = -1, parameter;

    while (!feof(f_ini)) {
        // read a word from the file
        int read = fscanf(f_ini, "%s", setting);

        // skip empty lines
        if(read <= 0 || !strlen(setting)) 
            continue;

        // skip comments
        if(setting[0] == '#') {
            if(fgets(setting, 256, f_ini) != NULL)
                continue;
            else
                printf("Couldn't read off comment line.\n");
        }

        // Section (Common or individual channel)
        if (setting[0] == '[') {
            if (strstr(setting, "Common")) {
	            ch = -1;
	            continue; 
            }
            
            sscanf(setting + 1, "%d", &parameter);
            if (parameter < 0 || parameter >= MaxNChannels) {
	            printf("%d: invalid channel number\n", parameter);
	            return;
            } 
            else
	            ch = parameter;
            continue;
        }
        printf("%s configuration loaded successfully.\n", setting);

        // coicidence mode
        if (strstr(setting, "COINCIDENCEMODE") != NULL) {
            read = fscanf(f_ini, "%u", &Params[0].CoincMode);
            continue;
        }

        // number of waveform samples (ns)
        if (strstr(setting, "RECORDLENGTH") != NULL) {
            read = fscanf(f_ini, "%u", &Params[0].RecordLength);
            continue;
        }

        // pre trigger size (samples)
        if (strstr(setting, "PRETRIGGERSIZE") != NULL) {
            read = fscanf(f_ini, "%hhu", &Params[0].PreTriggerSize);
            continue;
        }

        // DC offset (0 to 65535)
        if (strstr(setting, "DCOFFSET") != NULL) {
            read = fscanf(f_ini, "%hd", &Params[0].DCOffset);
            continue;
        } 

        // Trigger threshold [LSB]
        if (strstr(setting, "TRIGGERTHRESHOLD") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].thr[j] = parameter;
            }
            else
                DPPParams[0].thr[ch] = parameter;
            continue;
        } 

        // trigger polarity - positive of negative
        if (strstr(setting, "TRIGGERPOLARITY") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(parameter == 0)
                    Params[0].PulsePolarity = CAEN_DGTZ_PulsePolarityPositive;
            else
                Params[0].PulsePolarity = CAEN_DGTZ_PulsePolarityNegative;
            
            continue;
        }

        // charge sensibility [fC/LSB]
        if (strstr(setting, "CSENSIBILITY") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].csens[j] = parameter;
            }
            else
                DPPParams[0].csens[ch] = parameter;
            continue;
        } 

        // short gate (samples)
        if (strstr(setting, "SHORTGATE") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].sgate[j] = parameter;
            }
            else
                DPPParams[0].sgate[ch] = parameter;
            continue;
        } 

        // long gate (samples)
        if (strstr(setting, "LONGGATE") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].lgate[j] = parameter;
            }
            else
                DPPParams[0].lgate[ch] = parameter;
            continue;
        } 

        // p gate (samples)
        if (strstr(setting, "PGATE") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].pgate[j] = parameter;
            }
            else
                DPPParams[0].pgate[ch] = parameter;
            continue;
        } 

        // channel self trigger
        if (strstr(setting, "SELFTRIGGER") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].selft[j] = parameter;
            }
            else
                DPPParams[0].selft[ch] = parameter;
            continue;
        } 

        // number of baseline samples
        if (strstr(setting, "BASELINESAMPLES") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].nsbl[j] = parameter;
            }
            else
                DPPParams[0].nsbl[ch] = parameter;
            continue;
        } 

        // discriminator type
        if (strstr(setting, "DISCRIMINATORTYPE") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++){
                    if(parameter == 0)
                        DPPParams[0].discr[j] = CAEN_DGTZ_DPP_DISCR_MODE_LED;
                    else
                        DPPParams[0].discr[j] = CAEN_DGTZ_DPP_DISCR_MODE_CFD;
                }
                    
            }
            else
                if(parameter == 0)
                    DPPParams[0].discr[ch] = CAEN_DGTZ_DPP_DISCR_MODE_LED;
                else
                    DPPParams[0].discr[ch] = CAEN_DGTZ_DPP_DISCR_MODE_CFD;
            continue;
        } 

        // CFD fraction
        if (strstr(setting, "CFDFRACTION") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].cfdf[j] = parameter;
            }
            else
                DPPParams[0].cfdf[ch] = parameter;
            continue;
        }

        // CFD delay
        if (strstr(setting, "CFDDELAY") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].cfdd[j] = parameter;
            }
            else
                DPPParams[0].cfdd[ch] = parameter;
            continue;
        }

        // trigger validation in coincidence mode
        if (strstr(setting, "COINCIDENCEWINDOW") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    DPPParams[0].tvaw[j] = parameter;
            }
            else
                DPPParams[0].tvaw[ch] = parameter;
            continue;
        }

        // channel enable
        if (strstr(setting, "CHANNELENABLE") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(ch == -1){
                for(int j = 0; j < MaxNChannels; j++)
                    Params[0].ChannelMask += pow(2, j)*parameter;
            }
            else
                Params[0].ChannelMask += pow(2, ch)*parameter;
            continue;
        }

        // trigger polarity - positive of negative
        if (strstr(setting, "ACQUISITIONMODE") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(parameter == 0)
                Params[0].AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_Oscilloscope;
            else if(parameter == 1)
                Params[0].AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_List;
            else if(parameter == 2)
                Params[0].AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_Mixed;
            continue;
        }

        // trigger holdoff
        if (strstr(setting, "TRIGGERHOLDOFF") != NULL) {
            read = fscanf(f_ini, "%i", &DPPParams[0].trgho);
            continue;
        }    

        // pileup rejection
        if (strstr(setting, "PILEUPREJECTION") != NULL) {
            read = fscanf(f_ini, "%d", &parameter);
            if(parameter == 0)
                DPPParams[0].purh = CAEN_DGTZ_DPP_PSD_PUR_DetectOnly;
            else
                DPPParams[0].purh = CAEN_DGTZ_DPP_PSD_PUR_Enabled;
            continue;
        }      

    }

    Params[0].LinkType = CAEN_DGTZ_USB;  // Link Type
    Params[0].VMEBaseAddress = 0;  // For direct USB connection, VMEBaseAddress must be 0
    Params[0].IOlev = CAEN_DGTZ_IOLevel_TTL;
    Params[0].EventAggr = 0;      

  return;
}

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPPParamsPHA_t DPPParams)
*   \brief   Program the registers of the digitizer with the relevant parameters
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */
int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PSD_Params_t DPPParams)
{
    /* This function uses the CAENDigitizer API functions to perform the digitizer's initial configuration */
    int i, ret = 0;

    /* Reset the digitizer */
    ret |= CAEN_DGTZ_Reset(handle);

    if (ret) {
        printf("ERROR: can't reset the digitizer.\n");
        return -1;
    }

    /* Set the DPP acquisition mode
    This setting affects the modes Mixed and List (see CAEN_DGTZ_DPP_AcqMode_t definition for details)
    CAEN_DGTZ_DPP_SAVE_PARAM_EnergyOnly        Only charge (DPP-PSD/DPP-CI v2) is returned
    CAEN_DGTZ_DPP_SAVE_PARAM_TimeOnly        Only time is returned
    CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime    Both charge and time are returned
    CAEN_DGTZ_DPP_SAVE_PARAM_None            No histogram data is returned */
    ret |= CAEN_DGTZ_SetDPPAcquisitionMode(handle, Params.AcqMode, CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);

    // Set the I/O level (CAEN_DGTZ_IOLevel_NIM or CAEN_DGTZ_IOLevel_TTL)
    ret |= CAEN_DGTZ_SetIOLevel(handle, Params.IOlev);

    // Set the digitizer acquisition mode (CAEN_DGTZ_SW_CONTROLLED or CAEN_DGTZ_S_IN_CONTROLLED)
    ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);

    /* Set the digitizer's behaviour when an external trigger arrives:

    CAEN_DGTZ_TRGMODE_DISABLED: do nothing
    CAEN_DGTZ_TRGMODE_EXTOUT_ONLY: generate the Trigger Output signal
    CAEN_DGTZ_TRGMODE_ACQ_ONLY = generate acquisition trigger
    CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT = generate both Trigger Output and acquisition trigger

    see CAENDigitizer user manual, chapter "Trigger configuration" for details */
    ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_DISABLED);

    // Set the enabled channels
    ret |= CAEN_DGTZ_SetChannelEnableMask(handle, Params.ChannelMask);
    
    /* Set the mode used to syncronize the acquisition between different boards.
    In this example the sync is disabled */
    ret |= CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);

    if (ret) {
        printf("ERROR: %d\n", ret);
        return -1;
    }
    
    for(i=0; i<MaxNChannels; i++) {
        if (Params.ChannelMask & (1<<i)) {
            // Set the number of samples for each waveform (you can set different RL for different channels)
            ret |= CAEN_DGTZ_SetRecordLength(handle, Params.RecordLength, i);

            // Set a DC offset to the input signal to adapt it to digitizer's dynamic range
            // 0xFFFF for signal in range (0 V, 1 V)
            ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, Params.DCOffset);
            //printf("%d\n", Params.DCOffset[i]);
            
            // Set the Pre-Trigger size (in samples)
            ret |= CAEN_DGTZ_SetDPPPreTriggerSize(handle, i, Params.PreTriggerSize);
            
            // Set the polarity for the given channel (CAEN_DGTZ_PulsePolarityPositive or CAEN_DGTZ_PulsePolarityNegative)
            ret |= CAEN_DGTZ_SetChannelPulsePolarity(handle, i, Params.PulsePolarity);
        }
    }
    
    // Set how many events to accumulate in the board memory before being available for readout
    ret |= CAEN_DGTZ_SetDPPEventAggregation(handle, Params.EventAggr, 0);

    /* Set the virtual probes

    DPP-PSD for x751 boards can save:
    2 analog waveforms:
        Analog Trace 1: it is always the input signal;
        Analog Trace 2: it is always the baseline

    3 digital waveforms:
        Digital Trace 1:   it is always the trigger
        Digital Trace 2/3: they can be specified with the DIGITALPROBE 1 and 2 parameters

    CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE    -> Save only the Input Signal waveform
    CAEN_DGTZ_DPP_VIRTUALPROBE_DUAL      -> Save also the waveform specified in VIRTUALPROBE

    Virtual Probes types for Trace 2:
        CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Baseline         -> Save the Baseline waveform (mean on nsbl parameter)

    Digital Probes types for Digital Trace 2(x751):
      ### Virtual Probes only for FW >= 13X.6 ###
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_OverThr
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_TrigOut
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_CoincWin
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_PileUp
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_Coincidence
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_GateLong     NOTE: x751 only
      ### Virtual Probes only for FW <= 13X.5 ###
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_OverThr		    NOTE: x751 only
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_GateShort		NOTE: x751 only
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_None            NOTE: x751 only

    Digital Probes types for Digital Trace 3(x751):
      ### Virtual Probes only for FW >= 13X.6 ###
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_GateShort
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_OverThr
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_TrgVal
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_TrgHO
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_PileUp
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_Coincidence
      ### Virtual Probes only for FW <= 13X.5 ###
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_GateShort		NOTE: x751 only
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_GateLong		NOTE: x751 only
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_None            NOTE: x751 only */ 
    
    ret |= CAEN_DGTZ_SetDPP_PSD_VirtualProbe(handle, CAEN_DGTZ_DPP_VIRTUALPROBE_DUAL, CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Baseline, CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_Coincidence, CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_GateShort);

    //ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, DIGITAL_TRACE_2, CAEN_DGTZ_DPP_DIGITALPROBE_CoincWin);


    // Set the DPP specific parameters for the channels in the given channelMask
    ret |= CAEN_DGTZ_SetDPPParameters(handle, Params.ChannelMask, &DPPParams);

    uint32_t reg;
    /* Coincidence setup - CH0 && CH1 */ 
    if (Params.CoincMode == 0) {
        CAEN_DGTZ_ReadRegister(handle, 0x8000, &reg);
        reg |= (0x1 << 2);   // Enable the propagation of the individual trigger (ITRG) from MB to Mezzanines
        CAEN_DGTZ_WriteRegister(handle, 0x8000, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1080, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH0
        CAEN_DGTZ_WriteRegister(handle, 0x1080, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1180, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1070, &reg);
        uint8_t Ts = DPPParams.tvaw[0] / 8;
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH0
        CAEN_DGTZ_WriteRegister(handle, 0x1070, reg);

        Ts = DPPParams.tvaw[1] / 8;
        CAEN_DGTZ_ReadRegister(handle, 0x1170, &reg);
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1170, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x106C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH0 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x106C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x116C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH1 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x116C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8180, &reg);
        reg |= (0x3 << 0);   // CH0 and CH1 enable
        reg |= (0x1 << 8);   // Trigger validation logic CH0 - AND
        CAEN_DGTZ_WriteRegister(handle, 0x8180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8184, &reg);
        reg |= (0x3 << 0);   // CH0 and CH1 enable
        reg |= (0x1 << 8);   // Trigger validation logic CH1 - AND
        CAEN_DGTZ_WriteRegister(handle, 0x8184, reg);
    }

    /* coincidence setup - (CH0 && CH1) || (CH0 && CH2) */
    if (Params.CoincMode == 1) {
        CAEN_DGTZ_ReadRegister(handle, 0x8000, &reg);
        reg |= (0x1 << 2);   // Enable the propagation of the individual trigger (ITRG) from MB to Mezzanines
        CAEN_DGTZ_WriteRegister(handle, 0x8000, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1080, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH0
        CAEN_DGTZ_WriteRegister(handle, 0x1080, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1180, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1280, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH2
        CAEN_DGTZ_WriteRegister(handle, 0x2180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1070, &reg);
        uint8_t Ts = DPPParams.tvaw[0] / 8;
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH0
        CAEN_DGTZ_WriteRegister(handle, 0x1070, reg);

        Ts = DPPParams.tvaw[1] / 8;
        CAEN_DGTZ_ReadRegister(handle, 0x1170, &reg);
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1170, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1270, &reg);
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1270, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x106C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH0 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x106C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x116C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH1 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x116C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x126C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH1 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x126C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8180, &reg);
        reg |= (0x7 << 0);   // CH0 and CH1 and CH2 enable
        reg |= (0x2 << 8);   // Trigger validation logic CH0 - MAJORITY
        reg |= (0x1 << 10);  // Majority level 2
        CAEN_DGTZ_WriteRegister(handle, 0x8180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8184, &reg);
        reg |= (0x3 << 0);   // CH0 and CH1 enable
        reg |= (0x1 << 8);   // Trigger validation logic CH1 - AND
        CAEN_DGTZ_WriteRegister(handle, 0x8184, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8188, &reg);
        reg |= (0x5 << 0);   // CH0 and CH2 enable
        reg |= (0x1 << 8);   // Trigger validation logic CH2 - AND
        CAEN_DGTZ_WriteRegister(handle, 0x8188, reg);
    }

    /* coincidence setup - (CH0 && CH1 && CH2) */
    if (Params.CoincMode == 2) {
        CAEN_DGTZ_ReadRegister(handle, 0x8000, &reg);
        reg |= (0x1 << 2);   // Enable the propagation of the individual trigger (ITRG) from MB to Mezzanines
        CAEN_DGTZ_WriteRegister(handle, 0x8000, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1080, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH0
        CAEN_DGTZ_WriteRegister(handle, 0x1080, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1180, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1280, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH2
        CAEN_DGTZ_WriteRegister(handle, 0x2180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1070, &reg);
        uint8_t Ts = DPPParams.tvaw[0] / 8;
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH0
        CAEN_DGTZ_WriteRegister(handle, 0x1070, reg);

        Ts = DPPParams.tvaw[1] / 8;
        CAEN_DGTZ_ReadRegister(handle, 0x1170, &reg);
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1170, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1270, &reg);
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1270, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x106C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH0 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x106C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x116C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH1 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x116C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x126C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH1 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x126C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8180, &reg);
        reg |= (0x7 << 0);   // CH0 and CH1 and CH2 enable
        reg |= (0x1 << 8);   // Trigger validation logic CH0 - AND
        CAEN_DGTZ_WriteRegister(handle, 0x8180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8184, &reg);
        reg |= (0x3 << 0);   // CH0 and CH1 enable
        reg |= (0x1 << 8);   // Trigger validation logic CH1 - AND
        CAEN_DGTZ_WriteRegister(handle, 0x8184, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8188, &reg);
        reg |= (0x5 << 0);   // CH0 and CH2 enable
        reg |= (0x1 << 8);   // Trigger validation logic CH2 - AND
        CAEN_DGTZ_WriteRegister(handle, 0x8188, reg);
    }

    /* coincidence setup - (CH0 && CH1 && CH2) */
    if (Params.CoincMode == 3) {
        CAEN_DGTZ_ReadRegister(handle, 0x8000, &reg);
        reg |= (0x1 << 2);   // Enable the propagation of the individual trigger (ITRG) from MB to Mezzanines
        CAEN_DGTZ_WriteRegister(handle, 0x8000, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1080, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH0
        CAEN_DGTZ_WriteRegister(handle, 0x1080, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1180, &reg);
        reg |= (0x1 << 18);   // Enable coincidence CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x1070, &reg);
        uint8_t Ts = DPPParams.tvaw[0] / 8;
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH0
        CAEN_DGTZ_WriteRegister(handle, 0x1070, reg);

        Ts = DPPParams.tvaw[1] / 8;
        CAEN_DGTZ_ReadRegister(handle, 0x1170, &reg);
        reg |= (Ts << 0);   // length of coincidence shaped trigger (clock cycles - 8 ns) - CH1
        CAEN_DGTZ_WriteRegister(handle, 0x1170, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x106C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH0 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x106C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x116C, &reg);
        reg |= (0x9 << 0);   // Trigger latency CH1 (must be 9 clock cycles)
        CAEN_DGTZ_WriteRegister(handle, 0x116C, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8180, &reg);
        reg |= (0x7 << 0);   // CH0 and CH1 and CH2 enable
        reg |= (0x0 << 8);   // Trigger validation logic CH0 - OR
        CAEN_DGTZ_WriteRegister(handle, 0x8180, reg);

        CAEN_DGTZ_ReadRegister(handle, 0x8184, &reg);
        reg |= (0x3 << 0);   // CH0 and CH1 enable
        reg |= (0x0 << 8);   // Trigger validation logic CH1 - OR
        CAEN_DGTZ_WriteRegister(handle, 0x8184, reg);
    }


    /* setup to activate extended and fine timetags */
    /* ONLY WORKS FOR FW VERSION > 132.32 */
    
    CAEN_DGTZ_ReadRegister(handle, 0x8000, &reg);
    reg |= (0x1 << 17);     // Allow EXTRAS recording
    reg |= (0x1 << 31);     // Allow second word of format setting - needed to read EXTRAS word
    CAEN_DGTZ_WriteRegister(handle, 0x8000, reg); 

    // to debug, write 0x7 to bit 11 - in that case, EXTRAS return 0x12345678
    CAEN_DGTZ_ReadRegister(handle, 0x1080, &reg);
    reg |= (0x2 << 11);     // Format extras for CH0 (32 bit word, Extended time stamp [31:16], flags [15:10], fine times time stamp [9:0])
    CAEN_DGTZ_WriteRegister(handle, 0x1080, reg);
    
    CAEN_DGTZ_ReadRegister(handle, 0x1180, &reg);
    reg |= (0x2 << 11);     // Format extras for CH1 (32 bit word, Extended time stamp [31:16], flags [15:10], fine times time stamp [9:0])
    CAEN_DGTZ_WriteRegister(handle, 0x1180, reg);

    CAEN_DGTZ_ReadRegister(handle, 0x8080, &reg);
    reg |= (0x2 << 11);     // Same as two above but for global settings
    CAEN_DGTZ_WriteRegister(handle, 0x8080, reg);

    if (ret) {
        printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");
        return ret;
    } else {
        return 0;
    }    

}