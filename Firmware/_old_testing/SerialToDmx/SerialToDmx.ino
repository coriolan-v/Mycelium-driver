#define Channel_WinchSpeed 3
#define DMXpin_TX6 24

#include <DmxSimple.h>

void setup() {
  Serial.begin(9600);
  Serial.println("SerialToDmx ready");
  Serial.println();
  Serial.println("Syntax:");
  Serial.println(" 123c : use DMX channel 123");
  Serial.println(" 45w  : set current channel to value 45");

  DmxSimple.usePin(DMXpin_TX6)
}

int value = 0;
int channel;

void loop() {
  int c;

  while(!Serial.available());
  c = Serial.read();
  if ((c>='0') && (c<='9')) {
    value = 10*value + c - '0';
  } else {
    if (c=='c') channel = value;
    else if (c=='w') {
      DmxSimple.write(channel, value);
      Serial.println();
    }
    value = 0;
  }
}

void setWinchSpeed()
{
  DmxSimple.write(Channel_WinchSpeed, 200);
}
