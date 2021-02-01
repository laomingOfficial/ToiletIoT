/*
  Code by: 老明
  Youtube Channel: https://www.youtube.com/channel/UCVoGtuR1U5B-KK1kgMIoJWA
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// 更換這裏
const char* ssid = "********";
const char* password = "********";
const char* mqttServer = "iot.reyax.com";
const int mqttPort = 1883;
const char* mqttUser = "********";
const char* mqttPassword = "********";
const char* mqttDeviceId = "********";
const char* mqttTopicPublish = "api/request";
const char* mqttTopicSubscribe = "api/command/NetworkId/#";
const char* iotId = "Toilet1";
// 到這裏

WiFiClient espClient;
PubSubClient client(espClient);
DynamicJsonDocument doc(1024);
JsonObject jsonObject;

#define MAGNET_PIN 33
#define DEBOUNCE_TIME 50
#define BUZZER_PIN 22
DRAM_ATTR int magnetState = 0, magnetStatePrev = 0;
volatile uint32_t DebounceTimer = 0;

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

// 旋律start
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, REST,
  REST,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, REST,
  REST,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, REST,
  REST,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, REST,
  REST, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  REST, REST, REST
};

int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};

int demonSlayer_melody[] = {
  NOTE_G5, NOTE_FS5, NOTE_G5,
  NOTE_G5, NOTE_FS5, NOTE_G5,
  NOTE_G5, NOTE_FS5, NOTE_E5, NOTE_D5, NOTE_D5,
  NOTE_B4, NOTE_D5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_A5,
  NOTE_G5, NOTE_A5, NOTE_B5,
  NOTE_E5, NOTE_G5, NOTE_FS5, NOTE_D5, NOTE_E5, NOTE_D5,
  NOTE_A4, NOTE_G4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_E4,
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_B4, NOTE_D5, NOTE_E5, NOTE_G5, NOTE_FS5, NOTE_D5, NOTE_E5, NOTE_D5,
  NOTE_B5, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_B5, NOTE_G5, NOTE_G5, NOTE_FS5, NOTE_FS5, NOTE_G5, NOTE_A5
};

int demonSlayer_tempo[] = {
  8, 8, 4,
  8, 8, 4,
  8, 8, 6, 10, 4,
  6, 10, 4,
  6, 10, 4,
  6, 10, 2,
  10, 10, 8, 12, 12, 4,
  12, 12, 12, 12, 12, 4,
  12, 14, 12, 10, 12, 12, 12, 10, 10, 8, 12, 12, 4,
  12, 12, 12, 12, 12, 4, 12, 10, 10, 12, 8
};
// 旋律end

void IRAM_ATTR magnetChanged() {
  if ( millis() - DEBOUNCE_TIME  >= DebounceTimer ) {
    DebounceTimer = millis();
    magnetState = digitalRead(MAGNET_PIN);
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MAGNET_PIN, INPUT_PULLUP);
  magnetState = magnetStatePrev = digitalRead(MAGNET_PIN);
  attachInterrupt(digitalPinToInterrupt(MAGNET_PIN), magnetChanged, CHANGE);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // 如果之前磁鐵和現在的狀態不一樣
  // 發佈信號
  if (magnetStatePrev != magnetState) {
    Serial.println("Magnet " + String(magnetState));
    sendStatus();
    magnetStatePrev = magnetState;
  }
}

void setup_wifi() {
  delay(10);
  // 開始連接Wifi
  Serial.println();
  Serial.print("連接 ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi 已連接");
  Serial.println("IP 地址: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

  // 解讀json
  deserializeJson(doc, payload);
  jsonObject = doc.as<JsonObject>();

  if (jsonObject["action"].as<String>() == "command/insert") {
    String commandStr = jsonObject["command"]["command"].as<String>();
    if (commandStr == "beepAlarm") {
      // 如果是這IoTID才繼續beep
      if (jsonObject["command"]["parameters"]["iotId"].as<String>() == iotId) {
        beepAlarm(doc["command"]["result"]["beepSong"].as<String>());
      }
    } else if (commandStr == "retrieveStatus") {
      sendStatus();
    }
  }
}

void reconnect() {
  // Loop到連接爲止
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // 創建新clientId
    String clientId = String(mqttUser) + "_" + String(random(0xffff), HEX);
    // 嘗試連接
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("已連接");
      // 發佈notification
      sendStatus();
      // 訂閲command
      client.subscribe(mqttTopicSubscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" 5秒後再嘗試連接");
      delay(5000);
    }
  }
}

void sendStatus() {
  // 讀取電池電量
  int powerLevel = 100;
  // 準備reyax需要的payload格式
  String payload = String("") + "{\"action\":\"notification/insert\",\"deviceId\":\"" + mqttDeviceId + "\",\"notification\":{\"notification\":\"toiletStatus\",\"parameters\":{\"iotId\":\"" + String(iotId) + "\",\"status\":" + (magnetState ? "true" : "false") + ",\"power\":" + String(powerLevel) + "}}}";
  // 發佈notification
  client.publish(mqttTopicPublish, payload.c_str());
}

void beepAlarm(String beepSong) {
  Serial.println(beepSong);
  if (beepSong == "DemonSlayer") {
    playMelody(demonSlayer_melody, demonSlayer_tempo, sizeof(demonSlayer_melody));
  } else if (beepSong == "MarioUnderworld") {
    playMelody(underworld_melody, underworld_tempo, sizeof(underworld_tempo));
  }
}

void playMelody(int melodyArr[], int tempoArr[], int arrSize) {
  int size = arrSize / sizeof(int);
  //  Serial.println(String(sizeof(melodyArr)));
  Serial.println(size);
  for (int thisNote = 0; thisNote < size; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / tempoArr[thisNote];

    buzz(BUZZER_PIN, melodyArr[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    buzz(BUZZER_PIN, 0, noteDuration);
  }
}

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 0; // calculate the delay value between transitions
  if (frequency != 0) {
    delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  }
  // 1 second's worth of microseconds, divided by the frequency, then split in half since
  // there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  // multiply frequency, which is really cycles per second, by the number of seconds to
  // get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
}
