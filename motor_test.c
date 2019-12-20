#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include "get_model.h"

// PWM pin only works on 1 and 23(23 shared with Audio signal)
#define PWM 1 

// 57.67Hz == 17.34ms per cycle
// PWM 1.5ms = 1.5/17.34 % duty cycle
// PWM 1.1ms = 1.1/17.34 % duty cycle
// PWM 1.9ms = 1.9/17.34 % duty cycle
// PWM Range = [0, 1500)
const int PWM_FORWARD_LOW_RANGE = 120;
const int PWM_FORWARD_HIGH_RANGE = 95;
const int PWM_NEUTRAL = 130;
const int PWM_REVERSE_HIGH_RANGE = 164;
const int PWM_REVERSE_LOW_RANGE = 140;

const int PWM_SETUP_DIVISORS[] = {
	222, 222, 222, 222, 624
};

const int PWM_SETUP_RANGE = 1500;

// power ranges from -1.0 to 1.0
int get_motor_pwm(float power)
{
  if (power > 0)
    if (power <= 1.0f)	
      return PWM_FORWARD_LOW_RANGE + (int)((PWM_FORWARD_HIGH_RANGE - PWM_FORWARD_LOW_RANGE) * power);
    else
      return PWM_FORWARD_HIGH_RANGE;
  else if(power < 0)
  {
    power *= -1.0f;
    if (power >= -1.0f)
      return PWM_REVERSE_LOW_RANGE + (int)((PWM_REVERSE_HIGH_RANGE - PWM_REVERSE_LOW_RANGE) * power);
    else
      return PWM_REVERSE_HIGH_RANGE;
  }
  else
      return PWM_NEUTRAL;
} 

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

float lerp(int lhs, int rhs, float percent)
{
   return lhs + (rhs - lhs) * percent;
}
    
static void int_handler()
{ 
  printf("Exiting, setting to neutral...");
  pwmWrite(PWM, PWM_NEUTRAL);
  printf(" Done!\n");
  exit(0);
}

int main (void) {
  const int model = get_raspi_model();
  if (model == RPi_Unknown)
  {
    fprintf(stderr, "Unknown Raspberry Pi model, exit.\n");
    exit(-1);
  }

  printf("Raspberry Pi + wiringPi for Tamiya ESC test\n");
  printf("Detected model: %s\n", RaspiNames[model]) ;

  if (wiringPiSetup () == -1)
    exit (1) ;

  signal(SIGINT, int_handler);
  pinMode (PWM, PWM_OUTPUT) ;
  pwmSetMode(PWM_MODE_MS);

  pwmSetRange(PWM_SETUP_RANGE);
  pwmSetClock(PWM_SETUP_DIVISORS[model]);

  int test = 0;
//  const int INCR = 50;
  const int WAIT = 2500;
  for (;;)
  {
//    int next_test = (test+1) % (sizeof(SERVO_PWM_RANGE) / sizeof(int));
    int duty_cycle = get_motor_pwm(MOTOR_POWER_RANGE[test]);
//    int next_duty_cycle = get_motor_pwm(MOTOR_POWER_RANGE[next_test]);
    //float time_percent = 0.0f;
    //int time = 0;
    //while(time <= WAIT)
    {
      //time_percent = (float)time / (float)WAIT;
      //float interp = lerp(duty_cycle, next_duty_cycle, time_percent);
      pwmWrite(PWM, (int)duty_cycle);
      printf("State: %s\t PWM: %d\n", DIRECTION_NAME[test], (int)duty_cycle);
      // time += INCR;
      delay(WAIT);
    }

    test = (test + 1) % (sizeof(MOTOR_POWER_RANGE) / sizeof(int));
  }

  return 0 ;
}
