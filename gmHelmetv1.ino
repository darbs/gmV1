//NOT I REVERSED THE ORDER OF THE INPUTS
// ser -> rclk ->srclk
//How many of the shift registers - change this
#define number_of_74hc595s 1 

//do not touch
#define numOfRegisterPins number_of_74hc595s * 8
int buttonPin = 13;
int buttonState = 0, lastButtonState = 0;
int patternNum = 0;

int bot_LSER = 9;
int bot_RSER = 10;
int bot_RCLK = 11;
int bot_SRCLK = 12;

int mid_SER = 6;   //pin 14 on the 75HC595
int mid_RCLK = 7;  //pin 12 on the 75HC595
int mid_SRCLK = 8;

int top_LSER = 2;
int top_RSER = 3;   //pin 14 on the 75HC595
int top_RCLK = 4;  //pin 12 on the 75HC595
int top_SRCLK = 5; //pin 11 on the 75HC595

long midInterval = 60, topInterval = 110, botInterval = 300, randomInterval = 50; 
long previousMid = 0, previousTop = 0, previousBot = 0;

int n = 0, i = 0, m = numOfRegisterPins;

int x;
int y;
byte dataToSend;

boolean top[numOfRegisterPins];
boolean mid[numOfRegisterPins];

int botData[] = {
  15,15,15,15};
void (*pattern[3]) (unsigned long currentMillis);

void setup(){
  pinMode(bot_LSER, OUTPUT);
  pinMode(bot_RSER, OUTPUT);
  pinMode(bot_RCLK, OUTPUT);
  pinMode(bot_SRCLK, OUTPUT);  

  pinMode(mid_SER, OUTPUT);
  pinMode(mid_RCLK, OUTPUT);
  pinMode(mid_SRCLK, OUTPUT);

  pinMode(top_LSER, OUTPUT);
  pinMode(top_RSER, OUTPUT);
  pinMode(top_RCLK, OUTPUT);
  pinMode(top_SRCLK, OUTPUT);

  pinMode(buttonPin, INPUT);

  //reset all register pins
  clearTopRegister();
  writeTopRegister();
  writeMidRegister();


  pattern[0] = runPattern0;
  pattern[1] = runPattern1;
  pattern[2] = runPattern2;

}               

//default
int midIndex = 0, topIndex = 0, botIndex = 0;
int bot_column = 0;
int botRowValueIndex = 0;
void runPattern0(unsigned long current) {
  if(current - previousTop > 110) {
    previousTop = current;
    topDefault();
    ++topIndex;
  }

  if(current - previousMid > 60) {
    previousMid = current;
    midDefault();
    ++midIndex;
  }

  if(current - previousBot > 300) {
    previousBot = current;
    botRowValueIndex %= 4;
    botRowValueIndex++;
    botDefault();
  }
}

//Randomize bro....probably not seziure freindly
void runPattern1(unsigned long current) {
  if(current - previousTop > 60) {
    previousTop = current;
    topRandom();
  }

  // patter 0 mid
  if(current - previousMid > 60) {
    previousMid = current;
    midRandom();
  }

  if(current - previousBot > 60) {
    previousBot = current;
    botRandom();
  }
}

//slider
void runPattern2(unsigned long current) {
  if(current - previousTop > 110) {
    previousTop = current;
    sliderTop(3);
    //++topIndex;
  }

  if(current - previousMid > 60) {
    previousMid = current;
    midDefault();
    ++midIndex;
  }

  if(current - previousBot > 300) {
    previousBot = current;
    botRowValueIndex %= 4;
    botRowValueIndex++;
    botDefault();
  }
}

void topDefault(){
  // Reverse direction if index is out of bound
  if(topIndex >=7){
    topIndex *= -1;
  }  

  clearTopRegister();
  setTopRegisterPin(abs(topIndex), HIGH); 
  writeTopRegister();
}

void midDefault(){
  // Reverse direction if index is out of bound
  if(midIndex >=7){
    midIndex *= -1;
  }

  clearMidRegister();
  setMidRegisterPin(abs(midIndex), HIGH); 
  writeMidRegister();
}

