/****************************************************************************************************************************\
 * Arduino project "Arduino Easy" © Copyright www.letscontrolit.com
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You received a copy of the GNU General Public License along with this program in file 'License.txt'.
 *
 * IDE download    : https://www.arduino.cc/en/Main/Software
 *
 * Source Code     : https://github.com/ESP8266nu/ESPEasy
 * Support         : http://www.letscontrolit.com
 * Discussion      : http://www.letscontrolit.com/forum/
 *
 * Additional information about licensing can be found at : http://www.gnu.org/licenses
\*************************************************************************************************************************/

// This file incorporates work covered by the following copyright and permission notice:

/****************************************************************************************************************************\
* Arduino project "Nodo" © Copyright 2010..2015 Paul Tonkes
*
* This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
* You received a copy of the GNU General Public License along with this program in file 'License.txt'.
*
* Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
* Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
* Compiler voor deze programmacode te downloaden op : http://arduino.cc
\*************************************************************************************************************************/

// ********************************************************************************
//   User specific configuration
// ********************************************************************************

// Set default configuration settings if you want (not mandatory)
// You can always change these during runtime and save to eeprom
// After loading firmware, issue a 'reset' command to load the defaults.

#define DEFAULT_NAME        "newdevice"         // Enter your device friendly name
#define DEFAULT_SERVER      "192.168.0.8"       // Enter your Domoticz Server IP address
#define DEFAULT_PORT        8080                // Enter your Domoticz Server port value
#define DEFAULT_DELAY       60                  // Enter your Send delay in seconds

#define DEFAULT_USE_STATIC_IP   false           // true or false enabled or disabled set static IP
#define DEFAULT_IP          "192.168.0.50"      // Enter your IP address
#define DEFAULT_DNS         "192.168.0.1"       // Enter your DNS
#define DEFAULT_GW          "192.168.0.1"       // Enter your gateway
#define DEFAULT_SUBNET      "255.255.255.0"     // Enter your subnet

#define DEFAULT_PROTOCOL    1                   // Protocol used for controller communications
#define UNIT                0

#define FEATURE_MQTT                    true
#define FEATURE_MQTT_DOM                false // Not tested yet!

// ********************************************************************************
//   DO NOT CHANGE ANYTHING BELOW THIS LINE
// ********************************************************************************

// Challenges on Arduino/W5100 ethernet platform:
// Only 4 ethernet sockets:
//  1: UPD traffic server/send
//  2: Webserver
//  3: MQTT client
//  4: Webclient, active when webserver serves an incoming client or outgoing webclient calls.

#define socketdebug                     false
#define ARDUINO_PROJECT_PID       2016110201L
#define VERSION                             2
#define BUILD                             147
#define BUILD_NOTES                        ""

#define NODE_TYPE_ID_ESP_EASY_STD           1
#define NODE_TYPE_ID_ESP_EASY4M_STD        17
#define NODE_TYPE_ID_ESP_EASY32_STD        33
#define NODE_TYPE_ID_ARDUINO_EASY_STD      65
#define NODE_TYPE_ID                        NODE_TYPE_ID_ARDUINO_EASY_STD

#define CPLUGIN_PROTOCOL_ADD                1
#define CPLUGIN_PROTOCOL_TEMPLATE           2
#define CPLUGIN_PROTOCOL_SEND               3
#define CPLUGIN_PROTOCOL_RECV               4
#define CPLUGIN_GET_DEVICENAME              5
#define CPLUGIN_WEBFORM_SAVE                6
#define CPLUGIN_WEBFORM_LOAD                7

#define LOG_LEVEL_ERROR                     1
#define LOG_LEVEL_INFO                      2
#define LOG_LEVEL_DEBUG                     3
#define LOG_LEVEL_DEBUG_MORE                4

#define CMD_REBOOT                         89

