#include <SPI.h>
#include <EEPROM.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "stm_int.h"
#include "bitmap.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "config.h"
#include "commands_helper.h"
#include "DMD_RGB.h"
#include <stdio.h>


#include "fonts/FONT8.h"
#include "fonts/FONT10.h"
#include "fonts/FONT12.h"
#include "fonts/FONT14.h"
#include "fonts/FONT16.h"
#include "fonts/FONT18.h"
#include "fonts/FONT20.h"
#include "fonts/FONT22.h"
#include "fonts/FONT24.h"
#include "fonts/FONT26.h"
#include "fonts/FONT28.h"
#include "fonts/FONT30.h"
#include "fonts/FONT32.h"
#include "fonts/FONT34.h"
#include "fonts/FONT36.h"
#include "fonts/FONT38.h"
#include "fonts/FONT40.h"
#include "fonts/FONT42.h"
#include "fonts/FONT44.h"
#include "fonts/FONT46.h"
#include "fonts/FONT48.h"
#include "fonts/FONT50.h"
#include "fonts/FONT52.h"
#include "fonts/FONT54.h"
#include "fonts/FONT56.h"
#include "fonts/FONT58.h"
#include "fonts/FONT60.h"
#include "fonts/FONT62.h"
#include "fonts/FONT64.h"
#include "fonts/FONT66.h"
#include "fonts/FONT68.h"
#include "fonts/FONT70.h"
#include "fonts/FONT86.h"
#include "fonts/FONT95.h"

#include "fonts/Bahnschrift_Condensed30_b_033_126.h"
#include "fonts/Bahnschrift_Condensed36_b_037_057.h"
//#include "Bahnschrift_Condensed48_b_037_057.h"
#include "fonts/Microsoft_Sans_Serif84_033_126.h"
//#include "gfx_fonts/GlametrixBold12pt7b.h"
#include "st_fonts/SystemFont5x7.h"

const uint16_t* bitmaps[] = {flag};

unsigned long timerStart = 0;
EthernetUDP ntpUDP;

// Public NTP servers are unreachable from this network segment (192.168.0.x has
// no internet gateway) - use the PC on the same subnet as a local NTP source.
IPAddress ntpServer(192, 168, 0, 11);   // PC acting as local NTP server
const unsigned int NTP_PORT = 123;

unsigned long ntpTime = 0;
bool ntpSynced = false;

struct adc {
  volatile uint32_t CS, RESULT,
           FCS, FIFO, DIV, INTR, INTE, INTF,
           INTS;
};
#define ADC ((struct adc*)0x4004C000)

EthernetClient client;
EthernetServer server = EthernetServer(SERVER_PORT);
String  AliveN = "START,ALIVE,";

DMD_RGB <PANELTYPE, PANELCOLORMODE> PANEL(
  addressMuxList,
  PIN_nOE,
  PIN_SCLK,
  RGBPinsList,
  DISPLAYS_ACROSS,
  DISPLAYS_DOWN,
  ENABLE_DUAL_BUFFER);

DMD_Standard_Font font1(FONT8);
DMD_Standard_Font font2(FONT10);
DMD_Standard_Font font3(FONT12);
DMD_Standard_Font font4(FONT14);
DMD_Standard_Font font5(FONT16);
DMD_Standard_Font font6(FONT18);
DMD_Standard_Font font7(FONT20);
DMD_Standard_Font font8(FONT22);
DMD_Standard_Font font9(FONT24);
DMD_Standard_Font font10(FONT26);
DMD_Standard_Font font11(FONT28);
DMD_Standard_Font font12(FONT30);
DMD_Standard_Font font13(FONT32);
DMD_Standard_Font font14(FONT34);
DMD_Standard_Font font15(FONT36);
DMD_Standard_Font font16(FONT38);
DMD_Standard_Font font17(FONT40);
DMD_Standard_Font font18(FONT42);
DMD_Standard_Font font19(FONT44);
DMD_Standard_Font font20(FONT46);
DMD_Standard_Font font21(FONT48);
DMD_Standard_Font font22(FONT50);
DMD_Standard_Font font23(FONT52);
DMD_Standard_Font font24(FONT54);
DMD_Standard_Font font25(FONT56);
DMD_Standard_Font font26(FONT58);
DMD_Standard_Font font27(FONT60);
DMD_Standard_Font font28(FONT62);
DMD_Standard_Font font29(FONT64);
DMD_Standard_Font font30(FONT66);
DMD_Standard_Font font31(FONT68);
DMD_Standard_Font font32(FONT70);
DMD_Standard_Font font33(FONT86);
DMD_Standard_Font font34(FONT95);


