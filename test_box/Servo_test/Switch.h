const int switch1 = 34;
const int switch2 = 35;
const int switch3 = 32;
const int switch4 = 33;
const int switch5 = 25;
const int switch6 = 26;
const int switch7 = 27;
const int switch8 = 14;
const int switch9 = 12;
const int switch10 = 0;
const int switch11 = 13;
const int switch12 = 2;
int switchPin[] = { switch1, switch2, switch3, switch4, switch5,
                    switch6, switch7, switch8, switch9, switch10,
                    switch11, switch12 };
int switch_interval = 10;

void Swtich_setup() {
  //GPIO 34, 35はINPUT_PULLUPが使えないのでプルアップ抵抗を入れる
  pinMode(switchPin[0], INPUT);
  pinMode(switchPin[1], INPUT);
  for (int i = 2; i < 12; i++) {
    pinMode(switchPin[i], INPUT_PULLUP);
  }
}

void Switch_judge() {
  for (int i = 0; i < 12; i++) {
    if (i % 2 == 0) {
      if (digitalRead(switchPin[i]) == 0) {
        pos[i/2] += 1;
        delay(switch_interval);
      }
    } else {
      if (digitalRead(switchPin[i]) == 0) {
        pos[i/2] -= 1;
        delay(switch_interval);
      }
    }
  }
}

void Switch_debug() {
  for (int i = 0; i < 12; i++) {
    Serial.print(digitalRead(switchPin[i]));
  }
  Serial.println();
}