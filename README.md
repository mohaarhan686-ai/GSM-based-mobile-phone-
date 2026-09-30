#  Arduino GSM Phone

A DIY GSM-based mobile phone project built using **Arduino, GSM Module, 16×2 I2C LCD, and 4×4 Matrix Keypad**.

The system supports phone calls, incoming-call handling, SMS sending, predefined contacts, call timing, and keypad-based control.

## ✨ Features

* 📞 Make phone calls
* 📲 Receive incoming calls
* ☎️ Answer incoming calls
* ❌ Reject/end calls
* ⏱️ Call duration timer
* 💬 Send SMS
* 👥 Predefined contacts
* 🔢 Manual number dialing
* 📟 16×2 I2C LCD display
* 🔔 Incoming-call LED indicator
* 📜 Scrolling developer information
* 🎛️ Keypad-based operation

## 🛠️ Hardware Required

| Component             |    Quantity |
| --------------------- | ----------: |
| Arduino UNO           |           1 |
| GSM Module            |           1 |
| 16×2 I2C LCD          |           1 |
| 4×4 Matrix Keypad     |           1 |
| LED                   |           1 |
| Resistor              |           1 |
| SIM Card              |           1 |
| Jumper Wires          | As required |
| Suitable Power Supply |           1 |

## 🔌 Pin Configuration

### GSM Module

```cpp
SoftwareSerial Gsm(2, 3); // RX, TX
```

| Arduino | GSM    |
| ------- | ------ |
| D2      | GSM TX |
| D3      | GSM RX |
| GND     | GND    |

### I2C LCD

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 2);
```

| LCD | Arduino UNO |
| --- | ----------- |
| VCC | 5V          |
| GND | GND         |
| SDA | A4          |
| SCL | A5          |

### 4×4 Keypad

| Keypad | Arduino |
| ------ | ------- |
| R1     | D4      |
| R2     | D5      |
| R3     | D6      |
| R4     | D7      |
| C1     | D8      |
| C2     | D9      |
| C3     | D10     |
| C4     | D11     |

### Ring LED

```cpp
#define RING_LED 13
```

| Component | Arduino |
| --------- | ------- |
| LED       | D13     |

## 🎛️ Keypad Controls

| Key | Function             |
| --- | -------------------- |
| `1` | Call Contact 1       |
| `2` | Call Contact 2       |
| `3` | Call Contact 3       |
| `4` | Call Contact 4       |
| `5` | Call Contact 5       |
| `C` | Manual Dial          |
| `B` | Send SMS             |
| `D` | End / Reject Call    |
| `A` | Answer Incoming Call |
| `#` | Confirm / Send       |
| `*` | Clear / Backspace    |

## 👥 Fixed Contacts

The program contains five predefined contacts:

```cpp
String fixedNumbers[5] = {
  "8953422***",
  "89*********",
  "6394***29",
  "9***819140",
  "7355***280"
};

String fixedNames[5] = {
  "Akhilesh Sir",
  "Surya",
  "Shivam",
  "Suraj Sir",
  "Sandeep Sir"
};
```

Replace these values with your own contacts before using the project.

> **Privacy:** Do not upload real personal phone numbers to a public GitHub repository.

## 📞 Making a Call

### Quick Call

Press any key from `1` to `5` to call a predefined contact.

### Manual Dial

Press:

```text
C
```

Enter the phone number and press:

```text
#
```

The GSM module uses:

```text
ATD<number>;
```

to initiate the call.

## 📲 Receiving a Call

When an incoming call is detected, the LCD displays:

```text
Incoming Call!
A:Ans D:Reject
```

The LED on D13 starts blinking.

### Answer

Press:

```text
A
```

The Arduino sends:

```text
ATA
```

### Reject

Press:

```text
D
```

The Arduino sends:

```text
ATH
```

If the call is not answered within the configured timeout, it is treated as a missed call.

## ⏱️ Call Timer

During an active call, the LCD displays the elapsed call duration:

```text
Time: 25s
```

The timer is implemented using Arduino's `millis()` function.

## 💬 SMS

Press:

```text
B
```

Enter the recipient's phone number and press `#`.

Then enter the SMS message and press `#` to send.

The GSM module is configured for SMS text mode using:

```text
AT+CMGF=1
```

The message is then sent using:

```text
AT+CMGS
```

## 📚 Required Libraries

Install the following Arduino libraries:

```cpp
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
```

## 💻 Software Requirements

* Arduino IDE
* Arduino UNO or compatible board
* GSM module
* Active SIM card
* Required Arduino libraries

## 🚀 Installation

### 1. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/arduino-gsm-phone.git
```

### 2. Open the Project

Open the Arduino project in Arduino IDE.

### 3. Install Libraries

Install:

* LiquidCrystal_I2C
* Keypad

`SoftwareSerial` is normally available with the Arduino AVR platform.

### 4. Configure Contacts

Edit:

```cpp
fixedNumbers[]
fixedNames[]
```

### 5. Check Wiring

Verify:

* GSM TX/RX
* LCD SDA/SCL
* Keypad pins
* Ring LED
* Common GND

### 6. Upload

Select your Arduino board and COM port, then upload the program.

## 🧠 Working Principle

             ┌─────────────────┐
             │   4×4 Keypad    │
             └────────┬────────┘
                      │
                      ▼
             ┌─────────────────┐
             │     Arduino     │
             │                 │
             │ Call Processing │
             │ SMS Processing  │
             │ Keypad Control  │
             └───────┬─────────┘
                     │
            ┌────────┴─────────┐
            ▼                  ▼
     ┌─────────────┐    ┌──────────────┐
     │ GSM Module  │    │ 16×2 I2C LCD │
     │             │    │              │
     │ Call / SMS  │    │ User Display │
     └──────┬──────┘    └──────────────┘
            │
            ▼
       Cellular Network

## 📁 Repository Structure

arduino-gsm-phone/
│
├── Arduino_GSM_Phone
│   └── Arduino_GSM_Phone.ino
├── README.md
└── LICENSE

## ⚠️ Important Notes

* GSM modules require a stable power supply.
* GSM transmission can cause high current spikes.
* Use the recommended voltage/current supply for your GSM module.
* Make sure the SIM card is active.
* The LCD I2C address may vary; this project uses `0x27`.
* Keep real phone numbers private when publishing the project.

## 🔮 Future Improvements

Possible upgrades include:

* 📇 EEPROM contact storage
* 📞 Call history
* 💬 SMS inbox
* 🔊 Speaker and microphone
* 📶 GSM signal-strength display
* 🔋 Battery monitoring
* 🔐 PIN/password protection
* 🕐 RTC integration
* 🔔 Custom ringtone
* 👤 Contact management
* 🖥️ Improved menu interface

## 👨‍💻 Developer

**Surya Mani Bajpai **

Lucknow, Uttar Pradesh, India

Developed for educational and experimental **Embedded Systems & GSM Communication** projects.

## 📜 License

This project is intended for educational and experimental purposes.

You are free to modify and improve the project according to your requirements.

## ⭐ Support

If you find this project useful:

* ⭐ Star the repository
* 🍴 Fork the repository
* 🐛 Report issues
* 💡 Contribute improvements

---

**Made with ❤️ using Arduino + GSM + Embedded Systems**
