#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//Definitions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
//#define GPS_TX TX     //GPS serial comms pins
//#define GPS_RX RX

#define I2C_SDA D1    //I2C Pins
#define I2C_SCL D2   
#define UTC -7        //Local time correction for display

//setup strings for NMEA blocking, 10HZ refreshrate and NAV-PVT enable
const char UBLOX_INIT[] PROGMEM = {
  // Disable NMEA
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x24, // GxGGA off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x2B, // GxGLL off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x32, // GxGSA off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x39, // GxGSV off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x04,0x40, // GxRMC off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x01,0x05,0x47, // GxVTG off

  // Disable UBX
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0xDC, //NAV-PVT off
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0xB9, //NAV-POSLLH off
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x13,0xC0, //NAV-STATUS off

  // Enable UBX
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x18,0xE1, //NAV-PVT on
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, //NAV-POSLLH on
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x14,0xC5, //NAV-STATUS on

  // Rate
  0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, //(10Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39, //(1Hz)
};
const unsigned char UBX_HEADER[] = { 0xB5, 0x62 };


//HW Setup
HardwareSerial SerialP2 (2);
//TwoWire MyWire = TwoWire(0);

//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &MyWire, OLED_RESET);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  

struct NAV_PVT {
  unsigned char cls;
  unsigned char id;
  unsigned short len;
  unsigned long iTOW;          // GPS time of week of the navigation epoch (ms)
  
  unsigned short year;         // Year (UTC) 
  unsigned char month;         // Month, range 1..12 (UTC)
  unsigned char day;           // Day of month, range 1..31 (UTC)
  unsigned char hour;          // Hour of day, range 0..23 (UTC)
  unsigned char minute;        // Minute of hour, range 0..59 (UTC)
  unsigned char second;        // Seconds of minute, range 0..60 (UTC)
  char valid;                  // Validity Flags (see graphic below)
  unsigned long tAcc;          // Time accuracy estimate (UTC) (ns)
  long nano;                   // Fraction of second, range -1e9 .. 1e9 (UTC) (ns)
  unsigned char fixType;       // GNSSfix Type, range 0..5
  char flags;                  // Fix Status Flags
  unsigned char reserved1;     // reserved
  unsigned char numSV;         // Number of satellites used in Nav Solution
  
  long lon;                    // Longitude (deg)
  long lat;                    // Latitude (deg)
  long height;                 // Height above Ellipsoid (mm)
  long hMSL;                   // Height above mean sea level (mm)
  unsigned long hAcc;          // Horizontal Accuracy Estimate (mm)
  unsigned long vAcc;          // Vertical Accuracy Estimate (mm)
  
  long velN;                   // NED north velocity (mm/s)
  long velE;                   // NED east velocity (mm/s)
  long velD;                   // NED down velocity (mm/s)
  long gSpeed;                 // Ground Speed (2-D) (mm/s)
  long heading;                // Heading of motion 2-D (deg)
  unsigned long sAcc;          // Speed Accuracy Estimate
  unsigned long headingAcc;    // Heading Accuracy Estimate
  unsigned short pDOP;         // Position dilution of precision
  short reserved2;             // Reserved
  unsigned long reserved3;     // Reserved
  unsigned char dummy[8];      // EXtra for new packet data stuff
};

NAV_PVT pvt;

void calcChecksum(unsigned char* CK) {
  memset(CK, 0, 2);
  for (int i = 0; i < (int)sizeof(NAV_PVT); i++) {
    CK[0] += ((unsigned char*)(&pvt))[i];
    CK[1] += CK[0];
  }
}

