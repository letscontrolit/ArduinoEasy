#include <utility/W5100.h>
void ShowSocketStatus() {

  Serial.println("ETHERNET SOCKET LIST");
  Serial.println("#:Status Port Destination DPort");
  Serial.println("0=avail,14=waiting,17=connected,22=UDP,1C=close wait");
  String l_line = "";
  l_line.reserve(64);
  char l_buffer[10] = "";
  for (uint8_t i = 0; i < MAX_SOCK_NUM; i++) {
    l_line = "#" + String(i);
    uint8_t s = W5100.readSnSR(i); //status
    l_line += ":0x";
    sprintf(l_buffer,"%x",s);
    l_line += l_buffer;
    l_line += " ";
    l_line += String(W5100.readSnPORT(i)); //port
    l_line += " D:";
    uint8_t dip[4];
    W5100.readSnDIPR(i, dip); //IP Address
    for (int j=0; j<4; j++) {
      l_line += int(dip[j]);
      if (j<3) l_line += ".";
    }
    l_line += " (";
    l_line += String(W5100.readSnDPORT(i)); //port on destination
    l_line += ") ";
    Serial.println(l_line);
  }
}

void debugRAM(byte id, int objsize)
{
  Serial.print(F("Function:"));
  Serial.print(id);
  Serial.print(F(" RAM:"));
  Serial.print(FreeMem());
  Serial.print(F(" objsize:"));
  Serial.println(objsize);
}

void SelectSDCard(boolean sd)
  {
  digitalWrite(EthernetShield_CS_W5100, sd);
  digitalWrite(EthernetShield_CS_SDCard, !sd);
  }
  
/*********************************************************************************************\
   Get value count from sensor type
  \*********************************************************************************************/

byte getValueCountFromSensorType(byte sensorType)
{
  byte valueCount = 0;

  switch (sensorType)
  {
    case SENSOR_TYPE_SINGLE:                      // single value sensor, used for Dallas, BH1750, etc
    case SENSOR_TYPE_SWITCH:
    case SENSOR_TYPE_DIMMER:
      valueCount = 1;
      break;
    case SENSOR_TYPE_LONG:                      // single LONG value, stored in two floats (rfid tags)
      valueCount = 1;
      break;
    case SENSOR_TYPE_TEMP_HUM:
    case SENSOR_TYPE_TEMP_BARO:
    case SENSOR_TYPE_DUAL:
      valueCount = 2;
      break;
    case SENSOR_TYPE_TEMP_HUM_BARO:
    case SENSOR_TYPE_TRIPLE:
      valueCount = 3;
      break;
    case SENSOR_TYPE_QUAD:
      valueCount = 4;
      break;
  }
  return valueCount;
}


/*********************************************************************************************\
   Workaround for removing trailing white space when String() converts a float with 0 decimals
  \*********************************************************************************************/
String toString(float value, byte decimals)
{
  String sValue = String(value, decimals);
  sValue.trim();
  return sValue;
}

/*********************************************************************************************\
   Parse a string and get the xth command or parameter
  \*********************************************************************************************/
String parseString(String& string, byte indexFind)
{
  String tmpString = string;
  tmpString += ",";
  tmpString.replace(" ", ",");
  String locateString = "";
  byte count = 0;
  int index = tmpString.indexOf(',');
  while (index > 0)
  {
    count++;
    locateString = tmpString.substring(0, index);
    tmpString = tmpString.substring(index + 1);
    index = tmpString.indexOf(',');
    if (count == indexFind)
    {
      locateString.toLowerCase();
      return locateString;
    }
  }
  return "";
}


/*********************************************************************************************\
   Parse a string and get the xth command or parameter
  \*********************************************************************************************/
int getParamStartPos(String& string, byte indexFind)
{
  String tmpString = string;
  byte count = 0;
  tmpString.replace(" ", ",");
  for (int x = 0; x < tmpString.length(); x++)
  {
    if (tmpString.charAt(x) == ',')
    {
      count++;
      if (count == (indexFind - 1))
        return x + 1;
    }
  }
  return -1;
}


/*********************************************************************************************\
   set pin mode & state (info table)
  \*********************************************************************************************/
boolean setPinState(byte plugin, byte index, byte mode, uint16_t value)
{
  // plugin number and index form a unique key
  // first check if this pin is already known
  boolean reUse = false;
  for (byte x = 0; x < PINSTATE_TABLE_MAX; x++)
    if ((pinStates[x].plugin == plugin) && (pinStates[x].index == index))
    {
      pinStates[x].mode = mode;
      pinStates[x].value = value;
      reUse = true;
      break;
    }

  if (!reUse)
  {
    for (byte x = 0; x < PINSTATE_TABLE_MAX; x++)
      if (pinStates[x].plugin == 0)
      {
        pinStates[x].plugin = plugin;
        pinStates[x].index = index;
        pinStates[x].mode = mode;
        pinStates[x].value = value;
        break;
      }
  }
}


/*********************************************************************************************\
   get pin mode & state (info table)
  \*********************************************************************************************/
boolean getPinState(byte plugin, byte index, byte *mode, uint16_t *value)
{
  for (byte x = 0; x < PINSTATE_TABLE_MAX; x++)
    if ((pinStates[x].plugin == plugin) && (pinStates[x].index == index))
    {
      *mode = pinStates[x].mode;
      *value = pinStates[x].value;
      return true;
    }
  return false;
}


/*********************************************************************************************\
   check if pin mode & state is known (info table)
  \*********************************************************************************************/
boolean hasPinState(byte plugin, byte index)
{
  for (byte x = 0; x < PINSTATE_TABLE_MAX; x++)
    if ((pinStates[x].plugin == plugin) && (pinStates[x].index == index))
    {
      return true;
    }
  return false;
}


/*********************************************************************************************\
   report pin mode & state (info table) using json
  \*********************************************************************************************/
String getPinStateJSON(boolean search, byte plugin, byte index, String& log, uint16_t noSearchValue)
{
  printToWebJSON = true;
  byte mode = PIN_MODE_INPUT;
  uint16_t value = noSearchValue;
  String reply = "";
  boolean found = false;

  if (search)
  {
    for (byte x = 0; x < PINSTATE_TABLE_MAX; x++)
      if ((pinStates[x].plugin == plugin) && (pinStates[x].index == index))
      {
        mode = pinStates[x].mode;
        value = pinStates[x].value;
        found = true;
        break;
      }
  }

  if (!search || (search && found))
  {
    reply += F("{\n\"log\": \"");
    reply += log.substring(7, 32); // truncate to 25 chars, max MQTT message size = 128 including header...
    reply += F("\",\n\"plugin\": ");
    reply += plugin;
    reply += F(",\n\"pin\": ");
    reply += index;
    reply += F(",\n\"mode\": \"");
    switch (mode)
    {
      case PIN_MODE_UNDEFINED:
        reply += F("undefined");
        break;
      case PIN_MODE_INPUT:
        reply += F("input");
        break;
      case PIN_MODE_OUTPUT:
        reply += F("output");
        break;
      case PIN_MODE_PWM:
        reply += F("PWM");
        break;
      case PIN_MODE_SERVO:
        reply += F("servo");
        break;
    }
    reply += F("\",\n\"state\": ");
    reply += value;
    reply += F("\n}\n");
    return reply;
  }
  return "?";
}


