
/*
 * AUDIO SPECTRUM VISUALIZER
 * ETHAN WALKOWIAK
 * NOVEMBER 5, 2018
 *
 * https://urldefense.proofpoint.com/v2/url?u=https-3A__www.sparkfun.com_datasheets_Components_General_MSGEQ7.pdf&d=DwIFAg&c=5gKwI2ieO4OUatCz3W6kQyIxKlq99XVpunNsU9eo3co&r=lqfJULZneR5BAzkdvO_wKrBE9dMYkhIxP0zwfRM_iRI&m=6Bn1AXPjqHYgSl2vLw7l9vxlUgiLPjwVyPaPtkB-QT4&s=gWlQ_6ng4YLXjrqgy5HWlM58HecaPm1jMvtpD2CHM8Y&e=
 *
 */

// SHIFT REGISTER //
int latchPin = 8;   // latch pin 
int clockPin = 12;  // clock pin
int dataPin = 11;   // data pin

// MSGEQ7 CHIP //
int strobe = 2;     // strobe pin
int reset = 3;      // reset pin
int analogPin = 0;  // output, banded frequencies

int n = 0;

int band;
int start;
int lit;

int spectrum[7];      // storing band values
//int spectrumCopy[7];  // storing copy of band values

int spectrumAVG[7];       // storing average band values
//int spectrumMOVINGAVG[7]; // storing moving band average
//int spectrumSTD[7];       // storing band variance

int spectrumMIN[7];
int spectrumMAX[7];

// FREQUENCY - SHIFT REGISTER LOCATIONS // 
//int barone[8] = {40,41,42,43,44,45,46,47}; 
//int bartwo[8] = {32,33,34,35,36,37,38,39}; 
//int barthree[8] = {24,25,26,27,28,29,30,31}; 
//int barfour[8] = {16,17,18,19,20,21,22,23}; 
//int barfive[8] = {8,10,11,12,13,14,15}; 
//int barsix[8] = {0,1,2,3,4,5,6,7};

int barStart[6] = {0,8,16,24,32,40};

// MISCELLANEOUS //
int output,a,k;
int numRegisters = 6; // input total number of Shift Registers
byte* registerState;  //
int speed = 2;        // delay speed

int potval;
int potPin = 2;

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

void setup() {
  
  // CREATING ARRAY //
  registerState = new byte[numRegisters];
  for (size_t i = 0; i < numRegisters; i++) {
    registerState[i] = 0;
  }
  
  // SHIFT REGISTER CONTROL //
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  // MSGEQ7 CHIP CONTROL //
  pinMode(analogPin, INPUT);
  pinMode(strobe, OUTPUT);
  pinMode(reset, OUTPUT);
  digitalWrite(strobe, HIGH);
  digitalWrite(reset, LOW); 

  analogReference(DEFAULT);

  Serial.begin(9600); // Initialize Serial

  INITIALIZE();
  
}

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

void loop() {
  n += 1;
  
  readMSGEQ7();
  
  potval = analogRead(potPin);                  // Read Potentiometer Setting
  potval = map(potval,0,1023,0,100);            // Map Between 0 & 100

  for(int s = 0; s < 7; s++){
    if(n % 10000 == 0){
      spectrumMIN[s] = spectrumAVG[s];
      spectrumMAX[s] = spectrumAVG[s];
    }

//    spectrum[s] =  abs(spectrum[s] - spectrumAVG[s] - spectrumSTD[s]);
//    spectrum[s] = spectrum[s] / max(1,spectrumSTD[s]);
    
    spectrum[s] = (spectrum[s] + spectrumAVG[s]) / 2;
    spectrumAVG[s] = spectrum[s];

    spectrumMAX[s] = max(spectrum[s], spectrumMAX[s]);
    spectrumMIN[s] = min(spectrum[s], spectrumMIN[s]);

    spectrum[s] = map(spectrum[s],spectrumMIN[s],spectrumMAX[s],0,80);

    Serial.print(" ");
    Serial.print(spectrum[s]);
  }
  Serial.println();

  //--------------------------------------------------------------------------------------//
  
  // COLUMN LOOP//

  for(int bar = 0; bar < 6; bar++){
    LevelCheck(spectrum[bar]);
    lightBar(bar, k);
  }
}
  //--------------------------------------------------------------------------------------//

//    for(int level = 0; level <= k; level++){
//      lightBar(bar, level);
//    }

