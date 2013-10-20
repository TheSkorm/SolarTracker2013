#define EAST 1
#define WEST 2
#define STOPPED 0
#define HOME 3

const byte LIMIT_EAST = A0;
const byte LIMIT_WEST = A1;
const byte MOTOR_IN1 = 7;
const byte MOTOR_IN2 = 8;
const byte MOTOR_ENABLE = 10;
const byte MOTOR_DUTY_PULL = 32;
const byte MOTOR_DUTY_PUSH = 25;
const byte LDR_EAST = A3;
const byte LDR_WEST = A4;
const byte LDR_NIGHT = 5;
const unsigned long CHECK_INTERVAL = 1000;

float THRESHOLD = 0.25; // threshold in percentage

unsigned long lastChecked = 0;

byte currentDirection = STOPPED;

void setup(){
  Serial.begin(57600);
  delay(1000);
  Serial.println("DEBUG: System Started");
  pinMode(LIMIT_EAST, INPUT);
  pinMode(LIMIT_WEST, INPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  digitalWrite(LIMIT_EAST, HIGH);
  digitalWrite(LIMIT_WEST, HIGH);
}

void loop(){
  checkLDR();
  checkLimits();
  delay(50); // slow down the process
}

// this function drives the motor east until a limit is reached
void returnHome(){
  if (!digitalRead(LIMIT_EAST)){
    Serial.println("DEBUG: Returning to East");
    currentDirection = HOME;
    digitalWrite(MOTOR_IN1, false);
    digitalWrite(MOTOR_IN2, true);
    analogWrite(MOTOR_ENABLE, MOTOR_DUTY_PULL);
    while (!digitalRead(LIMIT_EAST)){
      delay(10);
    } 
    analogWrite(MOTOR_ENABLE, 0);
  } 
  else {
    Serial.println("DEBUG: Already home");
  }
}

//checks the LDR, makes drive decession. Return 0 to do nothing, return 1 to move east, return 2 to move west. 
byte checkLDR(){
  if (lastChecked + CHECK_INTERVAL < millis() ){
    int EastLDRValue = analogRead(LDR_EAST);
    int WestLDRValue = analogRead(LDR_WEST);

    if (EastLDRValue < LDR_NIGHT && WestLDRValue < LDR_NIGHT){
      returnHome();
    }
    else if ((float) (EastLDRValue - WestLDRValue) / (float) EastLDRValue > THRESHOLD ){
      drive(EAST);
    } 
    else if ((float) (WestLDRValue - EastLDRValue) / (float) WestLDRValue > THRESHOLD) {
      drive(WEST);
    } 
    else {
      lastChecked = millis();
      stopDrive();
    } 
  }
}

//checks that the limit switches haven't been hit.
void checkLimits(){
  if (currentDirection != STOPPED){
    if (currentDirection == EAST){
      // Going EAST so only check the east limit
      if (digitalRead(LIMIT_EAST)) {
        Serial.println("DEBUG: Stopping because of chekLimits");
        stopDrive();
      }
    } 
    else if (currentDirection == WEST){
      // Going WEST so only check the west limit
      if (digitalRead(LIMIT_WEST)){
        Serial.println("DEBUG: Stopping because of chekLimits");
        stopDrive();
      }
    } 
    else {
      Serial.println("DEBUG: Stopping because of chekLimits bad data");
      stopDrive();
    }
  }
}

//sets the direction of travel and starts the motor
void drive(byte travelDirection){
  if (travelDirection == EAST && currentDirection != EAST){
    if (!digitalRead(LIMIT_EAST)){
      Serial.println("DEBUG: Driving East");
      currentDirection = EAST;
      digitalWrite(MOTOR_ENABLE, false);
      digitalWrite(MOTOR_IN1, false);
      digitalWrite(MOTOR_IN2, true);
      analogWrite(MOTOR_ENABLE, MOTOR_DUTY_PUSH);
    } 
    else {
      stopDrive(); 
    }
  } 
  else if (travelDirection == WEST && currentDirection != WEST) {
    if (!digitalRead(LIMIT_WEST)){
      Serial.println("DEBUG: Driving West");
      currentDirection = WEST;
      digitalWrite(MOTOR_ENABLE, false);
      digitalWrite(MOTOR_IN1, true);
      digitalWrite(MOTOR_IN2, false);
      analogWrite(MOTOR_ENABLE, MOTOR_DUTY_PULL);
    } 
    else { 
      stopDrive(); 
    }
  }
}

//stops the motor
void stopDrive(){
  if (currentDirection != STOPPED){
    currentDirection = STOPPED;
    digitalWrite(MOTOR_IN1, true);
    digitalWrite(MOTOR_IN2, true);
    digitalWrite(MOTOR_ENABLE, true);
    Serial.println("DEBUG: Stopping");
  }
}








