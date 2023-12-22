/* 
 * If configuration definement CUSTOM_COMMANDS is active, this code is run 
 * at the end of each main loop and utilizes the main loop variables 
 * custom_timer (set each loop to millis()) and custom_timer_compare.
 * This short example prints a "Ping!" message every 20 seconds.
*/

#include "custom_functions/Send_NTP_time_to_heater/BSB_LAN_custom.h"
#include "custom_functions/Emulate_room_unit_by_lowest_temp/BSB_LAN_custom.h"

// Save ESP battery info
custom_floats[0] = analogRead(GPIO_NUM_35) / 4096.0 * 7.445;
float normalizedBatteryPercentage = (custom_floats[0] - 3.3) / (4.2 - 3.3) * 100;
if (normalizedBatteryPercentage > 100)
{
  normalizedBatteryPercentage = 100;
}
else if (normalizedBatteryPercentage < 0)
{
  normalizedBatteryPercentage = 0;
}
custom_floats[1] = normalizedBatteryPercentage;

