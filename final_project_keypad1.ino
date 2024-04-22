// Keypad-Based Security System
// Isaiah Wallace
// December 13th, 2023
#include <Keypad.h>
const byte ROWS = 4;       //Four rows of keypad
const byte COLS = 4;       //Four columns of keypad
char keys[ROWS][COLS] = {  //matrix for the keypad grid
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 7, 6, 5, 4 };    //connect to the row pinouts of the keypad
byte colPins[COLS] = { 3, 2, A5, A4 };  //connect to the column pinouts of the keypad
String v_passcode = "";                 //stores typed attempt at regular password to check against password to see if it's correct
String currentCode = "";                //used to store password for display in system settings
String passCode = "1234*";              //starting passcode
String adminPass = "4321*";             //unchanging admin password
String adminAttempt = "";               //where the admin attempt password is stored to check if it's correct
int RLED = A3;
int GLED = A2;
int lockoutAttempts = 0;  //tracks how many failed password attemps user has
int lockoutSetting = 5;   //failed attempts before lockout is activated
bool Unlocked = false;    //while loop condition when system is unlocked
bool password = false;    //while loop for typing the password
bool intro = false;
bool option3 = false;
bool disabled = false;
bool lockedOut = false;  //lockout while loop
bool passwordChange = false;
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
//mapping the keypad using the prebuilt matrix

void setup() {
  Serial.begin(9600);
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
}

void loop() {
  start();
}


void start() {                 //initializes everything, the start up function
  char key = keypad.getKey();  //using the char key to get the pressed key from the keypad
  Serial.println("At Home Keypad Lock System");
  Serial.println("Press Any Key To Activate The System.");
  intro = true;
  while (intro == true) {
    char key = keypad.getKey();
    if (key != NO_KEY) {  //if key press is detected
      intro = false;
    }
  }
  Serial.println("Enter Your Password:\nThen Press '*' To Verify It");
  password = true;
  v_passcode = "";  //clears attempt passcode to store attempt
  while (password == true) {
    enterPassword();
  }
}

void enterPassword() {  //lets user enter their password and checks if it's correct
  char key = keypad.getKey();
  if (key != NO_KEY) {
    v_passcode = v_passcode + key;  //adds what was typed by user to the attempt string
    if (key == '*') {
      Serial.println(v_passcode);
      if (v_passcode == passCode || v_passcode == adminPass) {  //compares attempt to passcode and admin code
        Serial.println("Access Granted");
        lockoutAttempts = 0;  //resets lockout attempts once access is granted
        unlockedOptions();    //shows system settings
      } else {
        Serial.println("Access Denied");
        v_passcode = "";
        digitalWrite(RLED, HIGH);
        digitalWrite(GLED, LOW);
        lockoutAttempts++;                        //adds to lockout attempts on each failed password attempt
        if (lockoutAttempts == lockoutSetting) {  //checks on each failed attempt if attempts = the lockout conditions
          Serial.println("Due To Too Many Failed Attempts, The System Has Locked Down.");
          Serial.println("Type The Admin Password To Regain Access: ");
          lockedOut = true;
          while (lockedOut == true) {
            char key = keypad.getKey();
            if (key != NO_KEY) {
              adminAttempt = adminAttempt + key;  //stores typed attempt to check if it's correct
              if (key == '*') {                   //checks typed attempt
                Serial.println(adminAttempt);     //prints attempt
                if (adminAttempt == adminPass) {  //checks against the admin code
                  Serial.println("Admin Password Correct \nAccess To The System Has Been Granted");
                  lockoutAttempts = 0;  //resets lockout attempts
                  unlockedOptions();    //takes user to system settings
                } else {
                  Serial.println("Incorrect Admin Password.");
                  adminAttempt = "";  //clears the stored attempt for the next attempt
                }
              }
            }
          }
        }
      }
    }
  }
}

void unlockedOptions() {  //system settings to alter the default settings
  digitalWrite(RLED, LOW);
  digitalWrite(GLED, HIGH);
  Serial.println("System Settings");
  Serial.println("1. View The Current Passowrd");
  Serial.println("2. Change The Current Password");
  Serial.println("3. Add a Lockout Based On Number Of Failed Atempts");
  Serial.println("4. View Admin Password");
  Serial.println("5. Turn Off System");
  Unlocked = true;
  while (Unlocked == true) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      if (key == '1') {  //user can view the current password
        Serial.println("Current Password: ");
        currentCode = v_passcode;  //stores the current passcode
        Serial.println(currentCode);
        unlockedOptions();      //Takes user back to the settings
      } else if (key == '2') {  //allows user to change passcode
        Serial.println("Type The New Password Into The Keypad\n,Then Press '*'");
        v_passcode = "";  //clears old attempt
        passwordChange = true;
        while (passwordChange == true) {
          char key = keypad.getKey();
          if (key != NO_KEY) {
            v_passcode = v_passcode + key;  //stores the new passcode
            passCode = "";                  //clears old code for the new one to be stored
            if (key == '*') {
              passCode = passCode + v_passcode;  //storing the passcode
              Serial.println("Password has Been Change To: ");
              Serial.println(passCode);  //prints new code
              unlockedOptions();         //user goes to settings after
            }
          }
        }
      } else if (key == '3') {  //lets user change the failed attempts needed before lockout
        Serial.println("Current Needed Failed Attempts For Lockout: ");
        Serial.println(lockoutSetting);
        Serial.println("Press '#' To Change It To 3");
        Serial.println("Press '*' To Change It To 5");
        Serial.println("Press 'D' To Turn The Setting Off");
        char keyPressed = keypad.waitForKey();
        if (keyPressed == '#') {  //changed from 5(default) to 3
          Serial.println("Attempts Needed Changed To: \n3");
          lockoutSetting = 3;
          unlockedOptions();
        } else if (keyPressed == '*') {  //changed to default(5)
          Serial.println("Attempts Needed Changed To: \n5");
          lockoutSetting = 5;
          unlockedOptions();
        } else if (keyPressed == 'D' && disabled == false) {  //disables lockout
          Serial.println("Lockout Protocol Has Been Disabled");
          lockoutSetting = 10000;
          disabled = true;
          unlockedOptions();
        } else if (keyPressed == 'D' && disabled == true) {  //re-enables the lockout
          Serial.println("Lockout Protocol Has Been Re-enabled, \nSet Lockout to Default Attempts: \n5");
          lockoutSetting = 5;
          disabled = false;
          unlockedOptions();
        } else {  //incorrect input will send user back to the settings
          Serial.println("Incorrect Input, Back to The Menu");
          unlockedOptions();
        }
      } else if (key == '4') {  //shows admin password and talks about it brief, the functions of it
        Serial.println("The Admin Password is Unchangeable");
        Serial.println("It Will Always Work And Can Be Used After A Lockout");
        Serial.println(adminPass);
        unlockedOptions();
      } else if (key == '5') {  //turns off user and prompts them to boot the system up again
        start();                //the startup function
      }
    }
  }
}
