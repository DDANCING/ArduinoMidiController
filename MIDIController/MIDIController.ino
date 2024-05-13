#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define ATMEGA328 1 

#define USING_BUTTONS 8
#define USING_POTENTIOMETERS 2  

#ifdef Uno
#include <MIDI.h> 
//MIDI_CREATE_DEFAULT_INSTANCE();

#elif R3
#include "MIDIUSB.h"
#endif

#ifdef USING_POTENTIOMETERS
// incluir a biblioteca ResponsiveAnalogRead
#include <ResponsiveAnalogRead.h>  // [https://github.com/dxinteractive/ResponsiveAnalogRead](https://github.com/dxinteractive/ResponsiveAnalogRead)
#endif

// BOTÕES
#ifdef USING_BUTTONS

const int N_BUTTONS = 8;                                // número total de botões
const int BUTTON_ARDUINO_PIN[8] = {1, 2, 3, 4, 5, 6, 7, 8};  // pinos de cada botão conectados diretamente ao Arduino

int buttonCState[N_BUTTONS] = {};  // armazena o valor atual do botão
int buttonPState[N_BUTTONS] = {};  // armazena o valor anterior do botão

//#define pin13 1 // descomente se estiver usando o pino 13 (pino com LED), ou comente a linha se não estiver usando
byte pin13index = 12;  // coloque o índice do pino 13 do array buttonPin[] se estiver usando, caso contrário, comente

// debounce (eliminação de ruído)
unsigned long lastDebounceTime[N_BUTTONS] = { 0 };  // a última vez que o pino de saída foi alternado
unsigned long debounceDelay = 50;                   // tempo de debounce; aumente se a saída oscilar

#endif

/////////////////////////////////////////////
// POTENCIÔMETROS
#ifdef USING_POTENTIOMETERS

const int N_POTS = 2;                            // número total de potenciômetros (deslizante e rotativo)
const int POT_ARDUINO_PIN[2] = {00, 01};  // pinos de cada potenciômetro conectados diretamente ao Arduino

int potCState[N_POTS] = { 0 };  // Estado atual do potenciômetro
int potPState[N_POTS] = { 0 };  // Estado anterior do potenciômetro
int potVar = 0;                 // Diferença entre o estado atual e o anterior do potenciômetro

int midiCState[N_POTS] = { 0 };  // Estado atual do valor MIDI
int midiPState[N_POTS] = { 0 };  // Estado anterior do valor MIDI

const int TIMEOUT = 300;              // Tempo que o potenciômetro será lido após exceder o varThreshold
const int varThreshold = 20;          // Limite para a variação do sinal do potenciômetro
boolean potMoving = true;             // Se o potenciômetro está se movendo
unsigned long PTime[N_POTS] = { 0 };  // Tempo armazenado anteriormente
unsigned long timer[N_POTS] = { 0 };  // Armazena o tempo que passou desde que o timer foi resetado

int reading = 0;
// Leitura Analógica Responsiva
float snapMultiplier = 0.01;                      // (0.0 - 1.0) - Aumente para leitura mais rápida, mas menos suave
ResponsiveAnalogRead responsivePot[N_POTS] = {};  // cria um array para os potenciômetros responsivos. É preenchido no Setup.

int potMin = 10;
int potMax = 1023;

#endif

/////////////////////////////////////////////
// MIDI
byte midiCh = 0;  // Canal MIDI a ser usado - comece com 1 para a biblioteca MIDI.h ou 0 para a biblioteca MIDIUSB
byte note = 36;   // Nota mais baixa a ser usada
byte cc = 1;      // CC MIDI mais baixo a ser usado

/////////////////////////////////////////////
// SETUP
void setup() {

  // Taxa de Baud
  // use se estiver usando com ATmega328 (uno, mega, nano...)
  // 31250 para MIDI class compliant | 115200 para Hairless MIDI
  Serial.begin(115200);  //

#ifdef DEBUG
  Serial.println("Modo de depuração");
  Serial.println();
#endif

#ifdef USING_BUTTONS
  // Botões
  // Inicializa botões com resistores pull-up
  for (int i = 0; i < N_BUTTONS; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }

#ifdef pin13  // inicializa o pino 13 como entrada
  pinMode(BUTTON_ARDUINO_PIN[pin13index], INPUT);
#endif

#endif

#ifdef USING_POTENTIOMETERS
  for (int i = 0; i < N_POTS; i++) {
    responsivePot[i] = ResponsiveAnalogRead(0, true, snapMultiplier);
    responsivePot[i].setAnalogResolution(1023);  // define a resolução
  }
#endif
}

/////////////////////////////////////////////
// LOOP
void loop() {

#ifdef USING_BUTTONS
  buttons();
#endif

#ifdef USING_POTENTIOMETERS
  potentiometers();
#endif
}

/////////////////////////////////////////////
// BOTÕES
#ifdef USING_BUTTONS

