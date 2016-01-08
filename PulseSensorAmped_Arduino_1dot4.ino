
/*  Pulse Sensor Amped 1.4    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

----------------------  Notes ----------------------  ---------------------- 
This code:
1) Blinks an LED to User's Live Heartbeat   PIN 13
2) Fades an LED to User's Live HeartBeat
3) Determines BPM
4) Prints All of the Above to Serial

Read Me:
https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md   
 ----------------------       ----------------------  ----------------------
*/

///////// Joystick Controller //////////////

int vertical = A0;      // select the input pin for the vert
int horizontal = A1;    // select the input pin for the hort
int buttonPin = 2;
int x = 0;              // variable for store the value of the input of the vertical input
int y = 0;              // variable for store the value of the input of the horizontal input
int button = 0;
int TOPMARGIN=700;      // margin to determine the zone of no movement
int BOTTOMMARGIN = 200; // margin to determine the zone of no movement

////////// Skin Resistence Sensor ///////////

int humidityAPin = 3;
int humiditiResistenceValue = 0;


/////////  Pulse Sensor /////////////

//  Variables
int pulsePin = 2;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = false;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse 


////// Game hacks ///////

boolean correctDirection = true;
int hungry = 1;

int nervousLimit = 40;
int nervousCounter = 0;
int nervousMax = 20;

int skinResistenceLimit = 50;
int skinCounter = 0;
int skinMax = 20;

int calibrateLed = 12;
int calibrateSeconds = 1;

int countSensorSamples = 0;
int pulseSensorTempValue = 0;
int pulseSensorStableValue = 0;
int skinResisenceStableValue = 0;

boolean calibrate = false; 

//////////////////////////


void setup(){
   setupPulseSensor();
   setupJostickController();
   setupLed();
   calibratePulseSensor();
   calibrateSkinResitenceSensor();
}

void setupLed(){
  pinMode(calibrateLed, OUTPUT);
}




void setupPulseSensor(){
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  //Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
   // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE, 
   // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
//   analogReference(EXTERNAL);
}


void setupJostickController(){
  Serial.begin(9600);
  //pinMode(buttonPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, 1);
}

//  Where the Magic Happens
void loop(){
  
  loopPulseSensor();
  loopJostickController(); 

  checkPulseNervious();
  checkSkinResistense();
}

void checkSkinResistense(){

  int upSkinLimit = skinResisenceStableValue - skinResistenceLimit;
  int lastSkinLimit = skinResisenceStableValue + skinResistenceLimit;

  if(hungry = 0){
    if(skinCounter < skinMax){
      hungry = 1;
      //calibratePulseSensor();
      skinCounter = 0;
    }
     skinCounter =  skinCounter + 1;
  }

  int skinValue = analogRead(humidityAPin);

  if(skinValue > upSkinLimit){
    skinCounter = 1;
    hungry = 0;
  }else if(skinValue < lastSkinLimit){
    hungry = 1;
    //calibratePulseSensor();
  }else{
    skinCounter = 0;
    hungry = 1;
  }
  
}

void checkPulseNervious(){
  
  int upPulseLimit = pulseSensorStableValue - nervousLimit;
  int lastPulseLimit = pulseSensorStableValue + nervousLimit;

 
  if(!correctDirection){
    if(nervousCounter < nervousMax){
      correctDirection=true;
      //calibratePulseSensor();
      nervousCounter = 0;
    }
    nervousCounter = nervousCounter+1;
  }


  if(BPM > upPulseLimit){
    nervousCounter = 1;
    correctDirection = false;
  }else if(BPM < lastPulseLimit){
    correctDirection = true;
    //calibratePulseSensor();
  }else{
    nervousCounter = 0;
    correctDirection=true;
  }
}