#define DEVICES_MAX                         8 // ESP Easy 64
#define TASKS_MAX                           8 // ESP Easy 12
#define VARS_PER_TASK                       4
#define PLUGIN_MAX                          8 // ESP Easy 64
#define PLUGIN_CONFIGVAR_MAX                8
#define PLUGIN_CONFIGFLOATVAR_MAX           4
#define PLUGIN_CONFIGLONGVAR_MAX            4
#define PLUGIN_EXTRACONFIGVAR_MAX          16
#define CPLUGIN_MAX                         4 // ESP Easy 16
#define UNIT_MAX                           32 // Only relevant for UDP unicast message 'sweeps' and the nodelist.
#define RULES_TIMER_MAX                     8
#define SYSTEM_TIMER_MAX                    2 // ESP Easy  8
#define SYSTEM_CMD_TIMER_MAX                1 // ESP Easy  2
#define PINSTATE_TABLE_MAX                 16 // ESP Easy 32
#define RULES_MAX_SIZE                    512 // ESP Easy 2048
#define RULES_MAX_NESTING_LEVEL             3

#define PIN_MODE_UNDEFINED                  0
#define PIN_MODE_INPUT                      1
#define PIN_MODE_OUTPUT                     2
#define PIN_MODE_PWM                        3
#define PIN_MODE_SERVO                      4

#define SEARCH_PIN_STATE                 true
#define NO_SEARCH_PIN_STATE             false

#define DEVICE_TYPE_SINGLE                  1  // connected through 1 datapin
#define DEVICE_TYPE_I2C                     2  // connected through I2C
#define DEVICE_TYPE_ANALOG                  3  // tout pin
#define DEVICE_TYPE_DUAL                    4  // connected through 2 datapins
#define DEVICE_TYPE_DUMMY                  99  // Dummy device, has no physical connection

#define SENSOR_TYPE_SINGLE                  1
#define SENSOR_TYPE_TEMP_HUM                2
#define SENSOR_TYPE_TEMP_BARO               3
#define SENSOR_TYPE_TEMP_HUM_BARO           4
#define SENSOR_TYPE_DUAL                    5
#define SENSOR_TYPE_TRIPLE                  6
#define SENSOR_TYPE_QUAD                    7
#define SENSOR_TYPE_SWITCH                 10
#define SENSOR_TYPE_DIMMER                 11
#define SENSOR_TYPE_LONG                   20

#define PLUGIN_INIT_ALL                     1
#define PLUGIN_INIT                         2
#define PLUGIN_READ                         3
#define PLUGIN_ONCE_A_SECOND                4
#define PLUGIN_TEN_PER_SECOND               5
#define PLUGIN_DEVICE_ADD                   6
#define PLUGIN_EVENTLIST_ADD                7
#define PLUGIN_WEBFORM_SAVE                 8
#define PLUGIN_WEBFORM_LOAD                 9
#define PLUGIN_WEBFORM_SHOW_VALUES         10
#define PLUGIN_GET_DEVICENAME              11
#define PLUGIN_GET_DEVICEVALUENAMES        12
#define PLUGIN_WRITE                       13
#define PLUGIN_EVENT_OUT                   14
#define PLUGIN_WEBFORM_SHOW_CONFIG         15
#define PLUGIN_SERIAL_IN                   16
#define PLUGIN_UDP_IN                      17
#define PLUGIN_CLOCK_IN                    18
#define PLUGIN_TIMER_IN                    19

#define VALUE_SOURCE_SYSTEM                 1
#define VALUE_SOURCE_SERIAL                 2
#define VALUE_SOURCE_HTTP                   3
#define VALUE_SOURCE_MQTT                   4
#define VALUE_SOURCE_UDP                    5

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#if FEATURE_MQTT
#include <PubSubClient.h>
#include <ArduinoJson.h>
#endif

void(*Reboot)(void)=0;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// WebServer
EthernetServer WebServer(80);

#if FEATURE_MQTT
EthernetClient mqtt;
PubSubClient MQTTclient(mqtt);
#endif

#define EthernetShield_CS_SDCard     4
#define EthernetShield_CS_W5100     10  

// syslog stuff
EthernetUDP portUDP;

struct SecurityStruct
{
  char          ControllerUser[26];
  char          ControllerPassword[64];
  char          Password[26];
} SecuritySettings;