void buttons() {

  for (int i = 0; i < N_BUTTONS; i++) {

    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);  // lê os pinos do arduino

#ifdef pin13
    if (i == pin13index) {
      buttonCState[i] = !buttonCState[i];  // inverte o pino 13 porque ele tem um resistor pull-down em vez de pull-up
    }
#endif

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {

          // Envia o comando MIDI note ON de acordo com a placa escolhida
#ifdef ATMEGA328
          // use se estiver usando com ATmega328 (uno, mega, nano...)
          MIDI.sendNoteOn(note + i, 127, midiCh);  // nota, velocidade, canal

#elif ATMEGA32U4
          // use se estiver usando com ATmega32U4 (micro, pro micro, leonardo...)
          noteOn(midiCh, note + i, 127);  // canal, nota, velocidade
          MidiUSB.flush();

#elif TEENSY
          // use usbMIDI.sendNoteOn se estiver usando com Teensy
          usbMIDI.sendNoteOn(note + i, 127, midiCh);  // nota, velocidade, canal

#elif DEBUG
          Serial.print(i);
          Serial.println(": botão pressionado");
#endif

        } else {

          // Envia o comando MIDI note OFF de acordo com a placa escolhida
#ifdef ATMEGA328
          // use se estiver usando com ATmega328 (uno, mega, nano...)
          MIDI.sendNoteOn(note + i, 0, midiCh);  // nota, velocidade, canal

#elif ATMEGA32U4
          // use se estiver usando com ATmega32U4 (micro, pro micro, leonardo...)
          noteOn(midiCh, note + i, 0);  // canal, nota, velocidade
          MidiUSB.flush();

#elif TEENSY
          // use usbMIDI.sendNoteOn se estiver usando com Teensy
          usbMIDI.sendNoteOn(note + i, 0, midiCh);  // nota, velocidade, canal

#elif DEBUG
          Serial.print(i);
          Serial.println(": botão liberado");
#endif
        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

#endif

/////////////////////////////////////////////
// POTENCIÔMETROS
#ifdef USING_POTENTIOMETERS

void potentiometers() {

  for (int i = 0; i < N_POTS; i++) {  // Percorre todos os potenciômetros

    reading = analogRead(POT_ARDUINO_PIN[i]);
    responsivePot[i].update(reading);
    potCState[i] = responsivePot[i].getValue();

    potCState[i] = analogRead(POT_ARDUINO_PIN[i]);  // lê os pinos do arduino

    midiCState[i] = map(potCState[i], potMin, potMax, 0, 127);  // Mapeia a leitura do potCState para um valor utilizável no MIDI
    //midiCState[i] = map(potCState[i], 0, 4096, 0, 127);  // Mapeia a leitura do potCState para um valor utilizável no MIDI - use para ESP32

    if (midiCState[i] < 0) {
      midiCState[i] = 0;
    }
    if (midiCState[i] > 127) {
      midiCState[i] = 0;
    }

    potVar = abs(potCState[i] - potPState[i]);  // Calcula o valor absoluto entre a diferença do estado atual e anterior do potenciômetro
    //Serial.println(potVar);

    if (potVar > varThreshold) {  // Abre o gate se a variação do potenciômetro for maior que o limite
      PTime[i] = millis();        // Armazena o tempo anterior
    }

    timer[i] = millis() - PTime[i];  // Reseta o timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) {  // Se o timer for menor que o tempo máximo permitido, significa que o potenciômetro ainda está se movendo
      potMoving = true;
    } else {
      potMoving = false;
    }

    if (potMoving == true) {  // Se o potenciômetro ainda estiver se movendo, envia o change control
      if (midiPState[i] != midiCState[i]) {

        // Envia o comando MIDI CC de acordo com a placa escolhida
#ifdef ATMEGA328
        // use se estiver usando com ATmega328 (uno, mega, nano...)
        MIDI.sendControlChange(cc + i, midiCState[i], midiCh);  // número do CC, valor do CC, canal MIDI

#elif ATMEGA32U4
        // use se estiver usando com ATmega32U4 (micro, pro micro, leonardo...)
        controlChange(midiCh, cc + i, midiCState[i]);  // (canal, número do CC, valor do CC)
        MidiUSB.flush();

#elif TEENSY
        // use usbMIDI.sendControlChange se estiver usando com Teensy
        usbMIDI.sendControlChange(cc + i, midiCState[i], midiCh);  // número do CC, valor do CC, canal MIDI

#elif DEBUG
        Serial.print("Pot: ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(midiCState[i]);
//Serial.print("  ");
#endif

        potPState[i] = potCState[i];  // Armazena a leitura atual do potenciômetro para comparar com a próxima
        midiPState[i] = midiCState[i];
      }
    }
  }
}

#endif

#ifdef ATMEGA32U4

// Funções MIDI do Arduino (pro)micro Biblioteca MIDIUSB
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = { 0x08, 0x80 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}
#endif
