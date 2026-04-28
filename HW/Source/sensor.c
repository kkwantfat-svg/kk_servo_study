#include "sensor.h"
#include "pwm.h"
#include "gd32f10x_timer.h"

void sensor_init(void)
{
    pwm_init();
}

/**
 * Function: sensor_set_angle
 * Description: Set the angle of the sensor
 * Parameters: angle - the desired angle (0-180 degrees)
 */
void sensor_set_angle(uint8_t angle)
{
    // Limit the angle to the range [0, 180]
    if(angle > 180) angle = 180;
    if(angle < 0) angle = 0;
    // Convert the angle to a pulse width and configure the timer
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0,  (uint16_t)(angle / 180.0 * 2000  + 500));
}
