#include <SoftwareSerial.h>
#include <BPLib.h>
#include <SPI.h>
#include "mcp_can.h"


// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
SoftwareSerial swSer(5, 6);
const int SPI_CS_PIN = 10;
int RN42VCC = 3;
int led = 7;
unsigned char len = 0;
unsigned char buf[8];
MCP_CAN CAN(SPI_CS_PIN);
BPLib *BPMod;

void setup()
{ swSer.begin(115200);
  Serial.begin(115200);

  while (CAN_OK != CAN.begin(CAN_250KBPS))              // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);

  }
  Serial.println("CAN BUS Shield init ok!");

  CAN.init_Mask(0, 0, 0x3ff);                         // there are 2 mask in mcp2515, you need to set both of them
  CAN.init_Mask(1, 0, 0x3ff);
  //set filter, we can receive id from 0x04 ~ 0x09
  CAN.init_Filt(0, 0, 0x3E5);                          // there are 6 filter in mcp2515
  CAN.init_Filt(1, 0, 0x21F);                          // there are 6 filter in mcp2515

  //CAN.init_Filt(2, 0, 0x06);                          // there are 6 filter in mcp2515
  //CAN.init_Filt(3, 0, 0x07);                          // there are 6 filter in mcp2515
  //CAN.init_Filt(4, 0, 0x08);                          // there are 6 filter in mcp2515
  //CAN.init_Filt(5, 0, 0x09);                          // there are 6 filter in mcp2515
  pinMode(RN42VCC, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(RN42VCC, HIGH);
  BPMod = new BPLib(swSer);
  delay(1000);
  //BPMod->begin(BP_MODE_HID, BP_HID_KEYBOARD);
  BPMod->sendCmd(BP_RECONNECT);
}
void loop()
{
  CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
  unsigned long canId = CAN.getCanId();
  Serial.println("-----------------------------");
  Serial.print("Get data from ID: 0x");
  Serial.println(canId, HEX);

  for (int i = 0; i < len; i++) // print the data
  {
    Serial.print(buf[i], DEC);
    delay(50);
    Serial.print("\t");
  }
  Serial.println();
  if (canId == 0x21F && buf[0] == 128)
  {
    BPMod->nextTrack();
    delay(250);
    BPMod->keyRelease();
    delay(300);
  }
  if (canId == 0x21F && buf[0] == 64)
  {
    BPMod->prevTrack();
    delay(250);
    BPMod->keyRelease();
    delay(300);
  }
  if (canId == 0x21F && buf[0] == 12)
  {
    BPMod->playPause();
    delay(250);
    BPMod->keyRelease();
    delay(300);
  }
}
