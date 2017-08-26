/*********************************************************************************************\
   Syslog client
  \*********************************************************************************************/
void syslog(const char *message)
{
  if (Settings.Syslog_IP[0] != 0)
  {
    IPAddress broadcastIP(Settings.Syslog_IP[0], Settings.Syslog_IP[1], Settings.Syslog_IP[2], Settings.Syslog_IP[3]);
    portUDP.beginPacket(broadcastIP, 514);
    char str[256];
    str[0] = 0;
    snprintf_P(str, sizeof(str), PSTR("<7>ESP Unit: %u : %s"), Settings.Unit, message);
    portUDP.write(str);
    portUDP.endPacket();
  }
}


/*********************************************************************************************\
   Structs for UDP messaging
  \*********************************************************************************************/
struct infoStruct
{
  byte header = 255;
  byte ID = 3;
  byte sourcelUnit;
  byte destUnit;
  byte sourceTaskIndex;
  byte destTaskIndex;
  byte deviceNumber;
  char taskName[26];
  char ValueNames[VARS_PER_TASK][26];
};

struct dataStruct
{
  byte header = 255;
  byte ID = 5;
  byte sourcelUnit;
  byte destUnit;
  byte sourceTaskIndex;
  byte destTaskIndex;
  float Values[VARS_PER_TASK];
};

/*********************************************************************************************\
   Check UDP messages
  \*********************************************************************************************/
