// Add custom code for setup function here which will be included at the end of the function

#include <Arduino.h>
// #include <Update.h>
#include <SD.h>

// Power up DS18B20 3.3v PIN
pinMode(2, OUTPUT);
digitalWrite(2, HIGH);

// Look for update

File firmware = SD.open("/update.bin");
if (firmware) 
{
    // Update.begin(firmware.size(), U_FLASH);
    // Update.writeStream(firmware);
    // if (Update.end()) {

    // }
}