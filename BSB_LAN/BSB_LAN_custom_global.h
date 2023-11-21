// Add custom code for setup function here which will be included at the end of the global section

// PlatformIO signatures
uint8_t recognizeVirtualFunctionGroup(float nr);
String scanAndConnectToStrongestNetwork();
void query(float line);
void SetDevId();
void GetDevId();
void resetAverageCalculation();
const char* printError(uint16_t error);
int bin2hex(char *toBuffer, byte *fromAddr, int len, char delimiter);
void SerialPrintRAW(byte* msg, byte len);
int printFmtToDebug(const char *format, ...);

static const int numMQTTTopics = sizeof(MQTTSensors) / sizeof(MQTTSensors[0]);

void subscribeMQTTSensorTopics()
{
    for (int i = 0; i < numMQTTTopics; i++) {
        if (MQTTSensors[i].topic != NULL) {
          printFmtToDebug(PSTR("Subscribing MQTT topic \"%s\"\r\n"), MQTTSensors[i].topic);
          MQTTPubSubClient->subscribe(MQTTSensors[i].topic);
      }
    }
}

bool handleMQTTSensorTopic(char* topic, byte* payload, unsigned int length) 
{
    for (int i = 0; i < numMQTTTopics; i++) 
    {
        if (MQTTSensors[i].topic != NULL && strcmp(topic, MQTTSensors[i].topic) == 0)
        {
            int line = MQTTSensors[i].parameter - BSP_TEMP;
            if (line < numCustomTemps && line >= 0) {
                float tempValue = atof((char *) payload);
                custom_temps[line] = tempValue;
            } else {
                printFmtToDebug(PSTR("Invalid custom temp range %i (Sensor: %s, parameter: %i)\r\n"), line, MQTTSensors[i].topic, MQTTSensors[i].parameter);
            }

            return true;
        }
    }

    return false;
}
