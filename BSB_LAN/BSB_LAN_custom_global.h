// Add custom code for setup function here which will be included at the end of the global section

// PlatformIO signatures
uint8_t recognizeVirtualFunctionGroup(float nr);
String scanAndConnectToStrongestNetwork();
void query(float line);
void SetDevId();
void GetDevId();
void resetAverageCalculation();
const char *printError(uint16_t error);
int bin2hex(char *toBuffer, byte *fromAddr, int len, char delimiter);
void SerialPrintRAW(byte *msg, byte len);
int printFmtToDebug(const char *format, ...);

// My logic
static const int MQTT_TEMP_UNKNOWN = 128;
static const int MQTT_TEMP_WINDOW_OPEN = 127;
static const int MQTT_TEMP_UNRESPONSIVE_SENSOR = 126;
static const int MQTT_TEMP_VALID_THRESHOLD = 50;
static const int numMQTTTemps = sizeof(MQTTSensors) / sizeof(MQTTSensors[0]);
float MQTTTemps[numMQTTTemps] = {};

void initializeMQTTTemps()
{
    for (int i = 0; i < numMQTTTemps; i++)
    {
        MQTTTemps[i] = MQTT_TEMP_UNKNOWN;
    }
}

void subscribeMQTTSensorTopics()
{
    for (int i = 0; i < numMQTTTemps; i++)
    {
        if (strcmp(MQTTSensors[i], "") != 0)
        {
            printFmtToDebug(PSTR("Subscribing MQTT topic \"%s\"\r\n"), MQTTSensors[i]);
            MQTTPubSubClient->subscribe(MQTTSensors[i]);
        }
    }
}

bool handleMQTTSensorTopic(char *topic, byte *payload, unsigned int length)
{
    for (int i = 0; i < numMQTTTemps; i++)
    {
        if (strcmp(topic, MQTTSensors[i]) == 0)
        {
            MQTTTemps[i] = atof((char *)payload);

            return true;
        }
    }

    return false;
}
