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
#include "TFTPianoDisplay.h"

#include "MIDI.h"
#include "RtMidiMIDI.h"
#include "RtMidiTransport.h"
#define DEBOUNCE    150
#include "buttons.h"
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

typedef SceneController< VirtualView, Encoder, Button, RtMidiTransport<RtMidiMIDI>> MySceneController;

st7735_opengl<Encoder, Button> Display(true, 20, &encoderLeftRight, &encoderUpDown, &button, &button2, &button3);
VirtualView virtualDisplay(Display, 0, 0, 128, 128);
MySceneController sceneController(virtualDisplay, encoderLeftRight, encoderUpDown, button, button2, button3, MIDI);

void hideDialog(int buttonIndex);

#define NUM_DIALOG_MENU_ITEMS 3
#define Sapphire	0x092D
TeensyMenu *dialogMenu = new TeensyMenu(virtualDisplay, 88, 88, 19, 19, Sapphire, ST7735_BLACK);
TeensyMenuItem dialogMenuItems[NUM_DIALOG_MENU_ITEMS] = {
  TeensyMenuItem(*dialogMenu, [] (View *v) {v->drawString("Option 1", 0, 0);}, 8), 
  TeensyMenuItem(*dialogMenu, [] (View *v) {v->drawString("Option 2", 0, 0);}, 8), 
  TeensyMenuItem(*dialogMenu, [] (View *v) {v->drawString("close", 0, 0);}, 8, nullptr, nullptr, nullptr, hideDialog)
};

bool showingDialog = false;
void showDialog(int buttonIndex) {
    if (showingDialog == false) {
      showingDialog = true;

      Serial.printf("showDialog() \n");

      dialogMenu->NeedsUpdate = true;
      sceneController.AddDialog(dialogMenu);
    }
}

void hideDialog(int buttonIndex) {
  if (showingDialog) {
    Serial.printf("hideDialog() \n");
    sceneController.PopDialog();
    showingDialog = false;
  }
}

void DrawSettingsMenuItem0(View *v);

#define NUM_SETTINGS_MENU_ITEMS 20
TeensyMenu settingsMenu = TeensyMenu( virtualDisplay, 128, 128, 0, 0, ST7735_BLUE, ST7735_BLACK );
TeensyMenuItem settingMenuItems[NUM_SETTINGS_MENU_ITEMS] = {
  TeensyMenuItem(settingsMenu, DrawSettingsMenuItem0, 16),
  TeensyMenuItem(settingsMenu, 
    [] (View *v) {v->drawString("Dialog", 0, 0);}, 
    8, 
    nullptr,//   std::function<void(bool forward)> menuValueScroll =
    nullptr,//    std::function<bool(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity)> menuMidiNoteEvent = ,
    nullptr,//     std::function<bool(uint8_t channel, uint8_t data1, uint8_t data2)> menuMidiCCEvent = ,
    showDialog),
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

Scene *settingsScene = new Scene(virtualDisplay, 128, 128, 0, 0,
                        _bmp_settings_on, 
                        _bmp_settings_off, 
                        16, 16, 
                        [] { 
                              settingsMenu.Update(); 
                              pianoDisplay1.drawPiano();
                        },      //            std::function<void()> update = nullptr,  
                        [] { virtualDisplay.fillScreen(ST7735_BLUE); settingsMenu.NeedsUpdate = true; pianoDisplay1.displayNeedsUpdating(); },   //             std::function<void()> initScreen = nullptr, 
                        [] { },   //             std::function<void()> uninitScreen = nullptr, 
                        [] (unsigned index) { 
                            settingsMenu.ButtonDown(index);
                        }, //             std::function<void(unsigned)> buttonPressed = nullptr, 
                        [] (bool forward) { 
                            if (forward) settingsMenu.IncreaseSelectedIndex(); 
                            else settingsMenu.DecreaseSelectedIndex(); 
                        }, //std::function<void(bool)> rotary1Changed = nullptr, 
                        [] (bool forward) { } //std::function<void(bool)> rotary2Changed = nullptr
                        );

Scene *editScene = new Scene(virtualDisplay, 128, 128, 0, 0,
                        _bmp_edit_on, 
                        _bmp_edit_off, 
                        16, 16,
                        [] { }, 
                        [] { virtualDisplay.fillScreen(ST7735_RED); });

TeensyButtonBar button_bar(virtualDisplay, 128, 16, 0, 0);
TeensyButton button_rewind(button_bar, 16, 16, 0, 0);
TeensyButton button_play(button_bar, 16, 16, 0, 0);
TeensyButton button_pause(button_bar, 16, 16, 0, 0);
TeensyButton button_stop(button_bar, 16, 16, 0, 0);
TeensyButton button_fastfwd(button_bar, 16, 16, 0, 0);
TeensyButtonRecord button_record(button_bar);
TeensyButtonRecord button_record2(button_bar);
Scene *playScene = new Scene(virtualDisplay, 128, 128, 0, 0,
                        _bmp_play_on, 
                        _bmp_play_off, 
                        16, 16,
                        [] {
                          button_bar.Update();
                        },
                        [] {
                          virtualDisplay.fillScreen(ST7735_BLACK);
                        }, [] () {},
                        [](unsigned buttonIndex) {
                          button_bar.ButtonDown(buttonIndex);
                        },
                        [] (bool forward) {
                          if (forward)
                            button_bar.IncreaseSelectedIndex();
                          else
                            button_bar.DecreaseSelectedIndex();
                        },
                        [] (bool forward) {
                          button_bar.ValueScroll(forward);
                        });

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
  Display.setRotation(1);

  Display.fillScreen(ST7735_BLACK);

  button_play.SetDepressed(true);
  //button_stop.SetHighlighted(true);
  button_record2.SetRecording(true);

  button_bar.AddButton(&button_rewind);
  button_bar.AddButton(&button_record);
  button_bar.AddButton(&button_record2);

  button_bar.AddButton(&button_play);
  button_bar.AddButton(&button_pause);
  button_bar.AddButton(&button_stop);
  button_bar.AddButton(&button_fastfwd);


  sceneController.Init();
  sceneController.AddScene(settingsScene);
  sceneController.AddScene(editScene);
  sceneController.AddScene(playScene);

  sceneController.SetCurrentSceneIndex(0);
  sceneController.SetActive(false);

  //settingsScene.SetUpdateFunction(updateSettingsScene);
  //editScene.SetUpdateFunction(updateEditScene);
  //playScene.SetUpdateFunction(updatePlayScene);

  for (int i = 0; i < NUM_SETTINGS_MENU_ITEMS; i++) {
    settingsMenu.AddControl(&settingMenuItems[i]);
  }

  for (int i = 0; i < NUM_DIALOG_MENU_ITEMS; i++) {
    dialogMenu->AddControl(&dialogMenuItems[i]);
  }
}

int loopCounter = 0;

void loop() {
  sceneController.Process();
 
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