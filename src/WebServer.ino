String webdata = "";

void WebServerHandleClient() {
  EthernetClient client = WebServer.available();
  if (client) {
#if socketdebug
    ShowSocketStatus();
#endif
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    String request = "";
    boolean getrequest = true;
    webdata = "";
    webdata.reserve(500);
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (getrequest)
          request += c;

        #ifdef DEBUG_WEB
          Serial.write(c);
        #endif
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {

          // request is known here, in case of 'rules' we could handle this smarter (save index+6)?
          while (client.available()) { // post data...
            char c = client.read();
            webdata += c;
          }

          if (webdata.length() !=0)
            webdata = "&" + webdata;
            
          #ifdef DEBUG_WEB
            Serial.print("webdata0:");
            Serial.println(webdata);
            Serial.print("len:");
            Serial.println(webdata.length());
          #endif

          int pos = request.indexOf("/");
          if (pos > 0)
            request = request.substring(pos + 1);
          pos = request.indexOf(" ");
          if (pos > 0)
            request = request.substring(0, pos);
          pos = request.indexOf("?");
          if (pos >= 0)
          {
            String args = request.substring(pos + 1);
            webdata += "&" + args;
            request = request.substring(0, pos);
          }

          webdata = URLDecode(webdata.c_str());

          #ifdef DEBUG_WEB
            Serial.print("webdata1:");
            Serial.println(webdata);
          #endif
          
          if (request.startsWith(F(" HTTP")) or request.length() == 0) // root page
          {
            addHeader(true, client);
            handle_root(client, webdata);
          }
          else if (request.equals(F("control")))
          {
            handle_control(client, webdata);
          }
          else if (request.equals(F("config")))
          {
            addHeader(true, client);
            handle_config(client, webdata);
          }
          else if (request.equals(F("hardware")))
          {
            addHeader(true, client);
            handle_hardware(client, webdata);
          }
          else if (request.equals(F("devices")))
          {
            addHeader(true, client);
            handle_devices(client, webdata);
          }
          else if (request.equals(F("rules")))
          {
            addHeader(true, client);
            handle_rules(client, webdata);
          }
          else if (request.equals(F("tools")))
          {
            addHeader(true, client);
            handle_tools(client, webdata);
          }
          else if (request.equals(F("advanced")))
          {
            addHeader(true, client);
            handle_advanced(client, webdata);
          }
          else if (request.equals(F("SDfilelist")))
          {
            addHeader(true, client);
            handle_SDfilelist(client, webdata);
          }
          else if (request.equals(F("i2cscanner")))
          {
            addHeader(true, client);
            handle_i2cscanner(client, webdata);
          }
          else if (request.equals(F("log")))
          {
            addHeader(true, client);
            handle_log(client, webdata);
          }
          else if (request.equals(F("sysinfo")))
          {
            addHeader(true, client);
            handle_sysinfo(client, webdata);
          }
          else
            handle_unknown(client, request);
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          getrequest = false;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
    webdata = "";
  }
}

String WebServerarg(String arg)
{
  #ifdef DEBUG_WEB2
    Serial.print("webdata3:");
    Serial.println(webdata);
  #endif
  
  arg = "&" + arg;

  #ifdef DEBUG_WEB2
    Serial.print("arg:");
    Serial.println(arg);
  #endif

  String returnarg = "";
  int pos = webdata.indexOf(arg);
  if (pos >= 0)
  {
    returnarg = webdata.substring(pos+1,pos+81); // max field content 80 ?
    pos = returnarg.indexOf("&");
    if (pos > 0)
      returnarg = returnarg.substring(0, pos);
    pos = returnarg.indexOf("=");
    if (pos > 0)
      returnarg = returnarg.substring(pos + 1);
  }
  return returnarg;
}


//********************************************************************************
// Add top menu
//********************************************************************************
void addHeader(boolean showMenu, EthernetClient client)
{
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println(F("Connection: close"));  // the connection will be closed after completion of the response
  client.println();

  String str = "";
  boolean cssfile = false;

  str += F("<script language=\"javascript\"><!--\n");
  str += F("function dept_onchange(frmselect) {frmselect.submit();}\n");
  str += F("//--></script>");
  str += F("<head><title>");
  str += Settings.Name;
  str += F("</title>");

  if (!cssfile)
  {
    str += F("<style>");
    str += F("* {font-family:sans-serif; font-size:12pt;}");
    str += F("h1 {font-size:16pt; color:black;}");
    str += F("h6 {font-size:10pt; color:black; text-align:center;}");
    str += F(".button-menu {background-color:#ffffff; color:blue; margin: 10px; text-decoration:none}");
    str += F(".button-link {padding:5px 15px; background-color:#0077dd; color:#fff; border:solid 1px #fff; text-decoration:none}");
    str += F(".button-menu:hover {background:#ddddff;}");
    str += F(".button-link:hover {background:#369;}");
    str += F("th {padding:10px; background-color:black; color:#ffffff;}");
    str += F("td {padding:7px;}");
    str += F("table {color:black;}");
    str += F(".div_l {float: left;}");
    str += F(".div_r {float: right; margin: 2px; padding: 1px 10px; border-radius: 7px; background-color:#080; color:white;}");
    str += F(".div_br {clear: both;}");
    str += F("</style>");
  }
  else
    str += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"esp.css\">");

  str += F("</head>");

  str += F("<h1>Welcome to Arduino Easy: ");
  str += Settings.Name;

  str += F("</h1>");

  if (showMenu)
  {
    str += F("<BR><a class=\"button-menu\" href=\".\">Main</a>");
    str += F("<a class=\"button-menu\" href=\"config\">Config</a>");
    str += F("<a class=\"button-menu\" href=\"hardware\">Hardware</a>");
    str += F("<a class=\"button-menu\" href=\"devices\">Devices</a>");
    if (Settings.UseRules)
      str += F("<a class=\"button-menu\" href=\"rules\">Rules</a>");
    str += F("<a class=\"button-menu\" href=\"tools\">Tools</a><BR><BR>");
  }
  client.print(str);
}


//********************************************************************************
// Add footer to web page
//********************************************************************************
void addFooter(String& str)
{
  str += F("<h6>Powered by www.letscontrolit.com</h6></body>");
}

