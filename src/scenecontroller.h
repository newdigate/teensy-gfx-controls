#ifndef TEENSY_SCENE_CONTROLLER_SCENECONTROLLER_H
#define TEENSY_SCENE_CONTROLLER_SCENECONTROLLER_H
#include <queue>
#include <stack>
#include <functional>
#include "teensy_controls.h"
#include "MIDI.h"

class BaseScene : public TeensyControl {
public:
    BaseScene(View &view, unsigned int width, unsigned int height, unsigned int left, unsigned int top,
            const uint16_t * iconOn, 
            const uint16_t * iconOff, 
            unsigned int iconWidth, 
            unsigned int iconHeight) : TeensyControl(view, nullptr, width, height, left, top),
        _iconWidth(iconWidth),
        _iconHeight(iconHeight),
        _iconOn(iconOn), 
        _iconOff(iconOff)
    {
    }

    virtual ~BaseScene() {
    }

    virtual void Update() {}
    virtual void InitScreen () {}
    virtual void UninitScreen () {}

    virtual void ButtonPressed(unsigned buttonIndex) {}
    virtual void Rotary1Changed(bool forward) {}
    virtual void Rotary2Changed(bool forward) {}

    const uint16_t * GetIcon(bool on) {
        return (on)? _iconOn : _iconOff;
    }
    unsigned int GetIconWidth() { return _iconWidth; }
    unsigned int GetIconHeight() { return _iconHeight; }

protected:
    unsigned int _iconWidth;
    unsigned int _iconHeight;
    const uint16_t * _iconOn;
    const uint16_t * _iconOff;
};

class Scene : public BaseScene {
public:
    Scene(View &view, unsigned int width, unsigned int height, unsigned int left, unsigned int top,
            const uint16_t * iconOn, 
            const uint16_t * iconOff, 
            unsigned int iconWidth, 
            unsigned int iconHeight, 
            std::function<void()> update = nullptr, 
            std::function<void()> initScreen = nullptr, 
            std::function<void()> uninitScreen = nullptr, 
            std::function<void(unsigned)> buttonPressed = nullptr, 
            std::function<void(bool)> rotary1Changed = nullptr, 
            std::function<void(bool)> rotary2Changed = nullptr,
            std::function<bool(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity)> handleNoteOnOff = nullptr,
            std::function<bool(uint8_t channel, uint8_t data1, uint8_t data2)> handleControlChange = nullptr) 
        : BaseScene(view, width, height, left, top, iconOn, iconOff, iconWidth, iconHeight),
        f_update(update),
        f_initScreen(initScreen),
        f_uninitScreen(uninitScreen),
        f_buttonPressed(buttonPressed),
        f_rotary1Changed(rotary1Changed),
        f_rotary2Changed(rotary2Changed),
        f_handleNoteOnOff(handleNoteOnOff),
        f_handleControlChange(handleControlChange)
    {
    }

    virtual ~Scene() {
    }

    void SetUpdateFunction(std::function<void()> updateFn) {
        f_update = updateFn;
    }

    void SetButtonPressedFunction(std::function<void(unsigned)> buttonPressedFn) {
        f_buttonPressed = buttonPressedFn;
    }

    void SetRotary1ChangedFunction(std::function<void(bool)> rotaryChangedFn) {
        f_rotary1Changed = rotaryChangedFn;
    }

    void SetRotary2ChangedFunction(std::function<void(bool)> rotaryChangedFn) {
        f_rotary2Changed = rotaryChangedFn;
    }

    void Update() override {
        if (f_update != nullptr) {
            f_update();
        }
    }

    void InitScreen() override {
        if (f_initScreen != nullptr) {
            f_initScreen();
        }
    }

    void UninitScreen() override {
        if (f_uninitScreen != nullptr) {
            f_uninitScreen();
        }
    }


    void ButtonPressed(unsigned index) override {
        if (f_buttonPressed != nullptr) {
            f_buttonPressed(index);
        }
    }

