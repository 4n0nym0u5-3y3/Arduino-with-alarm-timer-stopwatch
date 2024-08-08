#include <LiquidCrystal.h>

const int RS_PIN = 12, 

EN_PIN = 11, 
D5_PIN = 6, 
D6_PIN = 5, 
D7_PIN = 4, 
D8_PIN = 3;

LiquidCrystal display(RS_PIN, EN_PIN, D5_PIN, D6_PIN, D7_PIN, D8_PIN);

bool alarmEnabled = false, stopwatchActive = false, timerActive = false;
const int BUZZER_PIN = 2, LED_PIN = 13;
const int BUTTON_1 = 10; 
const int BUTTON_2 = 9;
const int BUTTON_3 = 8;
const int BUTTON_4 = 7;

int currentHour = 10, currentMinute = 15, currentSecond = 5, blinkState = 0, millisCount = 0, lastUpdateTime = 0, currentTime;
int alarmHour = 0, alarmMinute = 0, alarmSecond = 0;
int currentMode = 0;
int AM_PM_Flag = 0, Alarm_AM_PM_Flag = 0;

int buttonState1; // Stores the state of BUTTON_1
int buttonState2;
int buttonState3;
int buttonState4;

void setup() {
  display.begin(16, 2);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop(){
  if(currentMode == 0) showClock();
  else if(currentMode == 1) adjustTime();
  else if(currentMode == 2) configureAlarm();
  else if(currentMode == 3) configureTimer();
  else if(currentMode == 4) startStopwatch();
}

void updateState(){
  buttonState1 = digitalRead(BUTTON_1);
  buttonState2 = digitalRead(BUTTON_2);
  buttonState3 = digitalRead(BUTTON_3);
  buttonState4 = digitalRead(BUTTON_4);
  
  if(buttonState1 || buttonState2 || buttonState3 || buttonState4) {
    if(buttonState1 == 1){
      currentMode = 1;
    }
    if(buttonState2 == 1){
      currentMode = 2;
    }
    if(buttonState3 == 1){
      currentMode = 3;
    }
    if(buttonState4 == 1){
      currentMode = 4;
    }
  } else {
    currentMode = 0;
  }
}

void showClock(){
  display.setCursor(0, 0);
  display.print("Current Time:");
  display.setCursor(0, 1);
  display.print("Time: ");
  if(currentHour < 10) display.print("0");
  display.print(currentHour);
  display.print(":");
  if(currentMinute < 10) display.print("0");
  display.print(currentMinute);
  display.print(":");
  if(currentSecond < 10) display.print("0");
  display.print(currentSecond);

  if(AM_PM_Flag == 0) display.print(" AM");
  if(AM_PM_Flag == 1) display.print(" PM");
 
  updateState();
  delay(200);
  updateClock(&currentHour, &currentMinute, &currentSecond, &currentTime, &lastUpdateTime, &millisCount, &AM_PM_Flag);
}

void updateClock(int* hr, int* min, int* sec, int* now, int* lastTime, int* mls, int* flag){
 int hourIncrement = 0; 
 *now = millis();
 *mls += (*now - *lastTime);
 *lastTime = *now;

 if(*mls >= 1000){
  *sec += (*mls / 1000); 
  *mls %= 1000;
  }
 if(*sec >= 60){
   *min += *sec / 60;
   *sec %= 60;
  }
 if(*min >= 60){
   hourIncrement = *min / 60;
   *min %= 60;
  }
  if(hourIncrement % 24 == 0); // no change in hour
  else {
    hourIncrement %= 24;
    if(hourIncrement < 12){
      if(*hr == 12){
        *hr = hourIncrement;
      }
      else if(*hr + hourIncrement >= 12){
        *hr -= 12;
        *flag = *flag == 0 ? 1 : 0;
      }
    }
    else{
      *hr = ((*hr + hourIncrement) % 12);
      *flag = *flag == 0 ? 1 : 0;
    }
    if(*flag == 0 && *hr == 12) *hr = 0;
    else if(*flag == 1 && *hr == 0) *hr = 12;
  }
  if(alarmEnabled) triggerAlarm();
}

void adjustTime() {
  int *timeVariable;
  int cursorX, cursorY;
  display.clear();
  
  int changeFocus = 1; // 1=hour, 2=minute, 3=second
  while(true) 
  {
    display.setCursor(0, 0);
    display.print("Adjust Time");
    display.setCursor(0, 1);
    if(currentHour < 10) display.print("0");
    display.print(currentHour);
    display.print(":");
    if(currentMinute < 10) display.print("0");
    display.print(currentMinute);
    display.print(":");
    if(currentSecond < 10) display.print("0");
    display.print(currentSecond);

    if(AM_PM_Flag == 0) display.print(" AM");
    if(AM_PM_Flag == 1) display.print(" PM");
    
    buttonState1 = digitalRead(BUTTON_1);
    buttonState2 = digitalRead(BUTTON_2);
    buttonState3 = digitalRead(BUTTON_3);
    buttonState4 = digitalRead(BUTTON_4);

    if(buttonState1 == 1){
      currentMode = 0;
      delay(200);
      break;
    }
    if(buttonState3 == 1){
      if(changeFocus == 3) changeFocus = 0;
      changeFocus += 1;
      delay(200);
    }
    if(changeFocus == 1){
      timeVariable = &currentHour;
      cursorX = 0;
      cursorY = 1;
     
      if(digitalRead(BUTTON_2) == 1){
        currentHour += 1;
        if(currentHour == 13) currentHour = 1;
        else if(currentHour == 12 && AM_PM_Flag == 0) currentHour = 0;
        delay(200);
      }
    }
    if(changeFocus == 2){
      timeVariable = &currentMinute;
      cursorX = 3;
      cursorY = 1;
     
      if(digitalRead(BUTTON_2) == 1){
        currentMinute += 1;
        if(currentMinute == 60) currentMinute = 0;
        delay(200);
      }
    }
    if(changeFocus == 3){
      timeVariable = &currentSecond;
      cursorX = 6;
      cursorY = 1;
     
      if(digitalRead(BUTTON_2) == 1){
        currentSecond += 1;
        if(currentSecond == 60) currentSecond = 0;
        delay(200);
      }
    }
    if(buttonState4 == 1){
      AM_PM_Flag = AM_PM_Flag == 0 ? 1 : 0;
      if(AM_PM_Flag == 0 && currentHour == 12) currentHour = 0;
      else if(AM_PM_Flag == 1 && currentHour == 0) currentHour = 12;
      delay(200);
    }
    blinkCursor(*timeVariable, cursorX, cursorY);
    updateClock(&currentHour, &currentMinute, &currentSecond, &currentTime, &lastUpdateTime, &millisCount, &AM_PM_Flag);
  }
}

void configureAlarm() {
  int *alarmVariable;
  int cursorX, cursorY;
  display.clear();
  
  int changeFocus = 1; // 1=hour, 2=minute, 3=second
  while(true) 
  {
    display.setCursor(0, 0);
    display.print("Alarm Setup");
    display.setCursor(0, 1);
    if(alarmHour < 10) display.print("0");
    display.print(alarmHour);
    display.print(":");
    if(alarmMinute < 10) display.print("0");
    display.print(alarmMinute);
    display.print(":");
    if(alarmSecond < 10) display.print("0");
    display.print(alarmSecond);

    if(Alarm_AM_PM_Flag == 0) display.print(" AM");
    if(Alarm_AM_PM_Flag == 1) display.print(" PM");
    
    buttonState1 = digitalRead(BUTTON_1);
    buttonState2 = digitalRead(BUTTON_2);
    buttonState3 = digitalRead(BUTTON_3);
    buttonState4 = digitalRead(BUTTON_4);

    if(buttonState1 == 1){
      currentMode = 0;
      delay(200);
      break;
    }
    if(buttonState3 == 1){
      if(changeFocus == 3) changeFocus = 0;
      changeFocus += 1;
      delay(200);
    }
    if(changeFocus == 1){
      alarmVariable = &alarmHour;
      cursorX = 0;
      cursorY = 1;
     
      if(digitalRead(BUTTON_2) == 1){
        alarmHour += 1;
        if(alarmHour == 13) alarmHour = 1;
        else if(alarmHour == 12 && Alarm_AM_PM_Flag == 0) alarmHour = 0;
        delay(200);
      }
    }
    if(changeFocus == 2){
      alarmVariable = &alarmMinute;
      cursorX = 3;
      cursorY = 1;
     
      if(digitalRead(BUTTON_2) == 1){
        alarmMinute += 1;
        if(alarmMinute == 60) alarmMinute = 0;
        delay(200);
      }
    }
    if(changeFocus == 3){
      alarmVariable = &alarmSecond;
      cursorX = 6;
      cursorY = 1;
     
      if(digitalRead(BUTTON_2) == 1){
        alarmSecond += 1;
        if(alarmSecond == 60) alarmSecond = 0;
        delay(200);
      }
    }
    if(buttonState4 == 1){
      Alarm_AM_PM_Flag = Alarm_AM_PM_Flag == 0 ? 1 : 0;
      if(Alarm_AM_PM_Flag == 0 && alarmHour == 12) alarmHour = 0;
      else if(Alarm_AM_PM_Flag == 1 && alarmHour == 0) alarmHour = 12;
      delay(200);
    }
    blinkCursor(*alarmVariable, cursorX, cursorY);
    updateClock(&currentHour, &currentMinute, &currentSecond, &currentTime, &lastUpdateTime, &millisCount, &AM_PM_Flag);
  }
}

void configureTimer() {
  // Timer setup logic here
}

void startStopwatch() {
  // Stopwatch logic here
}

void blinkCursor(int timeVariable, int cursorX, int cursorY) {
  display.setCursor(cursorX, cursorY);
  if(blinkState == 0){
    if(timeVariable < 10) display.print("0");
    display.print(timeVariable);
    blinkState = 1;
  }
  else{
    display.print("  ");
    blinkState = 0;
  }
  delay(300);
}

void triggerAlarm() {
  if(currentHour == alarmHour && currentMinute == alarmMinute && currentSecond == alarmSecond && AM_PM_Flag == Alarm_AM_PM_Flag){
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
}