//********************************************************************************
// Web Interface root page
//********************************************************************************
void handle_root(EthernetClient client, String &post) {

  //if (!isLoggedIn()) return;

  String sCommand = WebServerarg(F("cmd"));

  if (strcasecmp_P(sCommand.c_str(), PSTR("reboot")) != 0)
  {
    String reply = "";
    reply.reserve(1000);

    printToWeb = true;
    printWebString = "";
    if (sCommand.length() > 0)
      ExecuteCommand(VALUE_SOURCE_HTTP, sCommand.c_str());

    IPAddress ip = Ethernet.localIP();
    IPAddress gw = Ethernet.gatewayIP();

    reply += printWebString;
    reply += F("<form>");
    reply += F("<table><TH>System Info<TH>Value<TH><TH>System Info<TH>Value");

    reply += F("<TR><TD>Unit:<TD>");
    reply += Settings.Unit;

    reply += F("<TD><TD>Build:<TD>");
    reply += BUILD;
    reply += F(" ");
    reply += F(BUILD_NOTES);

    if (Settings.UseNTP)
    {
      reply += F("<TR><TD>System Time:<TD>");
      reply += hour();
      reply += ":";
      if (minute() < 10)
        reply += "0";
      reply += minute();
    }

    reply += F("<TD><TD>Uptime:<TD>");
    char strUpTime[40];
    int minutes = wdcounter / 2;
    int days = minutes / 1440;
    minutes = minutes % 1440;
    int hrs = minutes / 60;
    minutes = minutes % 60;
    sprintf_P(strUpTime, PSTR("%d days %d hours %d minutes"), days, hrs, minutes);
    reply += strUpTime;

    reply += F("<TR><TD>Load:<TD>");
    if (wdcounter > 0)
    {
      reply += 100 - (100 * loopCounterLast / loopCounterMax);
      reply += F("% (LC=");
      reply += int(loopCounterLast / 30);
      reply += F(")");
    }

    reply += F("<TD><TD>Free Mem:<TD>");
    reply += FreeMem();

    char str[20];
    sprintf_P(str, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
    reply += F("<TR><TD>IP:<TD>");
    reply += str;

    sprintf_P(str, PSTR("%u.%u.%u.%u"), gw[0], gw[1], gw[2], gw[3]);
    reply += F("<TD><TD>GW:<TD>");
    reply += str;

    client.print(reply);
    reply = "";

    reply += F("<TR><TH>Node List:<TH>Build<TH>IP<TH>Age<TH><TR><TD><TD><TD>");
    for (byte x = 0; x < UNIT_MAX; x++)
    {
      if (Nodes[x].ip[0] != 0)
      {
        char url[80];
        sprintf_P(url, PSTR("<a href='http://%u.%u.%u.%u'>%u.%u.%u.%u</a>"), Nodes[x].ip[0], Nodes[x].ip[1], Nodes[x].ip[2], Nodes[x].ip[3], Nodes[x].ip[0], Nodes[x].ip[1], Nodes[x].ip[2], Nodes[x].ip[3]);
        reply += F("<TR><TD>Unit ");
        reply += x;
        reply += F("<TD>");
        if (Nodes[x].build)
          reply += Nodes[x].build;
        reply += F("<TD>");
        reply += url;
        reply += F("<TD>");
        reply += Nodes[x].age;
        client.print(reply);
        reply = "";
      }
    }

    reply += F("</table></form>");
    addFooter(reply);
    client.print(reply);
    printWebString = "";
    printToWeb = false;
  }
  else
  {
    // have to disconnect or reboot from within the main loop
    // because the webconnection is still active at this point
    // disconnect here could result into a crash/reboot...
    if (strcasecmp_P(sCommand.c_str(), PSTR("reboot")) == 0)
    {
      String log = F("     : Rebooting...");
      addLog(LOG_LEVEL_INFO, log);
      cmd_within_mainloop = CMD_REBOOT;
    }
    client.print("ok");
  }
}


//********************************************************************************
// Web Interface config page
//********************************************************************************
void handle_config(EthernetClient client, String &post) {
  //if (!isLoggedIn()) return;

  update_config();

  webdata = "";
  String reply = "";
  reply.reserve(500);

  reply += F("<form name='frmselect' method='post'><table>");
  reply += F("<TH>Main Settings<TH><TR><TD>Name:<TD><input type='text' name='name' value='");
  Settings.Name[25] = 0;
  reply += Settings.Name;
  reply += F("'><TR><TD>Admin Password:<TD><input type='text' name='password' value='");
  SecuritySettings.Password[25] = 0;
  reply += SecuritySettings.Password;

  reply += F("'><TR><TD>Unit nr:<TD><input type='text' name='unit' value='");
  reply += Settings.Unit;

  client.print(reply);
  reply = "";
  
  reply += F("'><TR><TD>Protocol:");
  byte choice = Settings.Protocol;
  reply += F("<TD><select name='protocol' LANGUAGE=javascript onchange=\"return dept_onchange(frmselect)\" >");
  reply += F("<option value='0'>- Standalone -</option>");
  for (byte x = 0; x <= protocolCount; x++)
  {
    reply += F("<option value='");
    reply += Protocol[x].Number;
    reply += "'";
    if (choice == Protocol[x].Number)
      reply += F(" selected");
    reply += ">";

    String ProtocolName = "";
    CPlugin_ptr[x](CPLUGIN_GET_DEVICENAME, 0, ProtocolName);
    reply += ProtocolName;
    reply += F("</option>");
  }
  reply += F("</select>");
  reply += F("<a class=\"button-link\" href=\"http://www.letscontrolit.com/wiki/index.php/EasyProtocols\" target=\"_blank\">?</a>");

  client.print(reply);
  reply = "";

  char str[20];

  if (Settings.Protocol)
  {
    byte choice = Settings.UseDNS;
    String options[2];
    options[0] = F("Use IP address");
    options[1] = F("Use Hostname");
    int optionValues[2];
    optionValues[0] = 0;
    optionValues[1] = 1;
    reply += F("<TR><TD>Locate Controller:<TD><select name='usedns' LANGUAGE=javascript onchange=\"return dept_onchange(frmselect)\" >");
    for (byte x = 0; x < 2; x++)
    {
      reply += F("<option value='");
      reply += optionValues[x];
      reply += "'";
      if (choice == optionValues[x])
        reply += F(" selected");
      reply += ">";
      reply += options[x];
      reply += F("</option>");
    }
    reply += F("</select>");

    client.print(reply);
    reply = "";

    if (Settings.UseDNS)
    {
      reply += F("<TR><TD>Controller Hostname:<TD><input type='text' name='controllerhostname' size='64' value='");
      reply += Settings.ControllerHostName;
    }
    else
    {
      reply += F("<TR><TD>Controller IP:<TD><input type='text' name='controllerip' value='");
      sprintf_P(str, PSTR("%u.%u.%u.%u"), Settings.Controller_IP[0], Settings.Controller_IP[1], Settings.Controller_IP[2], Settings.Controller_IP[3]);
      reply += str;
    }

    reply += F("'><TR><TD>Controller Port:<TD><input type='text' name='controllerport' value='");
    reply += Settings.ControllerPort;

    byte ProtocolIndex = getProtocolIndex(Settings.Protocol);
    if (Protocol[ProtocolIndex].usesAccount)
    {
      reply += F("'><TR><TD>Controller User:<TD><input type='text' name='controlleruser' value='");
      reply += SecuritySettings.ControllerUser;
    }

    client.print(reply);
    reply = "";

    if (Protocol[ProtocolIndex].usesPassword)
    {
      reply += F("'><TR><TD>Controller Password:<TD><input type='text' name='controllerpassword' value='");
      reply += SecuritySettings.ControllerPassword;
    }
    reply += F("'>");

    CPlugin_ptr[ProtocolIndex](CPLUGIN_WEBFORM_LOAD, 0, reply);

  }

  client.print(reply);
  reply = "";

  reply += F("<TR><TD>Sensor Delay:<TD><input type='text' name='delay' value='");
  reply += Settings.Delay;
  reply += F("'>");

  reply += F("<TR><TH>Optional Settings<TH>");

  reply += F("<TR><TD>ESP IP:<TD><input type='text' name='espip' value='");
  sprintf_P(str, PSTR("%u.%u.%u.%u"), Settings.IP[0], Settings.IP[1], Settings.IP[2], Settings.IP[3]);
  reply += str;

  reply += F("'><TR><TD>ESP GW:<TD><input type='text' name='espgateway' value='");
  sprintf_P(str, PSTR("%u.%u.%u.%u"), Settings.Gateway[0], Settings.Gateway[1], Settings.Gateway[2], Settings.Gateway[3]);
  reply += str;

  reply += F("'><TR><TD>ESP Subnet:<TD><input type='text' name='espsubnet' value='");
  sprintf_P(str, PSTR("%u.%u.%u.%u"), Settings.Subnet[0], Settings.Subnet[1], Settings.Subnet[2], Settings.Subnet[3]);
  reply += str;

  client.print(reply);
  reply = "";

  reply += F("'><TR><TD>ESP DNS:<TD><input type='text' name='espdns' value='");
  sprintf_P(str, PSTR("%u.%u.%u.%u"), Settings.DNS[0], Settings.DNS[1], Settings.DNS[2], Settings.DNS[3]);
  reply += str;

  reply += F("'><TR><TD><TD><input class=\"button-link\" type='submit' value='Submit'>");
  reply += F("</table></form>");
  addFooter(reply);
  //  debugRAM(3,reply.length());
  client.print(reply);
}


void update_config()
{
  char tmpString[64];

  #ifdef DEBUG_WEB
    Serial.print("webdata2:");
    Serial.println(webdata);
  #endif
  
  String arg = "";
  arg = WebServerarg(F("name"));

  #ifdef DEBUG_WEB
  Serial.print("name:");
  Serial.println(arg);
  #endif
  
  if (arg[0] != 0)
  {
    strncpy(Settings.Name, arg.c_str(), sizeof(Settings.Name));
    arg = WebServerarg(F("password"));
    strncpy(SecuritySettings.Password, arg.c_str(), sizeof(SecuritySettings.Password));

    arg = WebServerarg(F("protocol"));
    if (Settings.Protocol != arg.toInt())
    {
      Settings.Protocol = arg.toInt();
      byte ProtocolIndex = getProtocolIndex(Settings.Protocol);
      Settings.ControllerPort = Protocol[ProtocolIndex].defaultPort;
      if (Protocol[ProtocolIndex].usesTemplate)
        CPlugin_ptr[ProtocolIndex](CPLUGIN_PROTOCOL_TEMPLATE, 0, dummyString);
    }
    else
    {
      if (Settings.Protocol != 0)
      {
        byte ProtocolIndex = getProtocolIndex(Settings.Protocol);
        CPlugin_ptr[ProtocolIndex](CPLUGIN_WEBFORM_SAVE, 0, dummyString);
        arg = WebServerarg(F("usedns"));
        Settings.UseDNS = arg.toInt();
        if (Settings.UseDNS)
        {
          arg = WebServerarg(F("controllerhostname"));
          strncpy(Settings.ControllerHostName, arg.c_str(), sizeof(Settings.ControllerHostName));
          getIPfromHostName();
        }
        else
        {
          arg = WebServerarg(F("controllerip"));
          if (arg.length() != 0)
          {
            arg.toCharArray(tmpString, 26);
            str2ip(tmpString, Settings.Controller_IP);
          }
        }

        arg = WebServerarg(F("controllerport"));
        Settings.ControllerPort = arg.toInt();
        arg = WebServerarg(F("controlleruser"));
        strncpy(SecuritySettings.ControllerUser, arg.c_str(), sizeof(SecuritySettings.ControllerUser));
        arg = WebServerarg(F("controllerpassword"));
        strncpy(SecuritySettings.ControllerPassword, arg.c_str(), sizeof(SecuritySettings.ControllerPassword));
      }
    }

    arg = WebServerarg(F("delay"));
    Settings.Delay = arg.toInt();

    arg = WebServerarg(F("espip"));
    arg.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.IP);
    arg = WebServerarg(F("espgateway"));
    arg.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.Gateway);
    arg = WebServerarg(F("espsubnet"));
    arg.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.Subnet);
    arg = WebServerarg(F("espdns"));
    arg.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.DNS);
    arg = WebServerarg(F("unit"));
    Settings.Unit = arg.toInt();
    SaveSettings();
  }
}

