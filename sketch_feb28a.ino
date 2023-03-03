#include <SoftwareSerial.h>
const int avgBase = 10;

const int in1 = 2;
const int in2 = 3;
const int en1 = 5;
const int in3 = 4;
const int in4 = 7;
const int en2 = 6;

const int trigPin = 9;
const int echoPin = 8;

String commandName = "";

bool cruiseControlOn = false;
bool ccGoBack = false; 

bool availableNewAvgDistance = false;

int avgIterationNumb = 0;
int avgSumm = 0;
int avgDistance = 0;

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

  if (Serial.available()) {
    commandName = Serial.readString();  // читаем очередной символ из буфера
    Serial.println(commandName);
    newCommand();
  }
  commandName = "";

  int ultraSonicDistance = getDistance();
  calculateAvgDistance(ultraSonicDistance);

  if (cruiseControlOn == true && availableNewAvgDistance == true) {
    availableNewAvgDistance = false;
    ccAction();
  }
}

void newCommand() {
  if (commandName == "forward") {
    stepForward();
  }
  if (commandName == "back") {
    stepBack();
  }
  if (commandName == "leftForward") {
    stepLeftForward();
  }
  if (commandName == "rightForward") {
    stepRightForward();
  }
  if (commandName == "leftBack") {
    stepLeftBack();
  }
  if (commandName == "rightBack") {
    stepRightBack();
  }
  if (commandName == "cruiseControl") {
    cruiseControlOn = !cruiseControlOn;
    if (cruiseControlOn) {
      Serial.println("Cruise control ON");
    } else {
      Serial.println("Cruise control OFF");
    }
  }
  if (commandName == "ccGoBack") {
    ccGoBack = !ccGoBack;
    if (ccGoBack) {
      Serial.println("Auto turn ON");
    } else {
      Serial.println("Auto turn OFF");
    }
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
    Serial.print(avgDistance);
    availableNewAvgDistance = true;
  }
}

void ccAction() {
  if (avgDistance > 100) {
    Serial.println("Go forward");
    stepForward();
  } else {
    stop();
    Serial.println("Stop");
    delay(1000);
    if (ccGoBack) {
      stepLeftBack();
      Serial.println("Turn around");
    }
  }
}

void stepForward() {
  goForward();
  delay(1000);
  stop();
}

void stepBack() {
  goBack();
  delay(1000);
  stop();
}

void stepLeftForward() {
  turnLeft();
  stepForward();
}

void stepRightForward() {
  turnRight();
  stepForward();
}

void stepLeftBack() {
  turnLeft();
  stepBack();
}

void stepRightBack() {
  turnRight();
  stepBack();
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