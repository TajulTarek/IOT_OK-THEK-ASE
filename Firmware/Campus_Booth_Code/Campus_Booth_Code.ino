
#define led1 15
#define led2 2
#define led3 4

#define led1_ 13
#define led2_ 12
#define led3_ 14

#define button1 16
#define button2 17
#define button3 5

#define ldr1 27   // LDR PIN
#define ldr2 26
#define ldr3 19

#include<NewPing.h>
#define tr1 25
#define ec1 25

#define tr2 33
#define ec2 33

#define tr3 32
#define ec3 32
NewPing sonar1(tr1, ec1, 1000);
NewPing sonar2(tr2, ec2, 1000);
NewPing sonar3(tr3, ec3, 1000);



// =====================================================================
// GLOBAL VARIABLES
// =====================================================================
unsigned long t1 = 0;
unsigned long t2 = 0;

unsigned long lastSampleTime = 0;
int crossingCount1 = 0;
int crossingCount2 = 0;
int crossingCount3 = 0;
int prevValue1 = 0;
int prevValue2 = 0;
int prevValue3 = 0;
int flag = 0;

// =====================================================================
// SETUP
// =====================================================================
void setup() {
  Serial.begin(115200);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  pinMode(led1_, OUTPUT);
  pinMode(led2_, OUTPUT);
  pinMode(led3_, OUTPUT);

  digitalWrite(led1_, HIGH);
  digitalWrite(led2_, HIGH);
  digitalWrite(led3_, HIGH);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);

}

// =====================================================================
// DISTANCE MEASUREMENT
// =====================================================================
void distance_measure() {

  unsigned int d1 = sonar1.ping_cm();
  unsigned int d2 = sonar2.ping_cm();
  unsigned int d3 = sonar3.ping_cm();

  if (d1 == 0) d1 = 9999;
  if (d2 == 0) d2 = 9999;
  if (d3 == 0) d3 = 9999;


  if (d1 <= 9 || d2 <= 9 || d3 <= 9) 
  {
    if (flag == 0)
    {
      flag=1;
      t2 = millis();
    }
    if(millis()-t2 >= 3500)
    {
      Serial.println("Trigger Activated");
      delay(1000);
      flag=0;
      t1 = millis();
      privilege_verification();
    }
  }
  if(d1 >=10  && d2 >=10 && d3>=10)
  {
    t2 = 0;
    flag = 0;
  }

  Serial.print("D1: "); Serial.print(d1);
  Serial.print("  D2: "); Serial.print(d2);
  Serial.print("  D3: "); Serial.print(d3);
  Serial.println(" cm");

  delay(30);
}

// =====================================================================
// LDR FREQUENCY CHECK
// =====================================================================
void privilege_verification() {
  int flg=0;
  while(millis() - t1 <= 20000 && flg==0)
  {

  int sensorValue1 = analogRead(ldr1);
  int sensorValue2 = analogRead(ldr2);
  int sensorValue3 = analogRead(ldr3);

  Serial.print("LDR Value1: ");
  Serial.print(sensorValue1);
  Serial.print("\tLDR Value2: ");
  Serial.print(sensorValue2);
  Serial.print("\tLDR Value3: ");
  Serial.println(sensorValue3);

  // Detect rising edge of laser pulses
  if (sensorValue1 > 2000 && prevValue1 <= 1000) {
    crossingCount1++;
  }

  if (sensorValue2 > 2000 && prevValue2 <= 1000) {
    crossingCount2++;
  }

  if (sensorValue3 > 2000 && prevValue3 <= 1000) {
    crossingCount3++;
  }



  prevValue1 = sensorValue1;
  prevValue2 = sensorValue2;
  prevValue3 = sensorValue3;

  // Measure frequency every 1 second
  if (millis() - lastSampleTime >= 1000) {
    float freq1 = crossingCount1; // each cycle = 2 edges
    float freq2 = crossingCount2;
    float freq3 = crossingCount3;

    if (freq1 >= 2.5 && freq1 <= 3.5) {
      Serial.println("✔ CORRECT FREQUENCY (3 Hz)");
      conformation1();
      flg=1;
      delay(2000);
    } else {
      Serial.println("✘ WRONG FREQUENCY");
    }

      if (freq2 >= 2.5 && freq3 <= 3.5) {
      Serial.println("✔ CORRECT FREQUENCY (3 Hz)");
      conformation2();
      flg=1;
      delay(2000);
    } else {
      Serial.println("✘ WRONG FREQUENCY");
    }

      if (freq3 >= 2.5 && freq3 <= 3.5) {
      Serial.println("✔ CORRECT FREQUENCY (3 Hz)");
      conformation3();
      flg=1;
      delay(2000);
    } else {
      Serial.println("✘ WRONG FREQUENCY");
    }

    crossingCount1 = 0;
    crossingCount2 = 0;
    crossingCount3 = 0;
    lastSampleTime = millis();
  }
  }
}

void conformation1()
{
  Serial.println("press the button for 3 second.");
  delay(3000);
  if(digitalRead(button1) == LOW)
  {
    Serial.println("request sent");
    digitalWrite(led2,HIGH);
    digitalWrite(led1_,LOW);
    delay(6000);
    digitalWrite(led2,LOW);
    digitalWrite(led1_,HIGH);
  }
}

void conformation2()
{
  Serial.println("press the button for 3 second.");
  delay(3000);
  if(digitalRead(button2) == LOW)
  {
    Serial.println("request sent");
    digitalWrite(led2,HIGH);
    digitalWrite(led2_,LOW);
    delay(5000);
    digitalWrite(led2,LOW);
    digitalWrite(led2_,HIGH);
  }
}

void conformation3()
{
  Serial.println("press the button for 3 second.");
  delay(3000);
  if(digitalRead(button3) == LOW)
  {
    Serial.println("request sent");
    digitalWrite(led2,HIGH);
    digitalWrite(led3_,LOW);
    delay(5000);
    digitalWrite(led2,LOW);
    digitalWrite(led3_,HIGH);
  }
}


// =====================================================================
// LOOP
// =====================================================================
void loop() {
  distance_measure();
}
