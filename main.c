#include <Servo.h>

Servo servoFlap;   
Servo servoDisk;

// ----------------------- PIN DEFINITIONS -----------------------
const int PIN_IR        = 5;    // IR obstacle sensor
const int PIN_METAL     = 6;    // Inductive metal sensor (via voltage divider)
const int PIN_MOISTURE  = A0;   // Moisture sensor
const int PIN_BUZZER    = 12;

// Servos
const int PIN_SERVO_FLAP = 7;
const int PIN_SERVO_DISK = 8;

// ----------------------- SERVO ANGLES -----------------------
const int ANGLE_HOME    = 0;    
const int ANGLE_DRY     = 0;    
const int ANGLE_WET     = 60;   
const int ANGLE_METAL   = 120;  

const int ANGLE_FLAP_CLOSED = 70;   
const int ANGLE_FLAP_OPEN   = 160;  

// ----------------------- MOISTURE SETTINGS -----------------------
const int MOISTURE_THRESHOLD = 550;  



// ================================================================
// SETUP
// ================================================================
void setup() {
  Serial.begin(9600);

  pinMode(PIN_IR, INPUT);

  // MOST IMPORTANT: use pull-up for metal sensor (NPN NO type)
  pinMode(PIN_METAL, INPUT_PULLUP);

  pinMode(PIN_BUZZER, OUTPUT);

  servoFlap.attach(PIN_SERVO_FLAP);
  servoDisk.attach(PIN_SERVO_DISK);

  servoFlap.write(ANGLE_FLAP_CLOSED);
  servoDisk.write(ANGLE_HOME);
  delay(1000);
  
  Serial.println("Smart Dustbin Ready!");
  Serial.println("Metal Sensor: PIN6 with INPUT_PULLUP");
}



// ================================================================
// MAIN LOOP
// ================================================================
void loop() {

  // IR sensor - waste detected
  if (digitalRead(PIN_IR) == LOW) {

    tone(PIN_BUZZER, 1500, 200); 
    delay(300);

    classifyAndSort();
    
    delay(1000); 
  }

  delay(100);
}



// ================================================================
// CLASSIFICATION & SORTING
// ================================================================
void classifyAndSort() {

  // ------------ SENSOR READING -------------------
  int metalRaw = digitalRead(PIN_METAL);  
  int moistureRaw = analogRead(PIN_MOISTURE);

  // NPN NO sensor gives:
  // NO metal -> HIGH (1)
  // Metal -> LOW (0)

  bool isMetal = (metalRaw == LOW);
  bool isWet = (moistureRaw > MOISTURE_THRESHOLD);

  // ------------ DEBUG OUTPUT ---------------------
  Serial.println("-----------------------------");
  Serial.print("IR: Object Detected\n");

  Serial.print("Metal Raw (PIN6): ");
  Serial.println(metalRaw);

  Serial.print("Metal Detected: ");
  Serial.println(isMetal ? "YES" : "NO");

  Serial.print("Moisture Raw: ");
  Serial.println(moistureRaw);

  Serial.print("Wet Waste: ");
  Serial.println(isWet ? "YES" : "NO");


  // ------------ CLASSIFY WASTE ------------------
  int targetAngle;

  if (isMetal) {
    Serial.println(">>> METAL Waste Selected");
    targetAngle = ANGLE_METAL;
  }
  else if (isWet) {
    Serial.println(">>> WET Waste Selected");
    targetAngle = ANGLE_WET;
  }
  else {
    Serial.println(">>> DRY Waste Selected");
    targetAngle = ANGLE_DRY;
  }


  // ------------ ROTATE BIN DISK -------------------
  servoDisk.write(targetAngle);
  delay(1200);


  // ------------ OPEN FLAP --------------------------
  servoFlap.write(ANGLE_FLAP_OPEN);
  tone(PIN_BUZZER, 2000, 100);
  delay(1000);

  // ------------ CLOSE FLAP -------------------------
  servoFlap.write(ANGLE_FLAP_CLOSED);
  delay(800);

  // ------------ RETURN DISK TO HOME ----------------
  servoDisk.write(ANGLE_HOME);
  delay(1200);

  Serial.println("Sorting Complete!");
  Serial.println("-----------------------------\n");
}