/********************************************************************************************\
  Unsigned long Timer timeOut check
  \*********************************************************************************************/

boolean timeOut(unsigned long timer)
{
  // This routine solves the 49 day bug without the need for separate start time and duration
  //   that would need two 32 bit variables if duration is not static
  // It limits the maximum delay to 24.9 days.

  unsigned long now = millis();
  if (((now >= timer) && ((now - timer) < 1 << 31))  || ((timer >= now) && (timer - now > 1 << 31)))
    return true;

  return false;
}


/********************************************************************************************\
  Status LED
  \*********************************************************************************************/
void statusLED(boolean traffic)
{
  if (Settings.Pin_status_led == -1)
    return;

  static unsigned long timer = 0;
  static byte currentState = 0;

  if (traffic)
  {
    currentState = HIGH;
    digitalWrite(Settings.Pin_status_led, currentState); // blink off
    timer = millis() + 100;
  }

  if (timer == 0 || millis() > timer)
  {
    timer = 0;
    byte state = HIGH;
// todo     if (WiFi.status() == WL_CONNECTED)
//     state = LOW;

    if (currentState != state)
    {
      currentState = state;
      pinMode(Settings.Pin_status_led, OUTPUT);
      digitalWrite(Settings.Pin_status_led, state);
    }
  }
}


/********************************************************************************************\
  delay in milliseconds with background processing
  \*********************************************************************************************/
void delayMillis(unsigned long delay)
{
  unsigned long timer = millis() + delay;
  while (millis() < timer)
    backgroundtasks();
}


/********************************************************************************************\
  Parse a command string to event struct
  \*********************************************************************************************/
void parseCommandString(struct EventStruct *event, String& string)
{
  char command[80];
  command[0] = 0;
  char TmpStr1[80];
  TmpStr1[0] = 0;

  string.toCharArray(command, 80);
  event->Par1 = 0;
  event->Par2 = 0;
  event->Par3 = 0;

  if (GetArgv(command, TmpStr1, 2)) event->Par1 = str2int(TmpStr1);
  if (GetArgv(command, TmpStr1, 3)) event->Par2 = str2int(TmpStr1);
  if (GetArgv(command, TmpStr1, 4)) event->Par3 = str2int(TmpStr1);
}

/********************************************************************************************\
  Clear task settings for given task
  \*********************************************************************************************/
void taskClear(byte taskIndex, boolean save)
{
  Settings.TaskDeviceNumber[taskIndex] = 0;
  ExtraTaskSettings.TaskDeviceName[0] = 0;
  Settings.TaskDeviceID[taskIndex] = 0;
  Settings.TaskDeviceDataFeed[taskIndex] = 0;
  Settings.TaskDevicePin1[taskIndex] = -1;
  Settings.TaskDevicePin2[taskIndex] = -1;
  Settings.TaskDevicePin3[taskIndex] = -1;
  Settings.TaskDevicePort[taskIndex] = 0;
  Settings.TaskDeviceSendData[taskIndex] = true;
  Settings.TaskDeviceGlobalSync[taskIndex] = false;
  Settings.TaskDeviceTimer[taskIndex] = 0;

  for (byte x = 0; x < PLUGIN_CONFIGVAR_MAX; x++)
    Settings.TaskDevicePluginConfig[taskIndex][x] = 0;

  for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
  {
    ExtraTaskSettings.TaskDeviceFormula[varNr][0] = 0;
    ExtraTaskSettings.TaskDeviceValueNames[varNr][0] = 0;
    ExtraTaskSettings.TaskDeviceValueDecimals[varNr] = 2;
  }

  for (byte varNr = 0; varNr < PLUGIN_EXTRACONFIGVAR_MAX; varNr++)
    ExtraTaskSettings.TaskDevicePluginConfigLong[varNr] = 0;

  if (save)
  {
    SaveTaskSettings(taskIndex);
    SaveSettings();
  }
}


/********************************************************************************************\
  Use DNS to resolve hostname to ip address
  \*********************************************************************************************/
void getIPfromHostName()
{
  IPAddress IP;
  if (Settings.ControllerHostName[0] != 0)
  {
    // todo WiFi.hostByName(Settings.ControllerHostName, IP);
    for (byte x = 0; x < 4; x++)
      Settings.Controller_IP[x] = IP[x];
  }
}


/********************************************************************************************\
  Fix stuff to clear out differences between releases
  \*********************************************************************************************/
void BuildFixes()
{
  Serial.println(F("\nBuild changed!"));


  Settings.Build = BUILD;
  SaveSettings();
}

void fileSystemCheck()
{
  pinMode(EthernetShield_CS_SDCard, OUTPUT);
  pinMode(EthernetShield_CS_W5100, OUTPUT);
  SelectSDCard(true);
  
  if (SD.begin(EthernetShield_CS_SDCard))
  {
    String log = F("SD Mount successful");
    Serial.println(log);
    if (!SD.exists("config.txt"))
    {
      String log = F("Creating config.txt");
      Serial.println(log);
      File f = SD.open(F("config.txt"), FILE_WRITE);
      if (f)
      {
        for (unsigned long x = 0; x < 32768; x++)
          f.write((byte)0);
        f.close();
      }
      log = F("Ready creating config.txt");
      Serial.println(log);
      f = SD.open(F("security.txt"), FILE_WRITE);
      if (f)
      {
        for (int x = 0; x < 512; x++)
          f.write((byte)0);
        f.close();
      }
      f = SD.open(F("rules.txt"), FILE_WRITE);
      f.close();
    }
  }
  else
  {
    String log = F("SD Mount failed");
    Serial.println(log);
  }
}


/********************************************************************************************\
  Find device index corresponding to task number setting
  \*********************************************************************************************/
byte getDeviceIndex(byte Number)
{
  byte DeviceIndex = 0;
  for (byte x = 0; x <= deviceCount ; x++)
    if (Device[x].Number == Number)
      DeviceIndex = x;
  return DeviceIndex;
}


/********************************************************************************************\
  Find protocol index corresponding to protocol setting
  \*********************************************************************************************/