//********************************************************************************
// Web Interface hardware page
//********************************************************************************
void handle_hardware(EthernetClient client, String &post) {
  //if (!isLoggedIn()) return;

  String edit = WebServerarg(F("edit"));

  if (edit.length() != 0)
  {
    Settings.PinBootStates[2]  =  WebServerarg(F("p2")).toInt();
    Settings.PinBootStates[3]  =  WebServerarg(F("p3")).toInt();
    Settings.PinBootStates[4]  =  WebServerarg(F("p4")).toInt();
    Settings.PinBootStates[5]  =  WebServerarg(F("p5")).toInt();
    Settings.PinBootStates[6] =  WebServerarg(F("p6")).toInt();
    Settings.PinBootStates[7] =  WebServerarg(F("p7")).toInt();
    Settings.PinBootStates[8] =  WebServerarg(F("p8")).toInt();
    Settings.PinBootStates[9] =  WebServerarg(F("p9")).toInt();
    Settings.PinBootStates[11] =  WebServerarg(F("p11")).toInt();
    Settings.PinBootStates[12] =  WebServerarg(F("p12")).toInt();
    SaveSettings();
  }

  String reply = "";
  reply.reserve(1000);

  reply += F("<form  method='post'><table><TH>Hardware Settings<TH><TR><TD>");
  reply += F("<TR><TD>GPIO boot states:<TD>");

  reply += F("<TR><TD>D2:<TD>");
  addPinStateSelect(reply, "p2", Settings.PinBootStates[2]);
  reply += F("<TR><TD>D3:<TD>");
  addPinStateSelect(reply, "p3", Settings.PinBootStates[3]);
  reply += F("<TR><TD>D4:<TD>");
  addPinStateSelect(reply, "p4", Settings.PinBootStates[4]);
  reply += F("<TR><TD>D5:<TD>");

  client.print(reply);
  reply = "";

  addPinStateSelect(reply, "p5", Settings.PinBootStates[5]);
  reply += F("<TR><TD>D6:<TD>");
  addPinStateSelect(reply, "p6", Settings.PinBootStates[6]);
  reply += F("<TR><TD>D7:<TD>");
  addPinStateSelect(reply, "p7", Settings.PinBootStates[7]);

  client.print(reply);
  reply = "";

  reply += F("<TR><TD>D8:<TD>");
  addPinStateSelect(reply, "p8", Settings.PinBootStates[8]);
  reply += F("<TR><TD>D9:<TD>");
  addPinStateSelect(reply, "p9", Settings.PinBootStates[9]);
  reply += F("<TR><TD>D11:<TD>");
  addPinStateSelect(reply, "p11", Settings.PinBootStates[11]);

  client.print(reply);
  reply = "";

  reply += F("<TR><TD>D12:<TD>");
  addPinStateSelect(reply, "p12", Settings.PinBootStates[12]);

  reply += F("<input type='hidden' name='edit' value='1'>");
  reply += F("<TR><TD><TD><input class=\"button-link\" type='submit' value='Submit'><TR><TD>");

  reply += F("</table></form>");
  client.print(reply);
}


//********************************************************************************
// Add a GPIO pin select dropdown list
//********************************************************************************
void addPinStateSelect(String& str, String name,  int choice)
{
  String options[4];
  options[0] = F("Default");
  options[1] = F("Output Low");
  options[2] = F("Output High");
  options[3] = F("Input");
  int optionValues[4];
  optionValues[0] = 0;
  optionValues[1] = 1;
  optionValues[2] = 2;
  optionValues[3] = 3;

  str += F("<select name='");
  str += name;
  str += "'>";
  for (byte x = 0; x < 4; x++)
  {
    str += F("<option value='");
    str += optionValues[x];
    str += "'";
    if (choice == optionValues[x])
      str += F(" selected");
    str += ">";
    str += options[x];
    str += F("</option>");
  }
  str += F("</select>");
}