DMD_Standard_Font fontHeadline(Bahnschrift_Condensed30_b_033_126);
DMD_Standard_Font font36(Bahnschrift_Condensed36_b_037_057);
DMD_Standard_Font font84(Microsoft_Sans_Serif84_033_126);

float getTemperature();
void HeartBeat();
void checkHardReset();
String getMessageEthernet();
int parseCommand(String str);
void printAlive();
void writeToDisplay();
void drawImage(int imageSize);
void clearDisplay(int x1, int x2, int y1, int y2);

void clearDisplay(int x1, int y1, int x2, int y2) {
  int i = x1;
  while (y1 < y2) {
    while (i < x2) {
      PANEL.drawPixel(i, y1, BLACK);
      i++;
    }
    y1++;
    i = x1;
  }
}

void drawTimeCounter()
{
    unsigned long displaySeconds;

    if (ntpSynced)
    {
        // Wall-clock time: NTP unix time at sync + seconds elapsed since sync,
        // converted from UTC to local time-of-day.
        unsigned long currentUnixTime = ntpTime + (millis() - timerStart) / 1000;
        displaySeconds = (currentUnixTime + TIME_ZONE_OFFSET_SECONDS) % 86400UL;
    }
    else
    {
        // No valid NTP time yet - fall back to elapsed-since-boot stopwatch.
        displaySeconds = (millis() - timerStart) / 1000;
    }

    int hours   = displaySeconds / 3600;
    int minutes = (displaySeconds % 3600) / 60;
    int seconds = displaySeconds % 60;

    char timeStr[9];

    sprintf(timeStr, "%02d:%02d:%02d",
            hours,
            minutes,
            seconds);

    // Small font for bottom timer
    PANEL.selectFont(&font7);   // FONT10

    PANEL.setTextColor(WHITE, BLACK);

    // 128x96 display
    // FONT10 approx. 10 px height
    // Bottom-right position
    PANEL.drawStringX(61, 80, timeStr, WHITE, 0);
}
void writeToDisplay(int x, int y, String text, int fontColour, int fontSize) {
  switch (fontSize) {
    case 1: PANEL.selectFont(&font1); break;
    case 2: PANEL.selectFont(&font2); break;
    case 3: PANEL.selectFont(&font3); break;
    case 4: PANEL.selectFont(&font4); break;
    case 5: PANEL.selectFont(&font5); break;
    case 6: PANEL.selectFont(&font6); break;
    case 7: PANEL.selectFont(&font7); break;
    case 8: PANEL.selectFont(&font8); break;
    case 9: PANEL.selectFont(&font9); break;
    case 10: PANEL.selectFont(&font10); break;
    case 11: PANEL.selectFont(&font11); break;
    case 12: PANEL.selectFont(&font12); break;
    case 13: PANEL.selectFont(&font13); break;
    case 14: PANEL.selectFont(&font14); break;
    case 15: PANEL.selectFont(&font15); break;
    case 16: PANEL.selectFont(&font16); break;
    case 17: PANEL.selectFont(&font17); break;
    case 18: PANEL.selectFont(&font18); break;
    case 19: PANEL.selectFont(&font19); break;
    case 20: PANEL.selectFont(&font20); break;
    case 21: PANEL.selectFont(&font21); break;
    case 22: PANEL.selectFont(&font22); break;
    case 23: PANEL.selectFont(&font23); break;
    case 24: PANEL.selectFont(&font24); break;
    case 25: PANEL.selectFont(&font25); break;
    case 26: PANEL.selectFont(&font26); break;
    case 27: PANEL.selectFont(&font27); break;
    case 28: PANEL.selectFont(&font28); break;
    case 29: PANEL.selectFont(&font29); break;
    case 30: PANEL.selectFont(&font30); break;
    case 31: PANEL.selectFont(&font31); break;
    case 32: PANEL.selectFont(&font32); break;
    case 33: PANEL.selectFont(&font84); break;
    case 34: PANEL.selectFont(&font34); break;
  }

  char buf[100];
  text.toCharArray(buf, text.length() + 1);
  int fclr = 0;
  //fclr = fontColour;

  switch (fontColour) {
    case 1: fclr = BLACK; break;
    case 2: fclr = BLUE; break;
    case 3: fclr = RED; break;
    case 4: fclr = GREEN; break;
    case 5: fclr = CYAN; break;
    case 6: fclr = MAGENTA; break;
    case 7: fclr = YELLOW; break;
    case 8: fclr = WHITE; break;
    case 9: fclr = ORANGE; break;
    default: fclr = fontColour; break;
  }

  PANEL.setTextColor(fclr, BLACK);
  PANEL.drawStringX(x, y, buf, fclr, 0);
}
// Persist the last successfully synced NTP time in its own EEPROM block
// (bytes 10-14, separate from the IP/port block at bytes 0-5) so a power
// cycle can restore it even if a fresh NTP sync isn't available yet.
void saveTimeToEEPROM(unsigned long unixTime)
{
    EEPROM.put(EEPROM_TIME_ADDR, unixTime);
    EEPROM.write(EEPROM_TIME_VALID_ADDR, EEPROM_TIME_VALID_MAGIC);
    EEPROM.commit();
}

