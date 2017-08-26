// R154 26-08-2017
// Removed pin select D4 from the list as it is used for SD Card
// Fixed Dallas plugin for AVR

// R153 25-08-2017
// Added nodetype to self.
// Minor cosmetic GUI fixes

// R152 21-08-2017
// Fixed a bug with getting NTP time when nodelist is active and fixed a socket issue when combined with MQTT
// Dummy WebServer class to mimic WebServer.arg calls from plugins
// Fixed GUI formatting bug when NPT disabled
// Added some GUI handlers from the Mega branch
// Added NodeType in main screen

// R151 14-08-2017
// Update Mini Dashboard

// R150 12-08-2017
// Fixed some webgui bugs and reduced RAM usage
// Fixed syslog feature
// Added some plugins: Dallas, DHT, DHT12
// Added authentication for C001 (Base64 lib added)
// Added Mini Dashboard

// R147 08-12-2016
// First alpha version (Proof Of Concept!)
// Based on ESP Easy R147

// Limited on several features:
//   SSDP is not implemented
//   No SPIFFS but using SD card instead. So it does not run without an SD card fitted!
//   Reused existing UDP socket for NTP, due to shortage of W5100 sockets.

// Known issues:
//   WD message was incorrect, failures reported as freemem, freemem is missing, workaround is using string object...
//   when timed reboot, system does not reboot ???