struct SettingsStruct
{
  unsigned long PID;
  int           Version;
  byte          Unit;
  int16_t       Build;
  byte          IP[4];
  byte          Gateway[4];
  byte          Subnet[4];
  byte          DNS[4];
  byte          Controller_IP[4];
  unsigned int  ControllerPort;
  byte          IP_Octet;
  char          NTPHost[64];
  unsigned long Delay;
  byte          Syslog_IP[4];
  unsigned int  UDPPort;
  byte          Protocol;
  char          Name[26];
  byte          SyslogLevel;
  byte          SerialLogLevel;
  unsigned long BaudRate;
  unsigned long MessageDelay;
  boolean       CustomCSS;
  char          ControllerHostName[64];
  boolean       UseNTP;
  boolean       DST;
  byte          WDI2CAddress;
  int8_t        PinBootStates[17];
  byte          UseDNS;
  boolean       UseRules;
  int8_t        Pin_status_led;
  boolean       UseSerial;
  boolean       GlobalSync;
  unsigned long ConnectionFailuresThreshold;
  int16_t       TimeZone;
  byte          SDLogLevel;
  byte          TaskDeviceNumber[TASKS_MAX];
  unsigned int  TaskDeviceID[TASKS_MAX];
  int8_t        TaskDevicePin1[TASKS_MAX];
  int8_t        TaskDevicePin2[TASKS_MAX];
  byte          TaskDevicePort[TASKS_MAX];
  boolean       TaskDevicePin1PullUp[TASKS_MAX];
  int16_t       TaskDevicePluginConfig[TASKS_MAX][PLUGIN_CONFIGVAR_MAX];
  boolean       TaskDevicePin1Inversed[TASKS_MAX];
  float         TaskDevicePluginConfigFloat[TASKS_MAX][PLUGIN_CONFIGFLOATVAR_MAX];
  long          TaskDevicePluginConfigLong[TASKS_MAX][PLUGIN_CONFIGLONGVAR_MAX];
  boolean       TaskDeviceSendData[TASKS_MAX];
  boolean       TaskDeviceGlobalSync[TASKS_MAX];
  int8_t        TaskDevicePin3[TASKS_MAX];
  byte          TaskDeviceDataFeed[TASKS_MAX];
  unsigned long TaskDeviceTimer[TASKS_MAX];
  boolean       MQTTRetainFlag;
  char          MQTTpublish[81];
  char          MQTTsubscribe[81];
} Settings;

struct ExtraTaskSettingsStruct
{
  byte    TaskIndex;
  char    TaskDeviceName[41];
  char    TaskDeviceFormula[VARS_PER_TASK][41];
  char    TaskDeviceValueNames[VARS_PER_TASK][41];
  long    TaskDevicePluginConfigLong[PLUGIN_EXTRACONFIGVAR_MAX];
  byte    TaskDeviceValueDecimals[VARS_PER_TASK];
} ExtraTaskSettings;

struct EventStruct
{
  byte Source;
  byte TaskIndex;
  byte BaseVarIndex;
  int idx;
  byte sensorType;
  int Par1;
  int Par2;
  int Par3;
  byte OriginTaskIndex;
  String String1;
  String String2;
  byte *Data;
};

struct DeviceStruct
{
  byte Number;
  byte Type;
  byte VType;
  byte Ports;
  boolean PullUpOption;
  boolean InverseLogicOption;
  boolean FormulaOption;
  byte ValueCount;
  boolean Custom;
  boolean SendDataOption;
  boolean GlobalSyncOption;
  boolean TimerOption;
  boolean TimerOptional;
  boolean DecimalsOnly;
} Device[DEVICES_MAX + 1]; // 1 more because first device is empty device

struct ProtocolStruct
{
  byte Number;
  boolean usesMQTT;
  boolean usesAccount;
  boolean usesPassword;
  int defaultPort;
  boolean usesTemplate;
} Protocol[CPLUGIN_MAX];

struct NodeStruct
{
  byte ip[4];
  byte age;
  uint16_t build;
} Nodes[UNIT_MAX];

struct systemTimerStruct
{
  unsigned long timer;
  byte plugin;
  byte Par1;
  byte Par2;
  byte Par3;
} systemTimers[SYSTEM_TIMER_MAX];

struct systemCMDTimerStruct
{
  unsigned long timer;
  String action;
} systemCMDTimers[SYSTEM_CMD_TIMER_MAX];

struct pinStatesStruct
{
  byte plugin;
  byte index;
  byte mode;
  uint16_t value;
} pinStates[PINSTATE_TABLE_MAX];

int deviceCount = -1;
int protocolCount = -1;

boolean printToWeb = false;
String printWebString = "";
boolean printToWebJSON = false;

float UserVar[VARS_PER_TASK * TASKS_MAX];
unsigned long RulesTimer[RULES_TIMER_MAX];

