/*
  Communications v5

  Transmits data using a white LED and recieves it using a photoresistor

*/

#define STX 0x70
#define ETX 0x71

char txButton, txTilt, txPot, txA, txB, txC, txD;
char txBuffer[8] = {0,0,0,0,0,0,0,ETX};
char rxBuffer[7];
char rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;
int rx_index;

int buttonPin = 2;
int buzzerPin = 8;
int buttonState = 0;

int tiltPin = 7;
int ledPin = 13;
int tiltState = 0;

int potenPin = A5;
float potenState;
int a = 4; 
int b = A4; 
int c = 10; 
int d = 12; 
int e = 11; 
int f = 5; 
int g = 6; 
int ledSegPins[7] = {a, b, c, d, e, f, g};

int VRx = A1;
int VRy = A2;
//int SW = 9;
int xPos = 0;
int yPos = 0;
int SW_State = 0;
int mapX = 0;
int mapY = 0;
int motorPin = 9;

void readInputs()
{
  // Reads the inputs in the mini-projects
  // Uses txButton, txTilt, txPot, txA, txB, txC, txD;
  //button
  txButton = digitalRead(buttonPin);
  
  //tilt
  txTilt = digitalRead(tiltPin);
  
  //potentiometer
  txPot = (int)round((analogRead(potenPin) / 1023.0) * 9.0);

  //joystick
  xPos = analogRead(VRx);
  yPos = analogRead(VRy);
  txA = (int)round(abs(((map(xPos, 0, 1023, -512, 512)) / 512.0) * 99.0));
  txB = (int)round(abs(((map(yPos, 0, 1023, -512, 512)) / 512.0) * 99.0));
  //SW_State = digitalRead(SW);
  txC = (int)round(((txA + txB + 71) / 3.0));
  txD = (int)round((txA + txB + txC + 19) / 4.0);
}

void writeOutputs()
{
  
  // Writes the outputs in the mini-projects
  // Uses rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD
  
  
  //buzzer  
  if (rxButton == HIGH){
    tone(buzzerPin, 740, 250);
  }
  
  //led
  digitalWrite(ledPin, rxTilt);
  
  
  //7 segment led
  clearDisplay();
  if (rxPot < 0.5){
    display0();
  }
  else if (rxPot < 1.5){
    display1();
  }
  else if (rxPot < 2.5){
    display2();
  }
  else if (rxPot < 3.5){
    display3();
  }
  else if (rxPot < 4.5){
    display4();
  }
  else if (rxPot < 5.5){
    display5();
  }
  else if (rxPot < 6.5){
    display6();
  }
  else if (rxPot < 7.5){
    display7();
  }
  else if (rxPot < 8.5){
    display8();
  }
  else if (rxPot < 9.5){
    display9();
  }
  else{
    Serial.println("default");
  }

  //motor
  int speed = (int)round((rxA / 99.0) * 255.0);
  if (speed > 50 && speed < 130){
    speed = 130;
  }
  analogWrite(motorPin, speed);
  /*if (SW_State == LOW){
    digitalWrite(motorPin, HIGH);
  }*/
}



int ledState = LOW;             // ledState used to set the LED


char encrypt(char in_char)
{
  char out_char;
  
  out_char = in_char;
  
  return out_char;
}


char decrypt(char in_char)
{
  char out_char;
  
  out_char = in_char;
  
  return out_char;
}



