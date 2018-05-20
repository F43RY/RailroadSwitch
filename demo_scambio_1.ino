#include <Keypad.h>
//#include <VirtualWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_PWMServoDriver.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x42);

#define TX_pin 33
const char *msg_ON = "LED_ON";
const char *msg_OFF = "LED_OFF";

#define SERVOMIN  130 // this is the 'minimum' pulse length count (out of 4096) 122
#define SERVOMAX  550 // this is the 'maximum' pulse length count (out of 4096) 560
#define FREQ 50
#define OE 11

// our servo # counter
int const NUM_SCAMBI = 24;

/*
     Definisce i gradi di rotazione per ogni passo
*/
int STEP = 1;
int DELAY = 20;


/*
   Il vettore di 8 descrive nel seguente ordine:
    0: SERVOMIN
    1: SERVOMAX
    2: ANGOLOMIN
    3: ANGOLOMAX
    4: POSIZIONE
    5: LOCK (0: in attesa commutazione, 1: in commutazione))
    6: DIREZIONE (1: aumenta, 2 diminuisce)
    7: MUOVITI (0: fermo, 1: ricevuto comando per rotazione)
    8: semaforo
*/
typedef struct {
  int servomin;
  int servomax;
  int angoloMin;
  int angoloMax;
  int posizione;
  boolean isLock;
  boolean isOrario;//direzione;
  boolean inAzione;
} exchange;

exchange  m0 = {130, 550, 40, 90, 40, false, false, false},
          m1 = {130, 550, 70, 110, 70, false, false, false},
          m2 = {130, 550, 45, 90, 45, false, false, false},
          m3 = {130, 550, 75, 137, 75, false, false, false},
          m4 = {130, 550, 10, 80, 10, false, false, false},
          m5 = {130, 550, 20, 80, 20, false, false, false},
          m6 = {130, 550, 40, 90, 40, false, false, false},
          m7 = {130, 550, 40, 90, 40, false, false, false},
          m8 = {130, 550, 30, 60, 30, false, false, false},
          m9 = {130, 550, 40, 90, 40, false, false, false},
          m10 = {130, 550, 40, 90, 40, false, false, false},
          m11 = {130, 550, 40, 90, 40, false, false, false},
          m12 = {130, 550, 40, 90, 40, false, false, false},
          m13 = {130, 550, 40, 90, 40, false, false, false},
          m14 = {130, 550, 40, 90, 40, false, false, false},
          m15 = {130, 550, 40, 90, 40, false, false, false},
          m16 = {130, 550, 40, 90, 40, false, false, false},
          m17 = {130, 550, 40, 90, 40, false, false, false},
          m18 = {130, 550, 40, 90, 40, false, false, false},
          m19 = {130, 550, 40, 90, 40, false, false, false},
          m20 = {130, 550, 40, 90, 40, false, false, false},
          m21 = {130, 550, 40, 90, 40, false, false, false},
          m22 = {130, 550, 40, 90, 40, false, false, false},
          m23 = {130, 550, 40, 90, 40, false, false, false};


exchange* motori[] = {&m0, &m1, &m2, &m3, &m4, &m5, &m6, &m7, &m8, &m9, &m10, &m11, &m12, &m13, &m14, &m15, &m16, &m17, &m18, &m19, &m20, &m21, &m22, &m23};


const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {51, 49, 47, 45}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {43, 41, 39, 37}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad tastiera = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


