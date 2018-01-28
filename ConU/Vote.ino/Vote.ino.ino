//Libraries
#include <Adafruit_Fingerprint.h>
// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
// uncomment this line:
#define mySerial Serial1

// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// comment these two lines if using hardware serial
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);  //use finger in order to interact with fingerprint scanner
int voters[128];      //slots in scanner range from 1-127
int choice1 = 0;
//probably setup names in setup, through prompt
String name1;
int choice2 = 0;
String name2;

//only runs once at the start
void setup() {
  
  Serial.begin(9600);

  //initialize voters array[128]
  for(int i =0;i<128;i++){
    //initialize all slots as "not-voted"
    voters[i] = 0;  
  }
  
  while(!Serial);//make sure its ready
  
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

  /*
   * TODO: Set up buttons
   */

}

//Code that gets repeated over and over
void loop() {
  Serial.println("\nPlease press start to vote.");

  /*
   * get button input for start and put here
   */

  //get slot of finger scanned, if not enrolled returns 0, -1 if error, slot if good
  int inSlot = getFinger();
  delay(50);

  //go into procedures depending on if enrolled or not

  //if finger was found
  if(inSlot){

    //check if isEligible to vote
    if(inSlot){
      if(isEligible(inSlot)) vote(inSlot);
      else Serial.println("You have already voted, please don't vote again.");
    }
    //if they are not enrolled, enroll them and vote
    else if(inSlot==0){
      int newSlot = enroll();
      if(newSlot){
        Serial.println("Finger successfully enroled");
        vote(newSlot);
      }
      else Serial.println("Could not enroll, try again");
    }
    //there was an error, so print it out and then
    else{
      Serial.println("We are experiencing technical and spiritual difficulties :( Please try again.");
    }
  }
}

/*
 * enroll
 * starts the process to enroll a finger that was not found.
 * returns slot number if successful, or 0 if there was an error
 */
int enroll(){

  //get current amount of fingers in scanner
  finger.getTemplateCount();
  int numberOfEntries = finger.templateCount;

  if
  
}

/*
 * vote
 * records a vote for one candidate or another, increments the total, and marks the id as voted
 */
void vote(int voterID){
  Serial.println("Please cast a vote for your candidate.")
  /*TODO: check for user press
   * confirm vote casting
   * increment global variable for candidate depending on choice
   */

  //update voting status of voter
  voters[voterID] = 1; 
}

/*
 * isEligible
 * Checks array to see if an enrolled voter has already voted or not
 * Array slot at the id contains a 1 if he already voted or a 0 if he did not
 * Returns boolean true if and only if a voter can vote
 */
boolean isEligible(int voterID){
  return voters[voterID] == 0
}

/*
* getFinger
* Get finger picture and return status:
* int >0--> already in the system
* String 0 --> not in system
*/
int getFinger(){

  //get a picture from finger
  uint8_t p = finger.getImage();

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
      Serial.println("Image converted");
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
    Serial.println("Found a print match!");
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