void checkUDP()
{
  if (Settings.UDPPort == 0)
    return;

  // UDP events
  int packetSize = portUDP.parsePacket();
  if (packetSize)
  {
    statusLED(true);

    IPAddress remoteIP = portUDP.remoteIP();
    if (portUDP.remotePort() == 123)
    {
      // unexpected NTP reply, drop for now...
      return;
    }
    byte packetBuffer[128];
    int len = portUDP.read(packetBuffer, 128);
    if (packetBuffer[0] != 255)
    {
      packetBuffer[len] = 0;
      addLog(LOG_LEVEL_DEBUG, (char*)packetBuffer);
      struct EventStruct TempEvent;
      String request = (char*)packetBuffer;
      parseCommandString(&TempEvent, request);
      TempEvent.Source = VALUE_SOURCE_SYSTEM;
      if (!PluginCall(PLUGIN_WRITE, &TempEvent, request))
        ExecuteCommand(VALUE_SOURCE_SYSTEM, (char*)packetBuffer);
    }
    else
    {
      if (packetBuffer[1] > 1 && packetBuffer[1] < 6)
      {
        String log = (F("UDP  : Sensor msg "));
        for (byte x = 1; x < 6; x++)
        {
          log += " ";
          log += (int)packetBuffer[x];
        }
        addLog(LOG_LEVEL_DEBUG_MORE, log);
      }

      // binary data!
      switch (packetBuffer[1])
      {
        case 1: // sysinfo message
          {
            byte mac[6];
            byte ip[4];
            byte unit = packetBuffer[12];
            for (byte x = 0; x < 6; x++)
              mac[x] = packetBuffer[x + 2];
            for (byte x = 0; x < 4; x++)
              ip[x] = packetBuffer[x + 8];

            if (unit < UNIT_MAX)
            {
              for (byte x = 0; x < 4; x++)
                Nodes[unit].ip[x] = packetBuffer[x + 8];
              Nodes[unit].age = 0; // reset 'age counter'
              if (len >20) // extended packet size
              {
                Nodes[unit].build = packetBuffer[13] + 256*packetBuffer[14];
                #if FEATURE_NODELIST_NAMES
                //if (Nodes[unit].nodeName==0)
                //    Nodes[unit].nodeName =  (char *)malloc(FEATURE_NODELIST_NAMESSIZE+1);
                memcpy(&Nodes[unit].nodeName,(byte*)packetBuffer+15,FEATURE_NODELIST_NAMESSIZE);
                Nodes[unit].nodeName[FEATURE_NODELIST_NAMESSIZE]=0;
                Nodes[unit].nodeType = packetBuffer[40];
                #endif
              }
            }

            char macaddress[20];
            sprintf_P(macaddress, PSTR("%02x:%02x:%02x:%02x:%02x:%02x"), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            char ipaddress[16];
            sprintf_P(ipaddress, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
            char log[80];
            sprintf_P(log, PSTR("UDP  : %s,%s,%u"), macaddress, ipaddress, unit);
            addLog(LOG_LEVEL_DEBUG_MORE, log);
            break;
          }

        case 2: // sensor info pull request
          {
            SendUDPTaskInfo(packetBuffer[2], packetBuffer[5], packetBuffer[4]);
            break;
          }

        case 3: // sensor info
          {
            if (Settings.GlobalSync)
            {
              struct infoStruct infoReply;
              memcpy((byte*)&infoReply, (byte*)&packetBuffer, sizeof(infoStruct));

              // to prevent flash wear out (bugs in communication?) we can only write to an empty task
              // so it will write only once and has to be cleared manually through webgui
              if (Settings.TaskDeviceNumber[infoReply.destTaskIndex] == 0)
              {
                Settings.TaskDeviceNumber[infoReply.destTaskIndex] = infoReply.deviceNumber;
                Settings.TaskDeviceDataFeed[infoReply.destTaskIndex] = 1;  // remote feed
                Settings.TaskDeviceSendData[infoReply.destTaskIndex] = false;
                strcpy(ExtraTaskSettings.TaskDeviceName, infoReply.taskName);
                for (byte x = 0; x < VARS_PER_TASK; x++)
                  strcpy( ExtraTaskSettings.TaskDeviceValueNames[x], infoReply.ValueNames[x]);
                SaveTaskSettings(infoReply.destTaskIndex);
                SaveSettings();
              }
            }
            break;
          }

        case 4: // sensor data pull request
          {
            SendUDPTaskData(packetBuffer[2], packetBuffer[5], packetBuffer[4]);
            break;
          }

        case 5: // sensor data
          {
            if (Settings.GlobalSync)
            {
              struct dataStruct dataReply;
              memcpy((byte*)&dataReply, (byte*)&packetBuffer, sizeof(dataStruct));

              // only if this task has a remote feed, update values
              if (Settings.TaskDeviceDataFeed[dataReply.destTaskIndex] != 0)
              {
                for (byte x = 0; x < VARS_PER_TASK; x++)
                {
                  UserVar[dataReply.destTaskIndex * VARS_PER_TASK + x] = dataReply.Values[x];
                }
                if (Settings.UseRules)
                  createRuleEvents(dataReply.destTaskIndex);
              }
            }
            break;
          }

        default:
          {
            struct EventStruct TempEvent;
            TempEvent.Data = (byte*)packetBuffer;
            TempEvent.Par1 = remoteIP[3];
            PluginCall(PLUGIN_UDP_IN, &TempEvent, dummyString);
            break;
          }
      }
    }
  }
}


/*********************************************************************************************\
   Send task info using UDP message
  \*********************************************************************************************/
void SendUDPTaskInfo(byte destUnit, byte sourceTaskIndex, byte destTaskIndex)
{
  struct infoStruct infoReply;
  infoReply.sourcelUnit = Settings.Unit;
  infoReply.sourceTaskIndex = sourceTaskIndex;
  infoReply.destTaskIndex = destTaskIndex;
  LoadTaskSettings(infoReply.sourceTaskIndex);
  infoReply.deviceNumber = Settings.TaskDeviceNumber[infoReply.sourceTaskIndex];
  strcpy(infoReply.taskName, ExtraTaskSettings.TaskDeviceName);
  for (byte x = 0; x < VARS_PER_TASK; x++)
    strcpy(infoReply.ValueNames[x], ExtraTaskSettings.TaskDeviceValueNames[x]);

  byte firstUnit = 1;
  byte lastUnit = UNIT_MAX - 1;
  if (destUnit != 0)
  {
    firstUnit = destUnit;
    lastUnit = destUnit;
  }
  for (byte x = firstUnit; x <= lastUnit; x++)
  {
    infoReply.destUnit = x;
    sendUDP(x, (byte*)&infoReply, sizeof(infoStruct));
    delay(10);
  }
  delay(50);
}


/*********************************************************************************************\
   Send task data using UDP message
  \*********************************************************************************************/
void SendUDPTaskData(byte destUnit, byte sourceTaskIndex, byte destTaskIndex)
{
  struct dataStruct dataReply;
  dataReply.sourcelUnit = Settings.Unit;
  dataReply.sourceTaskIndex = sourceTaskIndex;
  dataReply.destTaskIndex = destTaskIndex;
  for (byte x = 0; x < VARS_PER_TASK; x++)
    dataReply.Values[x] = UserVar[dataReply.sourceTaskIndex * VARS_PER_TASK + x];

  byte firstUnit = 1;
  byte lastUnit = UNIT_MAX - 1;
  if (destUnit != 0)
  {
    firstUnit = destUnit;
    lastUnit = destUnit;
  }
  for (byte x = firstUnit; x <= lastUnit; x++)
  {
    dataReply.destUnit = x;
    sendUDP(x, (byte*) &dataReply, sizeof(dataStruct));
    delay(10);
  }
  delay(50);
}


/*********************************************************************************************\
   Send event using UDP message
  \*********************************************************************************************/
void SendUDPCommand(byte destUnit, char* data, byte dataLength)
{
  byte firstUnit = 1;
  byte lastUnit = UNIT_MAX - 1;
  if (destUnit != 0)
  {
    firstUnit = destUnit;
    lastUnit = destUnit;
  }
  for (byte x = firstUnit; x <= lastUnit; x++)
  {
    sendUDP(x, (byte*)data, dataLength);
    delay(10);
  }
  delay(50);
}


/*********************************************************************************************\
   Send UDP message
  \*********************************************************************************************/
void sendUDP(byte unit, byte* data, byte size)
{
  if (Nodes[unit].ip[0] == 0)
    return;
  String log = F("UDP  : Send UDP message to ");
  log += unit;
  addLog(LOG_LEVEL_DEBUG_MORE, log);

  statusLED(true);

  IPAddress remoteNodeIP(Nodes[unit].ip[0], Nodes[unit].ip[1], Nodes[unit].ip[2], Nodes[unit].ip[3]);
  portUDP.beginPacket(remoteNodeIP, Settings.UDPPort);
  portUDP.write(data, size);
  portUDP.endPacket();
}


/*********************************************************************************************\
   Refresh aging for remote units, drop if too old...
  \*********************************************************************************************/
void refreshNodeList()
{
  for (byte counter = 0; counter < UNIT_MAX; counter++)
  {
    if (Nodes[counter].ip[0] != 0)
    {
      Nodes[counter].age++;  // increment age counter
      if (Nodes[counter].age > 10) // if entry to old, clear this node ip from the list.
        for (byte x = 0; x < 4; x++)
          Nodes[counter].ip[x] = 0;
    }
  }
}

void sendSysInfoUDP(byte repeats)
{
  char log[80];
  if (Settings.UDPPort == 0)
    return;

  // 1 byte 'binary token 255'
  // 1 byte id '1'
  // 6 byte mac
  // 4 byte ip
  // 1 byte unit
  // 2 byte build
  // 25 char name
  // 1 byte node type id

  // send my info to the world...
  strcpy_P(log, PSTR("UDP  : Send Sysinfo message"));
  addLog(LOG_LEVEL_DEBUG_MORE, log);
  for (byte counter = 0; counter < repeats; counter++)
  {
    byte data[80];
    data[0] = 255;
    data[1] = 1;
    for (byte x = 0; x < 6; x++)
      data[x + 2] = mac[x];
    IPAddress ip = Ethernet.localIP();
    for (byte x = 0; x < 4; x++)
      data[x + 8] = ip[x];
    data[12] = Settings.Unit;
    data[13] = Settings.Build & 0xff;
    data[14] = Settings.Build >> 8;
    memcpy((byte*)data+15,Settings.Name,25);
    data[40] = NODE_TYPE_ID;
    statusLED(true);

    IPAddress broadcastIP(255, 255, 255, 255);
    portUDP.beginPacket(broadcastIP, Settings.UDPPort);
    portUDP.write(data, 80);
    portUDP.endPacket();
    if (counter < (repeats - 1))
      delay(500);
  }

  // store my own info also in the list...
  if (Settings.Unit < UNIT_MAX)
  {
    IPAddress ip = Ethernet.localIP();
    for (byte x = 0; x < 4; x++)
      Nodes[Settings.Unit].ip[x] = ip[x];
    Nodes[Settings.Unit].age = 0;
    Nodes[Settings.Unit].build = Settings.Build;
    Nodes[Settings.Unit].nodeType = NODE_TYPE_ID;    
  }
}

