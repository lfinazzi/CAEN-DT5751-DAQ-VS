# CAEN-DT5751-DAQ-VS
Visual Studio Community 2022 project to control the CAEN DT5751 digitizer with a PC. This code has a menu for user interaction and a config file to change acquisition parameters without recompiling the source code. The code allows for coincidence measurements between 2 and 3 channels or individual channel measurements.

The main function is located in DAQ.c and cfg.txt is used for setting the acquisition parameters for the digitizer without the need for recompilation. 

Requirements:

  1. USB driver for digitizer for the corresponding user platform (found in https://www.caen.it/download/?filter=DT5751).
  2. CAENDigitizer library (found in https://www.caen.it/products/caendigitizer-library/).
  3. CAENComm library (found in https://www.caen.it/products/caencomm-library/).
  4. CAENVMELib library (found in https://www.caen.it/products/caenvmelib-library/).

Common problems:

Depending on the user platform (and the system USB port config), the function CAEN_DGTZ_OpenDigitizer() may not initialize the digitizer. If this happens, try changing the second and/or third argument of this function to 1 (i.e. different combinations of both parameters). The default value for both is 0, as these parameters represent linkNumber and conetNumber, respectively).

IMPORTANT:

  1. For the code to run, you need to create a folder "data" in the source file directory.
  2. For inndividual channel measurements (CH0 || CH1), both CH 0 and CH 1 need to be activated.

EVENT FORMAT:

Each acquisition saves one file per channel used. The data format for each event is the following:

  1. Extended timetag - 2B
  2. timetag - 4B
  3. fine timetag - 2B
  4. charge short - 2B
  5. charge long - 2B
  6. baseline 2B
  7. pileup rejection flag - 2B
  8. All waveform samples (if any) - 2B per sample

Each new event is saved after the previous one.
