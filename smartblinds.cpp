#include <DHT.h>

#define DHTTYPE DHT11
#define DHT1PIN A2
#define DHT2PIN A3

DHT dhtRoom(DHT1PIN, DHTTYPE);
DHT dhtOut(DHT2PIN, DHTTYPE);


typedef struct task {
 int state;
 unsigned long period;
 unsigned long elapsedTime;
 int (*TickFct)(int);
} task;

const unsigned short tasksNum = 6;

task tasks[tasksNum];

//lots of variables
int toAdjust = 0;
int amountAdjust = 0;


int tempRoom = 0;
int tempOut = 0;
enum TEMP_STATES { tempstart, ont };
int TEMP_TICK(int state1) {
 switch (state1) {
   case tempstart:
     state1 = ont;
     break;
   case ont:
     state1 = ont;
     break;
   default:
     break;
 }
 switch (state1) {
   case tempstart:
     break;
   case ont:
     tempRoom = dhtRoom.readTemperature(true);
     tempOut = dhtOut.readTemperature(true);
    
     // Serial.print("TEMPS in Celsius: ");
     // Serial.print("Room: ");
     // Serial.print(tempRoom);
     // Serial.print(", Outside: ");
     // Serial.println(tempOut);
     break;
   default:
     break;
 }
 return state1;
}


int lightLvlRoom = 0;
int lightLvlOut = 0;
enum LIGHT_STATES { lightstart, on };
int LIGHT_TICK(int state2) {
 switch (state2) {
   case lightstart:
     state2 = on;
     break;
   case on:
     state2 = on;
     break;
 }
 switch (state2) {
   case lightstart:
     break;
   case on:
     lightLvlRoom = analogRead(A0);
     lightLvlOut = analogRead(A1);
     // Serial.print("LIGHT: Room: ");
     // Serial.print(lightLvlRoom);
     // Serial.print(", Outside: ");
     // Serial.println(lightLvlOut);
     break;
 }
 return state2;
}




int backsteps[8][4] {
 {0,0,0,1},
 {0,0,1,1},
 {0,0,1,0},
 {0,1,1,0},
 {0,1,0,0},
 {1,1,0,0},
 {1,0,0,0},
 {1,0,0,1},
};
int forsteps[8][4] {
 {1,0,0,0},
 {1,1,0,0},
 {0,1,0,0},
 {0,1,1,0},
 {0,0,1,0},
 {0,0,1,1},
 {0,0,0,1},
 {1,0,0,1},
};


int i = 0;
int tempcounter = 0;
int phoneFor = 0;
int phoneBack = 0;
int adBack = 0;
int adFor = 0;
int sig[4] = {2, 3, 4, 5};
enum MOTOR_STATES { motorstart, forward, backward, motorstop };
int MOTOR_TICK( int state3 ){
 switch(state3){
   case motorstart:
     state3 = motorstop;
     break;
   case forward:
    
     if(i < 8){
       ++i;
     }
     else{
       i = 0;
       state3 = motorstop;
     }
     //Serial.println(digitalRead(8));
     //state3 = forward;
    
     break;
   case backward:
    
     // Serial.print("spin: ");
     // Serial.println(tempcounter);
     if(i < 8){
       ++i;
     }
     else{
       i = 0;
       state3 = motorstop;
     }
     break;
   case motorstop:
     int backwardsC = digitalRead(8);
     int forwardsC = digitalRead(9);
     if(backwardsC == 0 || adBack == 1){
       //++tempcounter;
       state3 = backward;
     }
     else if(forwardsC == 0 || adFor == 1){
       state3 = forward;
     }
     else{
       state3 = motorstop;
     }
     break;
   default:
     break;
 }


 switch(state3){
   case motorstart:
     break;
   case forward:
     for (int j=0; j < 4; j++) {
       if (forsteps[i][j] == 1) {
         digitalWrite(sig[j], HIGH);
       }
       else {
         digitalWrite(sig[j], LOW);
       }
     }
     break;


   case backward:
     for (int j=0; j < 4; j++) {
       if (backsteps[i][j] == 1) {
         digitalWrite(sig[j], HIGH);
       }
       else {
         digitalWrite(sig[j], LOW);
       }
     }


     break;
   case motorstop:
     break;
   default:
     break;
 }
 return state3;
}






int ismanualmode = 1;


int inputvals = 0;
int currPos = 0;
enum INPUTS_STATES { inputsstart, waitinputs, getinputs };
int INPUTS_TICK( int state4 ){
 switch(state4){
   case inputsstart:
     ismanualmode = 1;
     inputvals = 0;
     state4 = getinputs;
     break;
   case waitinputs:
    
     if(inputvals == 49){
       ismanualmode = 1;
       inputvals = 0;
       state4 = getinputs;
     }
     else{
       inputvals = 0;
       state4 = waitinputs;
     }
   
    
     break;
   case getinputs:


     if(inputvals == 50){   
       inputvals = 0;
       ismanualmode = 0;
       state4 = waitinputs;
     }
     else{
       inputvals = 0;
       state4 = getinputs;
     }
    
     break;
  
   default:
     break;
 }


 switch(state4){
   case inputsstart:
     break;
   case waitinputs:
    
     if(Serial.available() > 0){
       inputvals = Serial.read();
     }
     // Serial.println(inputvals);
     // Serial.println("automode");




     break;


   case getinputs:
     if(Serial.available() > 0){
       inputvals = Serial.read();
     }
     //Serial.println("manual mode");
     // if(Serial.available() > 0){
     //   inputvals = Serial.read();
     // }
      //Serial.println(inputvals);
     phoneFor = 0;
     phoneBack = 0;
     if(inputvals == 51){
       phoneFor = 0;
       phoneBack = 1;
       if(currPos != 0){
         toAdjust = currPos - 25;
       }
     } 
     else if(inputvals == 52){
       phoneBack = 0;
       phoneFor = 1; 
       if(currPos != 100){
         toAdjust = currPos + 25;
       }     
     } 


     if(inputvals == 53){
       phoneBack = 0;
       phoneFor = 0;
     } 
     // Serial.print("For: ");
     // Serial.print(phoneFor);
     // Serial.print(" Back: ");
     // Serial.println(phoneBack);
     break;
  
   default:
     break;
 }
 return state4;
}