//********************************************************************************
// Web Interface device page
//********************************************************************************
void handle_devices(EthernetClient client, String &post) {
  //if (!isLoggedIn()) return;

  update_device();

  //  debugRAM(2,0);

  struct EventStruct TempEvent;

  String taskindex = WebServerarg(F("index"));
  byte index = taskindex.toInt();
  byte page = WebServerarg(F("page")).toInt();
  if (page == 0)
    page = 1;
  byte setpage = WebServerarg(F("setpage")).toInt();
  if (setpage > 0)
  {
    if (setpage <= (TASKS_MAX / 4))
      page = setpage;
    else
      page = TASKS_MAX / 4;
  }

  byte DeviceIndex = 0;

  webdata = "";
  String reply = "";
  reply.reserve(1000);

  // show all tasks as table
  if (index == 0)
  {
    reply += F("<table cellpadding='4' border='1' frame='box' rules='all'><TH>");
    reply += F("<a class=\"button-link\" href=\"devices?setpage=");
    if (page > 1)
      reply += page - 1;
    else
      reply += page;
    reply += F("\"><</a>");
    reply += F("<a class=\"button-link\" href=\"devices?setpage=");
    if (page < (TASKS_MAX / 4))
      reply += page + 1;
    else
      reply += page;
    reply += F("\">></a>");

    reply += F("<TH>Task<TH>Device<TH>Name<TH>Port<TH>IDX/Variable<TH>GPIO<TH>Values");

    String deviceName;

    for (byte x = (page - 1) * 4; x < ((page) * 4); x++)
    {
      reply += F("<TR><TD>");
      reply += F("<a class=\"button-link\" href=\"devices?index=");
      reply += x + 1;
      reply += F("&page=");
      reply += page;
      reply += F("\">Edit</a>");
      reply += F("<TD>");
      reply += x + 1;
      reply += F("<TD>");

      if (Settings.TaskDeviceNumber[x] != 0)
      {
        LoadTaskSettings(x);
        DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[x]);
        TempEvent.TaskIndex = x;

        deviceName = "";
        if (Settings.TaskDeviceNumber[x] != 0)
          Plugin_ptr[DeviceIndex](PLUGIN_GET_DEVICENAME, &TempEvent, deviceName);

        reply += deviceName;
        reply += F("<TD>");
        reply += ExtraTaskSettings.TaskDeviceName;
        reply += F("<TD>");

        byte customConfig = false;
        customConfig = PluginCall(PLUGIN_WEBFORM_SHOW_CONFIG, &TempEvent, reply);
        if (!customConfig)
          if (Device[DeviceIndex].Ports != 0)
            reply += Settings.TaskDevicePort[x];

        reply += F("<TD>");

        if (Settings.TaskDeviceID[x] != 0)
          reply += Settings.TaskDeviceID[x];

        reply += F("<TD>");

        if (Settings.TaskDeviceDataFeed[x] == 0)
        {
          if (Device[DeviceIndex].Type == DEVICE_TYPE_I2C)
          {
            reply += F("GPIO-");
            //reply += Settings.Pin_i2c_sda; // todo
            reply += F("<BR>GPIO-");
            //reply += Settings.Pin_i2c_scl; // todo
          }
          if (Device[DeviceIndex].Type == DEVICE_TYPE_ANALOG)
            reply += F("ADC (TOUT)");

          if (Settings.TaskDevicePin1[x] != -1)
          {
            reply += F("GPIO-");
            reply += Settings.TaskDevicePin1[x];
          }

          if (Settings.TaskDevicePin2[x] != -1)
          {
            reply += F("<BR>GPIO-");
            reply += Settings.TaskDevicePin2[x];
          }

          if (Settings.TaskDevicePin3[x] != -1)
          {
            reply += F("<BR>GPIO-");
            reply += Settings.TaskDevicePin3[x];
          }
        }

        reply += F("<TD>");
        byte customValues = false;
        customValues = PluginCall(PLUGIN_WEBFORM_SHOW_VALUES, &TempEvent, reply);
        if (!customValues)
        {
          if (Device[DeviceIndex].VType == SENSOR_TYPE_LONG)
          {
            reply  += F("<div class=\"div_l\">");
            reply  += ExtraTaskSettings.TaskDeviceValueNames[0];
            reply  += F(":</div><div class=\"div_r\">");
            reply  += (unsigned long)UserVar[x * VARS_PER_TASK] + ((unsigned long)UserVar[x * VARS_PER_TASK + 1] << 16);
            reply  += F("</div>");
          }
          else
          {
            for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
            {
              if ((Settings.TaskDeviceNumber[x] != 0) and (varNr < Device[DeviceIndex].ValueCount))
              {
                if (varNr > 0)
                  reply += F("<div class=\"div_br\"></div>");
                reply += F("<div class=\"div_l\">");
                reply += ExtraTaskSettings.TaskDeviceValueNames[varNr];
                reply += F(":</div><div class=\"div_r\">");
                reply += String(UserVar[x * VARS_PER_TASK + varNr], ExtraTaskSettings.TaskDeviceValueDecimals[varNr]);
                reply += "</div>";
              }
            }
          }
        }
      }
      else
        reply += F("<TD><TD><TD><TD><TD>");

      client.print(reply);
      reply = "";

    } // next
    reply += F("</table>");
  }
  // Show edit form if a specific entry is chosen with the edit button
  else
  {
    LoadTaskSettings(index - 1);
    DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[index - 1]);
    TempEvent.TaskIndex = index - 1;

    reply += F("<BR><BR><form name='frmselect' method='post'><table><TH>Task Settings<TH>Value");

    reply += F("<TR><TD>Device:<TD>");
    addDeviceSelect(reply, "taskdevicenumber", Settings.TaskDeviceNumber[index - 1]);

    if (Settings.TaskDeviceNumber[index - 1] != 0 )
    {
      reply += F("<a class=\"button-link\" href=\"http://www.letscontrolit.com/wiki/index.php/Plugin");
      reply += Settings.TaskDeviceNumber[index - 1];
      reply += F("\" target=\"_blank\">?</a>");

      reply += F("<TR><TD>Name:<TD><input type='text' maxlength='40' name='taskdevicename' value='");
      reply += ExtraTaskSettings.TaskDeviceName;
      reply += F("'>");

      if (Device[DeviceIndex].TimerOption)
      {
        reply += F("<TR><TD>Delay:<TD><input type='text' name='taskdevicetimer' value='");
        reply += Settings.TaskDeviceTimer[index - 1];
        reply += F("'>");
        if (Device[DeviceIndex].TimerOptional)
          reply += F(" (Optional for this device)");
      }

      if (!Device[DeviceIndex].Custom)
      {
        reply += F("<TR><TD>IDX / Var:<TD><input type='text' name='taskdeviceid' value='");
        reply += Settings.TaskDeviceID[index - 1];
        reply += F("'>");
      }

      if (!Device[DeviceIndex].Custom && Settings.TaskDeviceDataFeed[index - 1] == 0)
      {
        if (Device[DeviceIndex].Ports != 0)
        {
          reply += F("<TR><TD>Port:<TD><input type='text' name='taskdeviceport' value='");
          reply += Settings.TaskDevicePort[index - 1];
          reply += F("'>");
        }

        client.print(reply);
        reply = "";

        if (Device[DeviceIndex].Type == DEVICE_TYPE_SINGLE || Device[DeviceIndex].Type == DEVICE_TYPE_DUAL)
        {
          reply += F("<TR><TD>1st GPIO:<TD>");
          addPinSelect(false, reply, "taskdevicepin1", Settings.TaskDevicePin1[index - 1]);
        }
        if (Device[DeviceIndex].Type == DEVICE_TYPE_DUAL)
        {
          reply += F("<TR><TD>2nd GPIO:<TD>");
          addPinSelect(false, reply, "taskdevicepin2", Settings.TaskDevicePin2[index - 1]);
        }

        if (Device[DeviceIndex].PullUpOption)
        {
          reply += F("<TR><TD>Pull UP:<TD>");
          if (Settings.TaskDevicePin1PullUp[index - 1])
            reply += F("<input type=checkbox name=taskdevicepin1pullup checked>");
          else
            reply += F("<input type=checkbox name=taskdevicepin1pullup>");
        }

        if (Device[DeviceIndex].InverseLogicOption)
        {
          reply += F("<TR><TD>Inversed:<TD>");
          if (Settings.TaskDevicePin1Inversed[index - 1])
            reply += F("<input type=checkbox name=taskdevicepin1inversed checked>");
          else
            reply += F("<input type=checkbox name=taskdevicepin1inversed>");
        }
      }

      client.print(reply);
      reply = "";

      if (Settings.TaskDeviceDataFeed[index - 1] == 0) // only show additional config for local connected sensors
        PluginCall(PLUGIN_WEBFORM_LOAD, &TempEvent, reply);

      if (Device[DeviceIndex].SendDataOption)
      {
        reply += F("<TR><TD>Send Data:<TD>");
        if (Settings.TaskDeviceSendData[index - 1])
          reply += F("<input type=checkbox name=taskdevicesenddata checked>");
        else
          reply += F("<input type=checkbox name=taskdevicesenddata>");
      }

      if (Settings.GlobalSync && Device[DeviceIndex].GlobalSyncOption && Settings.TaskDeviceDataFeed[index - 1] == 0 && Settings.UDPPort != 0)
      {
        reply += F("<TR><TD>Global Sync:<TD>");
        if (Settings.TaskDeviceGlobalSync[index - 1])
          reply += F("<input type=checkbox name=taskdeviceglobalsync checked>");
        else
          reply += F("<input type=checkbox name=taskdeviceglobalsync>");
      }

      client.print(reply);
      reply = "";

      if (!Device[DeviceIndex].Custom)
      {
        reply += F("<TR><TH>Optional Settings<TH>Value");

        if (Device[DeviceIndex].FormulaOption)
        {
          for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
          {
            reply += F("<TR><TD>Formula ");
            reply += ExtraTaskSettings.TaskDeviceValueNames[varNr];
            reply += F(":<TD><input type='text' maxlength='40' name='taskdeviceformula");
            reply += varNr + 1;
            reply += F("' value='");
            reply += ExtraTaskSettings.TaskDeviceFormula[varNr];
            reply += F("'>");

            reply += F(" Decimals: <input type='text' name='taskdevicevaluedecimals");
            reply += varNr + 1;
            reply += F("' value='");
            reply += ExtraTaskSettings.TaskDeviceValueDecimals[varNr];
            reply += F("'>");

            if (varNr == 0)
              reply += F("<a class=\"button-link\" href=\"http://www.letscontrolit.com/wiki/index.php/EasyFormula\" target=\"_blank\">?</a>");

            client.print(reply);
            reply = "";

          }
        }
        else
        {
          if (Device[DeviceIndex].DecimalsOnly)
            for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
            {
              reply += F("<TR><TD>Decimals ");
              reply += ExtraTaskSettings.TaskDeviceValueNames[varNr];
              reply += F(":<TD><input type='text' name='taskdevicevaluedecimals");
              reply += varNr + 1;
              reply += F("' value='");
              reply += ExtraTaskSettings.TaskDeviceValueDecimals[varNr];
              reply += F("'>");
              client.print(reply);
              reply = "";
            }
        }

        for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
        {
          reply += F("<TR><TD>Value Name ");
          reply += varNr + 1;
          reply += F(":<TD><input type='text' maxlength='40' name='taskdevicevaluename");
          reply += varNr + 1;
          reply += F("' value='");
          reply += ExtraTaskSettings.TaskDeviceValueNames[varNr];
          reply += F("'>");
          client.print(reply);
          reply = "";
        }
      }

    }
    reply += F("<TR><TD><TD><a class=\"button-link\" href=\"devices?setpage=");
    reply += page;
    reply += F("\">Close</a>");
    reply += F("<input class=\"button-link\" type='submit' value='Submit'>");
    reply += F("<input type='hidden' name='edit' value='1'>");
    reply += F("<input type='hidden' name='page' value='1'>");
    reply += F("</table></form>");
  }
  client.print(reply);
}

