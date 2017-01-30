/*
 * Author: Lorenz Adriaensen
 * Date: 2017-01-30
 *
 * A library for using the Wireless 868 MHz LoRa Shield for Arduino UNO.
 *
 * Change the device address, network (session) key, and app (session) key to the values
 * that are registered via the TTN dashboard.
 *
 * The RN2383 is connected as follows:
 * Reset -- 4
 * Uart TX -- 7
 * Uart RX -- 8
 *
 */

#include <rn2xx3.h>
#include <SoftwareSerial.h>

//Define RN2483 Pins - don't change
#define RN2483_reset_pin 4
#define RN2483_tx_pin 7
#define RN2483_rx_pin 8

SoftwareSerial mySerial(RN2483_rx_pin, RN2483_tx_pin); // RX, TX

//create an instance of the rn2483 library,
//giving the software serial as port to use
rn2xx3 myLora(mySerial);

// the setup routine runs once when you press reset:
void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(57600); //serial port to radio
  Serial.println("Startup");

  initialize_radio();

  //transmit a startup message
  myLora.tx("868 MHz LoRa Shield for Arduino UNO node");

  delay(2000);
}

void initialize_radio()
{
  //reset rn2483
  pinMode(RN2483_reset_pin, OUTPUT);
  digitalWrite(RN2483_reset_pin, LOW);
  delay(500);
  digitalWrite(RN2483_reset_pin, HIGH);

  delay(100); //wait for the RN2483's startup message
  mySerial.flush();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2483 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;

  //ABP: initABP(String addr, String AppSKey, String NwkSKey);
  join_result = myLora.initABP("02017201", "8D7FFEF938589D95AAD928C2E2E7E48F", "AE17E567AECC8787F749A62F5541D522");

  //OTAA: initOTAA(String AppEUI, String AppKey);
  //join_result = myLora.initOTAA("70B3D57ED00001A6", "A23C96EE13804963F8C2BD6285448198");

  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");

}

// the loop routine runs over and over again forever:
void loop()
{
    //execute every minute
    Serial.println("Sending message");
    myLora.tx("DE10DE1976");
    delay(60000);
}