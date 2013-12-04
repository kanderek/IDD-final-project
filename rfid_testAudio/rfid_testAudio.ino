#include <SoftwareSerial.h>
#include <SimpleSDAudio.h>

SoftwareSerial mySerial(2, 3); // RX, TX

int RFIDResetPin = 7;

//Register your RFID tags here

char tag1[13] = "1E009A4067A3";
char tag2[13] = "010230F28243";
char tag3[13] = "01023C013A04";
char tag4[13] = "01023101093A";
char tag5[13] = "01023C0A4376";
char tag6[13] = "01023C000E31";
char tag7[13] = "01023C0A3207";
char tag8[13] = "1A004116317C";
char tag9[13] = "1E009A81F9FC";
char tag10[13] = "1A004162261F";

//Bytes Read: 36 41 30 30 33 45 34 42 32 34 33 42
char tag11[13] = "6A003E4B243B";



void setup(){
  
  // If your SD card CS-Pin is not at Pin 4, enable and adapt the following line:
  SdPlay.setSDCSPin(10);
  
  // Init SdPlay and set audio mode and activate autoworker
  if (!SdPlay.init(SSDA_MODE_HALFRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER)) {
    while(1); // Error while initialization of SD card -> stop.
  } 
  
  Serial.begin(9600);

  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);

  //mySerial.begin(9600);
  Serial.println("Hi to my friend from Serial!");
  
  //ONLY NEEDED IF CONTROLING THESE PINS - EG. LEDs
  /*
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  */
  pinMode(5, OUTPUT);
  /*
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  */
}

void loop(){

  char tagString[13];
  int index = 0;
  boolean reading = false;

  while(Serial.available()){

    byte readByte = Serial.read(); //read next available byte
    /*
    if(readByte == 2) 
    { 
      reading = true; //begining of tag
      lightLED(13);
    }*/
    
    if(readByte == 3) reading = false; //end of tag

    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
      Serial.println(readByte);
    }
  }

  
  checkTag(tagString, "GOODJOB.AFM"); //Check if it is a match
  /*if(tagString[0] != ' '){
    Serial.println(tagString);
  }*/
  clearTag(tagString); //Clear the char of all value
  resetReader(); //eset the RFID reader
}

void checkTag(char tag[], char sampleFile[]){
///////////////////////////////////
//Check the read tag against known tags
///////////////////////////////////

  if(strlen(tag) == 0) return; //empty, no need to contunue

  if(compareTag(tag, tag1)){ // if matched tag1, do this
    lightLED(2);

  }else if(compareTag(tag, tag2)){ //if matched tag2, do this
    lightLED(3);

  }else if(compareTag(tag, tag3)){
    lightLED(4);

  }else if(compareTag(tag, tag4)){
    lightLED(5);

  }else if(compareTag(tag, tag5)){
    lightLED(6);

  }else if(compareTag(tag, tag6)){
    lightLED(7);

  }else if(compareTag(tag, tag7)){
    lightLED(8);

  }else if(compareTag(tag, tag8)){
    lightLED(9);

  }else if(compareTag(tag, tag9)){
    lightLED(10);

  }else if(compareTag(tag, tag10)){
    lightLED(11);

  }else if(compareTag(tag, tag11)){
    lightLED(5);
    //playSample(sampleFile);
  }
  else{
    Serial.println(tag); //read out any unknown tag
  }

}

void lightLED(int pin){
///////////////////////////////////
//Turn on LED on pin "pin" for 250ms
///////////////////////////////////
  Serial.println(pin);

  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
}

void playSample(char sampleFile[]) {
  // Select file to play
  if(!SdPlay.setFile(sampleFile)) {
    while(1); // Error file not found -> stop.
  } 
  
  // Start playback
  SdPlay.play();
  /*
  // Optional: Wait until playback is finished
  while(!SdPlay.isStopped()) {
	; // no SdPlay.worker() required anymore :-)
  }*/
}

void resetReader(){
///////////////////////////////////
//Reset the RFID reader to read again.
///////////////////////////////////
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}

void clearTag(char one[]){
///////////////////////////////////
//clear the char array by filling with null - ASCII 0
//Will think same tag has been read otherwise
///////////////////////////////////
  for(int i = 0; i < strlen(one); i++){
    one[i] = 0;
  }
}

boolean compareTag(char one[], char two[]){
///////////////////////////////////
//compare two value to see if same,
//strcmp not working 100% so we do this
///////////////////////////////////

  if(strlen(one) == 0) return false; //empty

  for(int i = 0; i < 12; i++){
    if(one[i] != two[i]) return false;
  }

  return true; //no mismatches
}