String scambio = "";
String vuota = "                    ";
void setup() {
//  vw_set_tx_pin(TX_pin); // Imposto il pin per la trasmissione
//  vw_setup(3000);        // Bits per sec
  pinMode (OE, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  pwm0.begin();
  pwm0.setPWMFreq(FREQ);
  pwm1.begin();
  pwm1.setPWMFreq(FREQ);
  for (uint8_t x = 0; x < NUM_SCAMBI; x++) {
    pwm0.setPWM(x, 0, map(motori[x]->posizione, 0, 180, motori[x]->servomin, motori[x]->servomax));
    pwm1.setPWM(x, 0, map(motori[x + 16]->posizione, 0, 180, motori[x + 16]->servomin, motori[x + 16]->servomax));
  }
  digitalWrite(OE, HIGH);
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.clear();
  Serial.begin(9600);
  defaultScreen();
}

void defaultScreen(){
  clearAll();
  scambio="";
  lcd.setCursor(0, 0);
  lcd.print("** Scelta scambio **");
  
}

void clearAll() {
  clearRow(1);
  clearRow(2);
  clearRow(3);
}

void clearRow(int riga) {
  lcd.setCursor(0, riga);
  lcd.print(vuota);
}

boolean speedSet = false;

void loop() {
  char carattere = tastiera.getKey();
  if (carattere != NO_KEY) {
    if (carattere == 'D') {
      clearAll();
      scambio = "";
    } else if (carattere == 'A') {
      lcd.setCursor(0, 0);
      lcd.print("** Imp.  velocita **");
      lcd.setCursor(0, 3);
      lcd.print("Imp. val. da 0 a 200");
      speedSet = true;
    }

    else if (carattere == '*') {
      clearRow(3);
      lcd.setCursor(0, 3);
      if (speedSet) {
        
        DELAY = scambio.toInt();
        speedSet = false;
        defaultScreen();
      } else {
        if (scambio.toInt() >= NUM_SCAMBI) {
          lcd.print("Scambio inesistente");
          scambio = "";
          clearRow(1);
        } else {
          motori[scambio.toInt()]->inAzione = 1;
//          sendSinottico(scambio.toInt());
          //scambi[scambio.toInt()][7] = 1;
          scambio = "";
          clearRow(1);
          //If Per testare semaforo. Da rimuovere a completamento
          if (scambio == 7) {
            digitalWrite(2, !digitalRead(2));
            digitalWrite(3, !digitalRead(3));
          }
        }
      }


    } else {
      lcd.setCursor(0, 1);
      scambio = scambio + String(carattere);
      lcd.print(scambio);
      lcd.setCursor(0, 3);
      lcd.print("Acquisizione scambio");

    }
  }
  for (int a = 0; a < NUM_SCAMBI; a++) {
    if (motori[a]->inAzione && !motori[a]->isLock) {
      motori[a]->inAzione = false;
      Serial.println("Letto valore comando azione su motore " + String(a));
      motori[a]->isLock = true;
      digitalWrite(OE, LOW);
      if (motori[a]->posizione == motori[a]->angoloMin) {
        motori[a]->isOrario = false;
      } else if (motori[a]->posizione == motori[a]->angoloMax) {

        motori[a]->isOrario = true;
      }
    }
    if (motori[a]->isLock) {
      
      gira(a);
    }
  }
}

/*void sendSinottico(int scambio){
  Serial.println("commuto scambio " + (String)scambio);
  Serial.println("Posizione: " + (String)motori[scambio]->posizione);
  Serial.println("AngoloMax: " + (String)motori[scambio]->angoloMax);
      if(motori[scambio]->posizione == motori[scambio]->angoloMax){
        vw_send((uint8_t *)msg_ON, strlen(msg_ON));
      }else{
        vw_send((uint8_t *)msg_OFF, strlen(msg_OFF));
      }
      vw_wait_tx();
  
}*/

void gira(int scambio) {
  lcd.setCursor(0, 3);
  //if (motori[scambio]->direzione == 1) {
  if (!motori[scambio]->isOrario) {
    if (motori[scambio]->posizione < motori[scambio]->angoloMax) {
      //motori[scambio]->posizione = motori[scambio]->posizione + STEP;
      motori[scambio]->posizione += STEP;
      lcd.print("Azionamento sc " + String(scambio));
      if (scambio < 16 && scambio >= 0) {
        pwm0.setPWM(scambio, 0, map(motori[scambio]->posizione, 0, 180, motori[scambio]->servomin, motori[scambio]->servomax));
      } else if (scambio > 15 && scambio < 24) {
        pwm1.setPWM(scambio - 17, 0, map(motori[scambio]->posizione, 0, 180, motori[scambio]->servomin, motori[scambio]->servomax));
      }
    } else {
      motori[scambio]->isLock = false;
      //motori[scambio]->direzione = 2;
      motori[scambio]->isOrario = true;
      digitalWrite(OE, HIGH);
      clearRow(3);
    }
    //} else if (motori[scambio]->direzione == 2) {
  } else if (motori[scambio]->isOrario) {
    if (motori[scambio]->posizione > motori[scambio]->angoloMin) {
      motori[scambio]->posizione -= STEP;
      lcd.print("Azionamento sc " + String(scambio));
      if (scambio < 16 && scambio >= 0) {
        pwm0.setPWM(scambio, 0, map(motori[scambio]->posizione, 0, 180, motori[scambio]->servomin, motori[scambio]->servomax));
      } else if (scambio > 15 && scambio < 24) {
        pwm1.setPWM(scambio - 17, 0, map(motori[scambio]->posizione, 0, 180, motori[scambio]->servomin, motori[scambio]->servomax));
      }
    } else {
      motori[scambio]->isLock = false;
      //motori[scambio]->direzione = 1;
      motori[scambio]->isOrario = false;
      digitalWrite(OE, HIGH);
      clearRow(3);
    }
  }
  delay(DELAY);

}
