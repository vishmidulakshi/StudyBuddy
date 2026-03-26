#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins
const int trigPin = 3;
const int echoPin = 2;
const int greenLED = 6;
const int redLED = 7;
const int buzzer = 8;
const int button = 9;

// Distance threshold
const int BAD_POSTURE_DIST = 30;

long duration;
int distance;

unsigned long startTime = 0;  // session start time
bool sessionStarted = false;

// Function to draw a simple happy face
void drawHappy(int x, int y) {
  // Eyes
  oled.fillCircle(x+4, y+4, 1, SSD1306_WHITE);
  oled.fillCircle(x+10, y+4, 1, SSD1306_WHITE);

  // Smile (approximate with line segments)
  oled.drawLine(x+3, y+10, x+5, y+12, SSD1306_WHITE);
  oled.drawLine(x+5, y+12, x+9, y+12, SSD1306_WHITE);
  oled.drawLine(x+9, y+12, x+11, y+10, SSD1306_WHITE);
}

// Function to draw a simple sad face
void drawSad(int x, int y) {
  // Eyes
  oled.fillCircle(x+4, y+4, 1, SSD1306_WHITE);
  oled.fillCircle(x+10, y+4, 1, SSD1306_WHITE);

  // Frown (approximate with line segments)
  oled.drawLine(x+3, y+12, x+5, y+10, SSD1306_WHITE);
  oled.drawLine(x+5, y+10, x+9, y+10, SSD1306_WHITE);
  oled.drawLine(x+9, y+10, x+11, y+12, SSD1306_WHITE);
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  Serial.begin(9600);

  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while(1);
  }
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0,0);
  oled.println("Press button to start");
  oled.display();
}

void loop() {
  // Check button press (simple toggle)
  if(digitalRead(button) == LOW) {
    delay(200); // debounce
    sessionStarted = !sessionStarted;  // toggle session ON/OFF

    oled.clearDisplay();
    oled.setCursor(0,0);

    if(sessionStarted) {
      startTime = millis(); // reset timer on new session
      oled.println("Session started!");
    } else {
      // Turn everything off
      digitalWrite(greenLED, LOW);
      digitalWrite(redLED, LOW);
      digitalWrite(buzzer, LOW);
      oled.println("Session stopped");
    }
    oled.display();
  }

  // Only run posture/timer logic if session is ON
  if(sessionStarted) {
    // Ultrasonic distance measurement
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH, 30000);
    distance = duration * 0.034 / 2;

    // Calculate session time
    unsigned long elapsed = millis() - startTime;
    int seconds = (elapsed / 1000) % 60;
    int minutes = (elapsed / 1000) / 60;

    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.print("Time: ");
    if(minutes < 10) oled.print("0");
    oled.print(minutes);
    oled.print(":");
    if(seconds < 10) oled.print("0");
    oled.println(seconds);

    // Posture check
    if(distance < BAD_POSTURE_DIST) {
      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
      digitalWrite(buzzer, HIGH);
      oled.println("Bad posture!");
      drawSad(90,20);
    } else {
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, HIGH);
      digitalWrite(buzzer, LOW);
      oled.println("Good posture!");
      drawHappy(90,20);
    }

    oled.display();
    delay(500);
  }
}