#include <Arduino.h>
#include <ST7735_t3.h>
#include <st7735_opengl.h>
#include <st7735_opengl_main.h>
// required libraries
#include "ST7735_t3.h"         // high speed display that ships with Teensy
#include <st7735_t3_font_Arial.h>
// required only if you are using an encoder to handle user input, mechanical buttons work as well
#include <Encoder.h>
#include <Bounce2.h>
#include "teensy_controls.h"
#include "VirtualView.h"
#include "TFTPianoDisplay.h"
#define DEBOUNCE    150

Bounce2::Button button = Bounce2::Button();
Encoder encoderLeftRight;
Encoder encoderUpDown;

st7735_opengl<Encoder, Bounce2::Button> Display(true, 20, &encoderUpDown, &encoderLeftRight, &button);
void ctrlUpdateFn();

VirtualView framebuffer = VirtualView(Display, 10, 10, 108, 108);
TeensyControl ctrl(framebuffer, ctrlUpdateFn, 128, 64, 0, 0);

ViewController<TeensyControl> viewController(ctrl, encoderUpDown, encoderLeftRight);

TFTPianoDisplay pianoDisplay1a(ctrl, 3, 3, 0, 16); //tft, byte octaves, byte startOctave, byte x, byte y

void ctrlUpdateFn() {
  //framebuffer.fillScreen(ST7735_BLACK);
  ctrl.fillRect(0, 0, 128, 64, ST7735_GREEN);
  ctrl.setTextColor(ST7735_WHITE);
  ctrl.drawString("Control-1ddddddd", 0, 0);
  pianoDisplay1a.drawFullPiano();
}

void setup() {

  Serial.begin(9600);

  button.attach( 10, 10 ); // USE EXTERNAL PULL-UP
  button.interval(5); 
  button.setPressedState(LOW); 

  delay(10);
  //Display.initR(INITR_GREENTAB);
  Display.setRotation(1);
  Display.fillScreen(ST7735_BLACK);

  viewController.AddControl(&ctrl);
}

void loop() {
  viewController.Update();
  pianoDisplay1a.keyDown(64);
}

int st7735_main(int numArgs, char **args) {
    /*
    if (numArgs < 2)
    {
        std::cout << "usage: " << args[0] << " <path-to-SDCard>" << std::endl;
        arduino_should_exit = true;
        exit(0);
    }
    std::cout << args[1] << std::endl;
    SD.setSDCardFolderPath(args[1]); */
    return 0;
}