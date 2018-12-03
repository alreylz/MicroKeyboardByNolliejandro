/* MICRO KEYBOARD by Nolliejandro */
#include <avr/pgmspace.h>

//Command Codes
#define NOTE_ON_CC 0x9
#define NOTE_OFF_CC 0x8
#define SYS_RT_CC 0xF1
#define POLY_PRESSURE_CC 0xA
#define CHANNEL_PRESSURE_CC 0xD
#define PITCH_BEND_CC 0xE0
#define CONTROL_CHANGE_CC 0xB
#define PROGRAM_CHANGE_CC 0xC
#define SYSTEM MESSAGES 0xF


#define NUM_KEYS 12
#define NUM_OCTAVES 6

#define Key1Pin 13
#define Key2Pin 12
#define Key3Pin 11
#define Key4Pin 10
#define Key5Pin 9
#define Key6Pin 8
#define Key7Pin 7
#define Key8Pin 6
#define Key9Pin 5
#define Key10Pin 4
#define Key11Pin 3
#define Key12Pin 2

#define Y_Joystick A4
#define X_Joystick A3

#define JOY_PUSH A5

#define SOUND 1
#define NO_SOUND 0

#define PRESSED 0
#define FREE 1

//Accidentals mode
#define SHARPS 1
#define FLATS 0

#define EXEC_MODE 0
// MODE 0 : Normal Operation
// MODE 1 : Debug

#define NUM_MODES 4
#define BENDING_MODE 0
#define KEY_SHIFT_MODE 1
#define PROG_CHANGE_MODE 2
#define CHANN_PRESSURE_MODE 3


struct octave_i_MidiMapping{
    //byte octave;  //Contains the number of the octave of a specific note
    char noteName[12][2] ;  //Contains the verbose name of a note: i.e. C or C#
    byte keyNumber_Midi[12]; //Contains the number of key associated with a specific note
};


const char c[] PROGMEM = "C" ;
const char cSharp[] PROGMEM = "C#;Db";
const char d[] PROGMEM = "D" ;
const char dSharp[] PROGMEM = "D#;Eb" ;
const char e[] PROGMEM = "E" ;
const char f[] PROGMEM = "F" ;
const char fSharp[] PROGMEM = "F#;Gb" ;
const char g[] PROGMEM = "G" ;
const char gSharp[] PROGMEM = "G#;Ab" ;
const char a[] PROGMEM = "A" ;
const char aSharp[] PROGMEM = "A#;Bb";
const char b[] PROGMEM = "B";

//Note names (Both flat and sharps)
const char* const noteNames[] PROGMEM = {c, cSharp, d, dSharp, e, f, fSharp, g, gSharp,a,aSharp, b};
//Configuration data
byte accidentals_Mode = FLATS;
byte activeOctave = 3;
//Array of pins which map to each piano key
int KeyPin [12] = {Key1Pin,Key2Pin,Key3Pin,Key4Pin,Key5Pin,Key6Pin,Key7Pin,Key8Pin,Key9Pin,Key10Pin,Key11Pin,Key12Pin};
//Status of pressing and note on for each one
byte keyState [12] = {1,1,1,1,1,1,1,1,1,1,1,1}; //Value of the PIN Associated with each keyboard independently.
byte keyOn_   [12] = {0,0,0,0,0,0,0,0,0,0,0,0}; //Tells whether the key(sound) is active (a NoteOn message has been sent and a Note Off hasn't been triggered)
//Octave mapping -Contains names and midi key values- (each part index is an octave);
octave_i_MidiMapping noteMapping[NUM_OCTAVES];

unsigned long elapsedSinceLast_Oct_Change = millis();
int pushCurrStatus = FREE;
int pushPrevStatus = FREE;
unsigned long pushStartTime = millis;
unsigned long pushEndTime = millis; 

byte timesPushed = 0;
unsigned long elapsedPushing = 0 ;

unsigned long clickingTimerBegin = 0;
unsigned long multiPushTimeThresh = 2000;

byte opMode = BENDING_MODE ;
int valX = 0;
int valY = 0;

void checkKey1_C(){
  keyState[0] = digitalRead(Key1Pin);
}
void checkKey2_Csharp(){
  keyState[1] = digitalRead(Key2Pin);
}
void checkKey3_D(){
  keyState[2] = digitalRead(Key3Pin);
}
void checkKey4_Dsharp(){
  keyState[3] = digitalRead(Key4Pin);
}
void checkKey5_E(){
  keyState[4] = digitalRead(Key5Pin);
}
void checkKey6_F(){
  keyState[5] = digitalRead(Key6Pin);
}
void checkKey7_Fsharp(){
  keyState[6] = digitalRead(Key7Pin);
}
void checkKey8_G(){
  keyState[7] = digitalRead(Key8Pin);
}
void checkKey9_Gsharp(){
  keyState[8] = digitalRead(Key9Pin);
}
void checkKey10_A(){
  keyState[9] = digitalRead(Key10Pin);
}
void checkKey11_Asharp(){
  keyState[10] = digitalRead(Key11Pin);
}
void checkKey12_B(){
  keyState[11] = digitalRead(Key12Pin);
}

