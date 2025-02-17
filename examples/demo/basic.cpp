#include <Arduino.h>
#include <ST7735_t3.h>
#include <st7735_opengl.h>
#include <st7735_opengl_main.h>
// required libraries
#include <st7735_t3_font_Arial.h>
#include <st7735_t3_font_ComicSansMS.h>
// required only if you are using an encoder to handle user input, mechanical buttons work as well
#include <Encoder.h>
#include <Bounce2.h>
#include "teensy_controls.h"
#include <math.h>
#include "VirtualView.h"
#include "ILI9341_t3_Controls.h"
#define DEBOUNCE    150

Bounce2::Button button = Bounce2::Button();
Encoder encoderLeftRight;
Encoder encoderUpDown;

st7735_opengl<Encoder, Bounce2::Button> Display(true, 20, &encoderLeftRight, &encoderUpDown, &button);

VirtualView framebuffer = VirtualView(Display, 10, 10, 108, 108);
/*
    TeensyControl(View &view, std::function<void()> updateFn, unsigned int width, unsigned int height, unsigned int x, unsigned int y) : 
*/
void buttonDown();

ViewController< VirtualView > viewController(framebuffer, encoderUpDown, encoderLeftRight, &button, buttonDown);

TeensyControl *control = 
    new TeensyControl(
        Display, 
        [] () {  Display.fillRect(20, 20, 60, 60, ST7735_WHITE);}, 
        60, 60, 20, 20);

void buttonDown() {
    viewController.AddDialog(control);
}

#define FONT_TITLE Arial_16
#define FONT_DATA ComicSansMS_10

// defines for locations
#define BXCENTER 16
#define BYCENTER 32
#define BDIAMETER 24
#define BLOWSCALE 0
#define BHIGHSCALE 1200
#define BSCALEINC 200
#define BSWEEPANGLE 300

#define VXCENTER 70
#define VYCENTER 32
#define VDIAMETER 24
#define VLOWSCALE 0
#define VHIGHSCALE 4
#define VSCALEINC .5
#define VSWEEPANGLE 300

// defines for colors
#define BNEEDLECOLOR C_ORANGE
#define BDIALCOLOR C_DKBLUE
#define BTEXTCOLOR C_WHITE
#define BTICCOLOR C_GREY

#define VNEEDLECOLOR C_WHITE
#define VDIALCOLOR C_DKBLUE
#define VTEXTCOLOR C_WHITE
#define VTICCOLOR C_GREY

int bBits;
float bVolts;
float bData;

// create the dial object(s)
Dial<VirtualView, ILI9341_t3_font_t> Bits(&framebuffer, BXCENTER, BYCENTER, BDIAMETER, BLOWSCALE , BHIGHSCALE, BSCALEINC, BSWEEPANGLE);
Dial<VirtualView, ILI9341_t3_font_t> Volts(&framebuffer, VXCENTER, VYCENTER, VDIAMETER, VLOWSCALE , VHIGHSCALE, VSCALEINC, VSWEEPANGLE);



void setup() {

    Serial.begin(9600);

    button.attach( 10, 10 ); // USE EXTERNAL PULL-UP
    button.interval(5);
    button.setPressedState(LOW);

    delay(10);
    //Display.initR(INITR_GREENTAB);
    Display.setRotation(1);
    Display.fillScreen(ST7735_BLACK);


    // initialize the dials
    Bits.init(BNEEDLECOLOR, BDIALCOLOR, BTEXTCOLOR, BTICCOLOR, "Bits", FONT_TITLE, FONT_DATA);
    Volts.init(VNEEDLECOLOR, VDIALCOLOR, VTEXTCOLOR, VTICCOLOR, "Volts", FONT_TITLE, FONT_DATA);

}
float f = 0;

void loop() {
    delay(1);


    f += 0.0001;
    int16_t scrollY = round(50.0 * sin(f));
    framebuffer.YOffset(-scrollY);

    framebuffer.fillScreen(ST7735_BLUE);
    //framebuffer.fillRect(32, 32, 64,64, ST7735_RED);
    //framebuffer.setTextColor(ST7735_WHITE);
    //framebuffer.drawString("XXXXX",0,0);

    bBits = f * 10000.0;
    bVolts = bBits * 3.3 / 1024;

    // update the dials
    Bits.draw(bBits);
    Volts.draw(bVolts);
    viewController.Update(millis());

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