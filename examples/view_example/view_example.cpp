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

#define DEBOUNCE    150

Bounce2::Button button = Bounce2::Button();
Encoder encoderLeftRight;
Encoder encoderUpDown;

st7735_opengl<Encoder, Bounce2::Button> Display(true, 20, &encoderLeftRight, &encoderUpDown, &button);

VirtualView framebuffer = VirtualView(Display, 10, 10, 108, 108);
ViewController< VirtualView > viewController(framebuffer, encoderUpDown, encoderLeftRight);

void ctrlUpdateFn(int16_t x, int16_t y);
void ctrl2UpdateFn(int16_t x, int16_t y);
void ctrl3UpdateFn(int16_t x, int16_t y);


TeensyControl ctrl(ctrlUpdateFn, 128, 64, 0, 0);
TeensyControl ctrl2(ctrl2UpdateFn, 128, 32, 0, 64);
TeensyControl ctrl3(ctrl3UpdateFn, 128, 16, 0, 96);

void ctrlUpdateFn(int16_t x, int16_t y) {
  //framebuffer.fillScreen(ST7735_BLACK);
  framebuffer.fillRect(ctrl.X()+x, ctrl.Y()+y, ctrl.Width(), ctrl.Height(), ST7735_GREEN);
  framebuffer.setTextColor(ST7735_WHITE);
  framebuffer.drawString("Control-1ddddddd", ctrl.X()+x, ctrl.Y()+y);
}

void ctrl2UpdateFn(int16_t x, int16_t y) {
  //framebuffer.fillScreen(ST7735_BLACK);
  framebuffer.fillRect(ctrl2.X()+x, ctrl2.Y()+y, ctrl2.Width(), ctrl2.Height(), ST7735_RED);
  framebuffer.setTextColor(ST7735_WHITE);
  framebuffer.drawString("Control2asdasdasd", ctrl2.X()+x, ctrl2.Y()+y);
}
void ctrl3UpdateFn(int16_t x, int16_t y) {
  //framebuffer.fillScreen(ST7735_BLACK);
  framebuffer.fillRect(ctrl3.X()+x, ctrl3.Y()+y, ctrl3.Width(), ctrl3.Height(), ST7735_BLUE);
  framebuffer.setTextColor(ST7735_WHITE);
  framebuffer.drawString("Control/3asdasdasd", ctrl3.X()+x, ctrl3.Y()+y);
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
  viewController.AddControl(&ctrl2);
  viewController.AddControl(&ctrl3);

}

void loop() {
  viewController.Update();
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