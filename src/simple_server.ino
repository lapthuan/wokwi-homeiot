
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include <WiFi.h>
#include <FirebaseESP32.h>

#include <addons/TokenHelper.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

#define FIREBASE_HOST "https://homeiotnew-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "UmKcjemiZ0aq9Tg14YGsbz3LD2dd6GqosFzCXmhn"

#include "DHTesp.h"

const int DHT_PIN = 15;
float flamelevel = 0;
DHTesp dhtSensor;

FirebaseData fbdo;

// temp symbol
byte t1[8] = {
    B00000,
    B00001,
    B00010,
    B00100,
    B00100,
    B00100,
    B00100,
    B00111,
};
byte t2[8] = {
    B00111,
    B00111,
    B00111,
    B01111,
    B11111,
    B11111,
    B01111,
    B00011,
};
byte t3[8] = {
    B00000,
    B10000,
    B01011,
    B00100,
    B00111,
    B00100,
    B00111,
    B11100,
};
byte t4[8] = {
    B11111,
    B11100,
    B11100,
    B11110,
    B11111,
    B11111,
    B11110,
    B11000,
};

// humidity symbol
byte hum1[8] = {
    B00000,
    B00001,
    B00011,
    B00011,
    B00111,
    B01111,
    B01111,
    B11111,
};
byte hum2[8] = {
    B11111,
    B11111,
    B11111,
    B01111,
    B00011,
    B00000,
    B00000,
    B00000,
};
byte hum3[8] = {
    B00000,
    B10000,
    B11000,
    B11000,
    B11100,
    B11110,
    B11110,
    B11111,
};
byte hum4[8] = {
    B11111,
    B11111,
    B11111,
    B11110,
    B11100,
    B00000,
    B00000,
    B00000,
};

// Home Symbol
byte house1[8] = {
    B00000,
    B00001,
    B00011,
    B00011,
    B00111,
    B01111,
    B01111,
    B11111,
};
byte house2[8] = {
    B11111,
    B11111,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
};
byte house3[8] = {
    B00000,
    B10010,
    B11010,
    B11010,
    B11110,
    B11110,
    B11110,
    B11111,
};
byte house4[8] = {
    B11111,
    B11111,
    B11111,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
};

byte d[8] = {0b00011, 0b00011, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000};

byte Lck[] = {B01110, B10001, B10001, B11111, B11011, B11011, B11111, B00000};
int dem = 0;
#define pirPin 2
#define Buzzer 33
#define BuzzerFlame 35
#define Led 13