/* MIDI MESSAGE FUNCTIONS */
int sendNoteOn(byte channel, byte noteNumber, byte velocity){
  if(channel<0 || channel>15 || noteNumber>127 || noteNumber <0|| velocity<0|| velocity>127 )
    return -1; 
  byte statusByte = NOTE_ON_CC<<4 | channel ; //
  Serial.write(statusByte);
  byte dataByte1_note = 0x00 | noteNumber  ; //
  Serial.write(dataByte1_note);
  byte dataByte2_vel = 0x00 | velocity ; //
  Serial.write(dataByte2_vel);
  return 0;
}
int sendNoteOff(byte channel, byte noteNumber, byte velocity){
  if(channel<0 || channel>15 || noteNumber>127 || noteNumber <0|| velocity<0|| velocity>127 )
    return -1; 
  byte statusByte = NOTE_OFF_CC<<4 | channel ; //
  Serial.write(statusByte);
  byte dataByte1_note = 0x00 | noteNumber  ; //
  Serial.write(dataByte1_note);
  byte dataByte2_vel = 0x00 | velocity ; //
  Serial.write(dataByte2_vel);
  return 0;
}
int sendImplicitNoteOff(byte channel, byte noteNumber){
  if(channel<0 || channel>15 || noteNumber>127 || noteNumber <0 )
    return -1; 
  byte statusByte = NOTE_ON_CC<<4 | channel ; //
  Serial.write(statusByte);
  byte dataByte1_note = 0x00 | noteNumber  ; //
  Serial.write(dataByte1_note);
  byte dataByte2_vel = 0x00 ; //
  Serial.write(dataByte2_vel);
  return 0;
}
int sendPolyphonicPressure(byte channel,byte noteNumber,byte pressure){
  if(channel<0 || channel>15 || noteNumber>127 || noteNumber <0 )
    return -1; 
  byte statusByte = POLY_PRESSURE_CC<<4 | channel ; //
  Serial.write(statusByte);
  byte dataByte1_note = 0x00 | noteNumber  ; //
  Serial.write(dataByte1_note);
  byte dataByte2_press = 0x00 | pressure ; //
  Serial.write(dataByte2_press);
  return 0;
}
int sendPitchBend(byte channel, unsigned short shifting){
    if(channel<0 || channel>15 || shifting<0 )
    return -1; 
  //Serial.print("Shifting: \t"); Serial.println(shifting); 
  byte statusByte = PITCH_BEND_CC | channel ; //
  Serial.write(statusByte);
  byte dataByte1_lsbShift = 0x00 | (shifting & 0x7F)  ; //
  //Serial.print("LSB "); Serial.println(dataByte1_lsbShift, BIN);
  Serial.write(dataByte1_lsbShift);
  byte dataByte2_msbShift = 0x00 | ((shifting >>7) & 0x7F) ; //
  //Serial.print("MSB "); Serial.println(dataByte2_msbShift, BIN);
  Serial.write(dataByte2_msbShift);
  return 0;
  
}
int sendProgramChange(byte channel, byte patch){
    if(channel<0 || channel>15 || patch<0 || patch>127)
    return -1; 
  byte statusByte = PROGRAM_CHANGE_CC<<4 | channel ; //
  Serial.write(statusByte);
  byte dataByte1_patch = 0x00 | patch ; //
  Serial.write(dataByte1_patch);
  return 0;
  
}

/*Debug status and active notes*/
void debugPrints(){
  Serial.println("Key State (Pressed or not):");
  
  char buffProgmem [5] ;
  /*PRINT STATUS OF THE BUTTONS (WHETHER PRESSED OR NOT*/
  for(int i= 0 ; i< NUM_KEYS; i++){
    //Get the name of the key from PROGMEM (Program space) to RAM ; strcpy_P(des,origin)
    strcpy_P(buffProgmem, (char*)pgm_read_word(&noteNames[i]));
    Serial.print(buffProgmem); Serial.print("\t :");
      if( keyState[i] == PRESSED ){
        Serial.println("PRESSED");
      }else{
        Serial.println("-");
      }
  }
  Serial.println("Note state (Sound or not):");
  /*PRINT ACTIVE NOTES (WHETHER PRESSED OR NOT */
  for(int i= 0 ; i< NUM_KEYS; i++){
    Serial.print("NOTE");Serial.print(i); Serial.print("\t :");
      if( keyOn_[i] == SOUND ){
        Serial.println("SOUND");
      }else{
        Serial.println("-");
      }
  }
  
  delay (1000);
}