void calibratePulseSensor(){

  calibrate= true;

   unsigned long start = millis();
   
   while (millis() - start <= 1000*calibrateSeconds) {
    ledBlink();
    loopPulseSensor();
   }
   pulseSensorStableValue = pulseSensorTempValue / countSensorSamples;
   countSensorSamples = 0;
   pulseSensorTempValue= 0;
   
   calibrate = false;

   //Serial.print("pulseSensorStableValue: ");
   //Serial.println(pulseSensorStableValue);
   
}

void calibrateSkinResitenceSensor(){

   int tempSkinresistence = 0;
   unsigned long start = millis();
   int count=0;
   
   while (millis() - start <= 1000*calibrateSeconds) {
    ledBlink();
    tempSkinresistence = tempSkinresistence + analogRead(humidityAPin); 
    count = count +1;
   }
  
   skinResisenceStableValue = tempSkinresistence/count;

   //Serial.print("skinResisenceStableValue: ");
   //Serial.println(skinResisenceStableValue);
   
}

void ledBlink(){
  digitalWrite(calibrateLed, HIGH);   // set the LED on
  delay(500);              // wait for a second
  digitalWrite(calibrateLed, LOW);    // set the LED off
  delay(500);              // wait for a second
}

void loopPulseSensor(){
  serialOutput() ;       
    
  if (QS == true){     // A Heartbeat Was Found
                       // BPM and IBI have been Determined
                       // Quantified Self "QS" true when arduino finds a heartbeat
        digitalWrite(blinkPin,HIGH);     // Blink LED, we got a beat. 
        fadeRate = 255;         // Makes the LED Fade Effect Happen
                                // Set 'fadeRate' Variable to 255 to fade LED with pulse
        serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.     
        QS = false;                      // reset the Quantified Self flag for next time    
  }
     
  ledFadeToBeat();                      // Makes the LED Fade Effect Happen 
  delay(20);
}

void loopJostickController(){
  x = analogRead(vertical);
  y = analogRead(horizontal);
  button = digitalRead(buttonPin);   // read the input pin
  //Serial.println("");
  //Serial.print(button);
  //Serial.println("");
  delay(50);

  sendDirection();
  
}

void sendDirection(){

  if(x > TOPMARGIN && y > TOPMARGIN){
    if(x>y && correctDirection){
      Serial.print("right;");
    }else{
      Serial.print("down;");
    }
  }else if(x > TOPMARGIN && y < TOPMARGIN){
    if(x>y && y>125 && correctDirection){
      Serial.print("right;");
    }else{
      Serial.print("up;");
    }
  }else if(x < BOTTOMMARGIN && y < BOTTOMMARGIN){
    if(x<y && y>125 && correctDirection){
      Serial.print("left;");
    }else{
      Serial.print("up;");
    }
  }else if(x < BOTTOMMARGIN && y < BOTTOMMARGIN){
    if(x>y && x>125 && correctDirection){
      Serial.print("down;");
    }else{
      Serial.print("left;");
    }
  }else if(x>TOPMARGIN){
    if(correctDirection){
      Serial.print("right;");
    }else{
      Serial.print("left;");
    }
    
  }else if(x<BOTTOMMARGIN){
    if(correctDirection){
      Serial.print("left;");
    }else{
      Serial.print("right;");
    }
    
  }else if(y>TOPMARGIN){
    if(correctDirection){
      Serial.print("down;");
    }else{
      Serial.print("up;");
    }
  }else if(y<BOTTOMMARGIN){
    if(correctDirection){
      Serial.print("up;");
    }else{
      Serial.print("down;");
    }
    
  }else{
    Serial.print("stop;");
    
  }
  Serial.print(pulseSensorStableValue);
  Serial.print(";");
  Serial.print(BPM);
  Serial.print(";");
  Serial.print(skinResisenceStableValue);
  Serial.print(";");
  Serial.print(analogRead(humidityAPin));
  Serial.print(";");
  Serial.print(hungry);
  Serial.println("$");
}


void ledFadeToBeat(){
    fadeRate -= 15;                         //  set LED fade value
    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin,fadeRate);          //  fade LED
  }




