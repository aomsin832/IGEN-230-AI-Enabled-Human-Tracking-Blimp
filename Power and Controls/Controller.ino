#include <Bluepad32.h>
#include <ESP32Servo.h>

/*
SET BOARD IN TOOLS->BOARD->DOIT ESP32 DEVKIT V1

LEFT STICK UP-DOWN YAW
RIGHT STICK MOVE FORWARD BACKWARD LEFT AND RIGHT
*/

Servo servo1;  //nose
Servo servo2;  //tail

//Theresholds
int axisThreshold = 250;
int servoMin = 20;
int servoMax = 160;
int servoMid = 90;

//Pins
int altitude1 = 14;  //bottom
int altitude2 = 27;
int front1 = 4;  //nose
int front2 = 16;
int back1 = 15;  //tail
int back2 = 19;
int servopin1 = 23; //nose
int servopin2 = 13;  //tail

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            break;
        }
    }
    
    digitalWrite(front1, LOW);
    digitalWrite(front2, LOW);
    digitalWrite(back1, LOW);
    digitalWrite(back2, LOW);
    digitalWrite(altitude1, LOW);
    digitalWrite(altitude2, LOW);
}

void processGamepad(ControllerPtr ctl) {
    if (ctl->axisY() > axisThreshold) {
        digitalWrite(altitude1, HIGH);
        digitalWrite(altitude2, LOW);
    } else if (ctl->axisY() < -axisThreshold) {
        digitalWrite(altitude1, LOW);
        digitalWrite(altitude2, HIGH);
    } else {
        digitalWrite(altitude1, LOW);
        digitalWrite(altitude2, LOW);
    }

    if (ctl->axisRX() > axisThreshold) {
        servo1.write(servoMin);
        servo2.write(servoMax);
        digitalWrite(front1, HIGH);
        digitalWrite(front2, LOW);
        digitalWrite(back1, HIGH);
        digitalWrite(back2, LOW);
    } else if (ctl->axisRX() < -axisThreshold) {
        servo1.write(servoMax);
        servo2.write(servoMin);
        digitalWrite(front1, HIGH);
        digitalWrite(front2, LOW);
        digitalWrite(back1, HIGH);
        digitalWrite(back2, LOW);
    } else {
        servo1.write(servoMid);
        servo2.write(servoMid);
        digitalWrite(front1, LOW);
        digitalWrite(front2, LOW);
        digitalWrite(back1, LOW);
        digitalWrite(back2, LOW);
      
      if (ctl->axisRY() > axisThreshold) {
          servo1.write(servoMid);
          servo2.write(servoMid);
          digitalWrite(front1, HIGH);
          digitalWrite(front2, LOW);
          digitalWrite(back1, LOW);
          digitalWrite(back2, HIGH);
      } else if (ctl->axisRY() < -axisThreshold) {
          servo1.write(servoMid);
          servo2.write(servoMid);
          digitalWrite(front1, LOW);
          digitalWrite(front2, HIGH);
          digitalWrite(back1, HIGH);
          digitalWrite(back2, LOW);
      } else {
          servo1.write(servoMid);
          servo2.write(servoMid);
          digitalWrite(front1, LOW);
          digitalWrite(front2, LOW);
          digitalWrite(back1, LOW);
          digitalWrite(back2, LOW);

        if (ctl->axisX() > axisThreshold) {
            servo1.write(servoMin);
            servo2.write(servoMin);
            digitalWrite(front1, HIGH);
            digitalWrite(front2, LOW);
            digitalWrite(back1, HIGH);
            digitalWrite(back2, LOW);
        } else if (ctl->axisX() < -axisThreshold) {
            servo1.write(servoMax);
            servo2.write(servoMax);
            digitalWrite(front1, HIGH);
            digitalWrite(front2, LOW);
            digitalWrite(back1, HIGH);
            digitalWrite(back2, LOW);
        } else {
            servo1.write(servoMid);
            servo2.write(servoMid);
            digitalWrite(front1, LOW);
            digitalWrite(front2, LOW);
            digitalWrite(back1, LOW);
            digitalWrite(back2, LOW);
        }
      }
    }
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            }
        }
    }
}

void setup() {
    Serial.begin(9600);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);

    pinMode(altitude1, OUTPUT);
    pinMode(altitude2, OUTPUT);
    pinMode(front1, OUTPUT);
    pinMode(front2, OUTPUT);
    pinMode(back1, OUTPUT);
    pinMode(back2, OUTPUT);
    servo1.attach(servopin1);
    servo2.attach(servopin2);
}

void loop() { 
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();

    // Failsafe: If no controller is connected, force all outputs to LOW.
    bool anyControllerConnected = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] != nullptr && myControllers[i]->isConnected()) {
            anyControllerConnected = true;
            break;
        }
    }
    if (!anyControllerConnected) {
        digitalWrite(front1, LOW);
        digitalWrite(front2, LOW);
        digitalWrite(back1, LOW);
        digitalWrite(back2, LOW);
        digitalWrite(altitude1, LOW);
        digitalWrite(altitude2, LOW);
    }


    vTaskDelay(1);
    delay(150);
}