/*CHECK KEY PRESS*/
void manageKeys(byte channel){
  for(int i=0 ; i<NUM_KEYS; i++){
    //Check button pressing
    keyState[i] = digitalRead(KeyPin[i]);
    if(keyState[i] == 0 && keyOn_[i]!=1 ){//Key pressed and No Sound
        sendNoteOn(channel,noteMapping[activeOctave].keyNumber_Midi[i],127);
        keyOn_[i] = 1;// Note is being played
     }
     else if(keyState[i]==FREE && keyOn_[i]==SOUND){
       sendImplicitNoteOff(channel,noteMapping[activeOctave].keyNumber_Midi[i]);
       keyOn_[i] = 0;// Note not heard
     }
  }
}

/* TELL THE NAME FOR AN ACCIDENTAL GIVEN A TYPE OF NOTATION (Flats or Sharps) */
char * getAccidentalNote( char noteNames [] ,byte sharpOrFlat ){
    //for(int l=0; l<noteNames.strlen();l++){
      if(sharpOrFlat == SHARPS){
        noteNames[2]='\0';
      }
      else{
      //change start index to 3
      noteNames = &noteNames[3];
    }
    return noteNames;
}

/* BENDING TRIGGERING */
void handleBending(byte channel){
  
    //Read Y joystick value (value to map to bending)
    valY = analogRead(Y_Joystick);  
    #if EXEC_MODE == 1 //DEBUG
      Serial.print("- Handle Bending- Y:");Serial.println(valY);
    #endif
    float bendValue = 0.0f;
    if(valY>500){
      bendValue =((valY-500)/1024.0f);
      if(bendValue>0.5){
        bendValue=0.5;
      }
      bendValue += 0.5f;
      
    }
    //No bend, joystick is released
    else if(valY==500 || valY == 499){
      bendValue = 0.5f;
    }
    else if(bendValue<499){
      bendValue = ((valY)/1000.0f);
    }
    
    if EXEC_MODE == 1 //DEBUG
      Serial.print("Bend percentage:");Serial.println(bendValue);
    #endif
    
    if(bendValue==0){
      bendValue = bendValue * (float)(1<<14);
    }
    else{
      bendValue = bendValue * (float)(1<<14) -1;
    }
    #if EXEC_MODE == 1 //DEBUG
      Serial.print("Bend value:");Serial.println(bendValue);
    #endif
    
    if(bendValue>8193 || bendValue<8191 ){
      sendPitchBend(1,(unsigned short)bendValue);
    #if EXEC_MODE == 1 //DEBUG
      Serial.print("Bend value:");Serial.println((int)bendValue);
    #endif
    }

  }
/** SHUT ALL NOTES ON OCTAVE CHANGE **/
void shutCurentOctaveNotes(byte channel){
  for(int noteToShut = 0; noteToShut<NUM_KEYS;noteToShut++){
      if(keyOn_[noteToShut] == SOUND){
        sendImplicitNoteOff(channel,noteMapping[activeOctave].keyNumber_Midi[noteToShut]);
      }
  }
}

void octaveChangeHandle(){
  int push = 0;
  valX = analogRead(X_Joystick);
  #if MODE == 1 //DEBUG
  Serial.print("X:");Serial.println(valX);
  #endif
    
  if( (valX == 0 || valX==1023)&& elapsedSinceLast_Oct_Change>2000 ){
    if( valX == 1023){
      if(activeOctave<NUM_OCTAVES){
        //Shut prev scale notes
        shutCurentOctaveNotes(1);
        activeOctave++;
      }
      delay(1000);
    }
    else if( valX == 0){
      if(activeOctave>0){
        shutCurentOctaveNotes(1);
        activeOctave--;
      }
      delay(1000);
    }
    elapsedSinceLast_Oct_Change = millis() - elapsedSinceLast_Oct_Change;
    
    #if MODE == 1 //DEBUG
    Serial.print("Active Octave:");Serial.println(activeOctave);
    delay(1000);
    #endif
  }
  
  elapsedSinceLast_Oct_Change = millis();

  
}

