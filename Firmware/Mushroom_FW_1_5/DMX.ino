#define Channel_WinchPos 1
#define Channel_WinchSpeed 3
#define DMXpin_TX6 24

void setupDMX()
{
   DmxSimple.usePin(DMXpin_TX6);

   setWinchSpeed(200);
}

void setWinchSpeed(int winchSpeed)
{
  DmxSimple.write(Channel_WinchSpeed, winchSpeed);
}

unsigned long prevMillis_DMX;
int interval_DMX = 30;
int old_winchPos;
void setWinchPosition(int winchPos)
{
  if(millis() - prevMillis_DMX >= interval_DMX){
    prevMillis_DMX = millis();

    if(old_winchPos != winchPos)
    {

     Serial.println(winchPos);
     DmxSimple.write(Channel_WinchPos, winchPos);

     old_winchPos = winchPos;
    }

     
  }

 
}