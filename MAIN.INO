
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial Gsm(2, 3); // RX, TX

#define RING_LED 13

// ------------------- Keypad setup -------------------
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {4, 5, 6, 7};
byte colPins[COLS] = {8, 9, 10, 11};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ------------------- Fixed Numbers -------------------
String fixedNumbers[5] = {
  "8953422***", "89*********", "6394***29", "9***819140", "7355***280"
};
String fixedNames[5] = {
  "*******", "*******", "*******", "********", "**********"// Add Names Accordingly 
};

// ------------------- Added Variables -------------------
unsigned long callStartTime = 0;
bool callActive = false;
unsigned long lastGsmActivity = 0;
const unsigned long idleTimeout = 20000; // 20 sec idle -> show developer info
bool showingDeveloper = false;

// for non-blocking developer scroll
unsigned long lastScroll = 0;
int scrollIndex = 0;
String developerText = "Mecatredz Softwares And Innovations Pvt Ltd Lucknow  Thanks For Choosing MT ";

// ------------------- Function Declarations -------------------
void manualDial();
void sendSMS();
void sendSMSMessage(String number, String message);
void makeCall(String number);
void showMainScreen();

// ------------------- Setup -------------------
void setup() {
  Serial.begin(9600);
  Gsm.begin(9600);
  pinMode(RING_LED, OUTPUT);
  digitalWrite(RING_LED, LOW);
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  GSM Phone");
  lcd.setCursor(0,1);
  lcd.print("  Ready...");
  delay(2000);

  Gsm.println("AT");
  delay(1000);
  
  showMainScreen();
  lastGsmActivity = millis();
}

// ------------------- Non-blocking Scroll -------------------
void showDeveloperInfo() {
  if (!showingDeveloper) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Developed At:");
    showingDeveloper = true;
    scrollIndex = 0;
  }
  if (millis() - lastScroll > 300) {
    lastScroll = millis();
    lcd.setCursor(0,1);
    if (scrollIndex + 16 <= developerText.length()) {
      lcd.print(developerText.substring(scrollIndex, scrollIndex + 16));
    } else {
      lcd.print(developerText.substring(scrollIndex) + "                ");
    }
    scrollIndex++;
    if (scrollIndex > developerText.length()) scrollIndex = 0;
  }
}

// ------------------- Main Loop -------------------
void loop() {
  // ---------- Non-blocking Developer Display ----------
  if (millis() - lastGsmActivity > idleTimeout && !callActive) {
    showDeveloperInfo();
  } else if (showingDeveloper && (Gsm.available() || keypad.getKey())) {
    showingDeveloper = false;
    lcd.clear();
    showMainScreen();
  }

  // ---------- GSM Data Handling ----------
  if (Gsm.available()) {
    lastGsmActivity = millis();
    showingDeveloper = false;
    String data = Gsm.readString();
    Serial.println(data);

    //  Added feature: Show incoming number and name if match found
    if (data.indexOf("+CLIP:") != -1) {
      int start = data.indexOf("\"") + 1;
      int end = data.indexOf("\"", start);
      String callerNumber = data.substring(start, end);
      String callerName = callerNumber;

      for (int i = 0; i < 5; i++) {
        if (fixedNumbers[i] == callerNumber) {
          callerName = fixedNames[i];
          break;
        }
      }

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Incoming Call:");
      lcd.setCursor(0,1);

      // scroll if >16 chars
      String displayText = callerName + " " + callerNumber;
      if (displayText.length() <= 16) {
        lcd.print(displayText);
      } else {
        for (int i = 0; i < displayText.length() - 15; i++) {
          lcd.setCursor(0,1);
          lcd.print(displayText.substring(i, i + 16));
          delay(300);
        }
      }
    }

    // Incoming ring
    if (data.indexOf("RING") != -1) {
      static unsigned long ringBlink = 0;
      bool blinking = true;
      unsigned long ringStart = millis();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Incoming Call!");
      lcd.setCursor(0,1);
      lcd.print("A:Ans D:Reject");

      while (millis() - ringStart < 15000) {
        if (millis() - ringBlink > 300) {
          ringBlink = millis();
          digitalWrite(RING_LED, !digitalRead(RING_LED));
        }
        char key = keypad.getKey();
        if (key == 'A') {
          Gsm.println("ATA");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Call Connected");
          lcd.setCursor(0,1);
          lcd.print("D:End Call");
          callActive = true;
          callStartTime = millis();
          digitalWrite(RING_LED, LOW);
          blinking = false;
          break;
        } else if (key == 'D') {
          Gsm.println("ATH");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Call Rejected");
          delay(1500);
          showMainScreen();
          digitalWrite(RING_LED, LOW);
          blinking = false;
          break;
        }
        if (Gsm.available()) {
          String update = Gsm.readString();
          if (update.indexOf("NO CARRIER") != -1) {
            blinking = false;
            break;
          }
        }
      }
      if (blinking) digitalWrite(RING_LED, LOW);
      if (!callActive) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Missed Call");
        delay(2000);
        showMainScreen();
      }
    }

    // Call ended
    if (data.indexOf("NO CARRIER") != -1 && callActive) {
      callActive = false;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Call Ended");
      delay(1000);
      showMainScreen();
    }
  }

  // ---------- Call Timer ----------
  if (callActive) {
    unsigned long elapsed = (millis() - callStartTime) / 1000;
    lcd.setCursor(0,1);
    lcd.print("Time: ");
    lcd.print(elapsed);
    lcd.print("s   ");
  }

  // ---------- Keypad Handling ----------
  char key = keypad.getKey();
  if (key) {
    lastGsmActivity = millis();
    showingDeveloper = false;
    switch (key) {
      case 'D':
        Gsm.println("ATH");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Call Ended");
        delay(1000);
        callActive = false;
        showMainScreen();
        break;

      case 'C': manualDial(); break;
      case 'B': sendSMS(); break;
      case '1': makeCall(fixedNumbers[0]); break;
      case '2': makeCall(fixedNumbers[1]); break;
      case '3': makeCall(fixedNumbers[2]); break;
      case '4': makeCall(fixedNumbers[3]); break;
      case '5': makeCall(fixedNumbers[4]); break;
    }
  }
}

