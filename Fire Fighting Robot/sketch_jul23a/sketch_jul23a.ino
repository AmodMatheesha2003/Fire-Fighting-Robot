#include <Servo.h> // Servo library for servo motors
// Create servo objects for two servo motors
Servo myservo;
Servo myservo2;
#define SERVO_PIN 2 // Define pin for myservo
#define SERVO_PIN2 3 // Define pin for myservo2
#include "DHT.h" // DHT library for temperature sensor
#define DHTPIN A4 // Define pin for DHT(Temperature) sensor
#define DHTTYPE DHT11 // type of DHT sensor
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor
// Define commands for robot movement
#define FORWARD 'F'
#define BACKWARD 'B'
#define LEFT 'L'
#define RIGHT 'R'
#define CIRCLE 'C'
#define CROSS 'X'
#define TRIANGLE 'T'
#define SQUARE 'S'
#define START 'A'
#define PAUSE 'P'
#define relayPin 5 // Define pin for relay (water pump)
#define gassen A0 // Define pin for gas sensor
#define flameSensorLeft A1 // Define pin for left flame sensor
#define flameSensorCenter A2 // Define pin for center flame sensor
#define flameSensorRight A3 // Define pin for right flame sensor
#define threshold 500 // for flame detection
#define irback 7 // Define pin for back IR sensor
#define irfront 4 // Define pin for front IR sensor
//servo positions for the first servo(myservo)
#define SERVO_MIN1 0
#define SERVO_MAX1 180
int currentPos1 = 90;
//servo positions for the second servo(myservo2)
#define SERVO_MIN2 40
#define SERVO_MAX2 100
int currentPos2 = 90;
//motor control pins
#define in2 13
#define in1 12
#define in4 9
#define in3 8
#define ena 11
#define enb 6
#define motorSpeed 90 // speed of motors
const int buzzerPin = 10; // pin for buzzer
float gasValue; // for gas sensor reading
void setup() {
 Serial.begin(9600); // for serial communication
 dht.begin(); // Initialize DHT sensor
 myservo.attach(SERVO_PIN); // Attach servo
 myservo2.attach(SERVO_PIN2);
 myservo.write(currentPos1); // Set start position for first servo (myservo)
 myservo2.write(currentPos2); // Set start position for the second servo (myservo2)
 // Set motor control pins
 pinMode(in1, OUTPUT);
 pinMode(in2, OUTPUT);
 pinMode(in3, OUTPUT);
 pinMode(in4, OUTPUT);
 pinMode(ena, OUTPUT);
 pinMode(enb, OUTPUT);
 // Set motor speed
 analogWrite(ena, motorSpeed);
 analogWrite(enb, motorSpeed);
 pinMode(gassen, INPUT); // Set gas sensor pin as input
 pinMode(buzzerPin, OUTPUT); // Set buzzer pin as output
 pinMode(irback, INPUT); // Set back IR sensor pin as input
 pinMode(irfront, INPUT); // Set front IR sensor pin as input
 pinMode(relayPin, OUTPUT); // Set relay pin as output (water pump)
}
void loop() {
 if (Serial.available()) { // Check if there is any command from the serial (mobile application)
 char command = Serial.read(); // Read the command
 executeCommand(command); // Execute the command
 }
 gassensor(); // check gas sensor
 temperature(); // check temperature DHT sensor
 fireDetect(); // check flame sensor
 objectIR();
}
void executeCommand(char command) {
 switch (command) {
 case FORWARD:
 forward(); // Move forward
 delay(100);
 break;
 case BACKWARD:
 backward(); // Move backward
 delay(100);
 break;
 case LEFT:
 turnLeft(); // Turn left
 delay(300);
 stop();
 break;
 case RIGHT:
 turnRight(); // Turn right
 delay(300);
 stop();
 break;
 case CIRCLE:
 currentPos1 -= 20; // rotating the spray arm left
 if (currentPos1 < SERVO_MIN1) currentPos1 = SERVO_MIN1;
 myservo.write(currentPos1);
 delay(10);
 break;
 case CROSS:
 currentPos2 += 20; // rotating the spray arm right
 if (currentPos2 > SERVO_MAX2) currentPos2 = SERVO_MAX2;
 myservo2.write(currentPos2);
 delay(10);
 break;
 case TRIANGLE:
 currentPos2 -= 20; // rotating the spray arm up
 if (currentPos2 < SERVO_MIN2) currentPos2 = SERVO_MIN2;
 myservo2.write(currentPos2);
 delay(10);
 break;
 case SQUARE:
 currentPos1 += 20; // rotating the spray arm down
 if (currentPos1 > SERVO_MAX1) currentPos1 = SERVO_MAX1;
 myservo.write(currentPos1);
 delay(10);
 break;
 case START:
 digitalWrite(relayPin, HIGH); // Start water pump
 delay(1000);
 break;
 case PAUSE:
 stop(); 
 digitalWrite(relayPin, LOW); // Stop water pump
 delay(100);
 break;
 default:
 break;
 }
}
void backward() {
 Serial.println("Moving backward");
 digitalWrite(in1, HIGH);
 digitalWrite(in2, LOW);
 digitalWrite(in3, HIGH);
 digitalWrite(in4, LOW);
}
void forward() {
 Serial.println("Moving forward");
 digitalWrite(in1, LOW);
 digitalWrite(in2, HIGH);
 digitalWrite(in3, LOW);
 digitalWrite(in4, HIGH);
}
void turnLeft() {
 Serial.println("Turning left");
 digitalWrite(in1, LOW);
 digitalWrite(in2, HIGH);
 digitalWrite(in3, HIGH);
 digitalWrite(in4, LOW);
}
void turnRight() {
 Serial.println("Turning right");
 digitalWrite(in1, HIGH);
 digitalWrite(in2, LOW);
 digitalWrite(in3, LOW);
 digitalWrite(in4, HIGH);
}
void stop() {
 Serial.println("Stopping");
 digitalWrite(in1, LOW);
 digitalWrite(in2, LOW);
 digitalWrite(in3, LOW);
 digitalWrite(in4, LOW);
}
void gassensor() {
 gasValue = analogRead(gassen); // Read gas sensor value
 Serial.print("Gas Sensor Value: ");
 Serial.println(gasValue);
 if (gasValue > 200) { //Exceeding 200 means that a gas(LP gas or smoke) has been identified
 Serial.println("Gas Detected");
 digitalWrite(buzzerPin, HIGH); // Sound buzzer if gas detected
 delay(70);
 digitalWrite(buzzerPin, LOW); // Turn off buzzer
 delay(70);
 } else {
 digitalWrite(buzzerPin, LOW); // Turn off buzzer if no gas detected
 delay(10);
 }
 delay(10);
}
void temperature() {
 delay(20);
 float t = dht.readTemperature(); // Read temperature
 if (isnan(t)) { //check DHT sensor
 Serial.println(F("Failed to read DHT sensor!")); 
 return;
 }
 Serial.print(F("Temperature: "));
 Serial.print(t);
 Serial.println(F("°C"));
 if (t > 100) { //Exceeding 100 °C means need to protect vehical from high tempature
 Serial.println("High Temperature detected");
 buzzer();
 backward(); // Move backward
 delay(500);
 stop();
 delay(10);
 backward();
 delay(500);
 stop(); //stop vehical
 delay(10000);
 } else {
 digitalWrite(buzzerPin, LOW); // Turn off buzzer
 }
}
void buzzer() {
 digitalWrite(buzzerPin, HIGH); // Turn on buzzer
 delay(70);
 digitalWrite(buzzerPin, LOW);
 delay(70);
}
void objectIR() {
 int irbackValue = digitalRead(irback); // Read back IR sensor value
 int irfrontValue = digitalRead(irfront); // Read front IR sensor value
 if (irbackValue == LOW && irfrontValue == HIGH) {
 Serial.println("Object detected on back");
 stop();
 delay(10);
 } else if (irbackValue == HIGH && irfrontValue == LOW) {
 Serial.println("Object detected on front");
 stop();
 delay(10);
 } else if (irbackValue == LOW && irfrontValue == LOW) {
 Serial.println("Object detected on the front and back");
 stop();
 delay(10);
 } else {
 Serial.println("No object detected");
 }
 delay(500);
}
void fireDetect() {
 int leftValue = analogRead(flameSensorLeft); // Read left flame sensor value
 int centerValue = analogRead(flameSensorCenter); // Read center flame sensor value
 int rightValue = analogRead(flameSensorRight); // Read right flame sensor value
 Serial.println(leftValue);
 Serial.println(centerValue);
 Serial.println(rightValue);
 if (leftValue < threshold && centerValue >= threshold && rightValue >= threshold) {
 Serial.println("Flame detected on the LEFT");
 stop();
 digitalWrite(relayPin, HIGH); // Start water pump
 handleFire(120, 180); // Rotate servo to extinguish fire
 digitalWrite(relayPin, LOW); // Stop water pump
 } else if (centerValue < threshold && leftValue >= threshold && rightValue >= threshold) {
 Serial.println("Flame detected in the CENTER");
 stop();
 digitalWrite(relayPin, HIGH); // Start water pump
 handleFire(50, 130); // Rotate servo to extinguish fire
 digitalWrite(relayPin, LOW); // Stop water pump
 } else if (rightValue < threshold && leftValue >= threshold && centerValue >= threshold) {
 Serial.println("Flame detected on the RIGHT");
 stop();
 digitalWrite(relayPin, HIGH); // Start water pump
 handleFire(0, 60); // Rotate servo to extinguish fire
 digitalWrite(relayPin, LOW); // Stop water pump
 } else if (rightValue < threshold && leftValue < threshold && centerValue < threshold) {
 Serial.println("Flame detected all around");
 stop();
 digitalWrite(relayPin, HIGH); // Start water pump
 handleFire(20, 170); // Rotate servo to extinguish fire
 digitalWrite(relayPin, LOW); // Stop water pump
 } else {
 Serial.println("No flame detected");
 }
 delay(500);
}
void handleFire(int startPos, int endPos) {
 moveServo(startPos, endPos); // Rotate servo from startPos to endPos
 delay(500);
 moveServo(endPos, startPos); // Rotate servo back from endPos to startPos
 delay(2000);
}
void moveServo(int startPos, int endPos) {
 if (startPos < endPos) {
 for (int pos = startPos; pos <= endPos; pos++) {
 myservo.write(pos); // Move servo to position
 delay(10);
 }
 } else {
 for (int pos = startPos; pos >= endPos; pos--) {
 myservo.write(pos);
 delay(10);
 }
 }
}