void update_device()
{
  char tmpString[41];
  struct EventStruct TempEvent;

  String arg = "";
  String taskindex = WebServerarg(F("index"));
  String taskdevicenumber = WebServerarg(F("taskdevicenumber"));
  String taskdeviceformula[VARS_PER_TASK];
  String taskdevicevaluename[VARS_PER_TASK];
  String taskdevicevaluedecimals[VARS_PER_TASK];

  for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
  {
    char argc[25];
    String arg = F("taskdeviceformula");
    arg += varNr + 1;
    arg.toCharArray(argc, 25);
    taskdeviceformula[varNr] = WebServerarg(argc);

    arg = F("taskdevicevaluename");
    arg += varNr + 1;
    arg.toCharArray(argc, 25);
    taskdevicevaluename[varNr] = WebServerarg(argc);

    arg = F("taskdevicevaluedecimals");
    arg += varNr + 1;
    arg.toCharArray(argc, 25);
    taskdevicevaluedecimals[varNr] = WebServerarg(argc);
  }

  String edit = WebServerarg(F("edit"));

  byte index = taskindex.toInt();
  byte DeviceIndex = 0;

  if (edit.toInt() != 0)
  {
    if (Settings.TaskDeviceNumber[index - 1] != taskdevicenumber.toInt()) // change of device, clear all other values
    {
      taskClear(index - 1, false); // clear settings, but do not save
      Settings.TaskDeviceNumber[index - 1] = taskdevicenumber.toInt();
      if (taskdevicenumber.toInt() != 0) // preload valuenames
      {
        TempEvent.TaskIndex = index - 1;
        if (ExtraTaskSettings.TaskDeviceValueNames[0][0] == 0) // if field set empty, reload defaults
          PluginCall(PLUGIN_GET_DEVICEVALUENAMES, &TempEvent, dummyString);
      }
    }
    else if (taskdevicenumber.toInt() != 0)
    {
      Settings.TaskDeviceNumber[index - 1] = taskdevicenumber.toInt();
      DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[index - 1]);

      arg = WebServerarg(F("taskdevicetimer"));
      if (arg.toInt() > 0)
        Settings.TaskDeviceTimer[index - 1] = arg.toInt();
      else
      {
        if (!Device[DeviceIndex].TimerOptional) // Set default delay, unless it's optional...
          Settings.TaskDeviceTimer[index - 1] = Settings.Delay;
        else
          Settings.TaskDeviceTimer[index - 1] = 0;
      }

      arg = WebServerarg(F("taskdevicename"));
      arg.toCharArray(tmpString, 41);
      strcpy(ExtraTaskSettings.TaskDeviceName, tmpString);

      arg = WebServerarg(F("taskdeviceport"));
      Settings.TaskDevicePort[index - 1] = arg.toInt();

      arg = WebServerarg(F("taskdeviceid"));
      if (Settings.TaskDeviceNumber[index - 1] != 0)
        Settings.TaskDeviceID[index - 1] = arg.toInt();
      else
        Settings.TaskDeviceID[index - 1] = 0;
        
      if (Device[DeviceIndex].Type == DEVICE_TYPE_SINGLE)
      {
        arg = WebServerarg(F("taskdevicepin1"));
        Settings.TaskDevicePin1[index - 1] = arg.toInt();
      }
      if (Device[DeviceIndex].Type == DEVICE_TYPE_DUAL)
      {
        arg = WebServerarg(F("taskdevicepin1"));
        Settings.TaskDevicePin1[index - 1] = arg.toInt();
        arg = WebServerarg(F("taskdevicepin2"));
        Settings.TaskDevicePin2[index - 1] = arg.toInt();
      }

      arg = WebServerarg(F("taskdevicepin3"));
      if (arg.length() != 0)
        Settings.TaskDevicePin3[index - 1] = arg.toInt();
      
      if (Device[DeviceIndex].PullUpOption)
      {
        arg = WebServerarg(F("taskdevicepin1pullup"));
        Settings.TaskDevicePin1PullUp[index - 1] = (arg == "on");
      }
      
      if (Device[DeviceIndex].InverseLogicOption)
      {
        arg = WebServerarg(F("taskdevicepin1inversed"));
        Settings.TaskDevicePin1Inversed[index - 1] = (arg == "on");
      }
      
      if (Device[DeviceIndex].SendDataOption)
      {
        arg = WebServerarg(F("taskdevicesenddata"));
        Settings.TaskDeviceSendData[index - 1] = (arg == "on");
      }
      
      if (Settings.GlobalSync)
      {
        if (Device[DeviceIndex].GlobalSyncOption)
        {
          arg = WebServerarg(F("taskdeviceglobalsync"));
          Settings.TaskDeviceGlobalSync[index - 1] = (arg == "on");
        }
        
        // Send task info if set global
        if (Settings.TaskDeviceGlobalSync[index - 1])
        {
          SendUDPTaskInfo(0, index - 1, index - 1);
        }
      }

      for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
      {
        taskdeviceformula[varNr].toCharArray(tmpString, 41);
        strcpy(ExtraTaskSettings.TaskDeviceFormula[varNr], tmpString);
        ExtraTaskSettings.TaskDeviceValueDecimals[varNr] = taskdevicevaluedecimals[varNr].toInt();
      }

      // task value names handling.
      for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
      {
        taskdevicevaluename[varNr].toCharArray(tmpString, 41);
        strcpy(ExtraTaskSettings.TaskDeviceValueNames[varNr], tmpString);
      }

      TempEvent.TaskIndex = index - 1;
      if (ExtraTaskSettings.TaskDeviceValueNames[0][0] == 0) // if field set empty, reload defaults
        PluginCall(PLUGIN_GET_DEVICEVALUENAMES, &TempEvent, dummyString);

      PluginCall(PLUGIN_WEBFORM_SAVE, &TempEvent, dummyString);
    }
    SaveTaskSettings(index - 1);
    SaveSettings();
    if (taskdevicenumber.toInt() != 0)
      PluginCall(PLUGIN_INIT, &TempEvent, dummyString);
  }
}


byte sortedIndex[DEVICES_MAX + 1];
//********************************************************************************
// Add a device select dropdown list
//********************************************************************************
void addDeviceSelect(String& str, String name,  int choice)
{
  // first get the list in alphabetic order
  for (byte x = 0; x <= deviceCount; x++)
    sortedIndex[x] = x;
  sortDeviceArray();

  String deviceName;

  str += F("<select name='");
  str += name;
  //str += "'>";
  str += "' LANGUAGE=javascript onchange=\"return dept_onchange(frmselect)\">";

  str += F("<option value='0'></option>");
  for (byte x = 0; x <= deviceCount; x++)
  {
    byte index = sortedIndex[x];
    if (Plugin_id[index] != 0)
      Plugin_ptr[index](PLUGIN_GET_DEVICENAME, 0, deviceName);
    str += F("<option value='");
    str += Device[index].Number;
    str += "'";
    if (choice == Device[index].Number)
      str += F(" selected");
    str += ">";
    str += deviceName;
    str += F("</option>");
  }
  str += F("</select>");
}


//********************************************************************************
// Device Sort routine, switch array entries
//********************************************************************************
void switchArray(byte value)
{
  byte temp;
  temp = sortedIndex[value - 1];
  sortedIndex[value - 1] = sortedIndex[value];
  sortedIndex[value] = temp;
}