void handleModeChanger(){


   //HOLD PUSH FUNCTIONALITY
  /* if(digitalRead(JOY_PUSH) == PRESSED && pushPrevStatus != PRESSED){ //NO PRESS-->PRESS (Pressing)
    pushCurrStatus = PRESSED;
    pushStartTime = millis();
    pushPrevStatus = pushCurrStatus;
   }
   else if(digitalRead(JOY_PUSH) == FREE && pushPrevStatus == PRESSED){//PRESS --> NO PRESS (Releasing)
      pushCurrStatus = FREE;
      pushEndTime = millis();
      elapsedPushing = pushEndTime - pushStartTime;
      if(elapsedPushing <2000){
        
        Serial.print("PUSHED FOR LESS THAN: ");Serial.print(elapsedPushing);
      }
      if( elapsedPushing> 2000 && elapsedPushing <4000){
        Serial.print("PUSHED FOR AROUND 2s: ");Serial.print(elapsedPushing);
      }
      else if(elapsedPushing >4000) {
        Serial.print("PUSHED FOR MORE THAN 4s: ");Serial.print(elapsedPushing);
    }
      pushPrevStatus = FREE;
   }*/
   

  //PRESSED FOR THE FIRST TIME
  pushCurrStatus = digitalRead(JOY_PUSH);
  
  if(pushCurrStatus == PRESSED && pushPrevStatus != PRESSED){ //NO PRESS-->PRESS (Pressing)
    pushStartTime = millis();
    pushPrevStatus = pushCurrStatus;
   }
    else if(pushCurrStatus == FREE && pushPrevStatus == PRESSED){//PRESS --> NO PRESS (Releasing)
      pushEndTime = millis();
      elapsedPushing = pushEndTime - pushStartTime;
      if( elapsedPushing <2000 && elapsedPushing>0 ){ //Release happens before 2 seconds (Check for multi-push action)
        clickingTimerBegin = millis();
        timesPushed++;
        //Serial.print("Rapid Push! Timing: ");Serial.println(elapsedPushing);
      }
      if( elapsedPushing> 2000 && elapsedPushing <4000){
        Serial.print("PUSHED FOR MORE THAN 2s: ");Serial.println(elapsedPushing);
      }
      else if(elapsedPushing >4000) {
        Serial.print("PUSHED FOR MORE THAN 4s: ");Serial.println(elapsedPushing);
      }
      pushPrevStatus = pushCurrStatus;
    }
    else{
      pushPrevStatus = pushCurrStatus;
    }
   
}

void execMultiTrigger(){
  switch(timesPushed){
    case 1: Serial.println("One Rapid Click");break;
    case 2: Serial.println("Two Rapid Clicks");break;
    case 3: Serial.println("Three Rapid Clicks");break;
  }
  timesPushed = 0;
}




void setup() {
   #if MODE == 0
  Serial.begin(115200);
  #elif MODE == 1
  Serial.begin(9600);
  #endif
  
  
  for(int octave=0; octave<NUM_OCTAVES; octave++){
    #if MODE == 1
    Serial.print("### OCTAVE STARTUP:"); Serial.println(octave);
    #endif
    for(int note=0; note<NUM_KEYS; note++){
      char note_Name_Buffer[5];//Buffer to take out the proper name of all notes
      strcpy_P(note_Name_Buffer, (char*)pgm_read_word(&noteNames[note]));
      if(strlen(note_Name_Buffer)<2){//No accidentals
            //Copy Name of the Note as it is
            strcpy(noteMapping[octave].noteName[note],note_Name_Buffer);
      }
      else{ //Accidentals
             char * accNoteName = getAccidentalNote(note_Name_Buffer,accidentals_Mode);
             strcpy(noteMapping[octave].noteName[note],accNoteName);
      }
      #if MODE == 1 
        Serial.print(noteMapping[octave].noteName[note]);Serial.print("\t NoteNumber:");
      #endif
      //Set Midi Key Value
      noteMapping[octave].keyNumber_Midi[note] = (octave*NUM_KEYS) + note;
      #if MODE == 1 
      Serial.println(noteMapping[octave].keyNumber_Midi[note]);
       delay(500);
      #endif
      }
  }
  
  for(int i=0;i<NUM_KEYS;i++){
    pinMode(KeyPin[i],INPUT_PULLUP);
  }
  
  pinMode(Y_Joystick,INPUT);
  pinMode(X_Joystick,INPUT);
  pinMode(JOY_PUSH,INPUT_PULLUP);
  
}

void loop() {
  
  #if MODE == 1
   debugPrints();
  #endif

  if(opMode == BENDING_MODE){
    octaveChangeHandle();
    manageKeys(1);
    handleBending(1);
    handleModeChanger();
  }  
  else{
     
    if( (millis() - clickingTimerBegin > multiPushTimeThresh) && timesPushed!=0){ //As the pushing has been done several times within a time, as that time expires, the multi trigger counter is restarted 
                                                                           // along with the rest of the variables involved.
      execMultiTrigger();
    }
    handleModeChanger();
    
  }
  
}

