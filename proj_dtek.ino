int ref = 0; // specifies which value to update.
int pin_offset = 10; // The first pin that sends signal to CU.
int datas[4] = {0,0,0,0}; // The 4 values that are displayed on CU.
// 0: tmp  1: photo  2: #movements  3: time_movement
bool isMotion = false;
int pirPin = 2;
int mov_no = 0;
int mov_av = 0;

int mov_st = 0;
int mov_en = 0;
int mov_to = 0;

//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        


//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

void checkmotion() {
  
  Serial.println("hej");
       if(digitalRead(pirPin) == HIGH){
  
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         isMotion = true;
         mov_no++;
         mov_st = millis()/1000;
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           isMotion = false;
           
           mov_to += (millis() - pause)/1000 - mov_st;
           delay(50);
           }
       }
}


void setup() {
  Serial.begin(9600);
  for (int i = pin_offset; i <= pin_offset+3; i++) {
    pinMode(i, OUTPUT);
  }
  configurePIRSensor();
}

void loop() {
    checkmotion();
  //send_data((int)((((analogRead(A0)/1024.0)*5.0) - 0.5) * 100), 0);
  //send_data((analogRead(A1)/100), 1);
  send_data(mov_no,2);
  send_data(mov_to,3);
}

void send_signal(int val) {
  digitalWrite(pin_offset+2, ref & 1);
  digitalWrite(pin_offset+3, (ref >> 1) & 1);
  if (val == 1) {
    digitalWrite(pin_offset+1, 1);
    digitalWrite(pin_offset, 0);
  } else if (val == -1) {
    digitalWrite(pin_offset+1, 0);
    digitalWrite(pin_offset, 1);
  } else {
    digitalWrite(pin_offset+1, 0);
    digitalWrite(pin_offset, 0);
  }
}

void end_transmission() {
  digitalWrite(pin_offset+1, 1);
  digitalWrite(pin_offset, 1);
  delay(120);
  send_signal(0);
  delay(120);
}

void updaten(int val) {
  send_signal(val);
  delay(120);
  send_signal(0);
  delay(120);
}

void update_number(int newdata) {
  int dif = newdata - datas[ref];
  if (dif > 0) {
    for (int i = 0; i < dif; i++) {
      updaten(1);
    }
  } else if(dif < 0) {
    for (int i = 0; i < -dif; i++) {
      updaten(-1);
    }
  }
  datas[ref] += dif;
}

// Use this to send info to the Central Unit (ChipKIT).
void send_data(int data, int sensor) {
  ref = sensor;
  update_number(data);
  end_transmission();
}

void test() {
  send_data(0, 0);
  send_data(4, 0);
  send_data(5, 0);
  send_data(6, 0);
  send_data(12, 0);
  
  send_data(0, 1);
  send_data(6, 1);
  
  send_data(3, 2);
  send_data(12, 3);
}

void configurePIRSensor() {
  pinMode(pirPin, INPUT);

  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
    }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

