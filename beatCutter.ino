#include <MIDI.h>
#include <Bounce.h>

//CUT A 4 BAR LOOP in 16 pieces,
//assign them sequentially from C1 in sampler
//enjoy
//analog 0 maxPhraselength
//analog 1 repeatChance 
//analog 2 unitsInCut 
//analog 3 maxRepeats 

//STORES NOTEON FOR LATER NOTEOFF
#define VOICE_COUNT 16
byte NOTE_ON[VOICE_COUNT];
byte NOTE_CHAN[VOICE_COUNT];
unsigned long NOTE_OFF[VOICE_COUNT];
byte led=7;
//CLOCK STUFF
long _clockCounter = -1;
long _posCounter = 0;
byte clockInState = LOW ;
byte clockState = LOW ;
long lastClock = millis();
long currentMillis = millis();

byte division=1;
byte divTemp=division;
byte rootNote=36;
byte tempoLED=7;



//DEBOUNCE DIGITAL INS
byte debounce= 5; //DEOUCE TIME
Bounce shift= Bounce( 6,  debounce); 

Bounce divdown= Bounce( 2,  debounce); 
Bounce divup= Bounce( 3,  debounce); 

Bounce notedown= Bounce( 5,  debounce); 
Bounce noteup= Bounce( 4,  debounce); 


// /Cutter STUFF
byte subDiv= 16;
byte maxPhraselength=1;
float repeatChance= 0.00;
byte unitsInCut=1;
byte maxRepeats=1;
byte unitsLeft=0;
byte unitsDone=0;
byte totalUnits=0;
byte barsNow;
byte repeats=0;
byte repeatsDone=0;
boolean repeating=false;
byte index=0;
///
byte segs[64];
byte indx[64];
byte dindx[64];

byte segsCount=0;
byte indxCount=0;
byte dindxCount=0;
byte dindxTotal=0;

///

void setup() {
  setupMidi();                                  // set baud rate to midi
  pinMode(tempoLED, OUTPUT);  
  cutter();
}

void loop() {
    //clockIn(8);
   // switchLed();
  MIDI.read();
  if(millis()>currentMillis+(1000/map(analogRead(5), 0,1023, 2,140))){
      //midiLoop();
     // tickClockLed(3*division);
      currentMillis=millis();
      HandleClock();
      trace( 56, maxPhraselength);
	trace( 57, int(repeatChance) );
	trace( 58, unitsInCut );
	trace( 59, maxRepeats );
  }
  // control();
 
}

void cutter(){
	maxPhraselength = map(analogRead(0), 0, 1023, 1, 4); 
	repeatChance = map(analogRead(1), 0, 1023, 0, 99); 
	unitsInCut = map(analogRead(2), 0, 1023, 1, 4); 
	maxRepeats = map(analogRead(3), 0, 1023, 1, 8); 
    
	segsCount=0;
	indxCount=1;
	dindxCount=0;
	barsNow=byte(random(1, maxPhraselength));
	totalUnits=byte(barsNow*subDiv);
	unitsDone=0;
	repeats=0;
	repeatsDone=0;
	index=0;
	indx[0]=(0);
	while(unitsDone<totalUnits){
       unitsLeft= byte(totalUnits - unitsDone);
       if(repeatsDone==repeats){
           repeatsDone=0;
           if((unitsLeft<(subDiv/2)) && (random(0,100) < repeatChance)){
                unitsInCut=1;
          	repeats=unitsLeft;
           } else {
               byte temp, max;
               temp=byte(subDiv/2);
               if(temp % 2==0){
                   max=byte((temp-2)/2);
               } else {
                   max=byte((temp-1)/2);
               }
               unitsInCut=byte(random(0,max));
             unitsInCut=byte(2*unitsInCut+1);
            repeating=true;
            while(unitsInCut>unitsLeft){
              unitsInCut=byte(unitsInCut-2); 
            }
            repeats=byte(random(1, maxRepeats));
            if(repeats*unitsInCut>unitsLeft){
                unitsInCut=unitsLeft;
            repeats=1;

            }
           }
           
          index= byte((index+unitsInCut)%subDiv);
         
       }

      //segs.push(int(unitsInCut));
     // println(unitsInCut);
      segs[segsCount]=(byte(unitsInCut));
      // segs = append(segs, byte(unitsInCut));
      segsCount++;
     
      // indx=append(indx, index);
      indx[indxCount]=(byte(index));

      indxCount++;
      unitsDone=byte(unitsDone+unitsInCut);
      repeatsDone=byte(repeatsDone + 1);

    }  
    //printArray(segs);
    //println("vvvvvvvvvv");
    //printArray(indx);
    int ctr=0;
    for(int s=0; s<segsCount; s++){
      
        for(int i=0; i<segs[s]; i++){
         //dindx[i]=byte((i+ctr)%subDiv);
         ctr=indx[s];
         // dindx[dindxCount]=( dindxCount);
         dindx[dindxCount]=( byte((i+ctr)%subDiv));
          
         dindxCount++;
         //dindx = append(dindx, byte((i)));
        
        }
    }

dindxTotal = dindxCount;
// printArray(sequence);
// println("------------");
}