//  LevelCheck(spectrum[0]);    // Loops through each column in order,
//  regWrite(barone[k],HIGH);   // displaying the measured frequency level
//  delay(speed);               // at corresponding LED within matrix 
//  regWrite(barone[k],LOW);

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

// READING MSGEQ7 //
void readMSGEQ7(){
  digitalWrite(reset, HIGH);
  digitalWrite(reset, LOW);
  
  for (int i = 0; i < 7; i++){
    digitalWrite(strobe, LOW);
    delayMicroseconds(30);
    spectrum[i] = analogRead(analogPin);
    digitalWrite(strobe, HIGH);
  }
}

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

// LED MATRIX WRITE //
void regWrite(int pin, bool state){
  int reg = pin / 8;                // Determines Register
  int actualPin = pin - (8 * reg);  // Corresponding Pin on determined Register
  digitalWrite(latchPin, LOW);      // Initiates Register
  for (int i = 0; i < numRegisters; i++){
    byte* states = &registerState[i]; // Finds Register State
    if (i == reg){  // Updates State
      bitWrite(*states, actualPin, state);  // Writes to bit
    }
    shiftOut(dataPin, clockPin, MSBFIRST, *states);
  }
  //End session
  digitalWrite(latchPin, HIGH);     // Deactivates Register
}

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

//  for(int bar = 0; bar < 6; bar++){
//    LevelCheck(spectrum[bar]);
//    lightBar(bar, k);
//  }

void lightBar(int bar, int level){
  
  start = barStart[bar];
  lit = start + level;

  regWrite(lit, HIGH);
  delay(2);
  regWrite(lit, LOW);

}

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

// LEVEL CHECK //
void LevelCheck(int output){
  // Categorizes each LED into corresponding response level
  if (output <= 10){
    k = 0;  // LED - in order from base to top of LED column
  }
  if (output > 10){
    k = 1;
  }
  if (output > 20){
    k = 2;
  }
  if (output > 30){
    k = 3;
  }
  if (output > 40){
    k = 4;
  }
  if (output > 50){
    k = 5;
  }
  if (output > 60){
    k = 6;
  }
  if (output > 70){
    k = 7;
  }
}

//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//

void INITIALIZE(){
  Serial.println();
  Serial.println("RESETTING LEDS");

  for(k = 0; k < 48; k++){
    regWrite(k, HIGH);
    delay(50);
    regWrite(k, LOW);
  }
  
  Serial.println("FINDING AVERAGE");
  // Initialize as 0
  for(int a = 0; a < 7; a++){
    spectrumAVG[a] = 0;
  }
  // Sample 100 Points
  for(int sample = 0; sample < 200; sample++){
    readMSGEQ7();
    for(int bar = 0; bar < 6; bar++){
      LevelCheck(spectrum[bar]);
      lightBar(bar, k);
    }
    if(sample >= 20){
      for(int a = 0; a < 7; a++){
        spectrumAVG[a] += spectrum[a];
      }
    }
  }
  // Calculate Average
  for(int a = 0; a < 7; a++){
    spectrumAVG[a] = spectrumAVG[a] / 180;
    spectrumMAX[a] = spectrumAVG[a];
    spectrumMIN[a] = spectrumAVG[a];
    Serial.print(" ");
    Serial.print(spectrumAVG[a]);
  }

//  Serial.println();
//  Serial.println("FINDING STANDARD DEVIATION");
//  // Initialize as 0
//  for(int v = 0; v < 7; v++){
//    spectrumSTD[v] = 0;
//  }
//  // Sample 100 Points
//  for(int sample = 0; sample < 200; sample++){
//    readMSGEQ7();
//    for(int bar = 0; bar < 6; bar++){
//      LevelCheck(spectrum[bar]);
//      lightBar(bar, k);
//    }
//    if(sample >= 20){
//      for(int v = 0; v < 7; v++){
//        spectrumSTD[v] += sq(spectrum[v] - spectrumAVG[v]);
//      }
//    }
//  }
//  for(int v = 0; v < 7; v++){
//    spectrumSTD[v] = sqrt(spectrumSTD[v] / 180);
//    Serial.print(" ");
//    Serial.print(spectrumSTD[v]);
//  }

  for(k = 0; k < 48; k++){
    regWrite(k, HIGH);
    delay(50);
    regWrite(k, LOW);
  }

  Serial.println();
  Serial.println("DONE READING NOISE");
  delay(1000);
}
