//Libraries
#include <Adafruit_Fingerprint.h>

//this is the serial through which the arduino 101 transmits and receives
#define mySerial Serial1

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);  //use finger in order to interact with fingerprint scanner
int voters[128];      //slots in scanner range from 1-127
int choice1 = 0;
//probably setup names in setup, through prompt
String name1;
int choice2 = 0;
String name2;

const int startPin = 5;
const int whitePin = 7; //choice 1
const int bluePin = 2;  //choice 2

//variables will change
int startButtonState = 0;
int whiteButtonState = 0;
int blueButtonState = 0;

//variable waiting for a button to be pressed in vote()
boolean stat = true;

//only runs once at the start
void setup() {
  
  Serial.begin(9600);

  //initialize voters array[128]
  for(int i =0;i<128;i++){
    //initialize all slots as "not-voted"
    voters[i] = 0;  
  }
  
  while(!Serial);//make sure its ready
  
  Serial.println("Welcome to election day 2018!");
  Serial.println("\n\nMaking sure fingerprint scanner is installed...");

  //initialize serial interface with fingerprint
   finger.begin(57600);   
  
  //checks if the sensors are active
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint scanner ready for duty!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  } 

  //initializing the button inputs
  pinMode(startPin, INPUT);

  pinMode(bluePin, INPUT);

  pinMode(whitePin, INPUT);

}

//Code that gets repeated over and over
void loop() {
  Serial.println("\nPlease press start to vote.");

  //get button input for start and put here
   do{
     startButtonState = digitalRead(startPin);
   }while(startButtonState != HIGH);

  //get slot of finger scanned, if not enrolled returns 0, -1 if error, slot if good
  Serial.println("Please place your right thumb on the scanner to vote.");
  int inSlot = getFinger();
  delay(50);

  //go into procedures depending on if enrolled or not

  //if finger was found
  if(inSlot>0){

    //if person is eligible to vote
    if(isEligible(inSlot)) vote(inSlot);
    
    else Serial.println("You have already voted, please don't vote again.");
  }

  //if finger was not matched
  else if(inSlot == 0) Serial.println("Finger not registered, please contact a representative.");

  else Serial.println("We are experiencing technical difficulties.");
}

/*
 * vote
 * records a vote for one candidate or another, increments the total, and marks the id as voted
 */
void vote(int voterID){
  Serial.println("Please cast a vote for your candidate.");
  /* check for user press
   * confirm vote casting
   * increment global variable for candidate depending on choice
   */
   while(stat != false){
    whiteButtonState = digitalRead(whitePin);
    blueButtonState = digitalRead(bluePin);

    if(whiteButtonState == HIGH){
      stat = false;
      choice1++;
    }
    if(blueButtonState == HIGH){
      stat = false;
      choice2++;
    }
  }

  //update voting status of voter
  voters[voterID] = 1; 

  Serial.println("Your vote was casted!");
  stat = true;
}

/*
 * isEligible
 * Checks array to see if an enrolled voter has already voted or not
 * Array slot at the id contains a 1 if he already voted or a 0 if he did not
 * Returns boolean true if and only if a voter can vote
 */
boolean isEligible(int voterID){
  return voters[voterID] == 0;
}

/*
* getFinger
* Get finger picture and return status:
* int >0--> already in the system
* String 0 --> not in system
*/
int getFinger(){

  //get a picture from finger
  uint8_t p=-1;
  while(p!=FINGERPRINT_OK){
  p = finger.getImage();
  }

  //make sure image was taken
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -1;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return -1;
    default:
      Serial.println("Unknown error");
      return -1;
  }

  //change pic into template
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -1;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return -1;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return -1;
    default:
      Serial.println("Unknown error");
      return -1;
  }

  //try finding finger in our stored finger scans
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a match.");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return -1;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return 0;
  } else {
    Serial.println("Unknown error");
    return -1;
  }
  
  //if found a match, return the slot # (id)
  return (int) finger.fingerID;
}



