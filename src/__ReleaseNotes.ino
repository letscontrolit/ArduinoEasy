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