enum CONTROL_STATES { controlstart, checkvals, manual };
int CONTROL_TICK( int state5 ){
 switch(state5){
   case controlstart:
     state5 = manual;
     break;
   case checkvals:
     //Serial.println(ismanualmode);
     if(ismanualmode == 1){
       state5 = manual;
     }
     else{
       state5 = checkvals;
     }
   
    
     break;
  
   case manual:
     if(ismanualmode == 0){
       state5 = checkvals;
     }
     else{
       state5 = manual;
     }
     break;
   default:
     break;
 }


 switch(state5){
   case controlstart:
     break;
   case checkvals:
    
     if(lightLvlOut < 10){
       if(tempRoom >= 80){
         toAdjust = 100;
       }
       else if(tempRoom >= 76 && tempRoom < 80){
         toAdjust = 50;         
       }
       else{
         toAdjust = 0;
       }
     }
     else if(lightLvlOut >= 10){
       if((tempRoom <= tempOut && tempRoom > tempOut-5) || (tempRoom >= tempOut && tempRoom < tempOut+5)){
         if(lightLvlRoom < lightLvlOut){
           toAdjust = 100;
         }
         else{
           toAdjust = 50;
         }
       }
       else if(tempOut > 78 && tempOut < 80 && tempRoom > 78){
         toAdjust = 100;
       }
       else if(tempOut > 80 && tempOut < 85){//changed here
         toAdjust = 50;
       }
       else if(tempOut >= 85 && tempRoom >= 80){
         toAdjust = 0;
       }
       else if(tempOut <= 78){
         toAdjust = 100;
       }
     }
    


     break;
   case manual:
     Serial.println("MANUAL");
     break;
   default:
     break;
 }
 return state5;
}




int adjustCount = 0;
int amountToMove = 0;
enum BLINDS_STATES { blindsstart, hold, move };
int BLINDS_TICK( int state6 ){
 switch(state6){
   case blindsstart:
    
     state6 = hold;
     break;
   case hold:
     Serial.print("toAd: ");
     Serial.print(toAdjust);
     Serial.print(" Curr: ");
     Serial.print(currPos);
     Serial.print(" TOMOVE: ");
     Serial.println(amountToMove);
     if(currPos == toAdjust){
       state6 = hold;
     }
     else{
       amountToMove = (toAdjust-currPos)/25;
       adjustCount = amountToMove * 15;
       if(adjustCount < 0){
         adjustCount = adjustCount * -1;
       }
       state6 = move;       
     }
     break;
   case move:
     if(adjustCount == 0){
       currPos = toAdjust;
       adBack = 0;
       adFor = 0;
       state6 = hold;
     }
     else{
       --adjustCount;
       state6 = move;
     }
    
    
     break;
  
   default:
     break;
 }


 switch(state6){
   case blindsstart:
     break;
   case hold:
    
    
     break;


   case move:
     if(amountToMove < 0){
       adFor = 1;      
     }
     else if(amountToMove > 0){
       adBack = 1;
     }
     break;
  
   default:
     break;
 }
 return state6;
}




void setup() {


 pinMode(2, OUTPUT);
 pinMode(3, OUTPUT);
 pinMode(4, OUTPUT);
 pinMode(5, OUTPUT);
 pinMode(8, INPUT_PULLUP);//backward
 pinMode(9, INPUT_PULLUP);//forward


 unsigned char i = 0;
 tasks[i].state = tempstart;
 tasks[i].period = 1000;
 tasks[i].elapsedTime = 0;
 tasks[i].TickFct = &TEMP_TICK;
 ++i;
 tasks[i].state = lightstart;
 tasks[i].period = 1000;
 tasks[i].elapsedTime = 0;
 tasks[i].TickFct = &LIGHT_TICK;
 ++i;
 tasks[i].state = motorstart;
 tasks[i].period = 1;
 tasks[i].elapsedTime = 0;
 tasks[i].TickFct = &MOTOR_TICK;
 ++i;
 tasks[i].state = inputsstart;
 tasks[i].period = 1000;
 tasks[i].elapsedTime = 0;
 tasks[i].TickFct = &INPUTS_TICK;
 ++i;
 tasks[i].state = controlstart;
 tasks[i].period = 1000;
 tasks[i].elapsedTime = 0;
 tasks[i].TickFct = &CONTROL_TICK;
 ++i;
 tasks[i].state = blindsstart;
 tasks[i].period = 500;
 tasks[i].elapsedTime = 0;
 tasks[i].TickFct = &BLINDS_TICK;
 ++i;


 Serial.begin(9600);
 dhtRoom.begin();
 dhtOut.begin();
}


void loop() {
 unsigned char i;
 for (i = 0; i < tasksNum; ++i) {
   if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
     tasks[i].state = tasks[i].TickFct(tasks[i].state);
     tasks[i].elapsedTime = millis();
   }
 }
}
