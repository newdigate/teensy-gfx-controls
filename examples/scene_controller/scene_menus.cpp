#include <Arduino.h>
#include <ST7735_t3.h>
#include <st7735_opengl.h>
#include <st7735_opengl_main.h>
#include "ST7735_t3.h"         // high speed display that ships with Teensy
#include <st7735_t3_font_Arial.h>
#include <Encoder.h>
#include <Bounce2.h>
#include "icons.h"
#include "scenecontroller.h"

#define DEBOUNCE    150

//Scene(const uint16_t * iconOn, const uint16_t * iconOff, unsigned int iconWidth, unsigned int iconHeight) 
Scene settingsScene = Scene(_bmp_settings_on, _bmp_settings_off, 16, 16);
Scene editScene = Scene(_bmp_edit_on, _bmp_edit_off, 16, 16);
Scene playScene = Scene(_bmp_play_on, _bmp_play_off, 16, 16);


// encoder stuff
long Position = 0, oldPosition = 0;
long PositionY = 0, oldPositionY = 0;


// create some selectable menu sub-items, these are lists inside a menu item
const char *ReadoutItems[] =   {"Absolute", "Deg F", "Deg C"
                               };
const char *RefreshItems[] =   {"Off", "1 second", "2 seconds", "10 seconds",
                                "30 seconds", "1 minute", "5 minutes"
                               };
const char *PrecisionItems[] =   {"10", "0", "0.0", "0.00", "0.000"
                                 };
const char *TuneItems[] =   {"Slope/Offset", "Simple", "Linear", "2nd order ", "3rd order", "Log"
                            };
const char *OffOnItems[] =   {"Off", "On"
                             };

const char *DataRateItems[] =   {"300 baud", "1.2 kbd", "2.4 kbd", "4.8 kbd", "9.6 kbd", "19.2 kbd", "56 kbd"
                                };

// OK i'm going crazy with examples, but this will help show more processing when an int is needed but a float returned
// from the menu code
const char *C_NAMES[] = {"White", "Black", "Grey", "Blue", "Red", "Green", "Cyan", "Magenta",
                         "Yellow", "Teal", "Orange", "Pink", "Purple", "Lt Grey", "Lt Blue", "Lt Red",
                         "Lt Green", "Lt Cyan", "Lt Magenta", "Lt Yellow", "Lt Teal", "Lt Orange", "Lt Pink", "Lt Purple",
                         "Medium Grey", "Medium Blue", "Medium Red", "Medium Green", "Medium Cyan", "Medium Magenta", "Medium Yellow", "Medium Teal",
                         "Medium Orange", "Medium Pink", "Medium Purple", "Dk Grey", "Dk Blue", "Dk Red", "Dk Green", "Dk Cyan",
                         "Dk Magenta", "Dk Yellow", "Dk Teal", "Dk Orange", "Dk Pink", "Dk Purple"
                        };

const uint16_t  C_VALUES[] = {  0XFFFF, 0X0000, 0XC618, 0X001F, 0XF800, 0X07E0, 0X07FF, 0XF81F, //7
                                0XFFE0, 0X0438, 0XFD20, 0XF81F, 0X801F, 0XE71C, 0X73DF, 0XFBAE, //15
                                0X7FEE, 0X77BF, 0XFBB7, 0XF7EE, 0X77FE, 0XFDEE, 0XFBBA, 0XD3BF, //23
                                0X7BCF, 0X1016, 0XB000, 0X0584, 0X04B6, 0XB010, 0XAD80, 0X0594, //31
                                0XB340, 0XB00E, 0X8816, 0X4A49, 0X0812, 0X9000, 0X04A3, 0X0372, //39
                                0X900B, 0X94A0, 0X0452, 0X92E0, 0X9009, 0X8012 //45
                             };
// set default colors
uint16_t MENU_TEXT = C_VALUES[1];
uint16_t MENU_BACKGROUND = C_VALUES[0];
uint16_t MENU_HIGHLIGHTTEXT = C_VALUES[1];
uint16_t MENU_HIGHLIGHT = C_VALUES[21];
uint16_t MENU_HIGHBORDER = C_VALUES[10];
uint16_t MENU_SELECTTEXT = C_VALUES[0];
uint16_t MENU_SELECT = C_VALUES[4];
uint16_t MENU_SELECTBORDER = C_VALUES[37];
uint16_t MENU_DISABLE = C_VALUES[2];
uint16_t TITLE_TEXT = C_VALUES[13];
uint16_t TITLE_BACK = C_VALUES[36];

// this next rediciously long section is merely the icon image data
// scroll down to 1560 or so for the actual code

// Website for generating icons
// https://javl.github.io/image2cpp/

// all icons created at that site and copy / pasted here
// '64_wireless', 64x64px
Bounce2::Button button = Bounce2::Button();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// real code starts here
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProcessMainMenu();
void ProcessWirelessMenu();
void ProcessOptionMenu();
void ProcessColorMenu();

Encoder encoderLeftRight;
Encoder encoderUpDown;

st7735_opengl<Encoder, Bounce2::Button> Display(true, 20, &encoderLeftRight, &encoderUpDown, &button);

SceneController< st7735_opengl<Encoder, Bounce2::Button>, Encoder > sceneController(Display, encoderLeftRight, encoderUpDown);

// create encoder object
void updateSettingsScene() {
  Display.fillScreen(ST7735_BLUE);
}

void updateEditScene() {
  Display.fillScreen(ST7735_RED);
}

void updatePlayScene() {
  Display.fillScreen(ST7735_GREEN);
}

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

  settingsScene.SetUpdateFunction(updateSettingsScene);
  editScene.SetUpdateFunction(updateEditScene);
  playScene.SetUpdateFunction(updatePlayScene);
}

void loop() {
  sceneController.Process();
  if (!sceneController.Active()) {
    bool enter = false;
    // attempt to debouce these darn things...
    while (Serial.available()) {
      enter = true;
      int r = Serial.read();
    }

    if (enter) {
      sceneController.SetActive(true);
      //Display.fillScreen(MENU_BACKGROUND);
    } else {
      sceneController.Update();
    }
  } else {   
    bool exit = false;
    // attempt to debouce these darn things...
    while (Serial.available()) {
      exit = true;
      int r = Serial.read();
    }
    if (exit) {
      sceneController.SetActive(false);      
      //Display.fillScreen(MENU_BACKGROUND);
    } 
  }
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