bool loadTimeFromEEPROM(unsigned long &unixTime)
{
    if (EEPROM.read(EEPROM_TIME_VALID_ADDR) != EEPROM_TIME_VALID_MAGIC)
        return false;

    EEPROM.get(EEPROM_TIME_ADDR, unixTime);
    return true;
}

bool syncNTPTime()
{
    const int NTP_PACKET_SIZE = 48;
    byte packetBuffer[NTP_PACKET_SIZE];

    memset(packetBuffer, 0, NTP_PACKET_SIZE);

    // NTP request
    packetBuffer[0] = 0b11100011;
    packetBuffer[1] = 0;
    packetBuffer[2] = 6;
    packetBuffer[3] = 0xEC;

    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    ntpSynced = false;

    Serial.println("----- NTP DEBUG -----");
    Serial.print("Local IP: ");
    Serial.println(Ethernet.localIP());
    Serial.print("Gateway: ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("DNS: ");
    Serial.println(Ethernet.dnsServerIP());
    Serial.print("NTP Server: ");
    Serial.println(ntpServer);
    Serial.print("Link status: ");
    switch (Ethernet.linkStatus())
    {
        case LinkON:  Serial.println("ON");  break;
        case LinkOFF: Serial.println("OFF"); break;
        default:      Serial.println("Unknown"); break;
    }

    // Ensure a previous failed/aborted NTP socket cannot prevent this request.
    Serial.println("Opening UDP...");
    ntpUDP.stop();
    if (!ntpUDP.begin(2390))
    {
        Serial.println("NTP sync failed: UDP socket open failed");
        return false;
    }
    Serial.println("UDP started");

    Serial.println("Sending NTP request...");
    if (!ntpUDP.beginPacket(ntpServer, NTP_PORT))
    {
        ntpUDP.stop();
        Serial.println("NTP sync failed: NTP request creation failed");
        return false;
    }
    ntpUDP.write(packetBuffer, NTP_PACKET_SIZE);
    if (!ntpUDP.endPacket())
    {
        ntpUDP.stop();
        Serial.println("NTP sync failed: NTP UDP send failed");
        return false;
    }

    Serial.println("Waiting for NTP response...");

    unsigned long start = millis();

    // Wait max 2 seconds
    while (millis() - start < 2000)
    {
        int packetSize = ntpUDP.parsePacket();

        if (packetSize >= NTP_PACKET_SIZE)
        {
            ntpUDP.read(packetBuffer, NTP_PACKET_SIZE);

            // NTP replies must be server/broadcast mode.
            byte mode = packetBuffer[0] & 0x07;
            if (mode != 4 && mode != 5)
            {
                ntpUDP.stop();
                Serial.println("NTP sync failed: invalid NTP response");
                return false;
            }

            // A zero stratum means the server itself is unsynchronized (Kiss-o'-Death).
            if (packetBuffer[1] == 0)
            {
                ntpUDP.stop();
                Serial.println("NTP sync failed: NTP server unsynchronized");
                return false;
            }

            unsigned long highWord =
                word(packetBuffer[40], packetBuffer[41]);

            unsigned long lowWord =
                word(packetBuffer[42], packetBuffer[43]);

            unsigned long secondsSince1900 =
                (highWord << 16) | lowWord;

            const unsigned long seventyYears = 2208988800UL;

            ntpTime = secondsSince1900 - seventyYears;

            ntpUDP.stop();

            Serial.println("NTP sync succeeded");
            Serial.print("Unix time: ");
            Serial.println(ntpTime);

            ntpSynced = true;
            saveTimeToEEPROM(ntpTime);

            return true;
        }

        delay(10);
    }

    ntpUDP.stop();

    Serial.println("NTP sync failed: timeout/no response");

    return false;
}

// Apply a static address using the same network settings at boot and after SET.
void configureEthernet(IPAddress deviceIP)
{
  IPAddress dnsServer(dns_array[0], dns_array[1], dns_array[2], dns_array[3]);
  IPAddress gateway(gateway_array[0], gateway_array[1], gateway_array[2], gateway_array[3]);
  IPAddress subnet(subnet_array[0], subnet_array[1], subnet_array[2], subnet_array[3]);

  // The active TCP connection used to send SET belongs to the old IP.
  // Close it before reopening the listening socket with the new configuration.
  if (client)
    client.stop();

  Ethernet.begin(SERVER_MAC, deviceIP, dnsServer, gateway, subnet);

  // Let the network settle before the server/NTP socket comes up.
  delay(500);

  server.begin();

  Serial.print("Ethernet IP applied: ");
  Serial.println(Ethernet.localIP());
}
void Headline(const char* text, int color) {
  PANEL.selectFont(&font8);
  PANEL.setTextColor(color, BLACK);
  PANEL.drawStringX(3, 0, text, color, 0);
  PANEL.swapBuffers(true);
}

void displaySpeed(int vehicleSpeed, int colorCase) {
  PANEL.clearScreen(true);
  bool overspeed = vehicleSpeed > 100;

  // Same color-case numbering as writeToDisplay()'s fontColour switch.
  int color;
  switch (colorCase) {
    case 1: color = BLACK;   break;
    case 2: color = BLUE;    break;
    case 3: color = RED;     break;
    case 4: color = GREEN;   break;
    case 5: color = CYAN;    break;
    case 6: color = MAGENTA; break;
    case 7: color = YELLOW;  break;
    case 8: color = WHITE;   break;
    case 9: color = ORANGE;  break;
    default: color = colorCase; break;
  }

  // A large red glyph (font32) lights a lot of red LEDs at once, which this
  // panel chain's wiring can't fully power - it shows up as a red wash across
  // the whole display. Lowering brightness only for red draws keeps the
  // current draw (and the bleed) contained.
  bool usesRed = overspeed || (color == RED);
  PANEL.setBrightness(usesRed ? (MaxPanelBrightness / 2) : MaxPanelBrightness);

  Headline(overspeed ? "OVERSPEED" : "YOUR SPEED", overspeed ? RED : WHITE);
  PANEL.selectFont(&font29);

  int n = vehicleSpeed, digitCount = 0;
  do {
    n /= 10;
    ++digitCount;
  } while (n != 0);

  int x = 0, y = 0;
  n = vehicleSpeed;
  char s[20];
  sprintf (s, "%ld", n);
  int first_digit = s[0] - '0';

  if (digitCount == 1) {
    x = 45; y = 20;
  }
  else if (digitCount == 2) {
    x = 25; y = 20;
  }
  else if (digitCount == 3) {
    if (first_digit == 1) {
      x = 15; y = 20;
    } else if (first_digit > 1) {
      x = 5; y = 20;
    }
  }

  char sp[10] = "";
  sprintf(sp, "%d" , vehicleSpeed);

  PANEL.setTextColor(color, BLACK);
  PANEL.drawStringX(x, y, sp, color, 0);
  PANEL.swapBuffers(true);
}

void checkHardReset() {
  // lowSince must restart every time the pin goes HIGH again, otherwise a
  // single brief LOW glitch (long after boot) looks like a full 10s hold
  // and wipes the saved IP immediately. triggered stops the reset from
  // firing on every loop for as long as the button stays held past 10s.
  static unsigned long lowSince = 0;
  static bool wasLow = false;
  static bool triggered = false;

  bool isLow = (digitalRead(HARDRST) == LOW);

  if (isLow) {
    if (!wasLow) {
      lowSince = millis();
      wasLow = true;
      triggered = false;
    }
    else if (!triggered && millis() - lowSince > 10000) {
      EEPROM.write(EEPROM_IP_ADDR + 0, 192); EEPROM.commit();
      EEPROM.write(EEPROM_IP_ADDR + 1, 168); EEPROM.commit();
      EEPROM.write(EEPROM_IP_ADDR + 2, 0);   EEPROM.commit();
      EEPROM.write(EEPROM_IP_ADDR + 3, 125); EEPROM.commit();
      EEPROM.write(EEPROM_PORT_ADDR, 255);   EEPROM.commit();
      triggered = true;
//      NVIC_SystemReset();
    }
  }
  else {
    wasLow = false;
    triggered = false;
  }
}
String getMessageEthernet() {
  String str = "";
  size_t size;
  client = server.available();
  while ((size = client.available()) > 0) {
    char ch = client.read();
    str = str + String(ch);
  }
  str = str + "\0";
  return str;
}

int parseCommand(String str) {
  if (str != "") {
    int commaIndex = str.indexOf(',');
    int secondCommaIndex = str.indexOf(',', commaIndex + 1);
    String firstValue = str.substring(0, commaIndex);
    String secondValue = str.substring(commaIndex + 1);

    if (firstValue == "SET") {
      int commaIndex1 = str.indexOf(',');
      int secondCommaIndex1 = str.indexOf(',', commaIndex1 + 1);
      int thirdCommaIndex1 = str.indexOf(',', secondCommaIndex1 + 1);
      int fourthCommaIndex1 = str.indexOf(',', thirdCommaIndex1 + 1);

      String secondValue1 = str.substring(commaIndex1 + 1, secondCommaIndex1);
      String thirdValue1 = str.substring(secondCommaIndex1 + 1, thirdCommaIndex1);
      String fourthValue1 = str.substring(thirdCommaIndex1 + 1, fourthCommaIndex1);
      // The fourth octet is the remainder of SET,a,b,c,d.  Do not require a
      // trailing comma or any time field in the command.
      String fifthValue1  = str.substring(fourthCommaIndex1 + 1);

      EEPROM.write(EEPROM_IP_ADDR + 0, secondValue1.toInt()); EEPROM.commit();
      EEPROM.write(EEPROM_IP_ADDR + 1, thirdValue1.toInt());  EEPROM.commit();
      EEPROM.write(EEPROM_IP_ADDR + 2, fourthValue1.toInt()); EEPROM.commit();
      EEPROM.write(EEPROM_IP_ADDR + 3, fifthValue1.toInt());  EEPROM.commit();

      IPAddress newIP(secondValue1.toInt(), thirdValue1.toInt(),
                      fourthValue1.toInt(), fifthValue1.toInt());
      configureEthernet(newIP);

      if (syncNTPTime())
      {
        timerStart = millis();
        Serial.println("Timer reset to 00:00:00 after NTP synchronization");
      }
      else
      {
        Serial.println("Timer was not reset because NTP synchronization failed");
      }
    }
    else if (firstValue == "SPEED") {
      int commaIndex1 = str.indexOf(',');
      int secondCommaIndex1 = str.indexOf(',', commaIndex1 + 1);
      int thirdCommaIndex1 = str.indexOf(',', secondCommaIndex1 + 1);

      String firstValue1 = str.substring(0, commaIndex1);
      String secondValue1 = str.substring(commaIndex1 + 1, secondCommaIndex1);
      String thirdValue1 = str.substring(secondCommaIndex1 + 1, thirdCommaIndex1);

      int vehicle_speed = secondValue1.toInt();
      int colorCase = thirdValue1.toInt();

      displaySpeed(vehicle_speed, colorCase);

      server.print(vehicle_speed);
      server.print(" ");
      server.print(colorCase);
    }
    else if (firstValue == "CLEAR") {
      int packetIndex1 = str.indexOf(',');
      int packetIndex2 = str.indexOf(',', packetIndex1 + 1);
      int packetIndex3 = str.indexOf(',', packetIndex2 + 1);
      int packetIndex4 = str.indexOf(',', packetIndex3 + 1);
      int packetIndex5 = str.indexOf(',', packetIndex4 + 1);

      String packetValue1 = str.substring(0, packetIndex1);
      String packetValue2 = str.substring(packetIndex1 + 1, packetIndex2);
      String packetValue3 = str.substring(packetIndex2 + 1, packetIndex3);
      String packetValue4 = str.substring(packetIndex3 + 1, packetIndex4);
      String packetValue5 = str.substring(packetIndex4 + 1, packetIndex5);

      int x1 = packetValue2.toInt();
      int y1 = packetValue3.toInt();
      int x2 = packetValue4.toInt();
      int y2 = packetValue5.toInt();

      int i = x1;
      while (y1 < y2) {
        while (i < x2) {
          PANEL.drawPixel(i, y1, BLACK);
          i++;
        }
        y1++;
        i = x1;
      }
    }
  }
  return -1;
}


void printAlive() {
  delay(50);
  //Date = String(day()) + ":" + String(month()) + ":" + String(year());
  //Times = String(hour()) + ":" + String(minute()) + ":" + String(second());
  AliveN = AliveN + ",";
  AliveN = AliveN + "E";
  AliveN = AliveN + "N";
  AliveN = AliveN + "D";
  AliveN = AliveN + "\0";

  server.println(AliveN);
  Serial.println(AliveN);
  AliveN = "START,ALIVE,";
}
void HeartBeat()
{
  digitalWrite(HeartBeatLed, HIGH);
  delay(50);
  digitalWrite(HeartBeatLed, LOW);
  delay(50);
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(64);
  pinMode(ETHERNET_RST_PIN, OUTPUT);
  digitalWrite(ETHERNET_RST_PIN, LOW);
  delay(5);
  digitalWrite(ETHERNET_RST_PIN, HIGH);
  delay(150);
  Ethernet.init(ETHERNET_SS_PIN);

  Serial.print("PORT:");
  int port = SERVER_PORT;
  if (EEPROM.read(EEPROM_PORT_ADDR) < 254) {
    port = EEPROM.read(EEPROM_PORT_ADDR);
  }
  Serial.println(port);
  if (EEPROM.read(EEPROM_IP_ADDR + 0) != 255 && EEPROM.read(EEPROM_IP_ADDR + 1) != 255 &&
      EEPROM.read(EEPROM_IP_ADDR + 2) != 255 && EEPROM.read(EEPROM_IP_ADDR + 3) != 255) {
    IPAddress SERVER_IP(EEPROM.read(EEPROM_IP_ADDR + 0), EEPROM.read(EEPROM_IP_ADDR + 1),
                         EEPROM.read(EEPROM_IP_ADDR + 2), EEPROM.read(EEPROM_IP_ADDR + 3));
    configureEthernet(SERVER_IP);
  }
  else {
    IPAddress SERVER_IP(ip_array[0], ip_array[1], ip_array[2], ip_array[3]);
    configureEthernet(SERVER_IP);
  }

  Serial.print("My IP:");
  server.println(Ethernet.localIP());
  Serial.println(Ethernet.localIP());

  // Network has settled (configureEthernet() already waited 500ms after
  // Ethernet.begin()) - attempt boot-time NTP sync.
  if (syncNTPTime())
  {
    timerStart = millis();
    Serial.println("Timer reset to 00:00:00 after boot NTP synchronization");
  }
  else
  {
    Serial.println("Boot NTP synchronization failed");
    unsigned long savedTime;
    if (loadTimeFromEEPROM(savedTime))
    {
      ntpTime = savedTime;
      ntpSynced = true;
      timerStart = millis();
      Serial.print("Restored last known synced time from EEPROM. Unix time: ");
      Serial.println(ntpTime);
      Serial.println("Note: this time is stale by however long the device was powered off.");
    }
    else
    {
      Serial.println("No previously saved time in EEPROM - timer will run from device boot time");
      timerStart = millis();
    }
  }
  pinMode(HeartBeatLed, OUTPUT);
  pinMode(HARDRST, INPUT_PULLUP);
  PANEL.init();
  PANEL.setBrightness(MaxPanelBrightness);
    // Boot splash - show the logo for 5 seconds, then clear.
//  PANEL.drawRGBBitmap(0, 0, flag, 128, 96);
  PANEL.swapBuffers(true);
  delay(5000);
  PANEL.clearScreen(true);
}

void loop() {

    HeartBeat();

    String str = getMessageEthernet();

    if (str != "") {
        server.println(str);
        parseCommand(str);
        str = "";
    }

    // Counter continuously run
    drawTimeCounter();
    PANEL.swapBuffers(true);

    checkHardReset();
}