// the setup routine runs once when you press reset:
void setup()
{
  // set the digital pin as output:
  pinMode(3, OUTPUT);
  Serial.begin(9600);
  
  //initialise button
  pinMode(buttonPin, INPUT);
  
  //initialise tilt sensor and led
  pinMode(tiltPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  //initialise 1 digit 7 seg led
  for (int i = 0; i < 7; i++){
    pinMode(ledSegPins[i], OUTPUT);
  }

  //initialise joystick and motor
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  //pinMode(SW, INPUT_PULLUP);
  pinMode(motorPin, OUTPUT);
}


const long txInterval = 200;              // interval at which to tx bit (milliseconds)
int tx_state = 0;
char tx_char = 'H';
char chr;
unsigned long previousTxMillis = 0;        // will store last time LED was updated

char tx_string[] = "Hello World";
#define TX_START_OF_TEXT  -1
int tx_string_state = TX_START_OF_TEXT;



char getTxChar()
{
  char chr;
  
  switch (tx_string_state)
  {
    case TX_START_OF_TEXT:
    tx_string_state = 0;
    txBuffer[0] = txButton;
    txBuffer[1] = txTilt;
    txBuffer[2] = txPot;
    txBuffer[3] = txA;
    txBuffer[4] = txB;
    txBuffer[5] = txC;
    txBuffer[6] = txD;
    return STX;
    break;
    
    default:
    chr = txBuffer[tx_string_state];
    tx_string_state++;
    if (chr == ETX)  /* End of string? */
    {
      tx_string_state = TX_START_OF_TEXT;  /* Update the tx string state to start sending the string again */
      return ETX;  /* Send End of Text character */
    }
    else
    {
      return chr;  /* Send a character in the string */
    }
    break;
  }
}

void txChar()
{
  unsigned long currentTxMillis = millis();

  if (currentTxMillis - previousTxMillis >= txInterval)
  {
    // save the last time you blinked the LED (improved)
    previousTxMillis = previousTxMillis + txInterval;  // this version catches up with itself if a delay was introduced

    switch (tx_state)
    {
      case 0:
        chr = encrypt(getTxChar());
        digitalWrite(3, HIGH);  /* Transmit Start bit */
        tx_state++;
        break;

      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        if ((chr & 0x40) != 0)   /* Transmit each bit in turn */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
        chr = chr << 1;  /* Shift left to present the next bit */
        tx_state++;
        break;

      case 8:
        digitalWrite(3, HIGH);  /* Transmit Stop bit */
        tx_state++;
        break;

      default:
        digitalWrite(3, LOW);
        tx_state++;
        if (tx_state > 20) tx_state = 0;  /* Start resending the character */
        break;
    }
  }
}



const long rxInterval = 20;              // interval at which to read bit (milliseconds)
int rx_state = 0;
char rx_char;
unsigned long previousRxMillis = 0;        // will store last time LED was updated
int rx_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


void rxChar()
{
  unsigned long currentRxMillis = millis();
  int sensorValue;
  int i;

  if (currentRxMillis - previousRxMillis >= rxInterval)
  {
    // save the last time you read the analogue input (improved)
    previousRxMillis = previousRxMillis + rxInterval;  // this version catches up with itself if a delay was introduced

    sensorValue = analogRead(A0);
    Serial.println(sensorValue);

    switch (rx_state)
    {
      case 0:
        if (sensorValue >= 940)
        {
          rx_bits[0]++;
          rx_state++;
        }
        break;

      case 100:
        if ((rx_bits[0] >= 6) && (rx_bits[8] >= 6))  /* Valid start and stop bits */
        {
          rx_char = 0;

          for (i = 1; i < 8; i++)
          {
            rx_char = rx_char << 1;
            if (rx_bits[i] >= 6) rx_char = rx_char | 0x01;
          }
          rx_char = decrypt(rx_char);

          //Serial.println(rx_char + 0);

          switch (rx_char)
          {
            case STX:
            rx_index = 0;
            break;
            
            case ETX:
            rxButton = rxBuffer[0];
            rxTilt = rxBuffer[1];
            rxPot = rxBuffer[2];
            rxA = rxBuffer[3];
            rxB = rxBuffer[4];
            rxC = rxBuffer[5];
            rxD = rxBuffer[6];
            rx_index = 0;
            break;
            
            default:
            rxBuffer[rx_index] = rx_char;
            rx_index++;
            break;
          }
        }
        else
        {
          Serial.println("Rx error");
        }
//        for (i = 0; i < 10; i++)  /* Print the recieved bit on the monitor - debug purposes */
//        {
//          Serial.println(rx_bits[i]);
//        }
        for (i = 0; i < 10; i++)
        {
          rx_bits[i] = 0;
        }
        rx_state = 0;
        break;

      default:
        if (sensorValue >= 940)
        {
          rx_bits[rx_state / 10]++;
        }
        rx_state++;
        break;
    }
  }

}



// the loop routine runs over and over again forever:
void loop()
{
  readInputs();
  txChar();
  rxChar();
  writeOutputs();
}

void display1(void) 
{ 
  digitalWrite(b,HIGH);
  digitalWrite(c,HIGH);
} 
//display number2
void display2(void) 
{
  digitalWrite(a,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(g,HIGH);
  digitalWrite(e,HIGH);
  digitalWrite(d,HIGH);
} 
// display number3
void display3(void) 
{ 
  digitalWrite(a,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(c,HIGH);
  digitalWrite(d,HIGH);
  digitalWrite(g,HIGH);
} 
// display number4
void display4(void) 
{ 
  digitalWrite(f,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(g,HIGH);
  digitalWrite(c,HIGH);
  
} 
// display number5
void display5(void) 
{ 
  digitalWrite(a,HIGH);
  digitalWrite(f,HIGH);
  digitalWrite(g,HIGH);
  digitalWrite(c,HIGH);
  digitalWrite(d,HIGH);
} 
// display number6
void display6(void) 
{ 
  digitalWrite(a,HIGH);
  digitalWrite(f,HIGH);
  digitalWrite(g,HIGH);
  digitalWrite(c,HIGH);
  digitalWrite(d,HIGH);  
  digitalWrite(e,HIGH);  
} 
// display number7
void display7(void) 
{   
  digitalWrite(a,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(c,HIGH);
} 
// display number8
void display8(void) 
{ 
  digitalWrite(a,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(g,HIGH);
  digitalWrite(c,HIGH);
  digitalWrite(d,HIGH);  
  digitalWrite(e,HIGH);  
  digitalWrite(f,HIGH);  
} 
void clearDisplay(void) 
{ 
  digitalWrite(a,LOW);
  //digitalWrite(b,LOW);
  digitalWrite(g,LOW);
  digitalWrite(c,LOW);
  digitalWrite(d,LOW);  
  digitalWrite(e,LOW);  
  digitalWrite(f,LOW);  
} 
void display9(void) 
{ 
  digitalWrite(a,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(g,HIGH);
  digitalWrite(c,HIGH);
  digitalWrite(d,HIGH);  
  digitalWrite(f,HIGH);  
} 
void display0(void) 
{ 
  digitalWrite(a,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(c,HIGH);
  digitalWrite(d,HIGH);  
  digitalWrite(e,HIGH);  
  digitalWrite(f,HIGH);  
}