byte getProtocolIndex(byte Number)
{
  byte ProtocolIndex = 0;
  for (byte x = 0; x <= protocolCount ; x++)
    if (Protocol[x].Number == Number)
      ProtocolIndex = x;
  return ProtocolIndex;
}


/********************************************************************************************\
  Find positional parameter in a char string
  \*********************************************************************************************/
boolean GetArgv(const char *string, char *argv, int argc)
{
  int string_pos = 0, argv_pos = 0, argc_pos = 0;
  char c, d;

  while (string_pos < strlen(string))
  {
    c = string[string_pos];
    d = string[string_pos + 1];

    if       (c == ' ' && d == ' ') {}
    else if  (c == ' ' && d == ',') {}
    else if  (c == ',' && d == ' ') {}
    else if  (c == ' ' && d >= 33 && d <= 126) {}
    else if  (c == ',' && d >= 33 && d <= 126) {}
    else
    {
      argv[argv_pos++] = c;
      argv[argv_pos] = 0;

      if (d == ' ' || d == ',' || d == 0)
      {
        argv[argv_pos] = 0;
        argc_pos++;

        if (argc_pos == argc)
        {
          return true;
        }

        argv[0] = 0;
        argv_pos = 0;
        string_pos++;
      }
    }
    string_pos++;
  }
  return false;
}


/********************************************************************************************\
  Convert a char string to integer
  \*********************************************************************************************/
unsigned long str2int(char *string)
{
  unsigned long temp = atof(string);
  return temp;
}


/********************************************************************************************\
  Convert a char string to IP byte array
  \*********************************************************************************************/
boolean str2ip(char *string, byte* IP)
{
  byte c;
  byte part = 0;
  int value = 0;

  for (int x = 0; x <= strlen(string); x++)
  {
    c = string[x];
    if (isdigit(c))
    {
      value *= 10;
      value += c - '0';
    }

    else if (c == '.' || c == 0) // next octet from IP address
    {
      if (value <= 255)
        IP[part++] = value;
      else
        return false;
      value = 0;
    }
    else if (c == ' ') // ignore these
      ;
    else // invalid token
      return false;
  }
  if (part == 4) // correct number of octets
    return true;
  return false;
}


/********************************************************************************************\
  Save settings to SPIFFS
  \*********************************************************************************************/
void SaveSettings(void)
{
  SaveToFile(F("config.txt"), 0, (byte*)&Settings, sizeof(struct SettingsStruct));
  SaveToFile(F("security.txt"), 0, (byte*)&SecuritySettings, sizeof(struct SecurityStruct));
}


/********************************************************************************************\
  Load settings from SPIFFS
  \*********************************************************************************************/
boolean LoadSettings()
{
  LoadFromFile(F("config.txt"), 0, (byte*)&Settings, sizeof(struct SettingsStruct));
  LoadFromFile(F("security.txt"), 0, (byte*)&SecuritySettings, sizeof(struct SecurityStruct));
}


/********************************************************************************************\
  Save Task settings to SPIFFS
  \*********************************************************************************************/
void SaveTaskSettings(byte TaskIndex)
{
  ExtraTaskSettings.TaskIndex = TaskIndex;
  SaveToFile(F("config.txt"), 4096 + (TaskIndex * 1024), (byte*)&ExtraTaskSettings, sizeof(struct ExtraTaskSettingsStruct));
}


/********************************************************************************************\
  Load Task settings from SPIFFS
  \*********************************************************************************************/
void LoadTaskSettings(byte TaskIndex)
{
  if (ExtraTaskSettings.TaskIndex == TaskIndex)
    return;

  LoadFromFile(F("config.txt"), 4096 + (TaskIndex * 1024), (byte*)&ExtraTaskSettings, sizeof(struct ExtraTaskSettingsStruct));
  ExtraTaskSettings.TaskIndex = TaskIndex; // Needed when an empty task was requested
}


/********************************************************************************************\
  Save Custom Task settings to SPIFFS
  \*********************************************************************************************/
void SaveCustomTaskSettings(int TaskIndex, byte* memAddress, int datasize)
{
  if (datasize > 512)
    return;
  SaveToFile(F("config.txt"), 4096 + (TaskIndex * 1024) + 512, memAddress, datasize);
}


/********************************************************************************************\
  Save Custom Task settings to SPIFFS
  \*********************************************************************************************/
void LoadCustomTaskSettings(int TaskIndex, byte* memAddress, int datasize)
{
  if (datasize > 512)
    return;
  LoadFromFile(F("config.txt"), 4096 + (TaskIndex * 1024) + 512, memAddress, datasize);
}


/********************************************************************************************\
  Save Custom Controller settings to SPIFFS
  \*********************************************************************************************/
void SaveCustomControllerSettings(byte* memAddress, int datasize)
{
  if (datasize > 4096)
    return;
  SaveToFile(F("config.txt"), 28672, memAddress, datasize);
}


/********************************************************************************************\
  Save Custom Controller settings to SPIFFS
  \*********************************************************************************************/
void LoadCustomControllerSettings(byte* memAddress, int datasize)
{
  if (datasize > 4096)
    return;
  LoadFromFile(F("config.txt"), 28672, memAddress, datasize);
}


/********************************************************************************************\
  Save data into config file on SPIFFS
  \*********************************************************************************************/
void SaveToFile(const __FlashStringHelper* fname, int index, byte* memAddress, int datasize)
{
  File f = SD.open(fname, FILE_WRITE);
  if (f)
  {
    f.seek(index);
    byte *pointerToByteToSave = memAddress;
    for (int x = 0; x < datasize ; x++)
    {
      f.write(*pointerToByteToSave);
      pointerToByteToSave++;
    }
    f.close();
    String log = F("FILE : File saved");
    addLog(LOG_LEVEL_INFO, log);
  }
}


/********************************************************************************************\
  Load data from config file on SPIFFS
  \*********************************************************************************************/
void LoadFromFile( const __FlashStringHelper* fname, int index, byte* memAddress, int datasize)
{
  File f = SD.open(fname);
  if (f)
  {
    f.seek(index);
    byte *pointerToByteToRead = memAddress;
    for (int x = 0; x < datasize; x++)
    {
      *pointerToByteToRead = f.read();
      pointerToByteToRead++;// next byte
    }
    f.close();
  }
}


/********************************************************************************************\
  Reset all settings to factory defaults
  \*********************************************************************************************/
