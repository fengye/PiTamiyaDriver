#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include "get_model.h"
#include "motor_routine.h"


// PWM pin only works on 1 and 23(23 shared with Audio signal)
#define PWM 1 

const float MOTOR_POWER_RANGE[] = {
  0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 
  -0.5f, 
  0.0f, 
  -0.1f, -0.2f, -0.3f, -0.4f, -0.5f, -0.6f, -0.7f, -0.8f, -0.9f, -1.0f};

const char* DIRECTION_NAME[] = {
  "Neutral", "Forward 10%", "Forward 20%", "Forward 30%", "Forward 40%", "Forward 50%", "Forward 60%", "Forward 70%", "Forward 80%", "Forward 90%", "Forward 100%", 
  "Brake 50%", 
  "Neutral", 
  "Reverse 10%", "Reverse 20%", "Reverse 30%", "Reverse 40%", "Reverse 50%", "Reverse 60%", "Reverse 70%", "Reverse 80%", "Reverse 90%", "Reverse 100%"
};

int main (void) {
  const RaspiModel model = get_raspi_model();
  if (model == RPi_Unknown)
  {
    fprintf(stderr, "Unknown Raspberry Pi model, exit.\n");
    exit(-1);
  }

  printf("Raspberry Pi + wiringPi for Tamiya ESC test\n");
  printf("Detected model: %s\n", RaspiNames[model]) ;

  if (wiringPiSetup () == -1)
    exit (1) ;

  setup_motor(model, PWM);

  int test = 0;
  const int WAIT = 2500;
  for (;;)
  {
    int duty_cycle = drive_motor(MOTOR_POWER_RANGE[test]);
    printf("Change state: %s\t PWM: %d\n", DIRECTION_NAME[test], (int)duty_cycle);
    delay(WAIT);

    test = (test + 1) % (sizeof(MOTOR_POWER_RANGE) / sizeof(int));
  }

  return 0 ;
}
