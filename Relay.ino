#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Замените на свои сетевые данные
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Инициализация Telegram бота
#define BOT_TOKEN "6110568357:AAFKe-AxgDe9UsNDw6U2C4P2R76xm1XeTvs"  // Ваш Токен
#define CHAT_ID "847428051"                                        // ID чата

const unsigned long BOT_MTBS = 1000; // среднее время между сообщениями сканирования

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // последнее сканирование сообщений было сделано

const int Relay1 = D5;      //Реле1 подключено к D5
const int Relay2 = D6;      //Реле2 подключено к D6
int Relay1Status = 0;       //Переменная для хранения статуса Реле1
int Relay2Status = 0;       //Переменная для хранения статуса Реле2

void handleNewMessages(int numNewMessages)
{
  Serial.print("обработка Новых Сообщений ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/on")
    {
      digitalWrite(Relay1, LOW);  // включить Свет (НИЗКИЙ уровень реле)
      Relay1Status = 1;           // меняем статус на 1
      bot.sendMessage(chat_id, "Свет включен", "");
    }

    if (text == "/off")
    {
      digitalWrite(Relay1, HIGH);   // выключить Свет (ВЫСОКИЙ уровень реле)
      Relay1Status = 0;             // меняем статус на 0      
      bot.sendMessage(chat_id, "Свет выключен", "");
    }
        if (text == "/on2")
    {
      digitalWrite(Relay2, LOW);    // включить Реле (НИЗКИЙ уровень реле)
      Relay2Status = 1;             // меняем статус на 1
      bot.sendMessage(chat_id, "Реле включено", "");
    }
    if (text == "/off2")
    {
      digitalWrite(Relay2, HIGH);   // выключить Реле (ВЫСОКИЙ уровень реле)
      Relay2Status = 0;             // меняем статус на 0      
      bot.sendMessage(chat_id, "Реле выключено", "");
    }
    if (text == "/offall")
    {
      digitalWrite(Relay1, HIGH);   // выключить Свет (ВЫСОКИЙ уровень реле)
      digitalWrite(Relay2, HIGH);   // выключить Реле (ВЫСОКИЙ уровень реле)
      Relay1Status = 0;             // меняем статус на 0  
      Relay2Status = 0;             // меняем статус на 0     
      bot.sendMessage(chat_id, "Всё выключено", "");
    }
    if (text == "/status")
    {
      if (Relay1Status)bot.sendMessage(chat_id, "Свет включен", "");
      else bot.sendMessage(chat_id, "Свет выключен", "");
      if (Relay2Status)bot.sendMessage(chat_id, "Реле включено", "");
      else bot.sendMessage(chat_id, "Реле выключено", "");
    }

    if (text == "/start")
    {
      String welcome = "Добро пожаловать, " + from_name + ".\n";
      welcome += "Включить-выключить реле.\n\n";
      welcome += "------------------------------\n";
      welcome += "/on : включить свет\n";
      welcome += "------------------------------\n";
      welcome += "/off : выключить свет\n";
      welcome += "------------------------------\n";
      welcome += "/on2 : включить реле\n";
      welcome += "------------------------------\n";
      welcome += "/off2 : выключить реле\n";
      welcome += "------------------------------\n";
      welcome += "/offall : выключить всё\n";
      welcome += "------------------------------\n";
      welcome += "/status : показывает включено или выключено\n";
      welcome += "------------------------------\n";
      bot.sendMessage(chat_id, welcome, "");
    }
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(Relay1, OUTPUT);                  // Инициализируем как Выход
  pinMode(Relay2, OUTPUT);                  // Инициализируем как Выход
  digitalWrite(Relay1, HIGH);               // Выключаем свет
  digitalWrite(Relay2, HIGH);               // Выключаем реле

  // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");         // Сервер точного времени UTC NTP
  secured_client.setTrustAnchors(&cert);    // Добавляем сертификат api.telegram.org
  Serial.print("Соединяемся с Wifi ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nСоединились. IP адрес: ");
  Serial.println(WiFi.localIP());

  Serial.print("Получение времени: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("Ответ получен");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
