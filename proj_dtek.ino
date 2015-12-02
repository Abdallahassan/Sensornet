int ref = 0; // specifies which value to update.
int pin_offset = 10; // The first pin that sends signal to CU.
int datas[4] = {0,0,0,0}; // The 4 values that are displayed on CU.
// 0: tmp  1: photo  2: #movements  3: time_movement

void setup() {
  for (int i = pin_offset; i <= pin_offset+3; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  delay(5000);
  send_data((int)((((analogRead(A0)/1024.0)*5.0) - 0.5) * 100), 0);
  send_data((analogRead(A1)/100), 1);
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