void ResetFactory(void)
{
  // Direct Serial is allowed here, since this is only an emergency task.
  SD.remove(F("config.txt"));
  SD.remove(F("security.txt"));
  SD.remove(F("rules.txt"));
  File f = SD.open(F("config.txt"), FILE_WRITE);
  if (f)
  {
    for (unsigned long x = 0; x < 32768; x++)
      f.write((byte)0);
    f.close();

  }
  f = SD.open(F("security.txt"), FILE_WRITE);
  if (f)
  {
    for (int x = 0; x < 512; x++)
      f.write((byte)0);
    f.close();
  }
  f = SD.open(F("rules.txt"), FILE_WRITE);
  f.close();

  LoadSettings();
  // now we set all parameters that need to be non-zero as default value

#if DEFAULT_USE_STATIC_IP
  str2ip((char*)DEFAULT_IP, Settings.IP);
  str2ip((char*)DEFAULT_DNS, Settings.DNS);
  str2ip((char*)DEFAULT_GW, Settings.Gateway);
  str2ip((char*)DEFAULT_SUBNET, Settings.Subnet);
#endif

  Settings.PID             = ARDUINO_PROJECT_PID;
  Settings.Version         = VERSION;
  Settings.Unit            = UNIT;
  SecuritySettings.Password[0] = 0;
  str2ip((char*)DEFAULT_SERVER, Settings.Controller_IP);
  Settings.ControllerPort      = DEFAULT_PORT;
  Settings.Delay           = DEFAULT_DELAY;
  Settings.Pin_status_led  = -1;
  Settings.Protocol        = DEFAULT_PROTOCOL;
  strcpy_P(Settings.Name, PSTR(DEFAULT_NAME));
  Settings.SerialLogLevel  = 4;
  Settings.BaudRate        = 115200;
  Settings.MessageDelay = 1000;
  Settings.CustomCSS = false;
  for (byte x = 0; x < TASKS_MAX; x++)
  {
    Settings.TaskDevicePin1[x] = -1;
    Settings.TaskDevicePin2[x] = -1;
    Settings.TaskDevicePin3[x] = -1;
    Settings.TaskDevicePin1PullUp[x] = true;
    Settings.TaskDevicePin1Inversed[x] = false;
    Settings.TaskDeviceSendData[x] = true;
    Settings.TaskDeviceTimer[x] = Settings.Delay;
  }
  Settings.Build = BUILD;
  Settings.UseSerial = true;
  Settings.UDPPort =65500;
  SaveSettings();
  delay(1000);
  Reboot();
}


/********************************************************************************************\
  If RX and TX tied together, perform emergency reset to get the system out of boot loops
  \*********************************************************************************************/

void emergencyReset()
{
  // Direct Serial is allowed here, since this is only an emergency task.
  Serial.begin(115200);
  Serial.write(0xAA);
  Serial.write(0x55);
  delay(1);
  if (Serial.available() == 2)
    if (Serial.read() == 0xAA && Serial.read() == 0x55)
    {
      Serial.println(F("System will reset in 10 seconds..."));
      delay(10000);
      ResetFactory();
    }
}


/********************************************************************************************\
  Get free system mem
\*********************************************************************************************/
uint8_t *heapptr, *stackptr;

unsigned long FreeMem(void)
  {
  stackptr = (uint8_t *)malloc(4);        // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);                         // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);            // save value of stack pointer
  return (stackptr-heapptr);
}

/********************************************************************************************\
  In memory convert float to long
  \*********************************************************************************************/
unsigned long float2ul(float f)
{
  unsigned long ul;
  memcpy(&ul, &f, 4);
  return ul;
}


/********************************************************************************************\
  In memory convert long to float
  \*********************************************************************************************/
float ul2float(unsigned long ul)
{
  float f;
  memcpy(&f, &ul, 4);
  return f;
}


/********************************************************************************************\
  Add to log
  \*********************************************************************************************/
void addLog(byte loglevel, String& string)
{
  addLog(loglevel, string.c_str());
}

void addLog(byte loglevel, const char *line)
{
  if (Settings.UseSerial)
    if (loglevel <= Settings.SerialLogLevel)
      Serial.println(line);

  if (loglevel <= Settings.SyslogLevel)
    syslog(line);

  if (loglevel <= Settings.SDLogLevel)
  {
    File logFile = SD.open(F("log.txt"), FILE_WRITE);
    if (logFile)
      {
        String time = "";
        time += hour();
        time += ":";
        if (minute() < 10)
          time += "0";
        time += minute();
        time += ":";
        if (second() < 10)
          time += "0";
        time += second();
        time += F(" : ");
        logFile.print(time);
        logFile.println(line);
      }
    logFile.close();
  }
    
}


/********************************************************************************************\
  Delayed reboot, in case of issues, do not reboot with high frequency as it might not help...
  \*********************************************************************************************/
void delayedReboot(int rebootDelay)
{
  // Direct Serial is allowed here, since this is only an emergency task.
  while (rebootDelay != 0 )
  {
    Serial.print(F("Delayed Reset "));
    Serial.println(rebootDelay);
    rebootDelay--;
    delay(1000);
  }
  Reboot();
}


/********************************************************************************************\
  Convert a string like "Sun,12:30" into a 32 bit integer
  \*********************************************************************************************/
unsigned long string2TimeLong(String &str)
{
  // format 0000WWWWAAAABBBBCCCCDDDD
  // WWWW=weekday, AAAA=hours tens digit, BBBB=hours, CCCC=minutes tens digit DDDD=minutes

  char command[20];
  char TmpStr1[10];
  int w, x, y;
  unsigned long a;
  str.toLowerCase();
  str.toCharArray(command, 20);
  unsigned long lngTime = 0;

  if (GetArgv(command, TmpStr1, 1))
  {
    String day = TmpStr1;
    String weekDays = F("allsunmontuewedthufrisat");
    y = weekDays.indexOf(TmpStr1) / 3;
    if (y == 0)
      y = 0xf; // wildcard is 0xf
    lngTime |= (unsigned long)y << 16;
  }

  if (GetArgv(command, TmpStr1, 2))
  {
    y = 0;
    for (x = strlen(TmpStr1) - 1; x >= 0; x--)
    {
      w = TmpStr1[x];
      if (w >= '0' && w <= '9' || w == '*')
      {
        a = 0xffffffff  ^ (0xfUL << y); // create mask to clean nibble position y
        lngTime &= a; // maak nibble leeg
        if (w == '*')
          lngTime |= (0xFUL << y); // fill nibble with wildcard value
        else
          lngTime |= (w - '0') << y; // fill nibble with token
        y += 4;
      }
      else if (w == ':');
      else
      {
        break;
      }
    }
  }
  return lngTime;
}


/********************************************************************************************\
  Convert  a 32 bit integer into a string like "Sun,12:30"
  \*********************************************************************************************/
