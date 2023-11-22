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
