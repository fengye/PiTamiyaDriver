#ifndef _H_GET_MODEL_
#define _H_GET_MODEL_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Raspberry Pi 2-3 has a PWM clock hardware of 19.2MHz. In order to make 57.67 Hz Tamiya ESC control signal
// we have to divide it by 222, and make the range [0, 1500). Need to leave the range precision a bit higher
// because Tamiya PWM signal usually falls into a short duty cycle range. Thus 19.2M / 222 / 1500 = 57.66 Hz.  
// Raspberry Pi 4 has a PWM clock hardware of 54MHz. In order to make 57.67 Hz Tamiya ESC control signal
// we have to divide it by 624, and make the range [0, 1500). Thus 54M / 624 / 1500 = 57.69 Hz 
// Other model of Raspberry Pi need to adjust those two values based on their hardward respectively

typedef enum _RaspiModel {
	RPi_1 = 0,
	RPi_Zero,
	RPi_2,
	RPi_3,
	RPi_4,
	RPi_Unknown
} RaspiModel;

const char* RaspiNames[] = {
	"Raspberry Pi 1",
	"Raspberry Pi Zero",
	"Raspberry Pi 2",
	"Raspberry Pi 3",
	"Raspberry Pi 4",
	"Unknown model"
};

#define BUF_LEN 256
#define BUF_MAX 255

const char* SYS_MODEL_INFO_FILE = "/sys/firmware/devicetree/base/model";

int get_raspi_model()
{
  FILE* cpuinfo_fp = fopen(SYS_MODEL_INFO_FILE, "rb");
  if (!cpuinfo_fp)
  {
    fprintf(stderr, "Cannot open %s, exiting\n", SYS_MODEL_INFO_FILE);
    return RPi_Unknown;
  }

  char line[BUF_LEN];
  while(fgets(line, BUF_LEN, cpuinfo_fp))
  {
  }

  fclose(cpuinfo_fp);
  // last line contains model info
  int idx = 0;
  char* tokens[16] = {NULL};
  char* pch = strtok(line, " ,:-");
  while((tokens[idx++] = pch))
  {
    pch = strtok(NULL, " ,:-");
  }
  if (strcmp(tokens[0], "Raspberry") == 0 &&
      strcmp(tokens[1], "Pi") == 0)
  {
     if (strcmp(tokens[2], "1") == 0)
     {
        return RPi_1;
     }
     else if (strcmp(tokens[2], "2") == 0)
     {
        return RPi_2;
     }
     else if (strcmp(tokens[2], "3") == 0)
     {
        return RPi_3;
     }
     else if (strcmp(tokens[2], "4") == 0)
     {
        return RPi_4;
     }
     // TODO: Pi Zero
  }

  return RPi_Unknown;
}

#endif // _H_GET_MODEL_