String timeLong2String(unsigned long lngTime)
{
  unsigned long x = 0;
  String time = "";

  x = (lngTime >> 16) & 0xf;
  if (x == 0x0f)
    x = 0;
  String weekDays = F("AllSunMonTueWedThuFriSat");
  time = weekDays.substring(x * 3, x * 3 + 3);
  time += ",";

  x = (lngTime >> 12) & 0xf;
  if (x == 0xf)
    time += "*";
  else if (x == 0xe)
    time += "-";
  else
    time += x;

  x = (lngTime >> 8) & 0xf;
  if (x == 0xf)
    time += "*";
  else if (x == 0xe)
    time += "-";
  else
    time += x;

  time += ":";

  x = (lngTime >> 4) & 0xf;
  if (x == 0xf)
    time += "*";
  else if (x == 0xe)
    time += "-";
  else
    time += x;

  x = (lngTime) & 0xf;
  if (x == 0xf)
    time += "*";
  else if (x == 0xe)
    time += "-";
  else
    time += x;

  return time;
}


String parseTemplate(String &tmpString, byte lineSize)
{
  String newString = "";
  String tmpStringMid = "";

  // replace task template variables
  int leftBracketIndex = tmpString.indexOf('[');
  if (leftBracketIndex == -1)
    newString = tmpString;
  else
  {
    byte count = 0;
    byte currentTaskIndex = ExtraTaskSettings.TaskIndex;
    while (leftBracketIndex >= 0 && count < 10 - 1)
    {
      newString += tmpString.substring(0, leftBracketIndex);
      tmpString = tmpString.substring(leftBracketIndex + 1);
      int rightBracketIndex = tmpString.indexOf(']');
      if (rightBracketIndex)
      {
        tmpStringMid = tmpString.substring(0, rightBracketIndex);
        tmpString = tmpString.substring(rightBracketIndex + 1);
        int hashtagIndex = tmpStringMid.indexOf('#');
        String deviceName = tmpStringMid.substring(0, hashtagIndex);
        String valueName = tmpStringMid.substring(hashtagIndex + 1);
        String valueFormat = "";
        hashtagIndex = valueName.indexOf('#');
        if (hashtagIndex >= 0)
        {
          valueFormat = valueName.substring(hashtagIndex + 1);
          valueName = valueName.substring(0, hashtagIndex);
        }
        for (byte y = 0; y < TASKS_MAX; y++)
        {
          LoadTaskSettings(y);
          if (ExtraTaskSettings.TaskDeviceName[0] != 0)
          {
            if (deviceName.equalsIgnoreCase(ExtraTaskSettings.TaskDeviceName))
            {
              for (byte z = 0; z < VARS_PER_TASK; z++)
                if (valueName.equalsIgnoreCase(ExtraTaskSettings.TaskDeviceValueNames[z]))
                {
                  // here we know the task and value, so find the uservar
                  String value = "";
                  byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[y]);
                  if (Device[DeviceIndex].VType == SENSOR_TYPE_LONG)
                    value = (unsigned long)UserVar[y * VARS_PER_TASK + z] + ((unsigned long)UserVar[y * VARS_PER_TASK + z + 1] << 16);
                  else
                    value = toString(UserVar[y * VARS_PER_TASK + z], ExtraTaskSettings.TaskDeviceValueDecimals[z]);

                  if (valueFormat == "R")
                  {
                    int filler = lineSize - newString.length() - value.length() - tmpString.length() ;
                    for (byte f = 0; f < filler; f++)
                      newString += " ";
                  }
                  newString += String(value);
                  break;
                }
              break;
            }
          }
        }
      }
      leftBracketIndex = tmpString.indexOf('[');
      count++;
    }
    newString += tmpString;
    LoadTaskSettings(currentTaskIndex);
  }

  // replace other system variables like %sysname%, %systime%, %ip%
  newString.replace(F("%sysname%"), Settings.Name);

  String strTime = "";
  if (hour() < 10)
    strTime += " ";
  strTime += hour();
  strTime += ":";
  if (minute() < 10)
    strTime += "0";
  strTime += minute();
  newString.replace(F("%systime%"), strTime);

  newString.replace(F("%uptime%"), String(wdcounter / 2));

  IPAddress ip = Ethernet.localIP();
  char strIP[20];
  sprintf_P(strIP, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
  newString.replace(F("%ip%"), strIP);

  newString.replace(F("%sysload%"), String(100 - (100 * loopCounterLast / loopCounterMax)));

  // padding spaces
  while (newString.length() < lineSize)
    newString += " ";

  return newString;
}


/********************************************************************************************\
  Calculate function for simple expressions
  \*********************************************************************************************/
#define CALCULATE_OK                            0
#define CALCULATE_ERROR_STACK_OVERFLOW          1
#define CALCULATE_ERROR_BAD_OPERATOR            2
#define CALCULATE_ERROR_PARENTHESES_MISMATCHED  3
#define CALCULATE_ERROR_UNKNOWN_TOKEN           4
#define STACK_SIZE 10 // was 50
#define TOKEN_MAX 20

float globalstack[STACK_SIZE];
float *sp = globalstack - 1;
float *sp_max = &globalstack[STACK_SIZE - 1];

#define is_operator(c)  (c == '+' || c == '-' || c == '*' || c == '/' || c == '^')

int push(float value)
{
  if (sp != sp_max) // Full
  {
    *(++sp) = value;
    return 0;
  }
  else
    return CALCULATE_ERROR_STACK_OVERFLOW;
}

float pop()
{
  if (sp != (globalstack - 1)) // empty
    return *(sp--);
}

float apply_operator(char op, float first, float second)
{
  switch (op)
  {
    case '+':
      return first + second;
    case '-':
      return first - second;
    case '*':
      return first * second;
    case '/':
      return first / second;
    case '^':
      return pow(first, second);
    default:
      return 0;
  }
}

char *next_token(char *linep)
{
  while (isspace(*(linep++)));
  while (*linep && !isspace(*(linep++)));
  return linep;
}

int RPNCalculate(char* token)
{
  if (token[0] == 0)
    return 0; // geen moeite doen voor een lege string

  if (is_operator(token[0]))
  {
    float second = pop();
    float first = pop();

    if (push(apply_operator(token[0], first, second)))
      return CALCULATE_ERROR_STACK_OVERFLOW;
  }
  else // Als er nog een is, dan deze ophalen
    if (push(atof(token))) // is het een waarde, dan op de stack plaatsen
      return CALCULATE_ERROR_STACK_OVERFLOW;

  return 0;
}

// operators
// precedence   operators         associativity
// 3            !                 right to left
// 2            * / %             left to right
// 1            + - ^             left to right
int op_preced(const char c)
{
  switch (c)
  {
    case '^':
      return 3;
    case '*':
    case '/':
      return 2;
    case '+':
    case '-':
      return 1;
  }
  return 0;
}