//********************************************************************************
// Device Sort routine, compare two array entries
//********************************************************************************
byte arrayLessThan(char *ptr_1, char *ptr_2)
{
  char check1;
  char check2;

  int i = 0;
  while (i < strlen(ptr_1))    // For each character in string 1, starting with the first:
  {
    check1 = (char)ptr_1[i];  // get the same char from string 1 and string 2

    if (strlen(ptr_2) < i)    // If string 2 is shorter, then switch them
    {
      return 1;
    }
    else
    {
      check2 = (char)ptr_2[i];

      if (check2 > check1)
      {
        return 1;       // String 2 is greater; so switch them
      }
      if (check2 < check1)
      {
        return 0;       // String 2 is LESS; so DONT switch them
      }
      // OTHERWISE they're equal so far; check the next char !!
      i++;
    }
  }

  return 0;
}


//********************************************************************************
// Device Sort routine, actual sorting
//********************************************************************************
void sortDeviceArray()
{
  String deviceName;
  char deviceName1[41];
  char deviceName2[41];
  int innerLoop ;
  int mainLoop ;

  for ( mainLoop = 1; mainLoop <= deviceCount; mainLoop++)
  {
    innerLoop = mainLoop;
    while (innerLoop  >= 1)
    {
      Plugin_ptr[sortedIndex[innerLoop]](PLUGIN_GET_DEVICENAME, 0, deviceName);
      deviceName.toCharArray(deviceName1, 26);
      Plugin_ptr[sortedIndex[innerLoop - 1]](PLUGIN_GET_DEVICENAME, 0, deviceName);
      deviceName.toCharArray(deviceName2, 26);

      if (arrayLessThan(deviceName1, deviceName2) == 1)
      {
        switchArray(innerLoop);
      }
      innerLoop--;
    }
  }
}


//********************************************************************************
// Add a GPIO pin select dropdown list
//********************************************************************************
void addPinSelect(boolean forI2C, String& str, String name,  int choice)
{
  String options[11];
  options[0] = F(" ");
  options[1] = F("D2");
  options[2] = F("D3");
  options[3] = F("D4");
  options[4] = F("D5");
  options[5] = F("D6");
  options[6] = F("D7");
  options[7] = F("D8");
  options[8] = F("D9");
  options[9] = F("D11");
  options[10] = F("D12");
  int optionValues[11];
  optionValues[0] = -1;
  optionValues[1] = 2;
  optionValues[2] = 3;
  optionValues[3] = 4;
  optionValues[4] = 5;
  optionValues[5] = 6;
  optionValues[6] = 7;
  optionValues[7] = 8;
  optionValues[8] = 9;
  optionValues[9] = 11;
  optionValues[10] = 12;
  str += F("<select name='");
  str += name;
  str += "'>";
  for (byte x = 0; x < 11; x++)
  {
    str += F("<option value='");
    str += optionValues[x];
    str += "'";
    if (optionValues[x] != -1) // empty selection can never be disabled...
    {
      //if (Settings.UseSerial && ((optionValues[x] == 1) || (optionValues[x] == 3)))
      //  str += F(" disabled");
    }
    if (choice == optionValues[x])
      str += F(" selected");
    str += ">";
    str += options[x];
    str += F("</option>");
  }
  str += F("</select>");
}


//********************************************************************************
// Add a task select dropdown list
//********************************************************************************
void addTaskSelect(String& str, String name,  int choice)
{
  struct EventStruct TempEvent;
  String deviceName;

  str += F("<select name='");
  str += name;
  str += F("' LANGUAGE=javascript onchange=\"return dept_onchange(frmselect)\">");

  for (byte x = 0; x < TASKS_MAX; x++)
  {
    deviceName = "";
    if (Settings.TaskDeviceNumber[x] != 0 )
    {
      byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[x]);

      if (Plugin_id[DeviceIndex] != 0)
        Plugin_ptr[DeviceIndex](PLUGIN_GET_DEVICENAME, &TempEvent, deviceName);
    }
    LoadTaskSettings(x);
    str += F("<option value='");
    str += x;
    str += "'";
    if (choice == x)
      str += " selected";
    if (Settings.TaskDeviceNumber[x] == 0)
      str += " disabled";
    str += ">";
    str += x + 1;
    str += " - ";
    str += deviceName;
    str += " - ";
    str += ExtraTaskSettings.TaskDeviceName;
    str += "</option>";
  }
}


//********************************************************************************
// Add a Value select dropdown list, based on TaskIndex
//********************************************************************************
void addTaskValueSelect(String& str, String name,  int choice, byte TaskIndex)
{
  str += F("<select name='");
  str += name;
  str += "'>";

  byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[TaskIndex]);

  for (byte x = 0; x < Device[DeviceIndex].ValueCount; x++)
  {
    str += F("<option value='");
    str += x;
    str += "'";
    if (choice == x)
      str += " selected";
    str += ">";
    str += ExtraTaskSettings.TaskDeviceValueNames[x];
    str += "</option>";
  }
}

//********************************************************************************
// Web Interface rules page
//********************************************************************************
void handle_rules(EthernetClient client, String &post) {
  //if (!isLoggedIn()) return;

  String rules = post.substring(7);
  webdata = "";

  if (rules.length() > 0)
  {
    SD.remove("rules.txt");
    File f = SD.open("rules.txt", FILE_WRITE);
    if (f)
    {
      f.print(rules);
      f.close();
    }
  }

  rules = "";
  String reply = "";
  reply += F("<table><th>Rules<TR><TD>");

  // load form data from flash
  reply += F("<form method='post'>");
  reply += F("<textarea name='rules' rows='15' cols='80' wrap='off'>");
  client.print(reply);
  reply = "";

  File dataFile = SD.open("rules.txt");
  int filesize = dataFile.size();
  while (dataFile.available()) {
    client.write(dataFile.read());
  }
  reply += F("</textarea>");

  reply += F("<TR><TD>Current size: ");
  reply += filesize;
  reply += F(" characters (Max ");
  reply += RULES_MAX_SIZE;
  reply += F(")");

  reply += F("<TR><TD><input class=\"button-link\" type='submit' value='Submit'>");
  reply += F("</table></form>");
  addFooter(reply);
  client.print(reply);
}


//********************************************************************************
// Web Interface debug page
//********************************************************************************
void handle_tools(EthernetClient client, String &post) {
  //if (!isLoggedIn()) return;

  String webrequest = WebServerarg(F("cmd"));

  String reply = "";

  reply += F("<form>");
  reply += F("<table><TH>Tools<TH>");
  reply += F("<TR><TD>System<TD><a class=\"button-link\" href=\"/?cmd=reboot\">Reboot</a>");
  reply += F("<a class=\"button-link\" href=\"log\">Log</a>");
  reply += F("<a class=\"button-link\" href=\"sysinfo\">Info</a>");
  reply += F("<a class=\"button-link\" href=\"advanced\">Advanced</a><BR><BR>");
  reply += F("<TR><TD>Interfaces<TD><a class=\"button-link\" href=\"/i2cscanner\">I2C Scan</a><BR><BR>");
  reply += F("<TR><TD>Filesystem<TD><a class=\"button-link\" href=\"/SDfilelist\">SD Card</a><BR><BR>");

  reply += F("<TR><TD>Command<TD>");
  reply += F("<input type='text' name='cmd' value='");
  reply += webrequest;
  reply += F("'><TR><TD><TD><input class=\"button-link\" type='submit' value='Submit'><TR><TD>");

  printToWeb = true;
  printWebString = "";

  if (webrequest.length() > 0)
  {
    struct EventStruct TempEvent;
    parseCommandString(&TempEvent, webrequest);
    TempEvent.Source = VALUE_SOURCE_HTTP;
    if (!PluginCall(PLUGIN_WRITE, &TempEvent, webrequest))
      ExecuteCommand(VALUE_SOURCE_HTTP, webrequest.c_str());
  }

  if (printWebString.length() > 0)
  {
    reply += F("<TR><TD>Command Output<TD><textarea readonly rows='10' cols='60' wrap='on'>");
    reply += printWebString;
    reply += F("</textarea>");
  }
  reply += F("</table></form>");
  addFooter(reply);
  client.print(reply);
  printWebString = "";
  printToWeb = false;
}