    void Rotary1Changed(bool forward) override {
        if (f_rotary1Changed != nullptr) {
            f_rotary1Changed(forward);
        }
    }

    void Rotary2Changed(bool forward) override {
        if (f_rotary2Changed != nullptr) {
            f_rotary2Changed(forward);
        }
    }

    void NoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) override {
        if (f_handleNoteOnOff != nullptr) {
            f_handleNoteOnOff(true, channel, pitch, velocity);
        }
    }
    void NoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) override {
        if (f_handleNoteOnOff != nullptr) {
            f_handleNoteOnOff(false, channel, pitch, velocity);
        }
    }

    void ControlChange(uint8_t channel, uint8_t data1, uint8_t data2) override {
        if (f_handleControlChange != nullptr) {
            f_handleControlChange(channel, data1, data2);
        }
    }

private:
    std::function<void()> f_update = nullptr;
    std::function<void()> f_initScreen = nullptr;
    std::function<void()> f_uninitScreen = nullptr;

    std::function<void(unsigned)> f_buttonPressed = nullptr;
    std::function<void(bool)> f_rotary1Changed = nullptr;
    std::function<void(bool)> f_rotary2Changed = nullptr;

    std::function<void(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity)> f_handleNoteOnOff = nullptr;
    std::function<void(uint8_t channel, uint8_t data1, uint8_t data2)> f_handleControlChange = nullptr;
};

class BaseSceneController {
public:
    BaseSceneController() : _menuEnabled(false)
    {
    }

    virtual ~BaseSceneController() {
    }

    void SetActive(bool active) {
        if (active == _active)
            return;
        _active = active;
        DrawSceneMenu();
    }
    
    bool Active() {
        return _active;
    }

    void SetCurrentSceneIndex(int scene) {
        if (_currentScene != scene && scene > -1 && scene < _scenes.size()) {
            _previousScene = _currentScene;
            _currentScene = scene;
            _currentSceneNeedsInit = true;
        }
    }

    void Update() {
        if (!_active && _currentScene > -1) {
            _scenes[_currentScene]->Update();
        }
    }

    virtual void Process() = 0;

    virtual void FillRect(int x, int y, int width, int height, uint16_t color) = 0;

    virtual void DrawPixel(int x, int y, uint16_t color) = 0;

    void DrawSceneMenu() {
        if (!_active)
        {
            FillRect(0, 0, 128, 16, 0);
            return;
        }

        int sceneIndex = 0;

        int x = _scenes.size() * 16;
        FillRect(x, 0, 128, 16, 0xFFFF);
        
        x = 0;
        for(auto&& scene : _scenes) {
            const bool sceneSelected = sceneIndex == _currentScene;
            const uint16_t * icon = scene->GetIcon(sceneSelected);
            DrawIcon(icon, x, 0, scene->GetIconWidth(), scene->GetIconHeight());
            x+=16;
            sceneIndex++;
        }
    }
    void AddScene(BaseScene *scene){
        _scenes.push_back(scene);
        _menuEnabled = _scenes.size() > 1;
    }
    void AddDialog(TeensyControl *control){
        _dialogs.push(control);
    }

    void PopDialog() {
        if (_dialogs.size() > 0) {
            _dialogs.pop();
            _currentSceneNeedsInit = true;
        }
    }
    void DrawIcon(const uint16_t * icon, int x, int y, int iconWidth, int iconHeight) {
        for (int i=0; i < iconWidth; i++)
            for (int j=0; j < iconHeight; j++)
                DrawPixel(x+i, y+j, icon[i + (j * iconWidth)]);
    }