// ------------------- Manual Dial -------------------
void manualDial() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter Number:");
  lcd.setCursor(0,1);
  lcd.print("#=Call *=Clear");

  String number = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        if (number.length() >= 10) {
          makeCall(number);
          break;
        } else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Invalid Number");
          delay(1500);
          showMainScreen();
          break;
        }
      } else if (key == '*') {
        number = "";
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
      } else if (key >= '0' && key <= '9') {
        if (number.length() < 16) {
          number += key;
          lcd.setCursor(number.length()-1, 1);
          lcd.print(key);
        }
      }
    }
    if (Gsm.available()) break;
  }
}

// ------------------- Send SMS -------------------
void sendSMS() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SMS Number:");
  lcd.setCursor(0,1);
  lcd.print("#=Next *=Clear");

  String number = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        if (number.length() >= 10) break;
        else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Invalid Number");
          delay(1500);
          showMainScreen();
          return;
        }
      } else if (key == '*') {
        number = "";
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("#=Next *=Clear");
      } else if (key >= '0' && key <= '9') {
        if (number.length() < 16) {
          number += key;
          lcd.setCursor(number.length()-1, 1);
          lcd.print(key);
        }
      }
    }
    if (Gsm.available()) return;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Type Message:");
  lcd.setCursor(0,1);
  lcd.print("#=Send *=Clear");

  String message = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        if (message.length() > 0) {
          sendSMSMessage(number, message);
          break;
        }
      } else if (key == '*') {
        if (message.length() > 0) {
          message.remove(message.length()-1);
          lcd.setCursor(0,1);
          lcd.print("                ");
          lcd.setCursor(0,1);
          lcd.print(message);
        }
      } else {
        if (message.length() < 16) {
          message += key;
          lcd.setCursor(0,1);
          lcd.print("                ");
          lcd.setCursor(0,1);
          lcd.print(message);
        }
      }
    }
    if (Gsm.available()) return;
  }
}

// ------------------- Make Call -------------------
void makeCall(String number) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Calling...");
  lcd.setCursor(0,1);
  String contact = number;
  for (int i = 0; i < 5; i++) {
    if (fixedNumbers[i] == number) {
      contact = fixedNames[i];
      break;
    }
  }
  lcd.print(contact);

  Gsm.println("ATD" + number + ";");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connected");
  lcd.setCursor(0,1);
  lcd.print("D:End Call");
  callActive = true;
  callStartTime = millis();
}

// ------------------- Send SMS Message -------------------
void sendSMSMessage(String number, String message) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sending SMS...");

  Gsm.println("AT+CMGF=1");
  delay(1000);
  Gsm.print("AT+CMGS=\"");
  Gsm.print(number);
  Gsm.println("\"");
  delay(1000);
  Gsm.print(message);
  Gsm.write(26);
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SMS Sent!");
  delay(1500);

  showMainScreen();
}

// ------------------- Show Main Screen -------------------
void showMainScreen() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press1-5:CallFN.");
  lcd.setCursor(0,1);
  lcd.print("C:Dial B:SMS");
}