void botDefault(){
  // Reverse direction if index is out of bounds
  if(bot_column >=3){
    bot_column *= -1;
  }
  writeBotRegister(abs(bot_column),botRowValueIndex-1);
  bot_column++;    
}

void topRandom(){
  int r = random(8);
  clearTopRegister();
  setTopRegisterPin(r, HIGH); 
  writeTopRegister();
}

void midRandom(){
  int r = random(8);
  clearMidRegister();
  setMidRegisterPin(r, HIGH); 
  writeMidRegister();
}

int rando[] = {
  1,5,7,15};
void botRandom(){
  int r = random(4);
  int i = random(4);
  botData[i] = random(16);
  writeBotRegister(i, i);
}

//TODO empty properly FIX THIS SHIT
void sliderMid(){
  if(n > m){
    n = 0;
    --m;
  }

  setMidRegisterPin(n, HIGH); 
  setMidRegisterPin(n-1, LOW);    
  n++;

  writeMidRegister();

  if(m == 0){
    m = numOfRegisterPins;
  }
}

void oldDef(){
  if(i > numOfRegisterPins-1){
    i = 0;
  }

  clearTopRegister();
  setTopRegisterPin(i, HIGH);  
  writeTopRegister();

  i++;
}

//TODO refactor to look sexier
int dir = 1;
int low = 0, high = 0;
void sliderTop(int w){
  if(high > numOfRegisterPins-1){
    high = numOfRegisterPins;
    low = high-w;
    dir = -1;
  } 
  else if (low < 1){
    low = 0;
    high = low+w;
    dir = 1;
  }

  clearTopRegister();
  for(int j = low; j < high; j ++){
    setTopRegisterPin(j, HIGH);  
  }
  writeTopRegister();

  high += dir;
  low += dir;
}



//set all register pins to LOW
void clearTopRegister(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    top[i] = LOW;
  }
} 

void clearMidRegister(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    mid[i] = LOW;
  }
}


//Set and display registers
void writeTopRegister(){

  digitalWrite(top_RCLK, LOW);

  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(top_SRCLK, LOW);

    int val = top[i];
    digitalWrite(top_RSER, val);
    digitalWrite(top_LSER, val);
    digitalWrite(top_SRCLK, HIGH);

  }
  digitalWrite(top_RCLK, HIGH);

}

void writeMidRegister(){
  digitalWrite(mid_RCLK, LOW);

  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(mid_SRCLK, LOW);

    int val = mid[i];

    digitalWrite(mid_SER, val);

    digitalWrite(mid_SRCLK, HIGH);

  }
  digitalWrite(mid_RCLK, HIGH);

}

void writeBotRegister(int row, int value){

  byte dataToSend = (1 << (row+4)) | (15 & ~botData[value]);

  digitalWrite(bot_RCLK, LOW);

  for (int j=0;j<8;j++){
    digitalWrite(bot_SRCLK,LOW);
    digitalWrite(bot_LSER,((dataToSend>>j)&1));
    digitalWrite(bot_RSER,((dataToSend>>j)&1));
    digitalWrite(bot_SRCLK,HIGH);
  }


  //set latch pin high- this sends data to outputs so the LEDs will light up
  digitalWrite(bot_RCLK, HIGH);
}

//set an individual pin HIGH or LOW
void setTopRegisterPin(int index, int value){
  top[index] = value;
}


void setMidRegisterPin(int index, int value){
  mid[index] = value;
}

void checkButton(){
  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      patternNum++;
    } 
  }
  lastButtonState = buttonState;
}

// Method lights up all mid leds
void midTest(){
  for(int i = 0; i < 8; i ++){
    setMidRegisterPin(i, HIGH); 
  }
  writeMidRegister();
}  

void loop(){

  unsigned long currentMillis = millis();
  //buttonState = digitalRead(buttonPin);
  //checkButton();

  (*pattern[2]) (currentMillis);

}