//********************************************************************************
// Web Interface config page
//********************************************************************************
void handle_advanced(EthernetClient client, String &post) {
  //if (!isLoggedIn()) return;

  update_advanced();

  String reply = "";

  char str[20];

  reply += F("<form  method='post'><table>");
  reply += F("<TH>Advanced Settings<TH>Value");

  reply += F("<TR><TD>Subscribe Template:<TD><input type='text' name='mqttsubscribe' size=80 value='");
  reply += Settings.MQTTsubscribe;

  reply += F("'><TR><TD>Publish Template:<TD><input type='text' name='mqttpublish' size=80 value='");
  reply += Settings.MQTTpublish;

  reply += F("'><TR><TD>Message Delay (ms):<TD><input type='text' name='messagedelay' value='");
  reply += Settings.MessageDelay;

  reply += F("'><TR><TD>Fixed IP Octet:<TD><input type='text' name='ip' value='");
  reply += Settings.IP_Octet;
  reply += F("'>");

  client.print(reply);
  reply = "";

  reply += F("<TR><TD>Use NTP:<TD>");
  if (Settings.UseNTP)
    reply += F("<input type=checkbox name='usentp' checked>");
  else
    reply += F("<input type=checkbox name='usentp'>");

  reply += F("<TR><TD>NTP Hostname:<TD><input type='text' name='ntphost' size=64 value='");
  reply += Settings.NTPHost;

  reply += F("'><TR><TD>Timezone Offset: (Minutes)<TD><input type='text' name='timezone' size=2 value='");
  reply += Settings.TimeZone;
  reply += F("'>");

  reply += F("<TR><TD>DST:<TD>");
  if (Settings.DST)
    reply += F("<input type=checkbox name='dst' checked>");
  else
    reply += F("<input type=checkbox name='dst'>");

  client.print(reply);
  reply = "";

  reply += F("<TR><TD>Syslog IP:<TD><input type='text' name='syslogip' value='");
  str[0] = 0;
  sprintf_P(str, PSTR("%u.%u.%u.%u"), Settings.Syslog_IP[0], Settings.Syslog_IP[1], Settings.Syslog_IP[2], Settings.Syslog_IP[3]);
  reply += str;

  reply += F("'><TR><TD>Syslog Level:<TD><input type='text' name='sysloglevel' value='");
  reply += Settings.SyslogLevel;

  reply += F("'><TR><TD>UDP port:<TD><input type='text' name='udpport' value='");
  reply += Settings.UDPPort;
  reply += F("'>");

  reply += F("<TR><TD>Enable Serial port:<TD>");
  if (Settings.UseSerial)
    reply += F("<input type=checkbox name='useserial' checked>");
  else
    reply += F("<input type=checkbox name='useserial'>");

  reply += F("<TR><TD>Serial log Level:<TD><input type='text' name='serialloglevel' value='");
  reply += Settings.SerialLogLevel;

  reply += F("'><TR><TD>SD Card log Level:<TD><input type='text' name='sdloglevel' value='");
  reply += Settings.SDLogLevel;

  client.print(reply);
  reply = "";

  reply += F("'><TR><TD>Baud Rate:<TD><input type='text' name='baudrate' value='");
  reply += Settings.BaudRate;
  reply += F("'>");

  reply += F("<TR><TD>Rules:<TD>");
  if (Settings.UseRules)
    reply += F("<input type=checkbox name='userules' checked>");
  else
    reply += F("<input type=checkbox name='userules'>");

  reply += F("<TR><TD>Global Sync:<TD>");
  if (Settings.GlobalSync)
    reply += F("<input type=checkbox name='globalsync' checked>");
  else
    reply += F("<input type=checkbox name='globalsync'>");

  reply += F("<TR><TD><TD><input class=\"button-link\" type='submit' value='Submit'>");
  reply += F("<input type='hidden' name='edit' value='1'>");
  reply += F("</table></form>");
  addFooter(reply);
  client.print(reply);
}

void update_advanced()
{
  char tmpString[81];
  String arg = "";
  String edit = WebServerarg(F("edit"));

  if (edit.length() != 0)
  {
    arg = WebServerarg(F("mqttsubscribe"));
    arg.toCharArray(tmpString, 81);
    arg = WebServerarg(F("mqttpublish"));
    strcpy(Settings.MQTTsubscribe, tmpString);
    arg.toCharArray(tmpString, 81);
    strcpy(Settings.MQTTpublish, tmpString);
    arg = WebServerarg(F("messagedelay"));
    Settings.MessageDelay = arg.toInt();
    arg = WebServerarg(F("ip"));
    Settings.IP_Octet = arg.toInt();
    arg  = WebServerarg(F("ntphost"));
    arg.toCharArray(tmpString, 64);
    strcpy(Settings.NTPHost, tmpString);
    arg = WebServerarg(F("timezone"));
    Settings.TimeZone = arg.toInt();
    arg = WebServerarg(F("syslogip"));
    arg.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.Syslog_IP);
    arg = WebServerarg(F("udpport"));
    Settings.UDPPort = arg.toInt();
    arg = WebServerarg(F("sysloglevel"));
    Settings.SyslogLevel = arg.toInt();
    arg = WebServerarg(F("useserial"));
    Settings.UseSerial = (arg == "on");
    arg = WebServerarg(F("serialloglevel"));
    Settings.SerialLogLevel = arg.toInt();
    arg = WebServerarg(F("sdloglevel"));
    Settings.SDLogLevel = arg.toInt();
    arg = WebServerarg(F("baudrate"));
    Settings.BaudRate = arg.toInt();
    arg = WebServerarg(F("usentp"));
    Settings.UseNTP = (arg == "on");
    arg = WebServerarg(F("dst"));
    Settings.DST = (arg == "on");
    arg = WebServerarg(F("wdi2caddress"));
    Settings.WDI2CAddress = arg.toInt();
    arg = WebServerarg(F("userules"));
    Settings.UseRules = (arg == "on");
    arg = WebServerarg(F("globalsync"));
    Settings.GlobalSync = (arg == "on");
    arg = WebServerarg(F("cft"));
    Settings.ConnectionFailuresThreshold = arg.toInt();
    SaveSettings();
  }
}


//********************************************************************************
// Web Interface control page (no password!)
//********************************************************************************
void handle_control(EthernetClient client, String &post) {

  String webrequest = WebServerarg(F("cmd"));

  // in case of event, store to buffer and return...
  String command = parseString(webrequest, 1);
  if (command == F("event"))
  {
    eventBuffer = webrequest.substring(6);
    client.print("OK");
  }

  struct EventStruct TempEvent;
  parseCommandString(&TempEvent, webrequest);
  TempEvent.Source = VALUE_SOURCE_HTTP;

  printToWeb = true;
  printWebString = "";
  String reply = "";

  if (!PluginCall(PLUGIN_WRITE, &TempEvent, webrequest))
    reply += F("Unknown or restricted command!");

  reply += printWebString;

  client.println(F("HTTP/1.1 200 OK"));
  client.print(F("Content-Type:"));
  if (printToWebJSON)
    client.print("Content-Type: application/json");
  else
    client.print("Content-Type: text/html");
  client.println(F("Connection: close"));
  client.println("");
  client.print(reply);
  printWebString = "";
  printToWeb = false;
}


//********************************************************************************
// Web Interface SD card file list
//********************************************************************************
void handle_SDfilelist(EthernetClient client, String &post) {

  String fdelete = WebServerarg(F("delete"));

  if (fdelete.length() > 0)
  {
    SD.remove((char*)fdelete.c_str());
  }

  String reply = "";
  reply += F("<table cellpadding='4' border='1' frame='box' rules='all'><TH><TH>Filename:<TH>Size");

  File root = SD.open("/");
  root.rewindDirectory();
  File entry = root.openNextFile();
  while (entry)
  {
    if (!entry.isDirectory())
    {
      reply += F("<TR><TD>");
      if (entry.name() != "CONFIG.TXT" && entry.name() != "SECURITY.TXT")
      {
        reply += F("<a class=\"button-link\" href=\"SDfilelist?delete=");
        reply += entry.name();
        reply += F("\">Del</a>");
      }
      reply += F("<TD><a href=\"");
      reply += entry.name();
      reply += F("\">");
      reply += entry.name();
      reply += F("</a>");
      reply += F("<TD>");
      reply += entry.size();
    }
    entry.close();
    entry = root.openNextFile();
  }
  //entry.close();
  root.close();
  reply += F("</table></form>");
  //reply += F("<BR><a class=\"button-link\" href=\"/upload\">Upload</a>");
  addFooter(reply);
  client.print(reply);
}