int gerakanPir;
void setup()
{

    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

    pinMode(BuzzerFlame, OUTPUT);
    pinMode(Led, OUTPUT);
    pinMode(Buzzer, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(pirPin, INPUT);
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Thanh Tuan");
    lcd.setCursor(8, 1);
    lcd.print("2023");
    lcd.setCursor(0, 2);
    lcd.print("--------------------");

    delay(3000);
    lcd.clear();
    lcd.createChar(6, Lck);
    lcd.createChar(1, house1);
    lcd.createChar(2, house2);
    lcd.createChar(3, house3);
    lcd.createChar(4, house4);
    lcd.setCursor(1, 2);
    lcd.write(1);
    lcd.setCursor(1, 3);
    lcd.write(2);
    lcd.setCursor(2, 2);
    lcd.write(3);
    lcd.setCursor(2, 3);
    lcd.write(4);

    lcd.setCursor(17, 2);
    lcd.write(1);
    lcd.setCursor(17, 3);
    lcd.write(2);
    lcd.setCursor(18, 2);
    lcd.write(3);
    lcd.setCursor(18, 3);
    lcd.write(4);

    lcd.setCursor(19, 0);
    lcd.write(6);
    lcd.setCursor(9, 0);
    lcd.print("connected-");
    lcd.setCursor(2, 1);
    lcd.print("HOME AUTOMATION");
    lcd.setCursor(6, 2);
    lcd.print("USING IOT");
    delay(500);
    Firebase.deleteNode(fbdo, "/iotdata/dht/");
}

void loop()
{
    gerakanPir = digitalRead(pirPin);
    TempAndHumidity data = dhtSensor.getTempAndHumidity();

    float t = float(data.temperature);
    float h = float(data.humidity);
    Serial.println(t);
    Serial.println(h);

    int LDR_value = analogRead(34);
    Serial.println(LDR_value);
    if (LDR_value < 70)
    {
        Serial.println("toi");
        digitalWrite(Led, HIGH);
    }
    else
    {
        Serial.println("sang");
        digitalWrite(Led, LOW);
    }

    float analogValue = analogRead(32);

    flamelevel = map(analogValue, 4063, 32, 0, 100);
    Serial.print(flamelevel, 0);
    Serial.println("%");

    if (flamelevel > 15)
    { // fire out
        Firebase.setInt(fbdo, "iotdata/Flame", 1);
        digitalWrite(BuzzerFlame, HIGH);
        Serial.println("chay");
    }
    else
    {
        Firebase.setInt(fbdo, "iotdata/Flame", -1);
        digitalWrite(BuzzerFlame, LOW);
        Serial.println("binh thuong");
    }

    Firebase.setFloat(fbdo, "iotdata/dhtsensor/nhietdo", t);
    Firebase.setFloat(fbdo, "iotdata/dhtsensor/doam", h);

    Firebase.setFloat(fbdo, "/iotdata/dht/" + String(dem) + "/doam", h);
    Firebase.setFloat(fbdo, "/iotdata/dht/" + String(dem) + "/nhietdo", t);
    dem = dem + 1;
    if (gerakanPir == HIGH)
    {
        Firebase.setFloat(fbdo, "iotdata/chuyendong", 1);
        Serial.println("co nguoi");
        digitalWrite(Buzzer, HIGH);
    }
    else
    {
        Firebase.setFloat(fbdo, "iotdata/chuyendong", 0);
        Serial.println("khong co nguoi");
        digitalWrite(Buzzer, LOW);
    }

    Firebase.getInt(fbdo, "iotdata/control/0/status");
    if (fbdo.intData() == 1)
    {
        digitalWrite(19, HIGH);
        lcd.clear();
        lcd.setCursor(19, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print("SW_1= ");
        lcd.print("ON ");
    }
    else
    {
        digitalWrite(19, LOW);
        lcd.clear();
        lcd.setCursor(19, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print("SW_1= ");
        lcd.print("OFF");
    }
    Firebase.getInt(fbdo, "iotdata/control/1/status");
    if (fbdo.intData() == 1)
    {
        digitalWrite(18, HIGH);
        lcd.setCursor(11, 1);
        lcd.print("SW_2= ");
        lcd.print("ON ");
    }
    else
    {
        digitalWrite(18, LOW);
        lcd.setCursor(11, 1);
        lcd.print("SW_2= ");
        lcd.print("OFF");
    }
    Firebase.getInt(fbdo, "iotdata/control/2/status");
    if (fbdo.intData() == 1)
    {
        digitalWrite(5, HIGH);
        lcd.setCursor(0, 2);
        lcd.print("SW_3= ");
        lcd.print("ON ");
    }
    else
    {
        digitalWrite(5, LOW);
        lcd.setCursor(0, 2);
        lcd.print("SW_3= ");
        lcd.print("OFF");
    }
    Firebase.getInt(fbdo, "iotdata/control/3/status");
    if (fbdo.intData() == 1)
    {
        digitalWrite(4, HIGH);
        lcd.setCursor(11, 2);
        lcd.print("SW_4= ");
        lcd.print("ON ");
    }
    else
    {
        digitalWrite(4, LOW);
        lcd.setCursor(11, 2);
        lcd.print("SW_4= ");
        lcd.print("OFF");
    }
    delay(1500);

    lcd.clear();
    lcd.createChar(1, t1);
    lcd.createChar(2, t2);
    lcd.createChar(3, t3);
    lcd.createChar(4, t4);
    lcd.createChar(5, d);
    lcd.createChar(6, Lck);

    lcd.setCursor(19, 0);
    lcd.write(6);
    lcd.setCursor(1, 1);
    lcd.write(1);
    lcd.setCursor(1, 2);
    lcd.write(2);
    lcd.setCursor(2, 1);
    lcd.write(3);
    lcd.setCursor(2, 2);
    lcd.write(4);
    lcd.setCursor(4, 1);
    lcd.print("Nhiet do :");
    lcd.setCursor(7, 2);
    lcd.print(t);
    lcd.setCursor(11, 2);
    lcd.write(5);
    lcd.setCursor(12, 2);
    lcd.print("C");

    delay(750);
    lcd.clear();

    lcd.createChar(1, hum1);
    lcd.createChar(2, hum2);
    lcd.createChar(3, hum3);
    lcd.createChar(4, hum4);

    lcd.setCursor(19, 0);
    lcd.write(6);
    lcd.setCursor(3, 1);
    lcd.write(1);
    lcd.setCursor(3, 2);
    lcd.write(2);
    lcd.setCursor(4, 1);
    lcd.write(3);
    lcd.setCursor(4, 2);
    lcd.write(4);
    lcd.setCursor(6, 1);
    lcd.print("Do am :");
    lcd.setCursor(7, 2);
    lcd.print(h);
    lcd.setCursor(12, 2);
    lcd.print("%");
    delay(100);
}
