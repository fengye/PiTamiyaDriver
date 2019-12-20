#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "get_model.h"
#include <signal.h>

// PWM pin only works on 1 and 23(23 shared with Audio signal)
#define PWM 1 

// 57.67Hz == 17.34 per cycle
// PWM 1.5ms = 1.5/17.34 % duty cycle
// PWM 1.1ms = 1.1/17.34 % duty cycle
// PWM 1.9ms = 1.9/17.34 % duty cycle
const int SERVO_PWM_RANGE[] = {130, 105, 130, 150};

const char* DIRECTION_NAME[] = {
  "Neutral", "Left", "Neutral", "Right"
};

float lerp(int lhs, int rhs, float percent)
{
   return lhs + (rhs - lhs) * percent;
}

static void int_handler()
{
  printf("Exiting... ");
  pwmWrite(PWM, SERVO_PWM_RANGE[0]);
  delay(500);
  pwmWrite(PWM, 0);
  printf("Done!\n");
  exit(0);
}
    

int main (void)
{
  const int model = get_raspi_model();
  if (model == RPi_Unknown)
  {
    fprintf(stderr, "Unknown Raspberry Pi model, exit.\n");
    exit(-1);
  }

  printf("Raspberry Pi + wiringPi for Tamiya servo TSU-03 test\n");
  printf("Detected model: %s\n", RaspiNames[model]) ;

  if (wiringPiSetup () == -1)
    exit (1) ;

  signal(SIGINT, int_handler);
  pinMode (PWM, PWM_OUTPUT) ;
  pwmSetMode(PWM_MODE_MS);

  pwmSetRange(1500);
  pwmSetClock(222);

  int test = 0;
  const int INCR = 100;
  const int WAIT = 2500;
  for (;;)
  {
    int next_test = (test+1) % (sizeof(SERVO_PWM_RANGE) / sizeof(int));
    int duty_cycle = SERVO_PWM_RANGE[test];
    int next_duty_cycle = SERVO_PWM_RANGE[next_test];
    float time_percent = 0.0f;
    int time = 0;
    while(time <= WAIT)
    {
      time_percent = (float)time / (float)WAIT;
      float interp = lerp(duty_cycle, next_duty_cycle, time_percent);
      pwmWrite(PWM, (int)interp);
      printf("State: %s\t PWM: %d\n", DIRECTION_NAME[test], (int)interp);
      time += INCR;
      delay(INCR);
    }

    test = (test + 1) % (sizeof(SERVO_PWM_RANGE) / sizeof(int));
  }

  return 0 ;
}
