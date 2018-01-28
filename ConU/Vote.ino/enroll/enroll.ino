#include <Adafruit_Fingerprint.h>


#define mySerial Serial1


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;
const int startPin = 5;

//variables will change
int startButtonState = 0;

void setup()  
{
  Serial.begin(9600);
  while (!Serial); 
  delay(100);
  Serial.println("\n\nWelcome to fingerprint enrollment!");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("\nFound fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor.");
    while (1) { delay(1); }
  }
  //initializing the button inputs
  pinMode(startPin, INPUT);
}


void loop(){

   Serial.println("\nPlease press start to enroll a finger.");

  //get button input for start and put here
   do{
     startButtonState = digitalRead(startPin);
   }while(startButtonState != HIGH);
   
  finger.getTemplateCount();
  int numberOfEntries = (int) finger.templateCount;
  
  id = numberOfEntries+1;
  if(id>=127) Serial.println("Max memory reached, cannot enroll any more fingers.");
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  while (!  getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;

  //take picture
  Serial.print("\nPlace right thumb to enroll...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // convert image to template

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  //prepare to take second picture
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  
  p = -1;
  Serial.println("Place same finger again...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  
  //create second template from picture
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // Make model to store
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored! Thank you!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}


