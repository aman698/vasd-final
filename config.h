#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#pragma GCC diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic ignored "-Woverflow"
#pragma GCC diagnostic warning "-Wnarrowing"
#pragma GCC diagnostic warning "-Woverflow"

/*
   PIN DEFINES FOR PANEL
*/
#define PIN_A 6
#define PIN_B 7
#define PIN_C 8
#define PIN_D 9
#define PIN_E -1
#define PIN_nOE 10
#define PIN_SCLK 12
uint8_t addressMuxList[] = { PIN_A , PIN_B , PIN_C };
uint8_t RGBPinsList[] = { 11, 0, 1, 2, 3, 4, 5 }; // CLK, R0, G0, B0, R1, G1, B1

/*
 * Panel Specifications
 * Number of Panels in X Direction
 * Number of Panels in Y Direction
 * Dual Buffering Enable/Disable (Use swapBuffers(true) command)
 * 
 * Type of Panel Being Used
 * Colour Mode Used
 * Maximum Brightness
 * Minimum Brightness
 * 
 */
 
#define DISPLAYS_ACROSS 4              
#define DISPLAYS_DOWN 3
#define ENABLE_DUAL_BUFFER false
#define PANELTYPE   RGB32x32_S8_maxmurugan                       //RGB32x32plainS16
#define PANELCOLORMODE COLOR_4BITS
#define MaxPanelBrightness 255
#define MinPanelBrightness 0

/*
 * COLOUR DEFINES
 */
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFA00
#define WHITE    0xFFFF
#define ORANGE   0xFCBC

// W5500 SPI wiring - RP2040 default SPI0 pins: MISO=16, SCK=18, MOSI=19, CS/SS=17
#define ETHERNET_SS_PIN 17
// W5500 module RSTn pin, wired to GPIO24
#define ETHERNET_RST_PIN 24
#define HeartBeatLed 25
#define HARDRST 27
#define LDR -1
/*
   ETHERNET CONFIGS
*/

#define SERVER_PORT 1000
byte SERVER_MAC[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
const int ip_array[4] = {192, 168, 0, 125};
const int gateway_array[4] = {192, 168, 0, 1};
const int subnet_array[4] = {255, 255, 255, 0};
const int dns_array[4] = {192, 168, 0, 1};
// Used only when EEPROM has no valid PC Timer IP yet (first boot / old firmware).
// Same host that used to act as the local NTP server.
const int default_pc_timer_ip_array[4] = {192, 168, 0, 11};

// Local time zone offset from UTC, used to convert the PC-supplied UTC unix
// time to wall-clock time on the display. IST = UTC+5:30.
#define TIME_ZONE_OFFSET_SECONDS (5 * 3600 + 30 * 60)

/*
   EEPROM LAYOUT (EEPROM.begin(64) - 64 bytes available, this uses 0-24, 40-44)
   MCU IP octets       : bytes 0-3   (EEPROM_IP_ADDR .. +3)
   MCU server port     : byte  5     (EEPROM_PORT_ADDR)
   (bytes 6-9 reserved/unused - kept clear as a gap)
   PC Timer IP octets  : bytes 10-13 (EEPROM_TIMER_PC_IP_ADDR .. +3)
   PC Timer IP valid   : byte  14    (EEPROM_TIMER_PC_IP_VALID_ADDR, EEPROM_TIMER_PC_IP_VALID_MAGIC = valid)
   (bytes 15-19 reserved/unused - kept clear as a gap)
   Last known unix time: bytes 20-23 (EEPROM_TIME_ADDR, unsigned long, 4 bytes)
   Time-valid flag     : byte  24    (EEPROM_TIME_VALID_ADDR, EEPROM_TIME_VALID_MAGIC = valid)
   (bytes 25-39 reserved/unused - kept clear as a gap)
   Max speed limit     : bytes 40-43 (EEPROM_MAX_SPEED_ADDR, int, 4 bytes)
   Max speed valid flag: byte  44    (EEPROM_MAX_SPEED_VALID_ADDR, EEPROM_MAX_SPEED_VALID_MAGIC = valid)

   Each block has its own address range so a SET command can never overwrite
   the PC Timer IP or the saved time, a TIMERIP command can never overwrite
   the MCU IP, a time sync can never overwrite either IP, and a MAXLIMIT
   command can never overwrite any of the above (or vice versa).
*/
#define EEPROM_IP_ADDR                 0
#define EEPROM_PORT_ADDR               5

#define EEPROM_TIMER_PC_IP_ADDR        10
#define EEPROM_TIMER_PC_IP_VALID_ADDR  14
#define EEPROM_TIMER_PC_IP_VALID_MAGIC 0xBB

#define EEPROM_TIME_ADDR               20
#define EEPROM_TIME_VALID_ADDR         24
#define EEPROM_TIME_VALID_MAGIC        0xAA

#define EEPROM_MAX_SPEED_ADDR        40
#define EEPROM_MAX_SPEED_VALID_ADDR  44
#define EEPROM_MAX_SPEED_VALID_MAGIC 0xA5

#define DEFAULT_MAX_SPEED_LIMIT      100

/*
   SERIAL CONFIGS
*/
#define SERIAL_BAUDRATE 115200
#define SERIAL_INTERFACE Serial
#define COM_PORT Serial



#endif
