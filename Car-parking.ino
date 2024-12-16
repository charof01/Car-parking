#include <Keypad.h>
#include <Wire.h>                // ไลบรารีสำหรับการสื่อสาร I2C
#include <Adafruit_GFX.h>         // ไลบรารีสำหรับกราฟิก
#include <Adafruit_SSD1306.h>     // ไลบรารีสำหรับ OLED SSD1306
#include <Servo.h>
#include <TM1637Display.h>       // ไลบรารีสำหรับ TM1637

#define TRIG_PIN 8
#define ECHO_PIN 9

#define SERVO_PIN 4

#define SCREEN_WIDTH 128   // ความกว้างของจอ
#define SCREEN_HEIGHT 64   // ความสูงของจอ
#define OLED_RESET    -1   // ไม่ใช้พอร์ต reset
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define CLK_PIN 5
#define DIO_PIN 6
TM1637Display tm1637(CLK_PIN, DIO_PIN);

#define ROW_NUM    4  // 4 rows
#define COLUMN_NUM 4  // 4 columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {A0, A1, A2, A3}; // แถว
byte pin_column[COLUMN_NUM] = {9, 8, 7, 6}; // คอลัมน์

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// สร้างตัวแปร Servo
Servo parkingGateServo;

void setup() {
  Serial.begin(9600);

  // กำหนด pin สำหรับ HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // เริ่มต้นหน้าจอ OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // หยุดหากไม่สามารถเชื่อมต่อได้
  }

  // ล้างหน้าจอ OLED
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);  // กำหนดสีตัวอักษรเป็นสีขาว
  display.setTextSize(2);               // ขนาดตัวอักษรใหญ่ขึ้น
  display.setCursor(0, 0);              // ตั้งตำแหน่งการแสดงผล
  display.print(F("Parking System"));
  display.display();                    // แสดงผลบนหน้าจอ

  // เชื่อมต่อกับ Servo Motor
  parkingGateServo.attach(SERVO_PIN);

  // เริ่มต้น TM1637 display
  tm1637.setBrightness(0x0f);  // Set brightness to max
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = (duration * 0.0343) / 2;
  return distance;
}

void displayPrice(int price) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);

  // แสดงราคาที่จอ OLED
  display.print("Price: ");
  display.print(price);  // แสดงตัวเลข
  display.print(" Baht");

  display.display();

  // แสดงราคาที่ TM1637
  tm1637.showNumberDec(price);  // แสดงตัวเลขที่ TM1637 display
}

void openParkingGate() {
  parkingGateServo.write(0);  // เปิดประตู
  delay(6000);  // เปิด 6 วินาที
  parkingGateServo.write(90);   // ปิดประตู
}

void loop() {
  long distance = getDistance();
  if (distance <= 50) {
    Serial.println("Car detected, please select parking time.");

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Select time (hr):");
    display.display();

    char key = keypad.getKey();
    if (key) {
      int price = 0;

      // กำหนดราคาตามการกดปุ่ม
      switch (key) {
        case '0':
          price = 0;
          break;
        case '1':
          price = 10;
          break;
        case '2':
          price = 20;
          break;
        case '3':
          price = 30;
          break;
        case '4':
          price = 40;
          break;
        case '5':
          price = 50;
          break;
        case '6':
          price = 60;
          break;
        case '7':
          price = 70;
          break;
        case '8':
          price = 80;
          break;
        case '9':
          price = 90;
          break;
        default:
          price = 0;
          break;
      }

      Serial.print("You selected ");
      Serial.print(price);
      Serial.println(" Baht.");

      displayPrice(price);  // แสดงราคาบนจอ OLED และ TM1637
      openParkingGate();    // เปิดประตู

    }
  } else {
    Serial.println("No object detected.");
  }
  delay(100);
}
