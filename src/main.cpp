#include <Arduino.h>
#include <Ticker.h>

// const
const uint8_t LED_PIN = 13;
const uint8_t DHT_PIN = 2;
const uint32_t PERIOD = 3000;

// enum
enum DHT_STATUS
{
  DHT_OK = 0,
  DHT_ERROR_RESPONSE,
  DHT_ERROR_CHECK_SUM,
  DHT_ERROR_TIMEOUT
};

// prototypes
void action();

void DHT_begin();
DHT_STATUS DHT_startConversion();
uint8_t DHT_readByte();
DHT_STATUS DHT_readValues();

// obj
Ticker mticker(action, PERIOD, 0, MILLIS);

// var global
float theTemperature = 0;
float theHumidity = 0;

//**********
void action()
{
  DHT_STATUS status = DHT_readValues();

  if (status == DHT_OK)
  {
    Serial.println(theHumidity);
    Serial.println(theTemperature);
    Serial.println("*****");
  }
  else
  {
    Serial.println(status);
  }
}
//************** */
void DHT_begin()
{
  pinMode(DHT_PIN, INPUT);
  delay(2000);
}
//****************************** */
DHT_STATUS DHT_startConversion()
{
  DHT_STATUS res = DHT_OK;

  // wake up
  //********************* */
  pinMode(DHT_PIN, OUTPUT);
  //
  digitalWrite(DHT_PIN, 0);
  delayMicroseconds(1200);
  //
  digitalWrite(DHT_PIN, 1);
  delayMicroseconds(30);
  //********************** */
  // wait the response
  //********************** */
  pinMode(DHT_PIN, INPUT);
  delayMicroseconds(40);

  if (digitalRead(DHT_PIN) == 0)
  {
    delayMicroseconds(80);
    //
    if (digitalRead(DHT_PIN) == 1)
    {
      res = DHT_OK;
    }
    else
    {
      res = DHT_ERROR_RESPONSE;
    }
  }
  uint32_t now = millis();
  //
  while (digitalRead(DHT_PIN) == 1)
  {
    if ((millis() - now) > 20)
    {
      return DHT_ERROR_TIMEOUT;
    }
  }
  //
  return res;
}
//******************** */
uint8_t DHT_readByte()
{
  uint8_t res = 0x00;

  for (int i = 0; i < 8; i++)
  {
    while (digitalRead(DHT_PIN) == 0)
    {
    }
    //
    delayMicroseconds(40);
    //
    if (digitalRead(DHT_PIN) == 1)
    {
      res = res | (1 << (7 - i));
    }
    //
    while (digitalRead(DHT_PIN) == 1)
    {
    }
  }
  //
  return res;
}
//*************************$ */
DHT_STATUS DHT_readValues()
{
  DHT_STATUS status = DHT_OK;
  uint8_t signe = 0;
  uint8_t check = 0;
  uint8_t sum = 0;
  uint8_t hum_upper = 0;
  uint8_t hum_lower = 0;
  uint8_t temp_upper = 0;
  uint8_t temp_lower = 0;
  //
  status = DHT_startConversion();

  if (status == DHT_OK)
  {
    hum_upper = DHT_readByte();
    hum_lower = DHT_readByte();
    temp_upper = DHT_readByte();
    temp_lower = DHT_readByte();
    check = DHT_readByte();
    //
    sum = hum_upper + hum_lower + temp_upper + temp_lower;
    //
    if (check == sum)
    {
      theHumidity = (hum_upper << 8) | (hum_lower);
      theHumidity = theHumidity / 10.0;
      //
      signe = temp_upper & 0b10000000;
      if (signe > 0)
      {
        temp_upper = temp_upper & 0b01111111;
        theTemperature = (temp_upper << 8) | (temp_lower);
        theTemperature = (theTemperature / 10.0) * (-1);
      }
      else
      {
        theTemperature = (temp_upper << 8) | (temp_lower);
        theTemperature = theTemperature / 10.0;
      }
      return DHT_OK;
      //
    }
    else
    {
      return DHT_ERROR_CHECK_SUM;
      //
    }
  }
  //
  return status;
}
//************
void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 0);
  //
  DHT_begin();
  //
  mticker.start();
}

//**************
void loop()
{
  mticker.update();
}
