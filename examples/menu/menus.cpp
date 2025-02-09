#include <Arduino.h>
#include <ST7735_t3.h>
#include <st7735_opengl.h>
#include <st7735_opengl_main.h>
#include "ST7735_t3.h"         // high speed display that ships with Teensy
#include <Encoder.h>
#include <Bounce2.h>
#include "../scene_controller/icons.h"
#include "scenecontroller.h"
#include "teensy_controls.h"
#include "TFTPianoDisplay.h"

#include "MIDI.h"
#include "RtMidiMIDI.h"
#include "RtMidiTransport.h"
#define DEBOUNCE    150
#include "buttons.h"
#include "seven_segment.h"
//Scene(const uint16_t * iconOn, const uint16_t * iconOff, unsigned int iconWidth, unsigned int iconHeight) 

// MOVED to icons.h:
// this next rediciously long section is merely the icon image data
// scroll down to 1560 or so for the actual code
// Website for generating icons
// https://javl.github.io/image2cpp/
// all icons created at that site and copy / pasted here
// '64_wireless', 64x64px

using namespace Bounce2;
Button button = Button();
Button button2 = Button();
Button button3 = Button();
Encoder encoderLeftRight;
Encoder encoderUpDown;

MIDI_CREATE_RTMIDI_INSTANCE(RtMidiMIDI, rtMIDI,  MIDI);

typedef SceneHostControl< Encoder, Button, RtMidiTransport<RtMidiMIDI>> SceneHostControlType;

st7735_opengl<Encoder, Button> display(true, 20, &encoderLeftRight, &encoderUpDown, &button, &button2, &button3);
VirtualView virtualView(display, 0, 0, 128, 128);

void DrawSettingsMenuItem0(View *v);

#define NUM_SETTINGS_MENU_ITEMS 19
TeensyMenu settingsMenu = TeensyMenu(virtualView, 128, 128, 0, 0, ST7735_BLUE, ST7735_BLACK );
TeensyMenuItem settingMenuItems[NUM_SETTINGS_MENU_ITEMS] = {
  TeensyMenuItem(settingsMenu, DrawSettingsMenuItem0, 16),
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu 3  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu 4  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu 5  ", 0, 0);}, 8),
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu 6  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu 7  ", 0, 0);}, 8),
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu 8  ", 0, 0);}, 8),
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu 9  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu10  ", 0, 0);}, 8),
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu11  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu12  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu13  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu14  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu15  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu16  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu17  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu18  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu19  ", 0, 0);}, 8), 
  TeensyMenuItem(settingsMenu, [] (View *v) {
    v->drawPixel(0,0, ST7735_WHITE);
    v->drawString("Menu20  ", 0, 0);
  }, 8), 
};

TFTPianoDisplay pianoDisplay1(settingMenuItems[0], 3, 2, 0, 0); //tft, byte octaves, byte startOctave, byte x, byte y
void DrawSettingsMenuItem0(View *v) {
  pianoDisplay1.drawFullPiano();
}

void setup() {

  Serial.begin(9600);

  // while ((millis() > 5000) || (!Serial)) {}

  // button in the encoder
  //pinMode(SE_PIN, INPUT_PULLUP);

  // I use a digital pin to control LED brightness
  //pinMode(LED_PIN, OUTPUT);

  button.attach( 10, 10 ); // USE EXTERNAL PULL-UP
  button2.attach( 11, 11 ); // USE EXTERNAL PULL-UP
  button3.attach( 12, 12 ); // USE EXTERNAL PULL-UP


  // DEBOUNCE INTERVAL IN MILLISECONDS
  button.interval(5); 
  button2.interval(5); 
  button3.interval(5); 


  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  button.setPressedState(LOW); 
  button2.setPressedState(LOW); 
  button3.setPressedState(LOW); 

  delay(10);
  //Display.initR(INITR_GREENTAB);
  display.setRotation(1);
  display.fillScreen(ST7735_BLACK);




  settingsMenu.Initialize();

  for (int i = 0; i < NUM_SETTINGS_MENU_ITEMS; i++) {
    settingsMenu.AddControl(&settingMenuItems[i]);
  }
}

int loopCounter = 0;
long Position = 0, oldPosition = 0;
long PositionY = 0, oldPositionY = 0;

void loop() {

    button.update();
    button2.update();
    button3.update();

    if (button.changed() && !button.fell()) {
        settingsMenu.ButtonDown(0);
    }

    if (button2.changed() && !button2.fell()) {
        settingsMenu.ButtonDown(1);
    }

    if (button3.changed() && !button3.fell()) {
        settingsMenu.ButtonDown(2);
    }

    bool left = false, right = false, up = false, down = false;
    Position = encoderLeftRight.read() / 4;
    if ((Position - oldPosition) < 0) {
        left = true;
    }
    if ((Position - oldPosition) > 0) {
        right = true;
    }

    PositionY = encoderUpDown.read() / 4;
    if ((PositionY - oldPositionY) > 0) {
        down = true;
    }
    if ((PositionY - oldPositionY) < 0) {
        up = true;
    }

    oldPosition = Position;
    oldPositionY = PositionY;

    if (left) {
        settingsMenu.IndexScroll(false);
    } else if (right) {
        settingsMenu.IndexScroll(true);
    }

    if (up) {
        settingsMenu.ValueScroll(false);
    } else if (down) {
        settingsMenu.ValueScroll(true);
    }

    settingsMenu.Update(millis());

    if (loopCounter % 100 == 0) {
        pianoDisplay1.reset();
        float r = abs(rand()) / (float)INT32_MAX;
        unsigned char randomKey = (r * 32.0) + 12;
        pianoDisplay1.keyDown(  randomKey);
    }
    loopCounter++;
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