void runCutter(){
	if(_posCounter%(dindxTotal)==0){
		cutter();
		trace( 56, maxRepeats);
	}
	byte note = rootNote+(dindx[_posCounter%dindxTotal]);
	playNote(note,127, 500, 10	);
	

	
	//trace( 57, note-rootNote);
}

void switchLed(){
 if ((_clockCounter % (3)) == 0) {
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
 trace(10, maxRepeats );
  }else{
  	 digitalWrite(led, HIGH); 


  }


}
void tickClockLed(int divi){
  if ((_clockCounter % (divi)) == 0) {
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
// trace(10, 123 );
  }

  // reset trigger after number+2 CLOCK_DIVIDER ticks
  if (((_clockCounter+2) % (divi)) == 0) {
    // wait for a second
    digitalWrite(led, LOW);   
   // turn the LED off by making the voltage LOW

  }
}
void midiLoop(){
	if(_clockCounter%(3*division)==0){
	//trace(56, _clockCounter);
	//trace(57, _posCounter);

	//Play();
	//playNote(36, 127, 1000, 10);
	runCutter();
	_posCounter++;
	}
	
	//trace(66, 12 );ç 
  	noteKill();
  	
  	//partCheck();
}

void HandleClock() {
	
 
  midiLoop();
  tickClockLed(3*division);

	SendMidiClockTick();
  
   _clockCounter++;
}


void SendMidiClockTick(){

	MIDI.sendRealTime( Clock );
	
}
///MIDI NOTE FUNCS

void noteKill(){
unsigned long time=millis();
for (int i=0;i<VOICE_COUNT;i++){
if (NOTE_ON[i]>0 && NOTE_OFF[i]<time){  // if note value>0 and note off time < time
 
	byte pitch = NOTE_ON[i];
	byte velocity = 0;
	MIDI.sendNoteOff(pitch, velocity, NOTE_CHAN[i]);
	NOTE_ON[i]=0; //wipe NOTE_ON entry to signal no active note
}}}

void playNote(byte pitch, byte velocity, int duration, byte chan) {
unsigned long time = millis();
// find an empty note slot 
boolean found_slot = false;
for (int i=0;i<VOICE_COUNT;i++){
if (NOTE_ON[i] == 0){
  
  NOTE_ON[i] = pitch;
  NOTE_OFF[i] = time+duration;
  NOTE_CHAN[i] = chan;
  found_slot = true;
  
  break;
}
}

// RED LIGHT SHOWS NOTE BUFFER IS OVERFULL 
if (found_slot == false){
//digitalWrite (7, HIGH);
}
else{
//-  digitalWrite (7, LOW);
 MIDI.sendNoteOn(pitch, velocity, chan);
};

}

void setupMidi(){
  MIDI.begin();
  MIDI.setHandleClock ( HandleClock );
  MIDI.setHandleStart(reset);
  
}
void reset(){
_clockCounter=0;
_posCounter=0;
}
void trace(byte cc, byte val){
	 MIDI.sendControlChange(cc,val,16);
}
	 
