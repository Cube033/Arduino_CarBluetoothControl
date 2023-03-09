#include <SoftwareSerial.h>
const int avgBase = 100;

const int in1 = 2;
const int in2 = 3;
const int en1 = 5;
const int in3 = 4;
const int in4 = 7;
const int en2 = 6;

const int trigPin = 9;
const int echoPin = 8;

String commandName = "";
int commandCode = 0;

bool cruiseControlOn = false;
bool ccGoBack = false;

bool availableNewAvgDistance = false;

int avgIterationNumb = 0;
int avgSumm = 0;
int avgDistance = 0;

unsigned long stopTime = 0;
const long actionInterval = 1500;

int currentAction = 0;

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(en1, OUTPUT);

  analogWrite(en1, 255);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(en2, OUTPUT);

  analogWrite(en2, 255);

  pinMode(trigPin, OUTPUT);  // устанавливаем пин Trig как выход
  pinMode(echoPin, INPUT);   // устанавливаем пин Echo как вход

  Serial.begin(9600);
}

void loop() {

  //reading command from iOS app
  if (Serial.available()) {
    //commandName = Serial.readString();
    commandCode = Serial.read();
    if (commandCode != 0) {
      Serial.println("commandCode " + String(commandCode));
    }
    //Serial.println(commandName);
    performAction(commandCode);
  }
  //commandName = "";

  //distance
  int ultraSonicDistance = getDistance();
  calculateAvgDistance(ultraSonicDistance);

  //handling cruise control
  if (cruiseControlOn == true && availableNewAvgDistance == true) {
    availableNewAvgDistance = false;
    ccAction();
  }

  //checking end of action
  unsigned long currentTime = millis();
  if (currentTime >= stopTime && cruiseControlOn == false) {
    stop();
  }
}

void performAction(int action) {
  stopTime = millis() + actionInterval;
  if (action == currentAction) {
    return;  //already performing
  } else {
    currentAction = action;
  }
  switch (action) {
    case 0:  //Stop:
      //stop();
      break;
    case 1:  //GoForward:
      goForward();
      break;
    case 2:  //GoBack:
      goBack();
      break;
    case 3:  //GoForwardLeft:
      goLeftForward();
      break;
    case 4:  //GoForwardRight:
      goRightForward();
      break;
    case 5:  //GoBackLeft:
      goLeftBack();
      break;
    case 6:  //GoBackRight:
      goRightBack();
      break;
    case 7:  //cruiseControlOn
      cruiseControlOn = !cruiseControlOn;
      if (cruiseControlOn) {
        Serial.println("Cruise control ON");
      } else {
        Serial.println("Cruise control OFF");
      }
      currentAction = 0;
      break;
    case 8:  // ccGoBack
      ccGoBack = !ccGoBack;
      if (ccGoBack) {
        Serial.println("Auto turn ON");
      } else {
        Serial.println("Auto turn OFF");
      }
      currentAction = 0;
      break;
  }
}

int getDistance() {
  digitalWrite(trigPin, LOW);  // устанавливаем пин Trig в низкий уровень
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);  // устанавливаем пин Trig в высокий уровень
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);  // считываем время возврата звуковой волны
  int distance = duration / 58;            // переводим время в расстояние в сантиметрах
  if (distance < 0) {
    distance = avgDistance;
  } else if (distance > 250) {
    distance = 250;
  }
  return distance;
}

void calculateAvgDistance(int ultraSonicDistance) {
  avgIterationNumb++;
  avgSumm = avgSumm + ultraSonicDistance;
  if (avgIterationNumb == avgBase) {
    avgDistance = avgSumm / avgBase;
    avgIterationNumb = 0;
    avgSumm = 0;
    Serial.print(String(avgDistance));
    availableNewAvgDistance = true;
  }
}

void ccAction() {
  if (avgDistance > 60) {
    Serial.println("Go forward");
    goForward();
  } else {
    stop();
    Serial.println("Stop");
    if (ccGoBack) {
      goLeftBack();
      Serial.println("Turn around");
    }
  }
}

void goLeftForward() {
  turnLeft();
  goForward();
}

void goRightForward() {
  turnRight();
  goForward();
}

void goLeftBack() {
  turnLeft();
  goBack();
}

void goRightBack() {
  turnRight();
  goBack();
}

void goForward() {
  digitalWrite(in4, LOW);
  digitalWrite(in3, HIGH);
}

void goBack() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void turnRight() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void turnLeft() {
  digitalWrite(in2, LOW);
  digitalWrite(in1, HIGH);
}

void stopMovement() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void stopSteering() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void stop() {
  stopMovement();
  stopSteering();
}
