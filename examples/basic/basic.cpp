#include <Arduino.h>
#include <ST7735_t3.h>
#include <st7735_opengl.h>
#include <st7735_opengl_main.h>
// required libraries
#include <st7735_t3_font_Arial.h>
// required only if you are using an encoder to handle user input, mechanical buttons work as well
#include <Encoder.h>
#include <Bounce2.h>
#include "teensy_controls.h"
#include <math.h>
#define DEBOUNCE    150

Bounce2::Button button = Bounce2::Button();
Encoder encoderLeftRight;
Encoder encoderUpDown;

st7735_opengl Display(true, 20, &encoderLeftRight, &encoderUpDown, &button);

VirtualView framebuffer = VirtualView(Display, 10, 10, 108, 108);
ViewController< VirtualView > viewController(framebuffer, encoderUpDown, encoderLeftRight);

void setup() {

  Serial.begin(9600);

  button.attach( 10, 10 ); // USE EXTERNAL PULL-UP
  button.interval(5); 
  button.setPressedState(LOW); 

  delay(10);
  Display.initR(INITR_GREENTAB);
  Display.setRotation(1);
  Display.fillScreen(ST7735_BLACK);
 

}
float f = 0;

void loop() {
  f += 0.01;
  int16_t scrollY = round(60.0 * sin(f));
  framebuffer.YOffset(-scrollY);
  
  framebuffer.fillScreen(ST7735_BLUE);
  framebuffer.fillRect(32, 32, 64,64, ST7735_RED);
  framebuffer.setTextColor(ST7735_WHITE);
  framebuffer.drawString("XXXXX",0,0);
  delay(1);
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