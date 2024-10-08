
#include <HX711.h>
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
// Load Cell Pins
#define LOADCELL_DOUT_PIN 28
#define LOADCELL_SCK_PIN 30
HX711 scale;
float calibration_factor = 191;
// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowPins[ROWS] = {23, 25, 27, 29};
byte colPins[COLS] = {31, 33, 35, 37};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// Servo
Servo lidServo;
#define SERVO_PIN 9
// LEDs
#define LED1_PIN 45 // LED1 for weight above 2000g
#define LED2_PIN 47 // LED2 for weight below 2000g
// DS18B20 Temperature Sensor Pins
#define ONE_WIRE_BUS_1 34
#define ONE_WIRE_BUS_2 40
OneWire oneWire1(ONE_WIRE_BUS_1);
OneWire oneWire2(ONE_WIRE_BUS_2);
DallasTemperature sensors1(&oneWire1);
DallasTemperature sensors2(&oneWire2);
// Relay Pins
const int relay1Pin = 42;
const int relay2Pin = 44;
const int relay3Pin = 48; 
const int relay4Pin = 53;
const int relay5Pin = 51; 
const int relay6Pin = 49;
// Stepper Motor Pins
#define PUL_PIN1 5 // motor 1
#define DIR_PIN1 6
#define ENA_PIN1 7
#define PUL_PIN2 11 // motor 2
#define DIR_PIN2 12
#define ENA_PIN2 13
#define PUL_PIN3 2 // motor 3
#define DIR_PIN3 3
#define ENA_PIN3 4
const int stepsPerRevolution1 = 200;
const int stepsPerRevolution2 = 500;
const int stepsPerRevolution3 = 4200;
float targetWeight = 0;
// GSM
SoftwareSerial gsm(10, 8); // RX, TX
void setup() {
Serial.begin(9600);
Serial.println("Starting setup...");
gsm.begin(9600); // GSM initialization
delay(1000);
setupLoadCell();
setupLCD();
setupServo();
setupLEDs();
setupMotors();
setupTemperatureSensors();
setupRelays();
closeLid();
lcd.clear();
displayWeight(getWeight());
}
void loop() {
float currentWeight = getWeight();
displayWeight(currentWeight);
updateLEDs(currentWeight);
updateTemperatureControl();
char key = getKeypadInput();
if (key == 'A') {
handleGrainDispensing(currentWeight);
} else if (key == 'B') {
tareScale();
} else if (key == 'D') {
displayTemperature();
}
}
void setupLoadCell() {
scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
scale.set_scale(calibration_factor);
}
void setupLCD() {
lcd.init();
lcd.backlight();
}
void setupServo() {
lidServo.attach(SERVO_PIN);
}
void setupLEDs() {
pinMode(LED1_PIN, OUTPUT);
pinMode(LED2_PIN, OUTPUT);
}
void setupMotors() {
//setup motor 1
pinMode(PUL_PIN1, OUTPUT);
pinMode(DIR_PIN1, OUTPUT);
pinMode(ENA_PIN1, OUTPUT);
digitalWrite(ENA_PIN1, LOW);
digitalWrite(DIR_PIN1, HIGH);
//setup motor 2
pinMode(PUL_PIN2, OUTPUT);
pinMode(DIR_PIN2, OUTPUT);
pinMode(ENA_PIN2, OUTPUT);
digitalWrite(ENA_PIN2, LOW);
//setup motor 3
pinMode(PUL_PIN3, OUTPUT);
pinMode(DIR_PIN3, OUTPUT);
pinMode(ENA_PIN3, OUTPUT);
digitalWrite(ENA_PIN3, LOW);
}
void setupTemperatureSensors() {
sensors1.begin();
sensors2.begin();
}
void setupRelays() {
pinMode(relay1Pin, OUTPUT);
pinMode(relay2Pin, OUTPUT);
pinMode(relay3Pin, OUTPUT); 
pinMode(relay4Pin, OUTPUT); 
pinMode(relay5Pin, OUTPUT);
pinMode(relay6Pin, OUTPUT);
digitalWrite(relay1Pin, HIGH);
digitalWrite(relay2Pin, HIGH);
digitalWrite(relay3Pin, HIGH); 
digitalWrite(relay4Pin, HIGH); 
digitalWrite(relay5Pin, HIGH);
digitalWrite(relay6Pin, HIGH);
}
float getWeight() {
return scale.get_units(10);
}
void displayWeight(float weight) {
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Weight: ");
lcd.print(weight, 1);
lcd.print(" g");
}
char getKeypadInput() {
return keypad.getKey();
}
void updateLEDs(float weight) {
static bool messageSent = false;
if (weight > 2000) {
digitalWrite(LED1_PIN, HIGH);
digitalWrite(LED2_PIN, LOW);
messageSent = false; 
} else {
digitalWrite(LED1_PIN, LOW);
digitalWrite(LED2_PIN, HIGH);
// Send SMS when stock is below 2000 grams
if (!messageSent) {
sendStockLowAlert();
messageSent = true; 
}
}
}
void sendStockLowAlert() {
Serial.println("Sending stock low alert...");
gsm.println("AT+CMGF=1");
delay(1000);
gsm.println("AT+CMGS=\"+94707714611\""); // Phone number
delay(1000);
gsm.print("Stock is below 2000 grams!"); // Alert
delay(1000);
gsm.write(26); 
delay(5000);
Serial.println("Alert sent!");
}
void openLid() {
lidServo.write(155); // Open the lid
}
void closeLid() {
lidServo.write(0); // Close the lid
}
void handleGrainDispensing(float currentWeight) {
const float pricePerGram = 1.20; // Price per gram
const String currency = "Rs."; // Currency
lcd.clear();
lcd.print("Enter grams:");
targetWeight = 0;
String input = "";
while (true) {
char key = getKeypadInput();
if (key) {
if (key >= '0' && key <= '9') {
input += key;
lcd.setCursor(0, 1);
lcd.print(input);
} else if (key == '*') {
if (input.length() > 0) {
input.remove(input.length() - 1); 
lcd.setCursor(0, 1);
lcd.print(" "); 
lcd.setCursor(0, 1);
lcd.print(input); 
}
} else if (key == '#') {
if (input.length() > 0) {
targetWeight = input.toFloat() - 20; 
break;
} else {
lcd.clear();
lcd.print("Enter a fair amount");
delay(2000); // Display warning for 2 seconds
lcd.clear();
lcd.print("Enter grams:");
input = ""; // Clear the input string
}
}
}
}
float finalWeight = currentWeight - targetWeight;
lcd.clear();
lcd.print("Dispensing...");
Serial.print("Current weight: ");
Serial.println(currentWeight);
Serial.print("Target weight to dispense: ");
Serial.println(targetWeight);
Serial.print("Final weight: ");
Serial.println(finalWeight);
openLid();
while (getWeight() > finalWeight) {
currentWeight = getWeight();
Serial.print("Current weight: ");
Serial.println(currentWeight);
updateLEDs(currentWeight);
// Close the lid incrementally as we approach the target weight
if (currentWeight <= currentWeight - (targetWeight * 0.8)) {
lidServo.write(120); // Close the lid partially
}
if (currentWeight <= currentWeight - (targetWeight * 0.9)) {
lidServo.write(90); // Close the lid further
}
if (currentWeight <= finalWeight) {
lidServo.write(0); // Close the lid completely
break;
}
delay(100); 
}
closeLid();
lcd.clear();
lcd.print("Done");
delay(1000);
// Calculate and display the price
float price = (targetWeight + 20) * pricePerGram;
lcd.clear();
lcd.print("Price: ");
lcd.print(currency);
lcd.print(" ");
lcd.print(price);
delay(2000); // Display price for 2 seconds
displayWeight(getWeight());
// Turn on relay4 after dispensing is done
digitalWrite(relay4Pin, LOW);
digitalWrite(relay5Pin, LOW);
digitalWrite(relay6Pin, LOW); 
delay(2000); 
// Run the sealing process
runSealingProcess();
}
void tareScale() {
lcd.clear();
lcd.print("Tare...");
scale.tare();
delay(2000);
lcd.clear();
}
void runSealingProcess() {
// Run Motor 1
Serial.println("Running Motor 1...");
for (int i = 0; i < 16 * stepsPerRevolution1; i++) {
digitalWrite(PUL_PIN1, HIGH);
delayMicroseconds(1000);
digitalWrite(PUL_PIN1, LOW);
delayMicroseconds(1000);
}
Serial.println("Motor 1 done.");
// Run Motor 2 clockwise
Serial.println("Running Motor 2 Clockwise");
digitalWrite(DIR_PIN2, HIGH);
for (int i = 0; i < stepsPerRevolution2; i++) {
digitalWrite(PUL_PIN2, HIGH);
delayMicroseconds(2000);
digitalWrite(PUL_PIN2, LOW);
delayMicroseconds(2000);
}
delay(3000);
// Turn on the relay3 at the beginning
digitalWrite(relay3Pin, LOW);
// Run Motor 2 counterclockwise
Serial.println("Running Motor 2 Counterclockwise");
digitalWrite(DIR_PIN2, LOW);
for (int i = 0; i < stepsPerRevolution2; i++) {
digitalWrite(PUL_PIN2, HIGH);
delayMicroseconds(2000);
digitalWrite(PUL_PIN2, LOW);
delayMicroseconds(2000);
}
delay(500);
// Run Motor 3 clockwise
Serial.println("Running Motor 3 Clockwise");
digitalWrite(DIR_PIN3, HIGH);
for (int i = 0; i < 7 * stepsPerRevolution3; i++) {
digitalWrite(PUL_PIN3, HIGH);
delayMicroseconds(100);
digitalWrite(PUL_PIN3, LOW);
delayMicroseconds(100);
}
delay(500);
// Run Motor 3 counterclockwise
Serial.println("Running Motor 3 Counterclockwise");
digitalWrite(DIR_PIN3, LOW);
for (int i = 0; i < 7 * stepsPerRevolution3; i++) {
digitalWrite(PUL_PIN3, HIGH);
delayMicroseconds(100);
digitalWrite(PUL_PIN3, LOW);
delayMicroseconds(100);
}
Serial.println("Motor 3 done.");
digitalWrite(relay3Pin, HIGH);
delay(3000); 
// Turn off relay4 after sealing process is done
digitalWrite(relay4Pin, HIGH);
digitalWrite(relay5Pin, HIGH);
digitalWrite(relay6Pin, HIGH);
}
void updateTemperatureControl() {
sensors1.requestTemperatures();
sensors2.requestTemperatures();
float temperature1 = sensors1.getTempCByIndex(0);
float temperature2 = sensors2.getTempCByIndex(0);
if (temperature2 > 120) {
digitalWrite(relay1Pin, HIGH); // Turn off heater 1
} else {
digitalWrite(relay1Pin, LOW); // Turn on heater 1
}
if (temperature2 > 120) {
digitalWrite(relay2Pin, HIGH); // Turn off heater 2
} else {
digitalWrite(relay2Pin, LOW); // Turn on heater 2
}
}
void displayTemperature() {
sensors1.requestTemperatures();
sensors2.requestTemperatures();
float temperature1 = sensors1.getTempCByIndex(0);
float temperature2 = sensors2.getTempCByIndex(0);
lcd.clear();
lcd.print("Temp1: ");
lcd.print(temperature1);
lcd.setCursor(0, 1);
lcd.print("Temp2: ");
lcd.print(temperature2);
delay(2000); // Display temperature for 2 seconds
displayWeight(getWeight());
}