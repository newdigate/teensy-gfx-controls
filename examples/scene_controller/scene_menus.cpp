#include <Arduino.h>
#include <ST7735_t3.h>
#include <st7735_opengl.h>
#include <st7735_opengl_main.h>
#include "ST7735_t3.h"         // high speed display that ships with Teensy
#include <Encoder.h>
#include <Bounce2.h>
#include "icons.h"
#include "scenecontroller.h"
#include "teensy_controls.h"
#define DEBOUNCE    150

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
Encoder encoderLeftRight;
Encoder encoderUpDown;

st7735_opengl<Encoder, Button> Display(true, 20, &encoderLeftRight, &encoderUpDown, &button);
SceneController< st7735_opengl<Encoder, Button>, Encoder, Button> sceneController(Display, encoderLeftRight, encoderUpDown, button);

TeensyMenu settingsMenu = TeensyMenu( Display, 0, 0, 128, 128);
TeensyMenuItem firstSettingMenuItem = TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu1...", 0, 0);}, 8); 
TeensyMenuItem secondSettingMenuItem = TeensyMenuItem(settingsMenu, [] (View *v) {v->drawString("Menu2...", 0, 8);}, 8); 

Scene settingsScene = Scene(
                        _bmp_settings_on, 
                        _bmp_settings_off, 
                        16, 16, 
                        [] { settingsMenu.NeedsUpdate = true; settingsMenu.Update(); }, 
                        [] { Display.fillScreen(ST7735_BLUE); });

Scene editScene = Scene(
                        _bmp_edit_on, 
                        _bmp_edit_off, 
                        16, 16,
                        [] { }, 
                        [] { Display.fillScreen(ST7735_RED); });

Scene playScene = Scene(
                        _bmp_play_on, 
                        _bmp_play_off, 
                        16, 16,
                        [] { }, 
                        [] { Display.fillScreen(ST7735_GREEN); });                

void setup() {

  Serial.begin(9600);

  // while ((millis() > 5000) || (!Serial)) {}

  // button in the encoder
  //pinMode(SE_PIN, INPUT_PULLUP);

  // I use a digital pin to control LED brightness
  //pinMode(LED_PIN, OUTPUT);

  button.attach( 10, 10 ); // USE EXTERNAL PULL-UP

  // DEBOUNCE INTERVAL IN MILLISECONDS
  button.interval(5); 

  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  button.setPressedState(LOW); 
  

  delay(10);
  //Display.initR(INITR_GREENTAB);
  Display.setRotation(1);

  Display.fillScreen(ST7735_BLACK);

  sceneController.AddScene(&settingsScene);
  sceneController.AddScene(&editScene);
  sceneController.AddScene(&playScene);

  sceneController.SetCurrentSceneIndex(0);
  sceneController.SetActive(false);

  //settingsScene.SetUpdateFunction(updateSettingsScene);
  //editScene.SetUpdateFunction(updateEditScene);
  //playScene.SetUpdateFunction(updatePlayScene);

  settingsMenu.AddControl(&firstSettingMenuItem);
  settingsMenu.AddControl(&secondSettingMenuItem);
}

void loop() {
  sceneController.Process();
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