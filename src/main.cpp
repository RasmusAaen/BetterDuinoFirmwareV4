#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "MDuinoBase.h"
#include "MDuinoDome.h"
#include "MDuinoDomeMaster.h"
#include "MDuinoDomeMasterChopper.h"
#include "MDuinoDomeMasterBT1.h"
#include "MDuinoDomeSlave.h"
//#include "MDuinoDomeSlaveChopper.h"
//#include "MDuinoDomeSlaveBT1.h"
#include "MDuinoBodyMaster.h"
#include "MDuinoBodyMasterChopper.h"
#include "MDuinoBodyMasterBT1.h"
#include "MDuinoStorage.h"

MDuinoBase* MarcDuino = nullptr;

// Initialize common ressources in main (due to interrupts and globals)

SendOnlySoftwareSerial Serial1(P_SERIAL1);  // Slave or Magic
SendOnlySoftwareSerial Serial2(P_SERIAL2);  // MP3 or Teeces
#ifdef SEPARATE_DOMELIFT
SendOnlySoftwareSerial Serial3(P_AUX1);     // DomeLift separate Serial
#else
#define Serial3 Serial
#endif

// 11 Servos per Board
VarSpeedServo Servo1;
VarSpeedServo Servo2;
VarSpeedServo Servo3;
VarSpeedServo Servo4;
VarSpeedServo Servo5;
VarSpeedServo Servo6;
VarSpeedServo Servo7;
VarSpeedServo Servo8;
VarSpeedServo Servo9;
VarSpeedServo Servo10;
VarSpeedServo Servo11;

#ifdef INCLUDE_I2C_SLAVE
void I2C_Callback(int count)
{
  // MarcDuinio->I2C_Callback();
  #ifdef DEBUG_MSG
  Serial.print("\rI2C\r");
  #endif
}
#endif

void setup() {
  // put your setup code here, to run once:
  // Serial Port
  Serial.begin(SERIAL_BAUD);
  while(!Serial);

  // I2C - Master
  #ifdef INCLUDE_CLASSIC_I2C_SUPPORT
  Wire.begin();
  #endif

  MDuinoStorage Storage;

  #if defined(DEDICATED_FIRMWARE)
    #if defined(DEDICATED_MASTER)
      #if defined(R2D2)
        MarcDuino = new MDuinoDomeMaster(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(CHOPPER)
        MarcDuino = new MDuinoDomeMasterChopper(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(BT1)
        MarcDuino = new MDuinoDomeMasterBT1(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #endif
    #elif defined(DEDICATED_SLAVE)
      #if defined(R2D2)
        MarcDuino = new MDuinoDomeSlave(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(CHOPPER)
        MarcDuino = new MDuinoDomeSlaveChopper(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(BT1)
        MarcDuino = new MDuinoDomeSlaveBT1(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #endif
    #elif defined (DEDICATED_BODY)    
      #if defined(R2D2)
        MarcDuino = new MDuinoBodyMaster(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(CHOPPER)
        MarcDuino = new MDuinoBodyMasterChoppper(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(BT1)
        MarcDuino = new MDuinoBodyMasterBT1(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #endif
    #endif
  #else

  MDuinoStorage::MDuinoType type;
  type = Storage.getType();

  switch (type)
  {
    case MDuinoStorage::DomeMaster:
      #if defined(R2D2)
        MarcDuino = new MDuinoDomeMaster(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(CHOPPER)
        MarcDuino = new MDuinoDomeMasterChopper(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(BT1)
        MarcDuino = new MDuinoDomeMasterBT1(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #endif
      #ifdef INCLUDE_I2C_SLAVE
      Wire.begin(I2C_DOME_MASTER);
      Wire.onReceive(I2C_Callback);
      #endif
      break;
    case MDuinoStorage::DomeSlave:
      #if defined(R2D2)
        MarcDuino = new MDuinoDomeSlave(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(CHOPPER)
        //MarcDuino = new MDuinoDomeSlaveChopper(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(BT1)
        //MarcDuino = new MDuinoDomeSlaveBT1(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #endif
      #ifdef INCLUDE_I2C_SLAVE
      Wire.begin(I2C_DOME_SLAVE);
      Wire.onReceive(I2C_Callback);
      #endif
      break;
  #ifdef INCLUDE_BODY_MASTER
    case MDuinoStorage::BodyMaster:
      #if defined(R2D2)
        MarcDuino = new MDuinoBodyMaster(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(CHOPPER)
        MarcDuino = new MDuinoBodyMasterChopper(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(BT1)
        MarcDuino = new MDuinoBodyMasterBT1(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #endif
      #ifdef INCLUDE_I2C_SLAVE
      Wire.begin(I2C_BODY_MASTER);
      Wire.onReceive(I2C_Callback);
      #endif
      break;
  #endif
    case MDuinoStorage::UnknownMarcDuino:
    default:
      #if defined(R2D2)
        MarcDuino = new MDuinoDomeMaster(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(CHOPPER)
        MarcDuino = new MDuinoDomeMasterChopper(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #elif defined(BT1)
        MarcDuino = new MDuinoDomeMasterBT1(Serial1, Serial2, Serial3, Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11);
      #endif
      Storage.setType(MDuinoStorage::DomeMaster);    
      #ifdef INCLUDE_I2C_SLAVE
      Wire.begin(I2C_DOME_MASTER);
      Wire.onReceive(I2C_Callback);
      #endif
      break;
  }
  
  #endif
  
  MarcDuino->init();

  Serial.printf(F("%s\r\nVersion %s\r\n\r\n"), MarcDuino->getProductName(), VERSION);
}

void loop() {
  // put your main code here, to run repeatedly:
  MarcDuino->run();
}
