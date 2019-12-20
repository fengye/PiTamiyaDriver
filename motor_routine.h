#ifndef _MOTOR_ROUTINE_H_
#define _MOTOR_ROUTINE_H_

#include <wiringPi.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include "util.h"

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
const int POWER_LERP_TIME = 500;

// ============================================================================
static int PWM_PIN = 1;
static pthread_mutex_t update_mutex;
static pthread_t update_thread;
static bool s_running = false;


static float s_target_power = 0, s_source_power = 0, s_curr_power = 0;
static int s_curr_time = 0;

// power ranges from -1.0 to 1.0
static int _get_motor_pwm(float power)
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

float _get_lerp_power(float start_power, float end_power, int time_diff, int duration)
{
  return lerp(start_power, end_power, (float)time_diff / (float)duration);
}

int drive_motor(float power)
{
  pthread_mutex_lock(&update_mutex);

  // if it's brake
  if (s_curr_power > 0 && power < 0)
  {
    // need a sharp stop
    s_target_power = power;
    s_source_power = power;
    s_curr_time = POWER_LERP_TIME;
    s_curr_power = s_target_power;

    pwmWrite(PWM_PIN, _get_motor_pwm(s_curr_power));
    printf("\n");
  }
  else // otherwise low pass filter
  {
    s_target_power = power;
    s_source_power = s_curr_power;
    s_curr_time = 0;
  }

  int duty_cycle = _get_motor_pwm(s_curr_power);
  pthread_mutex_unlock(&update_mutex);

  return duty_cycle;
}


void shutdown_motor()
{
  printf("Quitting update thread...");
  pthread_mutex_lock(&update_mutex);
  s_running = false;
  pthread_mutex_unlock(&update_mutex);
  pthread_join(update_thread, NULL);
  pthread_mutex_destroy(&update_mutex);
  printf(" Done!\n");
  printf("Exiting, setting to neutral...");
  pwmWrite(PWM_PIN, PWM_NEUTRAL);
  printf(" Done!\n");
}

static void _int_handler()
{ 
  shutdown_motor();
  exit(0);
}


static void* _update_thread(void* param)
{
  for(;;)
  {
    pthread_mutex_lock(&update_mutex);
    if (!s_running)
    {
      pthread_mutex_unlock(&update_mutex);
      break;
    }

    if (s_curr_time >= POWER_LERP_TIME)
    {
      s_curr_power = s_target_power;
    }
    else
    {
      s_curr_power = _get_lerp_power(s_source_power, s_target_power, s_curr_time, POWER_LERP_TIME); // 500ms interpolation time
    }

    int duty_cycle = _get_motor_pwm(s_curr_power);
    pwmWrite(PWM_PIN, duty_cycle);
    printf("%.2f\tPWM: %d\r", s_curr_power, duty_cycle);
    fflush(stdout);

    s_curr_time += 16;

    pthread_mutex_unlock(&update_mutex);

    delay(16);

  }

  return NULL;
}

void setup_motor(RaspiModel model, int pwm_pin)
{
  PWM_PIN = pwm_pin;
  printf("Setting up PWM pin at %d\n", pwm_pin);
  signal(SIGINT, _int_handler);
  pinMode (PWM_PIN, PWM_OUTPUT) ;
  pwmSetMode(PWM_MODE_MS);

  pwmSetRange(PWM_SETUP_RANGE);
  pwmSetClock(PWM_SETUP_DIVISORS[(int)model]);

  printf("Starting update thread\n");
  pthread_mutex_init(&update_mutex, NULL);

  s_running = true;
  pthread_create(&update_thread, NULL, &_update_thread, NULL);
}

#endif //_MOTOR_ROUTINE_H_