    virtual void NoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
        if (_dialogs.size() > 0) {
            _dialogs.top()->NoteOn (channel, pitch, velocity);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->NoteOn(channel, pitch, velocity);
    }
    virtual void NoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
        if (_dialogs.size() > 0) {
            _dialogs.top()->NoteOff (channel, pitch, velocity);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->NoteOff(channel, pitch, velocity);
    }
    virtual void AfterTouchPoly(byte inChannel, byte inNote, byte inValue)  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->AfterTouchPoly (inChannel, inNote, inValue);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->AfterTouchPoly(inChannel, inNote, inValue);
    }
    virtual void ControlChange(byte inChannel, byte inNumber, byte inValue) {
        if (_dialogs.size() > 0) {
            _dialogs.top()->ControlChange (inChannel, inNumber, inValue);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->ControlChange(inChannel, inNumber, inValue);
    }
    virtual void ProgramChange(byte inChannel, byte inNumber)  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->ProgramChange (inChannel, inNumber);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->ProgramChange(inChannel, inNumber);
    }
    virtual void AfterTouchChannel(byte inChannel, byte inPressure)  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->AfterTouchChannel (inChannel, inPressure);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->AfterTouchChannel(inChannel, inPressure);
    }
    virtual void PitchBend(byte inChannel, int inValue) {
        if (_dialogs.size() > 0) {
            _dialogs.top()->PitchBend (inChannel, inValue);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->PitchBend(inChannel, inValue);
    }
    virtual void SysEx(byte* inData, unsigned inSize) {
        if (_dialogs.size() > 0) {
            _dialogs.top()->SysEx (inData, inSize);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->SysEx(inData, inSize);
    }
    virtual void MtcQuarterFrame(byte inData)  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->MtcQuarterFrame (inData);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->MtcQuarterFrame(inData);
    }
    virtual void SongPosition(unsigned inBeats)  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->SongPosition (inBeats);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->SongPosition(inBeats);
    }
    virtual void SongSelect(byte inSongNumber)  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->SongSelect (inSongNumber);
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->SongSelect(inSongNumber);
    }
    virtual void TuneRequest()  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->TuneRequest ();
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->TuneRequest();
    }
    virtual void Clock()  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->Clock ();
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->Clock();
    }
    virtual void Start()  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->Start ();
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->Start();
    }
    virtual void Continue()  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->Continue ();
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->Continue();
    }
    virtual void Stop()  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->Stop ();
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->Stop();
    }
    virtual void ActiveSensing()  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->ActiveSensing ();
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->ActiveSensing();
    }
    virtual void SystemReset() {
        if (_dialogs.size() > 0) {
            _dialogs.top()->SystemReset ();
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->SystemReset();
    }
    virtual void Tick()  {
        if (_dialogs.size() > 0) {
            _dialogs.top()->Tick ();
            return;
        }
        if (_currentScene < 0 || _currentScene >= _scenes.size())
            return ;
        _scenes[_currentScene]->Tick();
    }

protected:
    bool _active = false;
    std::vector< BaseScene* > _scenes;
    int _currentScene = -1;
    int _previousScene = -1;
    bool _menuEnabled;
    std::stack< TeensyControl* > _dialogs;
    bool _currentSceneNeedsInit = true;

};

template< class TDisplay, class TEncoder, class TButton, class TMidiTransport>
class SceneController : public BaseSceneController {
public:
    SceneController(TDisplay &display,
        TEncoder &encoderUpDown,
        TEncoder &encoderLeftRight,
        TButton &button,
        TButton &button2,
        TButton &button3,
        midi::MidiInterface<TMidiTransport> &midi) :
                        _display(display),
                       _button(button),
                       _button2(button2),
                       _button3(button3),
                       _encoderUpDown(encoderUpDown),
                       _encoderLeftRight(encoderLeftRight),
                       _midi(midi) {
        _instances.push_back(this);
        _midi.setHandleNoteOn( handleNoteOn );
        _midi.setHandleNoteOff( handleNoteOff );
        _midi.setHandleClock(  handleClock );
        _midi.setHandleContinue( handleContinue );
        _midi.setHandleStart( handleStart );
        _midi.setHandleStop(handleStop );
        _midi.setHandleTick(handleTick);
        _midi.setHandleActiveSensing(handleActiveSensing);
        _midi.setHandleControlChange(handleControlChange);
        _midi.setHandlePitchBend(handlePitchBend);
        _midi.setHandleProgramChange(handleProgramChange);
        _midi.setHandleSongPosition(handleSongPosition);
        _midi.setHandleSongSelect(handleSongSelect);
        _midi.setHandleSystemExclusive(handleSysEx);
        _midi.setHandleSystemReset(handleSystemReset);
        _midi.setHandleTuneRequest(handleTuneRequest);
        _midi.setHandleAfterTouchChannel(handleAfterTouchChannel);
        _midi.setHandleAfterTouchPoly(handleAfterTouchPoly);
        _midi.setHandleTimeCodeQuarterFrame(handleMtcQuarterFrame);
    }

