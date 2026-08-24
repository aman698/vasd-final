====================================================================
 VASD -- Vehicle & Ambient Speed Display
====================================================================

RP2040 (Arduino) firmware driving a 128x96 HUB75 RGB LED matrix
(4x3 panels of 32x32) over Ethernet (W5500). The panel shows a live
HH:MM:SS clock and can be commanded over TCP to show a vehicle speed
or update its own network settings.

--------------------------------------------------------------------
 HARDWARE PINOUT
--------------------------------------------------------------------
All pins are defined in config.h.

HUB75 panel:
  Signal                     GPIO
  -------------------------  ----
  A  (row address)           6
  B  (row address)           7
  C  (row address)           8
  D  (row address)           9
  E  (row address)           not used (-1)
  /OE (output enable)        10
  SCLK (latch clock)         12
  CLK  (shift clock)         11
  R0                         0
  G0                         1
  B0                         2
  R1                         3
  G1                         4
  B1                         5

  Panel chain: 4 panels across x 3 panels down = 128x96 pixels total
  (DISPLAYS_ACROSS / DISPLAYS_DOWN in config.h).
  Panel type: RGB32x32_S8_maxmurugan, colour mode: COLOR_4BITS.

W5500 Ethernet module (SPI0):
  Signal        GPIO
  ------------  ----
  MISO          16
  SCK           18
  MOSI          19
  CS / SS       17
  RSTn          24

Misc:
  Signal                                          GPIO
  ----------------------------------------------  ----
  Heartbeat LED                                   26
  Hard-reset button (active LOW, INPUT_PULLUP)    27
  LDR (light sensor)                               not used (-1)

--------------------------------------------------------------------
 NETWORK CONFIGURATION
--------------------------------------------------------------------
The board keeps TWO INDEPENDENT IPs in EEPROM, so they can be
changed without affecting each other:

  IP           Purpose                          Default          Set via
  -----------  -------------------------------  ---------------  -----------
  MCU IP       The board's own Ethernet address  192.168.0.125    SET
  PC NTP IP    PC that supplies time via NTP     192.168.0.11     TIMERIP

Other fixed network settings (gateway, subnet, DNS, MAC) are
compiled in via gateway_array, subnet_array, dns_array,
SERVER_MAC in config.h.

EEPROM layout (EEPROM.begin(64)):
  Bytes    Contents                              Macro
  -------  ------------------------------------  --------------------------------
  0-3      MCU IP octets                         EEPROM_IP_ADDR
  5        MCU server port                       EEPROM_PORT_ADDR
  10-13    PC NTP IP octets                      EEPROM_TIMER_PC_IP_ADDR
  14       PC NTP IP valid marker (0xBB)          EEPROM_TIMER_PC_IP_VALID_ADDR
  20-23    Last synced Unix timestamp            EEPROM_TIME_ADDR
  24       Time valid marker (0xAA)               EEPROM_TIME_VALID_ADDR

--------------------------------------------------------------------
 TCP COMMAND REFERENCE
--------------------------------------------------------------------
Commands are sent as plain comma-separated ASCII text to the
board's TCP server, SERVER_PORT (default 1000).

1) SET
   Format : SET,a,b,c,d
   Example: SET,192,168,0,130
   Effect : Sets the MCU's OWN IP to a.b.c.d, saves it to EEPROM,
            and reconfigures Ethernet immediately.
            Does NOT touch the PC NTP IP or the timer.

2) TIMERIP
   Format : TIMERIP,a,b,c,d
   Example: TIMERIP,192,168,0,11
   Effect : Saves the PC NTP SERVER IP to EEPROM. Takes effect on
            the NEXT boot (used by syncNTPTime()).
            Does NOT touch the MCU IP or Ethernet config.

3) SPEED
   Format : SPEED,speed,colorCase
   Example: SPEED,80,4
   Effect : Displays the given speed on the panel using colour
            case 1-9 (see writeToDisplay()); speeds over 100 show
            an "OVERSPEED" banner in red. Replies to the sender
            with "<speed> <colorCase>".

