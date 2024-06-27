#define DEBUG 1  // coloque "DEBUG 1" para depuração

/////////////////////////////////////////////
// Você está usando botões?
#define USING_BUTTONS 1  // comente se não estiver usando botões

/////////////////////////////////////////////
// Você está usando potenciômetros?
#define USING_POTENTIOMETERS 1  // comente se não estiver usando potenciômetros

/////////////////////////////////////////////
// BIBLIOTECAS
// -- Define a biblioteca MIDI -- //
#include <MIDI.h>  // por Francois Best
MIDI_CREATE_DEFAULT_INSTANCE();

#ifdef USING_POTENTIOMETERS
// incluir a biblioteca ResponsiveAnalogRead
#include <ResponsiveAnalogRead.h>  // [https://github.com/dxinteractive/ResponsiveAnalogRead](https://github.com/dxinteractive/ResponsiveAnalogRead)

#endif

/////////////////////////////////////////////
// BOTÕES
#ifdef USING_BUTTONS

const int N_BUTTONS = 5;                                // número total de botões
const int BUTTON_ARDUINO_PIN[N_BUTTONS] = { 1, 2, 3, 4, 5 };  // pinos de cada botão conectado diretamente ao Arduino
const int LED_PIN[N_BUTTONS - 1] = { 13, 12, 11, 10 };  // pinos de LEDs correspondentes aos botões 1 a 4

int buttonCState[N_BUTTONS] = {};  // armazena o valor atual do botão
int buttonPState[N_BUTTONS] = {};  // armazena o valor anterior do botão

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = { 0 };  // a última vez que o pino de saída foi alternado
unsigned long debounceDelay = 50;                   // o tempo de debounce; aumente se a saída oscilar

#endif

/////////////////////////////////////////////
// POTENCIÔMETROS
#ifdef USING_POTENTIOMETERS

const int N_POTS = 1;                            // número total de potenciômetros
const int POT_ARDUINO_PIN[N_POTS] = { A0 };      // pino do potenciômetro conectado diretamente ao Arduino

int potCState[N_POTS] = { 0 };  // estado atual do potenciômetro
int potPState[N_POTS] = { 0 };  // estado anterior do potenciômetro
int potVar = 0;                 // diferença entre o estado atual e anterior do potenciômetro

int midiCState[N_POTS] = { 0 };  // estado atual do valor midi
int midiPState[N_POTS] = { 0 };  // estado anterior do valor midi

const int TIMEOUT = 300;              // Tempo em que o potenciômetro será lido após exceder o varThreshold
const int varThreshold = 20;          // Limiar para a variação do sinal do potenciômetro
boolean potMoving = true;             // Se o potenciômetro está se movendo
unsigned long PTime[N_POTS] = { 0 };  // Tempo previamente armazenado
unsigned long timer[N_POTS] = { 0 };  // Armazena o tempo passado desde que o temporizador foi redefinido

int reading = 0;
// Leitura Responsiva Analógica
float snapMultiplier = 0.01;                      // (0.0 - 1.0) - Aumente para uma leitura mais rápida, mas menos suave
ResponsiveAnalogRead responsivePot[N_POTS] = {};  // cria um array para os potenciômetros responsivos. Ele é preenchido no Setup.

int potMin = 10;
int potMax = 1023;

#endif

/////////////////////////////////////////////
// MIDI
byte midiCh = 1;  // Canal MIDI a ser usado - comece com 1 para a biblioteca MIDI.h
byte note = 36;   // Nota mais baixa a ser usada
byte cc = 1;      // Menor CC MIDI a ser usado


/////////////////////////////////////////////
// CONFIGURAÇÃO
void setup() {

  // Taxa de baud
  // 115200 para Hairless MIDI
  Serial.begin(115200);  //

#ifdef DEBUG
  Serial.println("Modo de depuração");
  Serial.println();
#endif

#ifdef USING_BUTTONS
  // Botões
  // Inicializa os botões com resistores de pull up
  for (int i = 0; i < N_BUTTONS; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }

  // Inicializa os pinos dos LEDs como saída
  for (int i = 0; i < N_BUTTONS - 1; i++) {
    pinMode(LED_PIN[i], OUTPUT);
  }

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

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {

          // Envie a nota MIDI ON
          MIDI.sendNoteOn(note + i, 127, midiCh);  // nota, velocidade, canal

#ifdef DEBUG
          Serial.print(i);
          Serial.println(": botão ligado");
#endif

          // Acende o LED correspondente, se houver
          if (i < N_BUTTONS - 1) {
            digitalWrite(LED_PIN[i], HIGH);
          }

        } else {

          // Envie a nota MIDI OFF
          MIDI.sendNoteOn(note + i, 0, midiCh);  // nota, velocidade, canal

#ifdef DEBUG
          Serial.print(i);
          Serial.println(": botão desligado");
#endif

          // Apaga o LED correspondente, se houver
          if (i < N_BUTTONS - 1) {
            digitalWrite(LED_PIN[i], LOW);
          }
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

    midiCState[i] = map(potCState[i], potMin, potMax, 0, 127);  // Mapeia a leitura de potCState para um valor utilizável em midi

    if (midiCState[i] < 0) {
      midiCState[i] = 0;
    }
    if (midiCState[i] > 127) {
      midiCState[i] = 0;
    }

    potVar = abs(potCState[i] - potPState[i]);  // Calcula o valor absoluto entre a diferença entre o estado atual e anterior do potenciômetro

    if (potVar > varThreshold) {  // Abre o portão se a variação do potenciômetro for maior que o limiar
      PTime[i] = millis();        // Armazena o tempo anterior
    }

    timer[i] = millis() - PTime[i];  // Redefine o temporizador

    if (timer[i] < TIMEOUT) {  // Se o temporizador for menor que o tempo máximo permitido, significa que o potenciômetro ainda está se movendo
      potMoving = true;
    } else {
      potMoving = false;
    }

    if (potMoving == true) {  // Se o potenciômetro ainda estiver se movendo, envie a mudança de controle
      if (midiPState[i] != midiCState[i]) {

        // Envie o CC MIDI
        MIDI.sendControlChange(cc + i, midiCState[i], midiCh);  // número do cc, valor do cc, canal midi

#ifdef DEBUG
        Serial.print("Potenciômetro: ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(midiCState[i]);
#endif

        potPState[i] = potCState[i];  // Armazena a leitura atual do potenciômetro para comparar com a próxima
        midiPState[i] = midiCState[i];
      }
    }
  }
}

#endif
