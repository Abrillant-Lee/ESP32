#include "dht11_owner.h"

DHT dht(DHTPIN, DHTTYPE);

float gettemp()
{
  tem = dht.readTemperature();
  return tem;
}

float gethum()
{
  hum = dht.readHumidity();
  return hum;
}




// 检查是否任何读数失败，并退出早期（尽快重新开始）。

// if(isnan(h) || isnan(t) || isnan(f)) 
// {
//   Serial.println(F("Failed to read from DHT sensor!"));
//   return;
// }


//   Serial.print(F("Humidity: "));
//   Serial.print(h);
//   Serial.print(F("%  Temperature: "));
//   Serial.print(t);
//   Serial.print(F("°C "));
//   Serial.print(f);
//   Serial.println(F("°F"));
// }