bool processGPS() {
  static int fpos = 0;
  static unsigned char checksum[2];
  const int payloadSize = sizeof(NAV_PVT);

  while ( SerialP2.available() ) {
    byte c = SerialP2.read();

    if ( fpos < 2 ) {
      if ( c == UBX_HEADER[fpos] ){
        //Serial.println("UBX Detected");
        fpos++;}
      else
        fpos = 0;
    }
    else {      
      if ( (fpos-2) < payloadSize )
        ((unsigned char*)(&pvt))[fpos-2] = c;

      fpos++;

      if ( fpos == (payloadSize+2) ) {
        calcChecksum(checksum);
        //Serial.println("fail 1");
      }
      else if ( fpos == (payloadSize+3) ) {
        if ( c != checksum[0] )
          fpos = 0;
        //Serial.println("fail 2");
      }
      else if ( fpos == (payloadSize+4) ) {
        fpos = 0;
        if ( c == checksum[1] ) {
          return true;
        }
      }
      else if ( fpos > (payloadSize+4) ) {
        fpos = 0;
      }
    
    
    }
    
  }
  return false;
}


//get the cardinal direction
String getCardinalDirection(int degrees) {
  // Normalize the degrees to be within 0-360
  degrees = degrees % 360;
  if (degrees < 0) degrees += 360;

  // Define the cardinal directions
  String cardinalDirections[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};

  // Determine the index for the cardinal direction
  int index = (degrees + 22) / 45; // Adding 22 to handle mid-points
  index = index % 8; // Ensure index is within 0-7

  return cardinalDirections[index];
}



void setup() 
{
  //computer serial
  Serial.begin(115200);
  //gps Serial
  SerialP2.begin(115200, SERIAL_8N1, TX, RX);

  //i2C
  //MyWire.setPins(I2C_SDA, I2C_SCL);
  //MyWire.begin();
  Wire.begin(0);

  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
  }

  Serial.println("configuring UBLOX params");

  // send configuration data in UBX protocol
 
  for(int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
    SerialP2.write( pgm_read_byte(UBLOX_INIT+i) );
    
    delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }


  Serial.println("Now recieving : ");

}

void loop() {
  
  String dateformatted1 , dateformatted2 = "";

  if ( processGPS() ) {
    //pull date into formated string
    dateformatted1 = String(pvt.year) + "/" + String(pvt.month) + "/" + String(pvt.day) + " ";
    dateformatted2 = String(pvt.hour) + ":" + String(pvt.minute) + ":" + String(pvt.second);
    
    Serial.print("#SV: ");      Serial.print(pvt.numSV);
    Serial.print(" fixType: "); Serial.print(pvt.fixType);
    //Serial.print(" Date:");     Serial.print(pvt.year); Serial.print("/"); Serial.print(pvt.month); Serial.print("/"); Serial.print(pvt.day); Serial.print(" "); Serial.print(pvt.hour); Serial.print(":"); Serial.print(pvt.minute); Serial.print(":"); Serial.print(pvt.second);
    Serial.print(" Date:");    Serial.print(dateformatted1); Serial.print(dateformatted2);
    Serial.print(" lat/lon: "); Serial.print(pvt.lat/10000000.0f); Serial.print(","); Serial.print(pvt.lon/10000000.0f);
    Serial.print(" gSpeed: ");  Serial.print(pvt.gSpeed/1000.0f);
    Serial.print(" heading: "); Serial.print(pvt.heading/100000.0f);
    Serial.print(" hAcc: ");    Serial.print(pvt.hAcc/1000.0f);
    Serial.println();

  }
  

  //OLED update code
  
  
  display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0);              // Start at top-left corner
  display.print(F("Sat:"));
  display.println(pvt.numSV);
  display.setCursor(40,0);
  display.print(F("Fix:"));
  display.println(pvt.fixType);
  display.setCursor(80,0);
  display.print(F("ACC:"));
  if(pvt.hAcc < 100000 ){
    display.print(pvt.hAcc/1000.0f);
  } else
    display.print("N/A");

  //display body
  display.setCursor(0,18);
  display.println(dateformatted1);
  display.println(dateformatted2);
  display.print(F("Lat :"));
  display.println(pvt.lat/10000000.0f);
  display.print(F("Lon :"));
  display.println(pvt.lon/10000000.0f);
  display.print(F("Spd :"));
  display.print(pvt.gSpeed/1000.0f);
  display.print(F(" - "));
  display.println(getCardinalDirection(pvt.numSV));

  //display buffer
  display.display();
  
  
  //delay(1000);

}