unsigned long timerSensor[TASKS_MAX];
unsigned long timer;
unsigned long timer100ms;
unsigned long timer1s;
unsigned long timerwd;
unsigned long lastSend;
byte cmd_within_mainloop = 0;
unsigned long connectionFailures;
unsigned long wdcounter = 0;

boolean WebLoggedIn = false;
int WebLoggedInTimer = 300;

boolean (*Plugin_ptr[PLUGIN_MAX])(byte, struct EventStruct*, String&);
byte Plugin_id[PLUGIN_MAX];

boolean (*CPlugin_ptr[CPLUGIN_MAX])(byte, struct EventStruct*, String&);
byte CPlugin_id[CPLUGIN_MAX];

String dummyString = "";

boolean systemOK = false;

unsigned long start = 0;
unsigned long elapsed = 0;
unsigned long loopCounter = 0;
unsigned long loopCounterLast = 0;
unsigned long loopCounterMax = 1;

unsigned long flashWrites = 0;

String eventBuffer = "";

/*********************************************************************************************\
 * SETUP
\*********************************************************************************************/
void setup()
{
  Serial.begin(115200);
  
  fileSystemCheck();

  emergencyReset();

  LoadSettings();

  ExtraTaskSettings.TaskIndex = 255; // make sure this is an unused nr to prevent cache load on boot

  // if different version, eeprom settings structure has changed. Full Reset needed
  // on a fresh ESP module eeprom values are set to 255. Version results into -1 (signed int)
  if (Settings.Version == VERSION && Settings.PID == ARDUINO_PROJECT_PID)
  {
    systemOK = true;
  }
  else
  {
    // Direct Serial is allowed here, since this is only an emergency task.
    Serial.print(F("\nPID:"));
    Serial.println(Settings.PID);
    Serial.print(F("Version:"));
    Serial.println(Settings.Version);
    Serial.println(F("INIT : Incorrect PID or version!"));
    delay(1000);
    ResetFactory();
  }

  if (systemOK)
  {
    if (Settings.UseSerial)
      Serial.begin(Settings.BaudRate);

    if (Settings.Build != BUILD)
      BuildFixes();

    String log = F("\nINIT : Booting Build nr:");
    log += BUILD;
    addLog(LOG_LEVEL_INFO, log);

    hardwareInit();
    PluginInit();
    CPluginInit();

    mac[5] = Settings.Unit; // make sure every unit has a unique mac address
    if (Settings.IP[0] == 0)
      Ethernet.begin(mac);
    else
      Ethernet.begin(mac, Settings.IP, Settings.DNS, Settings.Gateway, Settings.Subnet);

    // setup UDP
    if (Settings.UDPPort != 0)
      portUDP.begin(Settings.UDPPort);

#if FEATURE_MQTT
    // Setup MQTT Client
    byte ProtocolIndex = getProtocolIndex(Settings.Protocol);
    if (Protocol[ProtocolIndex].usesMQTT)
      MQTTConnect();
#endif

    sendSysInfoUDP(3);

    log = F("INIT : Boot OK");
    addLog(LOG_LEVEL_INFO, log);

    // Setup timers
    byte bootMode = 0;
    if (bootMode == 0)
    {
      for (byte x = 0; x < TASKS_MAX; x++)
        if (Settings.TaskDeviceTimer[x] !=0)
          timerSensor[x] = millis() + 30000 + (x * Settings.MessageDelay);
      
      timer = millis() + 30000; // startup delay 30 sec
    }
    else
    {
      for (byte x = 0; x < TASKS_MAX; x++)
        timerSensor[x] = millis() + 0;
      timer = millis() + 0; // no startup from deepsleep wake up
    }

    timer100ms = millis() + 100; // timer for periodic actions 10 x per/sec
    timer1s = millis() + 1000; // timer for periodic actions once per/sec
    timerwd = millis() + 30000; // timer for watchdog once per 30 sec

    if (Settings.UseNTP)
      initTime();

    if (Settings.UseRules)
    {
      String event = F("System#Boot");
      rulesProcessing(event);
    }

  }
  else
  {
    Serial.println(F("Entered Rescue mode!"));
  }
}