bool op_left_assoc(const char c)
{
  switch (c)
  {
    case '^':
    case '*':
    case '/':
    case '+':
    case '-':
      return true;     // left to right
      //case '!': return false;    // right to left
  }
  return false;
}

unsigned int op_arg_count(const char c)
{
  switch (c)
  {
    case '^':
    case '*':
    case '/':
    case '+':
    case '-':
      return 2;
      //case '!': return 1;
  }
  return 0;
}


int Calculate(const char *input, float* result)
{
  const char *strpos = input, *strend = input + strlen(input);
  char token[25];
  char c, *TokenPos = token;
  char stack[32];       // operator stack
  unsigned int sl = 0;  // stack length
  char     sc;          // used for record stack element
  int error = 0;

  //*sp=0; // bug, it stops calculating after 50 times
  sp = globalstack - 1;

  while (strpos < strend)
  {
    // read one token from the input stream
    c = *strpos;
    if (c != ' ')
    {
      // If the token is a number (identifier), then add it to the token queue.
      if ((c >= '0' && c <= '9') || c == '.')
      {
        *TokenPos = c;
        ++TokenPos;
      }

      // If the token is an operator, op1, then:
      else if (is_operator(c))
      {
        *(TokenPos) = 0;
        error = RPNCalculate(token);
        TokenPos = token;
        if (error)return error;
        while (sl > 0)
        {
          sc = stack[sl - 1];
          // While there is an operator token, op2, at the top of the stack
          // op1 is left-associative and its precedence is less than or equal to that of op2,
          // or op1 has precedence less than that of op2,
          // The differing operator priority decides pop / push
          // If 2 operators have equal priority then associativity decides.
          if (is_operator(sc) && ((op_left_assoc(c) && (op_preced(c) <= op_preced(sc))) || (op_preced(c) < op_preced(sc))))
          {
            // Pop op2 off the stack, onto the token queue;
            *TokenPos = sc;
            ++TokenPos;
            *(TokenPos) = 0;
            error = RPNCalculate(token);
            TokenPos = token;
            if (error)return error;
            sl--;
          }
          else
            break;
        }
        // push op1 onto the stack.
        stack[sl] = c;
        ++sl;
      }
      // If the token is a left parenthesis, then push it onto the stack.
      else if (c == '(')
      {
        stack[sl] = c;
        ++sl;
      }
      // If the token is a right parenthesis:
      else if (c == ')')
      {
        bool pe = false;
        // Until the token at the top of the stack is a left parenthesis,
        // pop operators off the stack onto the token queue
        while (sl > 0)
        {
          *(TokenPos) = 0;
          error = RPNCalculate(token);
          TokenPos = token;
          if (error)return error;
          sc = stack[sl - 1];
          if (sc == '(')
          {
            pe = true;
            break;
          }
          else
          {
            *TokenPos = sc;
            ++TokenPos;
            sl--;
          }
        }
        // If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
        if (!pe)
          return CALCULATE_ERROR_PARENTHESES_MISMATCHED;

        // Pop the left parenthesis from the stack, but not onto the token queue.
        sl--;

        // If the token at the top of the stack is a function token, pop it onto the token queue.
        if (sl > 0)
          sc = stack[sl - 1];

      }
      else
        return CALCULATE_ERROR_UNKNOWN_TOKEN;
    }
    ++strpos;
  }
  // When there are no more tokens to read:
  // While there are still operator tokens in the stack:
  while (sl > 0)
  {
    sc = stack[sl - 1];
    if (sc == '(' || sc == ')')
      return CALCULATE_ERROR_PARENTHESES_MISMATCHED;

    *(TokenPos) = 0;
    error = RPNCalculate(token);
    TokenPos = token;
    if (error)return error;
    *TokenPos = sc;
    ++TokenPos;
    --sl;
  }

  *(TokenPos) = 0;
  error = RPNCalculate(token);
  TokenPos = token;
  if (error)
  {
    *result = 0;
    return error;
  }
  *result = *sp;
  return CALCULATE_OK;
}


/********************************************************************************************\
  Time stuff
  \*********************************************************************************************/

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define DAYS_PER_WEEK (7UL)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52UL)
#define SECS_YR_2000  (946684800UL) // the time at the start of y2k
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

struct  timeStruct {
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Wday;   // day of week, sunday is day 1
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;   // offset from 1970;
} tm;

uint32_t syncInterval = 3600;  // time sync will be attempted after this many seconds
uint32_t sysTime = 0;
uint32_t prevMillis = 0;
uint32_t nextSyncTime = 0;

byte PrevMinutes = 0;