    ~SceneController() override {
       // _instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
    };

    void Process() override {
        _button.update();
        _button2.update();
        _button3.update();

        if (_button.changed() && !_button.fell()) {
            // button has been pressed...
            if (!_active && _menuEnabled) {
                _active = true;
                _previousScene = _currentScene;
                DrawSceneMenu();
            } else {
                _active = false;
                if (_previousScene != _currentScene) {
                    if (_previousScene > -1) {
                        //Serial.print("UninitScreen:");
                        //Serial.println(_previousScene);
                        _scenes[_currentScene]->UninitScreen(); 
                    }
                    _scenes[_currentScene]->InitScreen(); 
                } else
                    _scenes[_currentScene]->Update(); 
            }
            //Serial.println(_active? "Open Menu" : "Close Menu");
            return;
        }

        if (_button2.changed() && !_button2.fell()) {
            if (_dialogs.size() > 0){
                _dialogs.top()->ButtonDown(2);
            } else 
            if (_currentScene > -1) {
                _scenes[_currentScene]->ButtonPressed(2); 
            }
        }

        if (_button3.changed() && !_button3.fell()) {
            if (_dialogs.size() > 0){
                _dialogs.top()->ButtonDown(3);
            } else 
            if (_currentScene > -1) {
                _scenes[_currentScene]->ButtonPressed(3); 
            }
        }

        bool left = false, right = false, up = false, down = false;
        Position = _encoderLeftRight.read() / 4;
        if ((Position - oldPosition) < 0) {
            left = true;
        }
        if ((Position - oldPosition) > 0) {
            right = true;
        }

        PositionY = _encoderUpDown.read() / 4;
        if ((PositionY - oldPositionY) > 0) {
            down = true;
        }
        if ((PositionY - oldPositionY) < 0) {
            up = true;
        }

        oldPosition = Position;
        oldPositionY = PositionY;
    
        if (_active) {
            // the scene menu is visible/active
            if (right) {
                if (_currentScene < _scenes.size()-1) {
                    _currentScene++;
                } else 
                    _currentScene = 0;
            } else if (left) {
                if (_currentScene > 0) {
                    _currentScene--;
                } else 
                    _currentScene = _scenes.size()-1;
            } 

            if (left | right) {
                DrawSceneMenu();
                _currentSceneNeedsInit = true;
                //Serial.println("_currentSceneNeedsInit = true");
            }
        } else 
        {
            // the scene menu is hidden and we are rending the current scene
            bool dialogActive = _dialogs.size() > 0;
             
            if (_currentScene > -1) {

                if (_currentSceneNeedsInit) {
                    _currentSceneNeedsInit = false;
                    _scenes[_currentScene]->InitScreen();
                    //Serial.println("Current scene: InitScreen");
                }
                if (!dialogActive) {
                    // marshall the inputs to th current scene
                    if (up) {
                        _scenes[_currentScene]->Rotary1Changed(false);
                    } else if (down) {
                        _scenes[_currentScene]->Rotary1Changed(true);
                    }

                    if (left) {
                        _scenes[_currentScene]->Rotary2Changed(false);
                    } else if (right) {
                        _scenes[_currentScene]->Rotary2Changed(true);
                    }
                }
                Update();
            }
        
            if (dialogActive) {
                if (up) {
                    _dialogs.top()->DecreaseSelectedIndex();
                } else if (down) {
                    _dialogs.top()->IncreaseSelectedIndex();
                }

                if (left) {
                    _dialogs.top()->ValueScroll(true);
                } else if (right) {
                    _dialogs.top()->ValueScroll(false);
                }
       
                _dialogs.top()->Update();
            } 
        }
    }

