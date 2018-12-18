#include <TroykaDHT.h>
DHT dht(4, DHT21); // передаём номер пина к которому подключён датчик и тип датчика // типы сенсоров: DHT11, DHT21, DHT22

#include <LCD_1602_RUS.h>
LCD_1602_RUS lcd(0x3F, 16, 2);

const byte RELAY = 2;
short lastState = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Диод для отображения готовности
  pinMode(RELAY, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RELAY, HIGH);
  
  Serial.begin(9600); // открываем последовательный порт для мониторинга действий в программе
  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(L"Сушилка 1.0");
  lcd.setCursor(0, 1);
  lcd.print(L"Загрузка");
  delay(1000);
  lcd.print(L".");
  delay(1000);
  lcd.print(L".");
  delay(1000);
  lcd.print(L".");
  delay(1000);
  lastState = dht.getState();
}

float humDone = 50;
unsigned long timer = 0;


void loop()
{
  dht.read(); // считывание данных с датчика
  float t,h;
  switch(dht.getState()) { // проверяем состояние данных
    case DHT_OK: // всё OK
      lastState = dht.getState();
      t = dht.getTemperatureC();
      h = dht.getHumidity();

      if(h < humDone)
      {
        if(timer == 0)
        {
          timer = millis();
        }
        else if(millis()-timer > 10*1000)
        {
          digitalWrite(LED_BUILTIN, LOW);
          digitalWrite(RELAY, LOW);
        }
        else
        {
          lcd.setCursor(0, 0);
          lcd.print(L"Время до выкл.:");
          lcd.setCursor(0, 1);

          int  Seconds = ( ((timer+10*1000)-millis() )/1000UL)%60;
          Serial.println(Seconds);
          /*byte  Minutes = (((millis()-(timer+4*60*60*1000))/1000UL)/60UL)%60;
          byte  Hours = (((millis()-(timer+4*60*60*1000))/1000UL)/3600UL)%24;
          
          lcd.print(Hours); lcd.print(":"); lcd.print(Minutes); lcd.print(":"); lcd.print(Seconds);*/
        }
      }
      else
      {
        // Выводим показания влажности и температуры
        /*Serial.print("Temperature = ");
        Serial.print(t);
        Serial.print(" C");
        Serial.print("Humidity = ");
        Serial.print(h);
        Serial.println(" %");*/
  
        if(lastState != DHT_OK) lcd.clear();
        lastState = DHT_OK;
        lcd.setCursor(0, 0);
        lcd.print(L"Температура=");
        lcd.print(t);
        lcd.setCursor(0, 1);
        lcd.print(L"Влажность = ");
        lcd.print(h);
      }
      break;
      
    // ошибка контрольной суммы
    case DHT_ERROR_CHECKSUM:
      //Serial.println("Checksum error");
      if(lastState != DHT_ERROR_CHECKSUM) lcd.clear();
      lastState = DHT_ERROR_CHECKSUM;
      lcd.setCursor(0, 0);
      lcd.print(L"Ошибка сенсора!");
      lcd.setCursor(0, 1);
      lcd.print(L"Битые данные");
      break;
      
    // превышение времени ожидания
    case DHT_ERROR_TIMEOUT:
      //Serial.println("Time out error");
      if(lastState != DHT_ERROR_TIMEOUT) lcd.clear();
      lastState = DHT_ERROR_TIMEOUT;
      lcd.setCursor(0, 0);
      lcd.print(L"Ошибка сенсора!");
      lcd.setCursor(0, 1);
      lcd.print(L"Не отвечает");
      break;
      
    // данных нет, датчик не реагирует или отсутствует
    case DHT_ERROR_NO_REPLY:
      //Serial.println("Sensor not connected");
      if(lastState != DHT_ERROR_NO_REPLY) lcd.clear();
      lastState = DHT_ERROR_NO_REPLY;
      lcd.setCursor(0, 0);
      lcd.print(L"Ошибка сенсора!");
      lcd.setCursor(0, 1);
      lcd.print(L"Подключи сенсор");
      break;
  }
  if(timer == 0) delay(5000);
  else delay(1000);
}