void breakTime(unsigned long timeInput, struct timeStruct &tm) {
  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;
  const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  time = (uint32_t)timeInput;
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24; // now it is days
  tm.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1

  year = 0;
  days = 0;
  while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  tm.Year = year; // year is offset from 1970

  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0

  days = 0;
  month = 0;
  monthLength = 0;
  for (month = 0; month < 12; month++) {
    if (month == 1) { // february
      if (LEAP_YEAR(year)) {
        monthLength = 29;
      } else {
        monthLength = 28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
      break;
    }
  }
  tm.Month = month + 1;  // jan is month 1
  tm.Day = time + 1;     // day of month
}

void setTime(unsigned long t) {
  sysTime = (uint32_t)t;
  nextSyncTime = (uint32_t)t + syncInterval;
  prevMillis = millis();  // restart counting from now (thanks to Korman for this fix)
}

unsigned long now() {
  // calculate number of seconds passed since last call to now()
  while (millis() - prevMillis >= 1000) {
    // millis() and prevMillis are both unsigned ints thus the subtraction will always be the absolute value of the difference
    sysTime++;
    prevMillis += 1000;
  }
  if (nextSyncTime <= sysTime) {
    unsigned long  t = getNtpTime();
    if (t != 0) {
      if (Settings.DST)
        t += SECS_PER_HOUR; // add one hour if DST active
      setTime(t);
    } else {
      nextSyncTime = sysTime + syncInterval;
    }
  }
  breakTime(sysTime, tm);
  return (unsigned long)sysTime;
}

int hour()
{
  return tm.Hour;
}

int minute()
{
  return tm.Minute;
}

int second()
{
  return tm.Second;
}

int weekday()
{
  return tm.Wday;
}

void initTime()
{
  nextSyncTime = 0;
  now();
}

void checkTime()
{
  now();
  if (tm.Minute != PrevMinutes)
  {
    PluginCall(PLUGIN_CLOCK_IN, 0, dummyString);
    PrevMinutes = tm.Minute;
    if (Settings.UseRules)
    {
      String weekDays = F("AllSunMonTueWedThuFriSat");
      String event = F("Clock#Time=");
      event += weekDays.substring(weekday() * 3, weekday() * 3 + 3);
      event += ",";
      if (hour() < 10)
        event += "0";
      event += hour();
      event += ":";
      if (minute() < 10)
        event += "0";
      event += minute();
      rulesProcessing(event);
    }
  }
}


unsigned long getNtpTime()
{
  const char* ntpServerName = "pool.ntp.org";
  unsigned long result=0;
  
  IPAddress timeServerIP;

  // The W5100 seems to have an issue with mixing TCP UDP on the same socket.
  // So we stop the default listening UDP socket now, so DNS client will reuse this UDP socket
  if (Settings.UDPPort != 0)
    portUDP.stop();
  int ret = 0;
  DNSClient dns;
  dns.begin(Ethernet.dnsServerIP());
  #if socketdebug
    ShowSocketStatus();
  #endif
  ret = dns.getHostByName(ntpServerName, timeServerIP);
  if (Settings.UDPPort != 0)
    portUDP.begin(Settings.UDPPort);  // re-use UDP socket for system packets if it was used before
  else
    portUDP.begin(123); // start listening only during this call on port 123
  #if socketdebug
    ShowSocketStatus();
  #endif

  if (ret){
    for (byte x = 1; x < 4; x++)
    {
      String log = F("NTP  : NTP sync request:");
      log += x;
      addLog(LOG_LEVEL_DEBUG_MORE, log);

      const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
      byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

      memset(packetBuffer, 0, NTP_PACKET_SIZE);
      packetBuffer[0] = 0b11100011;   // LI, Version, Mode
      packetBuffer[1] = 0;     // Stratum, or type of clock
      packetBuffer[2] = 6;     // Polling Interval
      packetBuffer[3] = 0xEC;  // Peer Clock Precision
      packetBuffer[12]  = 49;
      packetBuffer[13]  = 0x4E;
      packetBuffer[14]  = 49;
      packetBuffer[15]  = 52;

      portUDP.beginPacket(timeServerIP, 123); //NTP requests are to port 123
      portUDP.write(packetBuffer, NTP_PACKET_SIZE);
      portUDP.endPacket();

      uint32_t beginWait = millis();
      while (millis() - beginWait < 2000) {
        int size = portUDP.parsePacket();
        if (size == NTP_PACKET_SIZE && portUDP.remotePort() == 123) {
          portUDP.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
          unsigned long secsSince1900;
          // convert four bytes starting at location 40 to a long integer
          secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
          secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
          secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
          secsSince1900 |= (unsigned long)packetBuffer[43];
          log = F("NTP  : NTP replied: ");
          log += millis() - beginWait;
          log += F(" mSec");
          addLog(LOG_LEVEL_DEBUG_MORE, log);
          result = secsSince1900 - 2208988800UL + Settings.TimeZone * SECS_PER_MIN;
          break; // exit wait loop
        }
      }
      if (result)
        break; // exit for loop
      log = F("NTP  : No reply");
      addLog(LOG_LEVEL_DEBUG_MORE, log);
    } // for
  } // ret
  return result;
}


/********************************************************************************************\
  Rules processing
  \*********************************************************************************************/
void rulesProcessing(String& event)
{
  static uint8_t* data;
  static byte nestingLevel;

  String log = "";

  nestingLevel++;
  if (nestingLevel > RULES_MAX_NESTING_LEVEL)
  {
    log = F("EVENT: Error: Nesting level exceeded!");
    addLog(LOG_LEVEL_ERROR, log);
    nestingLevel--;
    return;
  }

  log = F("EVENT: ");
  log += event;
  addLog(LOG_LEVEL_INFO, log);

  // load rules from flash memory, stored in offset block 10
  if (data == NULL)
  {
    data = new uint8_t[RULES_MAX_SIZE];
    File f = SD.open(F("rules.txt"));
    if (f)
    {
      byte *pointerToByteToRead = data;
      for (int x = 0; x < f.size(); x++)
      {
        *pointerToByteToRead = f.read();
        pointerToByteToRead++;// next byte
      }
      data[f.size()] = 0;
      f.close();
    }
    data[RULES_MAX_SIZE - 1] = 0; // make sure it's terminated!
  }

  int pos = 0;
  String line = "";
  boolean match = false;
  boolean codeBlock = false;
  boolean isCommand = false;
  boolean conditional = false;
  boolean condition = false;
  boolean ifBranche = false;

  while (data[pos] != 0)
  {
    if (data[pos] != 0 && data[pos] != 10)
      line += (char)data[pos];

    if (data[pos] == 10)    // if line complete, parse this rule
    {
      line.replace("\r", "");
      if (line.substring(0, 2) != "//" && line.length() > 0)
      {
        isCommand = true;

        int comment = line.indexOf("//");
        if (comment > 0)
          line = line.substring(0, comment);

        line = parseTemplate(line, line.length());
        line.trim();

        String lineOrg = line; // store original line for future use
        line.toLowerCase(); // convert all to lower case to make checks easier

        String eventTrigger = "";
        String action = "";

        if (!codeBlock)  // do not check "on" rules if a block of actions is to be processed
        {
          if (line.startsWith("on "))
          {
            line = line.substring(3);
            int split = line.indexOf(F(" do"));
            if (split != -1)
            {
              eventTrigger = line.substring(0, split);
              action = lineOrg.substring(split + 7);
              action.trim();
            }
            match = ruleMatch(event, eventTrigger);
            if (action.length() > 0) // single on/do/action line, no block
            {
              isCommand = true;
              codeBlock = false;
            }
            else
            {
              isCommand = false;
              codeBlock = true;
            }
          }
        }
        else
        {
          action = lineOrg;
        }

        String lcAction = action;
        lcAction.toLowerCase();
        if (lcAction == "endon") // Check if action block has ended, then we will wait for a new "on" rule
        {
          isCommand = false;
          codeBlock = false;
        }

        if (match) // rule matched for one action or a block of actions
        {
          int split = lcAction.indexOf(F("if ")); // check for optional "if" condition
          if (split != -1)
          {
            conditional = true;
            String check = lcAction.substring(split + 3);
            condition = conditionMatch(check);
            ifBranche = true;
            isCommand = false;
          }

          if (lcAction == F("else")) // in case of an "else" block of actions, set ifBranche to false
          {
            ifBranche = false;
            isCommand = false;
          }

          if (lcAction == F("endif")) // conditional block ends here
          {
            conditional = false;
            isCommand = false;
          }

          // process the action if it's a command and unconditional, or conditional and the condition matches the if or else block.
          if (isCommand && ((!conditional) || (conditional && (condition == ifBranche))))
          {
            int equalsPos = event.indexOf("=");
            if (equalsPos > 0)
            {
              String tmpString = event.substring(equalsPos + 1);
              action.replace(F("%eventvalue%"), tmpString); // substitute %eventvalue% in actions with the actual value from the event
            }
            log = F("ACT  : ");
            log += action;
            addLog(LOG_LEVEL_INFO, log);

            struct EventStruct TempEvent;
            parseCommandString(&TempEvent, action);
            //yield();
            if (!PluginCall(PLUGIN_WRITE, &TempEvent, action))
              ExecuteCommand(VALUE_SOURCE_SYSTEM, action.c_str());
            //yield();
          }
        }
      }

      line = "";
    }
    pos++;
  }

  nestingLevel--;
  if (nestingLevel == 0)
  {
    delete [] data;
    data = NULL;
  }
}


/********************************************************************************************\
  Check if an event matches to a given rule
  \*********************************************************************************************/
boolean ruleMatch(String& event, String& rule)
{
  boolean match = false;
  String tmpEvent = event;
  String tmpRule = rule;

  // Special handling of literal string events, they should start with '!'
  if (event.charAt(0) == '!')
  {
    if (event.equalsIgnoreCase(rule))
      return true;
    else
      return false;
  }

  if (event.startsWith(F("Clock#Time"))) // clock events need different handling...
  {
    int pos1 = event.indexOf("=");
    int pos2 = rule.indexOf("=");
    if (pos1 > 0 && pos2 > 0)
    {
      tmpEvent = event.substring(0, pos1);
      tmpRule  = rule.substring(0, pos2);
      if (tmpRule.equalsIgnoreCase(tmpEvent)) // if this is a clock rule
      {
        tmpEvent = event.substring(pos1 + 1);
        tmpRule  = rule.substring(pos2 + 1);
        unsigned long clockEvent = string2TimeLong(tmpEvent);
        unsigned long clockSet = string2TimeLong(tmpRule);
        unsigned long Mask;
        for (byte y = 0; y < 8; y++)
        {
          if (((clockSet >> (y * 4)) & 0xf) == 0xf)  // if nibble y has the wildcard value 0xf
          {
            Mask = 0xffffffff  ^ (0xFUL << (y * 4)); // Mask to wipe nibble position y.
            clockEvent &= Mask;                      // clear nibble
            clockEvent |= (0xFUL << (y * 4));        // fill with wildcard value 0xf
          }
        }
        if (clockEvent == clockSet)
          return true;
        else
          return false;
      }
    }
  }


  // parse event into verb and value
  float value = 0;
  int pos = event.indexOf("=");
  if (pos)
  {
    tmpEvent = event.substring(pos + 1);
    value = tmpEvent.toFloat();
    tmpEvent = event.substring(0, pos);
  }

  // parse rule
  int comparePos = 0;
  char compare = ' ';
  comparePos = rule.indexOf(">");
  if (comparePos > 0)
  {
    compare = '>';
  }
  else
  {
    comparePos = rule.indexOf("<");
    if (comparePos > 0)
    {
      compare = '<';
    }
    else
    {
      comparePos = rule.indexOf("=");
      if (comparePos > 0)
      {
        compare = '=';
      }
    }
  }

  float ruleValue = 0;

  if (comparePos > 0)
  {
    tmpRule = rule.substring(comparePos + 1);
    ruleValue = tmpRule.toFloat();
    tmpRule = rule.substring(0, comparePos);
  }

  switch (compare)
  {
    case '>':
      if (tmpRule.equalsIgnoreCase(tmpEvent) && value > ruleValue)
        match = true;
      break;

    case '<':
      if (tmpRule.equalsIgnoreCase(tmpEvent) && value < ruleValue)
        match = true;
      break;

    case '=':
      if (tmpRule.equalsIgnoreCase(tmpEvent) && value == ruleValue)
        match = true;
      break;

    case ' ':
      if (tmpRule.equalsIgnoreCase(tmpEvent))
        match = true;
      break;
  }

  return match;
}


/********************************************************************************************\
  Check expression
  \*********************************************************************************************/
boolean conditionMatch(String& check)
{
  boolean match = false;

  int comparePos = 0;
  char compare = ' ';
  comparePos = check.indexOf(">");
  if (comparePos > 0)
  {
    compare = '>';
  }
  else
  {
    comparePos = check.indexOf("<");
    if (comparePos > 0)
    {
      compare = '<';
    }
    else
    {
      comparePos = check.indexOf("=");
      if (comparePos > 0)
      {
        compare = '=';
      }
    }
  }

  float Value1 = 0;
  float Value2 = 0;

  if (comparePos > 0)
  {
    String tmpCheck = check.substring(comparePos + 1);
    Value2 = tmpCheck.toFloat();
    tmpCheck = check.substring(0, comparePos);
    Value1 = tmpCheck.toFloat();
  }
  else
    return false;

  switch (compare)
  {
    case '>':
      if (Value1 > Value2)
        match = true;
      break;

    case '<':
      if (Value1 < Value2)
        match = true;
      break;

    case '=':
      if (Value1 == Value2)
        match = true;
      break;
  }
  return match;
}


/********************************************************************************************\
  Check rule timers
  \*********************************************************************************************/
void rulesTimers()
{
  for (byte x = 0; x < RULES_TIMER_MAX; x++)
  {
    if (RulesTimer[x] != 0L) // timer active?
    {
      if (RulesTimer[x] < millis()) // timer finished?
      {
        RulesTimer[x] = 0L; // turn off this timer
        String event = F("Rules#Timer=");
        event += x + 1;
        rulesProcessing(event);
      }
    }
  }
}


/********************************************************************************************\
  Generate rule events based on task refresh
  \*********************************************************************************************/

void createRuleEvents(byte TaskIndex)
{
  LoadTaskSettings(TaskIndex);
  byte BaseVarIndex = TaskIndex * VARS_PER_TASK;
  byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[TaskIndex]);
  byte sensorType = Device[DeviceIndex].VType;
  for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
  {
    String eventString = ExtraTaskSettings.TaskDeviceName;
    eventString += F("#");
    eventString += ExtraTaskSettings.TaskDeviceValueNames[varNr];
    eventString += F("=");

    if (sensorType == SENSOR_TYPE_LONG)
      eventString += (unsigned long)UserVar[BaseVarIndex] + ((unsigned long)UserVar[BaseVarIndex + 1] << 16);
    else
      eventString += UserVar[BaseVarIndex + varNr];

    rulesProcessing(eventString);
  }
}