//********************************************************************************
// URNEncode char string to string object
//********************************************************************************
String URLEncode(const char* msg)
{
  const char *hex = "0123456789abcdef";
  String encodedMsg = "";

  while (*msg != '\0') {
    if ( ('a' <= *msg && *msg <= 'z')
         || ('A' <= *msg && *msg <= 'Z')
         || ('0' <= *msg && *msg <= '9') ) {
      encodedMsg += *msg;
    } else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
    }
    msg++;
  }
  return encodedMsg;
}


//********************************************************************************
// Web Interface server web file from SPIFFS
//********************************************************************************
bool handle_unknown(EthernetClient client, String path) {
  //if (!isLoggedIn()) return false;

  String dataType = F("text/plain");
  if (path.endsWith("/")) path += F("index.htm");

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(F(".htm"))) dataType = F("text/html");
  else if (path.endsWith(F(".css"))) dataType = F("text/css");
  else if (path.endsWith(F(".js"))) dataType = F("application/javascript");
  else if (path.endsWith(F(".png"))) dataType = F("image/png");
  else if (path.endsWith(F(".gif"))) dataType = F("image/gif");
  else if (path.endsWith(F(".jpg"))) dataType = F("image/jpeg");
  else if (path.endsWith(F(".ico"))) dataType = F("image/x-icon");
  else if (path.endsWith(F(".txt"))) dataType = F("application/octet-stream");
  else if (path.endsWith(F(".dat"))) dataType = F("application/octet-stream");

  File dataFile = SD.open(path.c_str());
  if (!dataFile)
    return false;

  client.println(F("HTTP/1.1 200 OK"));
  client.print(F("Content-Type:"));
  client.println(dataType);
  client.println(F("Connection: close"));

  if (path.endsWith(F(".TXT")))
  {
    client.println(F("Content-Disposition:attachment"));
  }

  client.println();

  while (dataFile.available()) {
    client.write(dataFile.read());
  }
  dataFile.close();
  return true;
}


//********************************************************************************
// Web Interface I2C scanner
//********************************************************************************
void handle_i2cscanner(EthernetClient client, String path) {
  //if (!isLoggedIn()) return;

  String reply = "";
  reply += F("<table cellpadding='4' border='1' frame='box' rules='all'><TH>I2C Addresses in use<TH>Supported devices");

  byte error, address;
  int nDevices;
  nDevices = 0;
  for (address = 1; address <= 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      reply += "<TR><TD>0x";
      reply += String(address, HEX);
      reply += "<TD>";
      switch (address)
      {
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x25:
        case 0x26:
        case 0x27:
          reply += F("PCF8574<BR>MCP23017<BR>LCD");
          break;
        case 0x23:
          reply += F("PCF8574<BR>MCP23017<BR>LCD<BR>BH1750");
          break;
        case 0x24:
          reply += F("PCF8574<BR>MCP23017<BR>LCD<BR>PN532");
          break;
        case 0x29:
          reply += F("TLS2561");
          break;
        case 0x38:
        case 0x3A:
        case 0x3B:
        case 0x3E:
        case 0x3F:
          reply += F("PCF8574A");
          break;
        case 0x39:
          reply += F("PCF8574A<BR>TLS2561");
          break;
        case 0x3C:
        case 0x3D:
          reply += F("PCF8574A<BR>OLED");
          break;
        case 0x40:
          reply += F("SI7021<BR>INA219<BR>PCA9685");
          break;
        case 0x41:
        case 0x42:
        case 0x43:
          reply += F("INA219");
          break;
        case 0x48:
        case 0x4A:
        case 0x4B:
          reply += F("PCF8591<BR>ADS1115");
          break;
        case 0x49:
          reply += F("PCF8591<BR>ADS1115<BR>TLS2561");
          break;
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F:
          reply += F("PCF8591");
          break;
        case 0x5A:
          reply += F("MLX90614");
          break;
        case 0x5C:
          reply += F("DHT12<BR>BH1750");
          break;
        case 0x76:
          reply += F("BME280<BR>BMP280<BR>MS5607<BR>MS5611");
          break;
        case 0x77:
          reply += F("BMP085<BR>BMP180<BR>BME280<BR>BMP280<BR>MS5607<BR>MS5611");
          break;
        case 0x7f:
          reply += F("Arduino PME");
          break;
      }
      nDevices++;
    }
    else if (error == 4)
    {
      reply += F("<TR><TD>Unknown error at address 0x");
      reply += String(address, HEX);
    }
  }

  if (nDevices == 0)
    reply += F("<TR>No I2C devices found");

  reply += F("</table>");
  addFooter(reply);
  client.print(reply);
}

//********************************************************************************
// Web Interface log page
//********************************************************************************
void handle_log(EthernetClient client, String path) {
  //if (!isLoggedIn()) return;

  String reply = "";
  reply += F("<script language='JavaScript'>function RefreshMe(){window.location = window.location}setTimeout('RefreshMe()', 3000);</script>");
  reply += F("<table><TH>Log<TR><TD>");

  File dataFile = SD.open("log.txt");
  if (!dataFile)
    return;
  unsigned long filesize = dataFile.size();
  unsigned long position = 0;
  if (filesize > 1000)
  {
    position = filesize - 500;
    dataFile.seek(position);
    while (dataFile.available()) { // read until first complete line from this position
      char data = dataFile.read();
      if (data == '\n')
        break;
    }
  }
  while (dataFile.available()) {
    char data = dataFile.read();
    if (data == '\n')
      client.print("<BR>");
    else
      client.write(data);
  }
  reply += F("</table>");
  addFooter(reply);
  client.print(reply);
}


//********************************************************************************
// Web Interface root page
//********************************************************************************
void handle_sysinfo(EthernetClient client, String path) {
  //if (!isLoggedIn()) return;

  int freeMem = FreeMem();
  String reply = "";
  
  IPAddress ip = Ethernet.localIP();
  IPAddress gw = Ethernet.gatewayIP();

  reply += printWebString;
  reply += F("<form>");
  reply += F("<table><TH>System Info<TH>");

  reply += F("<TR><TD>Unit:<TD>");
  reply += Settings.Unit;

  if (Settings.UseNTP)
  {
    reply += F("<TR><TD>System Time:<TD>");
    reply += hour();
    reply += ":";
    if (minute() < 10)
      reply += "0";
    reply += minute();
  }

  reply += F("<TR><TD>Uptime:<TD>");
  char strUpTime[40];
  int minutes = wdcounter / 2;
  int days = minutes / 1440;
  minutes = minutes % 1440;
  int hrs = minutes / 60;
  minutes = minutes % 60;
  sprintf_P(strUpTime, PSTR("%d days %d hours %d minutes"), days, hrs, minutes);
  reply += strUpTime;

  reply += F("<TR><TD>Load:<TD>");
  if (wdcounter > 0)
  {
    reply += 100 - (100 * loopCounterLast / loopCounterMax);
    reply += F("% (LC=");
    reply += int(loopCounterLast / 30);
    reply += F(")");
  }

  reply += F("<TR><TD>Free Mem:<TD>");
  reply += freeMem;
  //reply += F(" (");
  //reply += lowestRAM;
  //reply += F(")");

  char str[20];
  sprintf_P(str, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
  reply += F("<TR><TD>IP:<TD>");
  reply += str;

  sprintf_P(str, PSTR("%u.%u.%u.%u"), gw[0], gw[1], gw[2], gw[3]);
  reply += F("<TR><TD>GW:<TD>");
  reply += str;

  reply += F("<TR><TD>Build:<TD>");
  reply += BUILD;
  reply += F(" ");
  reply += F(BUILD_NOTES);

  reply += F("<TR><TD>Devices:<TD>");
  reply += deviceCount + 1;

  reply += F("</table></form>");
  addFooter(reply);
  client.print(reply);
}


//********************************************************************************
// Decode special characters in URL of get/post data
//********************************************************************************
String URLDecode(const char *src)
{
  String rString;
  const char* dst = src;
  char a, b;

  while (*src) {

    if (*src == '+')
    {
      rString += ' ';
      src++;
    }
    else
    {
      if ((*src == '%') &&
          ((a = src[1]) && (b = src[2])) &&
          (isxdigit(a) && isxdigit(b))) {
        if (a >= 'a')
          a -= 'a' - 'A';
        if (a >= 'A')
          a -= ('A' - 10);
        else
          a -= '0';
        if (b >= 'a')
          b -= 'a' - 'A';
        if (b >= 'A')
          b -= ('A' - 10);
        else
          b -= '0';
        rString += (char)(16 * a + b);
        src += 3;
      }
      else {
        rString += *src++;
      }
    }
  }
  return rString;
}