/*********************************************************************************************\
 * MAIN LOOP
\*********************************************************************************************/
void loop()
{
  loopCounter++;

  if (Settings.UseSerial)
    if (Serial.available())
      if (!PluginCall(PLUGIN_SERIAL_IN, 0, dummyString))
        serial();

  if (systemOK)
  {
    if (millis() > timer100ms)
      run10TimesPerSecond();

    if (millis() > timer1s)
      runOncePerSecond();

    if (millis() > timerwd)
      runEach30Seconds();

    backgroundtasks();

  }
  else
    delay(1);
}


/*********************************************************************************************\
 * Tasks that run 10 times per second
\*********************************************************************************************/
void run10TimesPerSecond()
{
  start = micros();
  timer100ms = millis() + 100;
  PluginCall(PLUGIN_TEN_PER_SECOND, 0, dummyString);
  checkUDP();
  if (Settings.UseRules && eventBuffer.length() > 0)
  {
    rulesProcessing(eventBuffer);
    eventBuffer = "";
  }
  elapsed = micros() - start;
}


/*********************************************************************************************\
 * Tasks each second
\*********************************************************************************************/
void runOncePerSecond()
{
  timer1s = millis() + 1000;

  checkSensors();

  if (Settings.ConnectionFailuresThreshold)
    if (connectionFailures > Settings.ConnectionFailuresThreshold)
      delayedReboot(60);

  if (cmd_within_mainloop != 0)
  {
    switch (cmd_within_mainloop)
    {
      case CMD_REBOOT:
        {
          Reboot();
          break;
        }
    }
    cmd_within_mainloop = 0;
  }

  // clock events
  if (Settings.UseNTP)
    checkTime();

  unsigned long timer = micros();
  PluginCall(PLUGIN_ONCE_A_SECOND, 0, dummyString);

  checkSystemTimers();

  if (Settings.UseRules)
    rulesTimers();

  timer = micros() - timer;

  if (SecuritySettings.Password[0] != 0)
  {
    if (WebLoggedIn)
      WebLoggedInTimer++;
    if (WebLoggedInTimer > 300)
      WebLoggedIn = false;
  }

  // I2C Watchdog feed
  if (Settings.WDI2CAddress != 0)
  {
    Wire.beginTransmission(Settings.WDI2CAddress);
    Wire.write(0xA5);
    Wire.endTransmission();
  }

  if (Settings.SerialLogLevel == 5)
  {
    Serial.print(F("10 ps:"));
    Serial.print(elapsed);
    Serial.print(F(" uS  1 ps:"));
    Serial.println(timer);
  }
}

/*********************************************************************************************\
 * Tasks each 30 seconds
\*********************************************************************************************/
void runEach30Seconds()
{
  wdcounter++;
  timerwd = millis() + 30000;
  String log = F("WD   : Uptime ");
  log += wdcounter / 2;
  log += F(" ConnectFailures ");
  log += connectionFailures;
  log += F(" Freemem ");
  log += FreeMem();
  addLog(LOG_LEVEL_INFO, log);
  sendSysInfoUDP(1);
  refreshNodeList();
  loopCounterLast = loopCounter;
  loopCounter = 0;
  if (loopCounterLast > loopCounterMax)
    loopCounterMax = loopCounterLast;
  
}


/*********************************************************************************************\
 * Check sensor timers
\*********************************************************************************************/
void checkSensors()
{
    for (byte x = 0; x < TASKS_MAX; x++)
    {
      if ((Settings.TaskDeviceTimer[x] != 0) && (millis() > timerSensor[x]))
      {
        timerSensor[x] = millis() + Settings.TaskDeviceTimer[x] * 1000;
        if (timerSensor[x] == 0) // small fix if result is 0, else timer will be stopped...
          timerSensor[x] = 1;
        SensorSendTask(x);
      }
    }
}


/*********************************************************************************************\
 * send all sensordata
\*********************************************************************************************/
void SensorSend()
{
  for (byte x = 0; x < TASKS_MAX; x++)
  {
    SensorSendTask(x);
  }
}