Each octet in SET / TIMERIP must be a plain decimal number 0-255.
Malformed values are rejected and nothing is written to EEPROM.

--------------------------------------------------------------------
 TIME SYNCHRONIZATION (NTP)
--------------------------------------------------------------------
Time is NOT requested per-command. Once, at boot, the board:

  1. Configures its own IP (from EEPROM, or 192.168.0.125 default).
  2. Reads the PC NTP IP from EEPROM (or 192.168.0.11 default).
  3. Sends a UDP NTP request to that IP on PORT 123.
  4. On success, converts the NTP reply to a Unix timestamp, saves
     it to EEPROM, and starts the on-screen clock from it
     (converted to IST, UTC+5:30, via TIME_ZONE_OFFSET_SECONDS).
  5. On failure, restores the last successfully synced time from
     EEPROM (stale by however long the board was powered off) and
     keeps running.

syncNTPTime() is only ever called from setup() -- never from loop().

--------------------------------------------------------------------
 HARD RESET
--------------------------------------------------------------------
Holding the hard-reset button (GPIO 27) LOW for more than 10
seconds resets the MCU IP ONLY to 192.168.0.125 (port byte to 255)
in EEPROM. The PC NTP IP and the last saved time are left
untouched. The new IP takes effect on the next power cycle / reset
(the board does not reboot itself automatically).

--------------------------------------------------------------------
 FIREWALL CONFIGURATION (WINDOWS PC SIDE)
--------------------------------------------------------------------
The PC plays two roles on this network, and Windows Firewall blocks
both by default unless you open them:

  1. NTP SERVER      -- the PC must accept inbound UDP PORT 123
                         requests from the board.
  2. COMMAND SENDER   -- whatever PC-side app sends SET / TIMERIP /
                         SPEED needs to reach the board's TCP PORT
                         1000 (outbound from the PC's point of
                         view -- usually allowed by default, but
                         add an explicit rule if your firewall
                         policy is locked down).

Run these from an ELEVATED (Administrator) PowerShell or Command
Prompt on the PC:

  REM 1. Allow inbound UDP 123 (NTP requests coming FROM the board)
  netsh advfirewall firewall add rule name="VASD NTP Server (UDP 123 In)" dir=in action=allow protocol=UDP localport=123

  REM 2. Allow outbound TCP 1000 (commands going TO the board)
  netsh advfirewall firewall add rule name="VASD Board Command (TCP 1000 Out)" dir=out action=allow protocol=TCP remoteport=1000

  REM 3. (Optional) Allow inbound TCP 1000 too, in case the PC app
  REM    also needs to receive unsolicited connections back from
  REM    the board
  netsh advfirewall firewall add rule name="VASD Board Command (TCP 1000 In)" dir=in action=allow protocol=TCP localport=1000

Equivalent steps via the GUI:

  1. Open "Windows Defender Firewall with Advanced Security" (wf.msc).
  2. Inbound Rules -> New Rule -> Port -> UDP -> Specific local
     ports: 123 -> Allow the connection -> apply to all profiles
     (Domain/Private/Public as appropriate for your network) ->
     name it e.g. "VASD NTP Server".
  3. Outbound Rules -> New Rule -> Port -> TCP -> Specific remote
     ports: 1000 -> Allow the connection -> name it e.g.
     "VASD Board Command".

To verify the rules exist:

  netsh advfirewall firewall show rule name="VASD NTP Server (UDP 123 In)"
  netsh advfirewall firewall show rule name="VASD Board Command (TCP 1000 Out)"

To remove them later:

  netsh advfirewall firewall delete rule name="VASD NTP Server (UDP 123 In)"
  netsh advfirewall firewall delete rule name="VASD Board Command (TCP 1000 Out)"
  netsh advfirewall firewall delete rule name="VASD Board Command (TCP 1000 In)"

NOTE: Having an NTP SERVICE actually listening on UDP 123 on the PC
(Windows' built-in w32time, or a third-party NTP server) is a
separate requirement from the firewall rule above -- the firewall
rule only lets the traffic through; it doesn't start the service.
