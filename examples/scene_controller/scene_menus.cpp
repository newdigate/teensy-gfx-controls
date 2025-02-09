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
SceneHostControlType sceneHostControl(virtualView, 0, 0, 128, 128, encoderLeftRight, encoderUpDown, button, button2, button3, MIDI);

#define NUM_DIALOG_MENU_ITEMS 2
#define Sapphire	0x092D
TeensyMenu *dialogMenu = new TeensyMenu(display, 88, 88, 19, 19, Sapphire, ST7735_BLACK);
TeensyMenuItem dialogMenuItems[NUM_DIALOG_MENU_ITEMS] = {
  TeensyMenuItem(*dialogMenu, [] (View *v) {v->drawString("Option 1", 0, 0);}, 8), 
  TeensyMenuItem(*dialogMenu, [] (View *v) {v->drawString("Option 2", 0, 0);}, 8), 
};

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

Scene *settingsScene = new Scene(virtualView, 128, 128, 0, 0,
                                 _bmp_settings_on,
                                 _bmp_settings_off,
                                 16, 16,
                                 [] (unsigned millis){
                              settingsMenu.Update(millis);
                              pianoDisplay1.drawPiano();
                        },      //            std::function<void()> update = nullptr,  
                        [] {
                                virtualView.fillScreen(ST7735_BLUE);
                                settingsMenu.ForceRedraw();
                                pianoDisplay1.displayNeedsUpdating();
                        },   //             std::function<void()> initScreen = nullptr,
                        [] { },   //             std::function<void()> uninitScreen = nullptr, 
                        [] (unsigned index) { 
                            settingsMenu.ButtonDown(index);
                        }, //             std::function<void(unsigned)> buttonPressed = nullptr, 
                        [] (bool forward) { 
                            settingsMenu.IndexScroll(forward);
                        }, //std::function<void(bool)> rotary1Changed = nullptr,
                        [] (bool forward) { } //std::function<void(bool)> rotary2Changed = nullptr
                        );

Scene *editScene = new Scene(virtualView, 128, 128, 0, 0,
                             _bmp_edit_on,
                             _bmp_edit_off,
                             16, 16,
                             [](unsigned millis) { },
                             [] { virtualView.fillScreen(ST7735_RED); });
bool isRecording = false;
TeensyButtonBar button_bar(virtualView, 128, 16, 0, 0);
TeensyButtonRewind button_rewind(button_bar);
TeensyButtonPlay button_play(button_bar);
TeensyButtonPause button_pause(button_bar);
TeensyButtonStop button_stop(button_bar);
TeensyButtonFastfwd button_fastfwd(button_bar);
TeensyButtonRecord button_record(button_bar, isRecording);
int16_t segmentx = 0;
TeensySevenSegment s1(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment s2(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment s3(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment s4(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment s5(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment s6(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment s7(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment s8(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment s9(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment s0(virtualView, 5, 8, segmentx++ * 7, 24);
TeensySevenSegment* sevensegments[] = {&s0, &s1, &s2, &s3, &s4, &s5, &s6, &s7, &s8, &s9};
TeensyBarsAndBeatsIndicator bars_and_beats_indicator(virtualView, 128, 8, 0, 46);
TeensyTimeIndicator time_indicator(virtualView, 128, 8, 0, 36);
unsigned timeIndicatorMillis = 0, oldTimeIndicatorMillis = 0, oldBarsAndBeatsIndicatorMillis=0, beats =0, bars =0;
double tempo = 120.0;
Scene *playScene = new Scene(virtualView, 128, 128, 0, 0,
                             _bmp_play_on,
                             _bmp_play_off,
                             16, 16,
                             [](unsigned milliseconds){
                          button_bar.Update(milliseconds);
                          time_indicator.Update(milliseconds);
                          if (milliseconds > oldTimeIndicatorMillis + 20) {
                            unsigned millisecondsPerBeat = 60000.0 / tempo;
                            unsigned beatsDelta = (milliseconds - oldBarsAndBeatsIndicatorMillis)/millisecondsPerBeat;
                            if (beatsDelta > 0) {
                              beats += beatsDelta;
                              if (beats > 3) {
                                bars += beats/4;
                                beats = beats %4;
                              }
                              bars_and_beats_indicator.SetBarsAndBeats(bars, beats + 1);
                              oldBarsAndBeatsIndicatorMillis = milliseconds;
                            }
                            oldTimeIndicatorMillis = milliseconds;
                            time_indicator.SetTime(milliseconds - timeIndicatorMillis);
                          }
                          bars_and_beats_indicator.Update(milliseconds);
                          for (auto && sevensegment : sevensegments) {
                            sevensegment->Update(milliseconds);
                          }
                        },
                             [] {
                          timeIndicatorMillis = millis();
                          oldTimeIndicatorMillis = timeIndicatorMillis;
                          oldBarsAndBeatsIndicatorMillis = timeIndicatorMillis;
                          button_bar.ForceRedraw();
                          time_indicator.ForceRedraw();
                          bars_and_beats_indicator.ForceRedraw();
                          for (auto && sevensegment : sevensegments) {
                            sevensegment->ForceRedraw();
                          }
                          sceneHostControl.fillScreen(ST7735_BLACK);
                        }, [] () {},
                             [](unsigned buttonIndex) {
                          button_bar.ButtonDown(buttonIndex);
                        },
                             [] (const bool forward) {
                            button_bar.IndexScroll(forward);
                        },
                             [] (const bool forward) {
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
  display.setRotation(1);
  display.fillScreen(ST7735_BLACK);

  button_play.SetDepressed(true);

  uint8_t count = 0;
  for (auto sevensegment : sevensegments) {
    sevensegment->SetDigit(count);
    count++;
  }

  //button_stop.SetHighlighted(true);

  button_bar.AddButton(&button_rewind);
  button_bar.AddButton(&button_record);
  button_bar.AddButton(&button_play);
  button_bar.AddButton(&button_pause);
  button_bar.AddButton(&button_stop);
  button_bar.AddButton(&button_fastfwd);


  sceneHostControl.Init();
  sceneHostControl.AddScene(settingsScene);
  sceneHostControl.AddScene(editScene);
  sceneHostControl.AddScene(playScene);

  sceneHostControl.SetCurrentSceneIndex(0);
    sceneHostControl.SetTopMenuActive(false);

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
  sceneHostControl.Update(millis());
 
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