    void FillRect(int x, int y, int width, int height, uint16_t color) override {
        _display.fillRect(x, y, width, height, color);
    };

    void DrawPixel(int x, int y, uint16_t color) override {
        _display.Pixel(x, y, color);
    };



    static void handleNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
        Serial.println("midi note on");
        for (auto instance: _instances) {
            instance->NoteOn(channel, pitch, velocity);
        }
    }
    static void handleNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
        for (auto instance: _instances) {
            instance->NoteOff(channel, pitch, velocity);
        }
    }
    static void handleAfterTouchPoly(byte inChannel, byte inNote, byte inValue) {
        for (auto instance: _instances) {
            instance->AfterTouchPoly(inChannel, inNote, inValue);
        }
    }
    static void handleControlChange(byte inChannel, byte inNumber, byte inValue) {
        for (auto instance: _instances) {
            instance->ControlChange(inChannel, inNumber, inValue);
        }
    }
    static void handleProgramChange(byte inChannel, byte inNumber) {
        for (auto instance: _instances) {
            instance->ProgramChange(inChannel, inNumber);
        }
    }
    static void handleAfterTouchChannel(byte inChannel, byte inPressure) {
        for (auto instance: _instances) {
            instance->AfterTouchChannel( inChannel, inPressure);
        }
    }
    static void handlePitchBend(byte inChannel, int inValue) {
        for (auto instance: _instances) {
            instance->PitchBend(inChannel, inValue);
        }
    }
    static void handleSysEx(byte* inData, unsigned inSize) {
        for (auto instance: _instances) {
            instance->SysEx(inData, inSize);
        }
    }
    static void handleMtcQuarterFrame(byte inData) {
        for (auto instance: _instances) {
            instance->MtcQuarterFrame(inData);
        }
    }
    static void handleSongPosition(unsigned inBeats) {
        for (auto instance: _instances) {
            instance->SongPosition(inBeats);
        }
    }
    static void handleSongSelect(byte inSongNumber) {
        for (auto instance: _instances) {
            instance->SongSelect(inSongNumber);
        }
    }
    static void handleTuneRequest() {
        for (auto instance: _instances) {
            instance->TuneRequest();
        }
    }
    static void handleClock() {
        for (auto instance: _instances) {
            instance->Clock();
        }
    }
    static void handleStart() {
        for (auto instance: _instances) {
            instance->Start();
        }
    }
    static void handleContinue() {
        for (auto instance: _instances) {
            instance->Continue();
        }
    }
    static void handleStop() {
        for (auto instance: _instances) {
            instance->Stop();
        }
    }
    static void handleActiveSensing() {
        for (auto instance: _instances) {
            instance->ActiveSensing();
        }
    }
    static void handleSystemReset() {
            for (auto instance: _instances) {
            instance->SystemReset();
        }
    }
    static void handleTick() {
        for (auto instance: _instances) {
            instance->Tick();
        }
    }
protected:
    TDisplay &_display;
    TButton &_button;
    TButton &_button2;
    TButton &_button3;
    Encoder &_encoderUpDown;
    Encoder &_encoderLeftRight;
    midi::MidiInterface<TMidiTransport>&_midi;
    // encoder stuff
    long Position = 0, oldPosition = 0;
    long PositionY = 0, oldPositionY = 0;
    static std::vector<SceneController*> _instances;
};

template< class TDisplay, class TEncoder, class TButton, class TMidiTransport>
std::vector<SceneController<TDisplay, TEncoder, TButton, TMidiTransport>*> SceneController<TDisplay, TEncoder, TButton, TMidiTransport>::_instances;

#endif  //TEENSY_SCENE_CONTROLLER_SCENECONTROLLER_H