/*********************************************************************************************\
 * send specific sensor task data
\*********************************************************************************************/
void SensorSendTask(byte TaskIndex)
{
  if (Settings.TaskDeviceID[TaskIndex] != 0)
  {
    byte varIndex = TaskIndex * VARS_PER_TASK;

    boolean success = false;
    byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[TaskIndex]);
    LoadTaskSettings(TaskIndex);

    struct EventStruct TempEvent;
    TempEvent.TaskIndex = TaskIndex;
    TempEvent.BaseVarIndex = varIndex;
    TempEvent.idx = Settings.TaskDeviceID[TaskIndex];
    TempEvent.sensorType = Device[DeviceIndex].VType;

    float preValue[VARS_PER_TASK]; // store values before change, in case we need it in the formula
    for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
      preValue[varNr] = UserVar[varIndex + varNr];

    if(Settings.TaskDeviceDataFeed[TaskIndex] == 0)  // only read local connected sensorsfeeds
      success = PluginCall(PLUGIN_READ, &TempEvent, dummyString);
    else
      success = true;

    if (success)
    {
      for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
      {  
        if (ExtraTaskSettings.TaskDeviceFormula[varNr][0] != 0)
        {
          String spreValue = String(preValue[varNr]);
          String formula = ExtraTaskSettings.TaskDeviceFormula[varNr];
          float value = UserVar[varIndex + varNr];
          float result = 0;
          String svalue = String(value);
          formula.replace("%pvalue%", spreValue);
          formula.replace("%value%", svalue);
          byte error = Calculate(formula.c_str(), &result);
          if (error == 0)
            UserVar[varIndex + varNr] = result;
        }
      }
      sendData(&TempEvent);
    }
  }
}


/*********************************************************************************************\
 * set global system timer
\*********************************************************************************************/
boolean setSystemTimer(unsigned long timer, byte plugin, byte Par1, byte Par2, byte Par3)
{
  // plugin number and par1 form a unique key that can be used to restart a timer
  // first check if a timer is not already running for this request
  boolean reUse = false;
  for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
    if (systemTimers[x].timer != 0)
    {
      if ((systemTimers[x].plugin == plugin) && (systemTimers[x].Par1 == Par1))
      {
        systemTimers[x].timer = millis() + timer;
        reUse = true;
        break;
      }
    }

  if (!reUse)
  {
    // find a new free timer slot...
    for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
      if (systemTimers[x].timer == 0)
      {
        systemTimers[x].timer = millis() + timer;
        systemTimers[x].plugin = plugin;
        systemTimers[x].Par1 = Par1;
        systemTimers[x].Par2 = Par2;
        systemTimers[x].Par3 = Par3;
        break;
      }
  }
}


/*********************************************************************************************\
 * set global system command timer
\*********************************************************************************************/
boolean setSystemCMDTimer(unsigned long timer, String& action)
{
  for (byte x = 0; x < SYSTEM_CMD_TIMER_MAX; x++)
    if (systemCMDTimers[x].timer == 0)
    {
      systemCMDTimers[x].timer = millis() + timer;
      systemCMDTimers[x].action = action;
      break;
    }
}


/*********************************************************************************************\
 * check global system timers
\*********************************************************************************************/
boolean checkSystemTimers()
{
  for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
    if (systemTimers[x].timer != 0)
    {
      if (timeOut(systemTimers[x].timer))
      {
        struct EventStruct TempEvent;
        TempEvent.Par1 = systemTimers[x].Par1;
        TempEvent.Par2 = systemTimers[x].Par2;
        TempEvent.Par3 = systemTimers[x].Par3;
        for (byte y = 0; y < PLUGIN_MAX; y++)
          if (Plugin_id[y] == systemTimers[x].plugin)
            Plugin_ptr[y](PLUGIN_TIMER_IN, &TempEvent, dummyString);
        systemTimers[x].timer = 0;
      }
    }

  for (byte x = 0; x < SYSTEM_CMD_TIMER_MAX; x++)
    if (systemCMDTimers[x].timer != 0)
      if (timeOut(systemCMDTimers[x].timer))
      {
        struct EventStruct TempEvent;
        parseCommandString(&TempEvent, systemCMDTimers[x].action);
        if (!PluginCall(PLUGIN_WRITE, &TempEvent, systemCMDTimers[x].action))
          ExecuteCommand(VALUE_SOURCE_SYSTEM, systemCMDTimers[x].action.c_str());
        systemCMDTimers[x].timer = 0;
        systemCMDTimers[x].action = "";
      }
}


/*********************************************************************************************\
 * run background tasks
\*********************************************************************************************/
void backgroundtasks()
{
  WebServerHandleClient();
#if FEATURE_MQTT
  MQTTclient.loop();
#endif
  statusLED(false);
  checkUDP();
}

