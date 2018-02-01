const int interruttore = 7;
int buttonState = 0;
int lastButtonState = 0;
const int sc1 = 4;
const int sc2 = 2;
int scambio = 4;
const int rosso = 12;
const int verde = 8;
boolean last = true;

void setup() {
pinMode(LED_BUILTIN, OUTPUT);
pinMode(rosso, OUTPUT);
pinMode(verde, OUTPUT);
pinMode(sc1, OUTPUT);
pinMode(sc2, OUTPUT);
pinMode(interruttore, INPUT);
Serial.begin(9600);
digitalWrite(verde, HIGH);
digitalWrite(rosso, LOW);
digitalWrite(scambio, HIGH);
delay(50);
digitalWrite(scambio, LOW);
}

void loop() {
  
  buttonState = digitalRead(interruttore);
  if (buttonState != lastButtonState && buttonState == 1) {
    Serial.println("Premuto tasto!!");
    last = !last;
    commuta(last);
  }
  lastButtonState = buttonState;

}

void commuta(boolean stato){
if(stato){
  scambio = sc1;
}else{
  scambio = sc2;
  }
digitalWrite(rosso, !digitalRead(rosso));
digitalWrite(verde, !digitalRead(verde));
digitalWrite(scambio, HIGH);
delay(50);
digitalWrite(scambio, LOW);

}
