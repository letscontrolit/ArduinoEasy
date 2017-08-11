#define INPUT_COMMAND_SIZE          80
void ExecuteCommand(byte source, const char *Line)
{
  String status = "";
  boolean success = false;
  char TmpStr1[80];
  TmpStr1[0] = 0;
  char Command[80];
  Command[0] = 0;
  int Par1 = 0;
  int Par2 = 0;
  int Par3 = 0;

  GetArgv(Line, Command, 1);
  if (GetArgv(Line, TmpStr1, 2)) Par1 = str2int(TmpStr1);
  if (GetArgv(Line, TmpStr1, 3)) Par2 = str2int(TmpStr1);
  if (GetArgv(Line, TmpStr1, 4)) Par3 = str2int(TmpStr1);

  // ****************************************
  // commands for debugging
  // ****************************************

  if (strcasecmp_P(Command, PSTR("w5100")) == 0)
  {
    success = true;
    ShowSocketStatus();
  }

  if (strcasecmp_P(Command, PSTR("ntp")) == 0)
  {
    success = true;
    getNtpTime();
  }

  if (strcasecmp_P(Command, PSTR("sdcard")) == 0)
  {
    success = true;
    SelectSDCard(true);
    File root = SD.open("/");
    root.rewindDirectory();
    printDirectory(root, 0);
    root.close();
  }
    
  if (strcasecmp_P(Command, PSTR("sysload")) == 0)
  {
    success = true;
    Serial.print(100 - (100 * loopCounterLast / loopCounterMax));
    Serial.print(F("% (LC="));
    Serial.print(int(loopCounterLast / 30));
    Serial.println(F(")"));
  }
  
  if (strcasecmp_P(Command, PSTR("meminfo")) == 0)
  {
    success = true;
    Serial.print(F("SecurityStruct         : "));
    Serial.println(sizeof(SecuritySettings));
    Serial.print(F("SettingsStruct         : "));
    Serial.println(sizeof(Settings));
    Serial.print(F("ExtraTaskSettingsStruct: "));
    Serial.println(sizeof(ExtraTaskSettings));
  }

  if (strcasecmp_P(Command, PSTR("TaskClear")) == 0)
  {
    success = true;
    taskClear(Par1 - 1, true);
  }

  if (strcasecmp_P(Command, PSTR("build")) == 0)
  {
    success = true;
    Settings.Build = Par1;
    SaveSettings();
  }

  // ****************************************
  // commands for rules
  // ****************************************

  if (strcasecmp_P(Command, PSTR("TaskValueSet")) == 0)
  {
    success = true;
    if (GetArgv(Line, TmpStr1, 4))
    {
      float result = 0;
      byte error = Calculate(TmpStr1, &result);
      UserVar[(VARS_PER_TASK * (Par1 - 1)) + Par2 - 1] = result;
    }
  }

  if (strcasecmp_P(Command, PSTR("TaskRun")) == 0)
  {
    success = true;
    SensorSendTask(Par1 -1);
  }

  if (strcasecmp_P(Command, PSTR("TimerSet")) == 0)
  {
    success = true;
    RulesTimer[Par1 - 1] = millis() + (1000 * Par2);
  }

  if (strcasecmp_P(Command, PSTR("Delay")) == 0)
  {
    success = true;
    delayMillis(Par1);
  }

  if (strcasecmp_P(Command, PSTR("Rules")) == 0)
  {
    success = true;
    if (Par1 == 1)
      Settings.UseRules = true;
    else
      Settings.UseRules = false;
  }

  if (strcasecmp_P(Command, PSTR("Event")) == 0)
  {
    success = true;
    String event = Line;
    event = event.substring(6);
    event.replace("$", "#");
    if (Settings.UseRules)
      rulesProcessing(event);
  }

  if (strcasecmp_P(Command, PSTR("SendTo")) == 0)
  {
    success = true;
    String event = Line;
    event = event.substring(7);
    int index = event.indexOf(',');
    if (index > 0)
    {
      event = event.substring(index+1);
      SendUDPCommand(Par1, (char*)event.c_str(), event.length());
    }
  }
  
  if (strcasecmp_P(Command, PSTR("SendToUDP")) == 0)
  {
    success = true;
    String strLine = Line;
    String ip = parseString(strLine,2);
    String port = parseString(strLine,3);
    int msgpos = getParamStartPos(strLine,4);
    String message = strLine.substring(msgpos);
    byte ipaddress[4];
    str2ip((char*)ip.c_str(), ipaddress);
    IPAddress UDP_IP(ipaddress[0], ipaddress[1], ipaddress[2], ipaddress[3]);
    portUDP.beginPacket(UDP_IP, port.toInt());
    portUDP.write(message.c_str(), message.length());
    portUDP.endPacket();
  }

  if (strcasecmp_P(Command, PSTR("SendToHTTP")) == 0)
  {
    success = true;
    String strLine = Line;
    String host = parseString(strLine,2);
    String port = parseString(strLine,3);
    int pathpos = getParamStartPos(strLine,4);
    String path = strLine.substring(pathpos);
    EthernetClient client;
    if (client.connect(host.c_str(), port.toInt()))
    {
      String request = F("GET ");
      request += path;
      request += F(" HTTP/1.1\r\n");
      request += F("Host: ");
      request += host;
      request += F("\r\n");
//      request += authHeader;
      request += F("Connection: close\r\n\r\n");
      client.print(request);
      
      unsigned long timer = millis() + 200;
      while (!client.available() && millis() < timer)
        delay(1);

      while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line.substring(0, 15) == F("HTTP/1.1 200 OK"))
          addLog(LOG_LEVEL_DEBUG, line);
        delay(1);
      }
      client.flush();
      client.stop();
    }
  }

  // ****************************************
  // configure settings commands
  // ****************************************

  if (strcasecmp_P(Command, PSTR("Reboot")) == 0)
  {
    success = true;
    Reboot();
  }

  if (strcasecmp_P(Command, PSTR("Restart")) == 0)
  {
    success = true;
    Reboot();
  }
  
  if (strcasecmp_P(Command, PSTR("Reset")) == 0)
  {
    success = true;
    ResetFactory();
  }

  if (strcasecmp_P(Command, PSTR("Save")) == 0)
  {
    success = true;
    SaveSettings();
  }

  if (strcasecmp_P(Command, PSTR("Load")) == 0)
  {
    success = true;
    LoadSettings();
  }

  if (strcasecmp_P(Command, PSTR("Debug")) == 0)
  {
    success = true;
    Settings.SerialLogLevel = Par1;
  }

  if (strcasecmp_P(Command, PSTR("IP")) == 0)
  {
    success = true;
    if (GetArgv(Line, TmpStr1, 2))
      if (!str2ip(TmpStr1, Settings.IP))
        Serial.println("?");
  }

  if (strcasecmp_P(Command, PSTR("Settings")) == 0)
  {
    success = true;
    char str[20];
    Serial.println();

    Serial.println(F("System Info"));
    IPAddress ip = Ethernet.localIP();
    sprintf_P(str, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
    Serial.print(F("  IP Address    : ")); Serial.println(str);
    Serial.print(F("  Build         : ")); Serial.println((int)BUILD);
    Serial.print(F("  Unit          : ")); Serial.println((int)Settings.Unit);
    Serial.print(F("  Free mem      : ")); Serial.println(FreeMem());
  }
  
  if (success)
    status += F("\nOk");
  else  
    status += F("\nUnknown command!");
  SendStatus(source,status);
}

void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
