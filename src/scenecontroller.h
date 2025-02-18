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
            std::function<void(unsigned)> update = nullptr,
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

    void SetUpdateFunction(std::function<void(unsigned)> updateFn) {
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

    void Update(unsigned milliseconds) override {
        BaseScene::Update(milliseconds);
        if (f_update != nullptr) {
            f_update(milliseconds);
        }
    }

    void Initialize() override {
        BaseScene::Initialize();
        if (f_initScreen != nullptr) {
            f_initScreen();
        }
    }

    void Uninitialize() override {
        if (f_uninitScreen != nullptr) {
            f_uninitScreen();
        }
        BaseScene::Uninitialize();
    }

    void ButtonDown(unsigned char index) override {
        if (f_buttonPressed != nullptr) {
            f_buttonPressed(index);
        }
        BaseScene::ButtonDown(index);
    }

    void IndexScroll(bool forward) override {
        if (f_rotary1Changed != nullptr) {
            f_rotary1Changed(forward);
        }
        BaseScene::IndexScroll(forward);
    }

    void ValueScroll(bool forward) override {
        if (f_rotary2Changed != nullptr) {
            f_rotary2Changed(forward);
        }
        BaseScene::ValueScroll(forward);
    }

    void NoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) override {
        if (f_handleNoteOnOff != nullptr) {
            f_handleNoteOnOff(true, channel, pitch, velocity);
        }
        BaseScene::NoteOn(channel, pitch, velocity);
    }
    void NoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) override {
        if (f_handleNoteOnOff != nullptr) {
            f_handleNoteOnOff(false, channel, pitch, velocity);
        }
        BaseScene::NoteOff(channel, pitch, velocity);
    }

    void ControlChange(uint8_t channel, uint8_t data1, uint8_t data2) override {
        if (f_handleControlChange != nullptr) {
            f_handleControlChange(channel, data1, data2);
        }
        BaseScene::ControlChange(channel, data1, data2);
    }

private:
    std::function<void(unsigned)> f_update = nullptr;
    std::function<void()> f_initScreen = nullptr;
    std::function<void()> f_uninitScreen = nullptr;

    std::function<void(unsigned)> f_buttonPressed = nullptr;
    std::function<void(bool)> f_rotary1Changed = nullptr;
    std::function<void(bool)> f_rotary2Changed = nullptr;

    std::function<void(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity)> f_handleNoteOnOff = nullptr;
    std::function<void(uint8_t channel, uint8_t data1, uint8_t data2)> f_handleControlChange = nullptr;
};

class BaseSceneHostControl : public TeensyControl {
public:
    BaseSceneHostControl(View &view, unsigned int width, unsigned int height, unsigned int left, unsigned int top)
        : TeensyControl(view, nullptr, width, height, left, top),
        _menuEnabled(false)
    {
    }

    ~BaseSceneHostControl() override = default;

    void SetTopMenuActive(bool active) {
        if (active == _topMenuActive)
            return;
        _topMenuActive = active;
        DrawSceneMenu();
        _currentSceneNeedsInit = true;
    }
    
    bool IsTopMenuActive() {
        return _topMenuActive;
    }

    void SetCurrentSceneIndex(int scene) {
        if (_currentScene != scene && scene > -1 && scene < _children.size()) {
            _previousScene = _currentScene;
            _currentScene = scene;
            _currentSceneNeedsInit = true;
        }
    }

    void Update(unsigned milliseconds) override {
        if (!_topMenuActive && _currentScene > -1) {
            _children[_currentScene]->Update(milliseconds);
        }
    }

    void DrawSceneMenu() {
        if (!_topMenuActive)
        {
            _view.fillRect(0, 0, 128, 16, 0);
            return;
        }

        int sceneIndex = 0;

        int x = _children.size() * 16;
        _view.fillRect(x, 0, 128, 16, 0xFFFF);
        
        x = 0;
        for(auto&& scene : _children) {
            const bool sceneSelected = sceneIndex == _currentScene;
            auto *baseScene = static_cast<BaseScene*>(scene);
            const uint16_t * icon = baseScene->GetIcon(sceneSelected);
            DrawIcon(icon, x, 0, baseScene->GetIconWidth(), baseScene->GetIconHeight());
            x+=16;
            sceneIndex++;
        }
    }
    void AddScene(BaseScene *scene){
        _children.push_back(scene);
        _menuEnabled = _children.size() > 1;
    }

    void DrawIcon(const uint16_t * icon, int x, int y, int iconWidth, int iconHeight) {
        for (int i=0; i < iconWidth; i++)
            for (int j=0; j < iconHeight; j++)
                _view.drawPixel(x+i, y+j, icon[i + (j * iconWidth)]);
    }

    virtual void NoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->NoteOn(channel, pitch, velocity);
    }
    virtual void NoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->NoteOff(channel, pitch, velocity);
    }
    virtual void AfterTouchPoly(byte inChannel, byte inNote, byte inValue)  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->AfterTouchPoly(inChannel, inNote, inValue);
    }
    virtual void ControlChange(byte inChannel, byte inNumber, byte inValue) {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->ControlChange(inChannel, inNumber, inValue);
    }
    virtual void ProgramChange(byte inChannel, byte inNumber)  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->ProgramChange(inChannel, inNumber);
    }
    virtual void AfterTouchChannel(byte inChannel, byte inPressure)  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->AfterTouchChannel(inChannel, inPressure);
    }
    virtual void PitchBend(byte inChannel, int inValue) {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->PitchBend(inChannel, inValue);
    }
    virtual void SysEx(byte* inData, unsigned inSize) {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->SysEx(inData, inSize);
    }
    virtual void MtcQuarterFrame(byte inData)  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->MtcQuarterFrame(inData);
    }
    virtual void SongPosition(unsigned inBeats)  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->SongPosition(inBeats);
    }
    virtual void SongSelect(byte inSongNumber)  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->SongSelect(inSongNumber);
    }
    virtual void TuneRequest()  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->TuneRequest();
    }
    virtual void Clock()  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->Clock();
    }
    virtual void Start()  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->Start();
    }
    virtual void Continue()  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->Continue();
    }
    virtual void Stop()  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->Stop();
    }
    virtual void ActiveSensing()  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->ActiveSensing();
    }
    virtual void SystemReset() {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->SystemReset();
    }
    virtual void Tick()  {
        if (_currentScene < 0 || _currentScene >= _children.size())
            return ;
        _children[_currentScene]->Tick();
    }

protected:
    bool _topMenuActive = false;
    int _currentScene = -1;
    int _previousScene = -1;
    bool _menuEnabled;
    bool _currentSceneNeedsInit = true;

};

template<class TEncoder, class TButton, class TMidiTransport>
class SceneHostControl : public BaseSceneHostControl {
public:
    SceneHostControl(View &view, unsigned int width, unsigned int height, unsigned int left, unsigned int top,
                     TEncoder &encoderUpDown,
                     TEncoder &encoderLeftRight,
                     TButton &button,
                     TButton &button2,
                     TButton &button3,
                     midi::MidiInterface<TMidiTransport> &midi) :
                        BaseSceneHostControl(view, width, height, left, top),
                       _button(button),
                       _button2(button2),
                       _button3(button3),
                       _encoderUpDown(encoderUpDown),
                       _encoderLeftRight(encoderLeftRight),
                       _midi(midi)
   {
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

    ~SceneHostControl() override {
       // _instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
    };

    void Update(unsigned milliseconds) override {
        _button.update();
        _button2.update();
        _button3.update();

        if (_button.changed() && !_button.fell()) {
            // button has been pressed...
            if (!_topMenuActive && _menuEnabled) {
                _topMenuActive = true;
                _previousScene = _currentScene;
                _currentSceneNeedsInit = true;
                DrawSceneMenu();
            } else {
                _topMenuActive = false;
                if (_previousScene != _currentScene) {
                    if (_previousScene > -1) {
                        //Serial.print("UninitScreen:");
                        //Serial.println(_previousScene);
                        _children[_currentScene]->Uninitialize();
                        _previousScene = -1;
                    }
                    _children[_currentScene]->Initialize();
                } else if (_currentSceneNeedsInit) {
                    _children[_currentScene]->Initialize();
                    _children[_currentScene]->Update(milliseconds);
                } else
                    _children[_currentScene]->Update(milliseconds);
            }
            //Serial.println(_active? "Open Menu" : "Close Menu");
            return;
        }

        if (_button2.changed() && !_button2.fell()) {
            if (_currentScene > -1) {
                _children[_currentScene]->ButtonDown(2);
            }
        }

        if (_button3.changed() && !_button3.fell()) {
            if (_currentScene > -1) {
                _children[_currentScene]->ButtonDown(3);
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
    
        if (_topMenuActive) {
            // the scene menu is visible/active
            if (right) {
                if (_currentScene < _children.size() - 1) {
                    _currentScene++;
                } else 
                    _currentScene = 0;
            } else if (left) {
                if (_currentScene > 0) {
                    _currentScene--;
                } else 
                    _currentScene = _children.size() - 1;
            }

            if (left | right) {
                DrawSceneMenu();
                //Serial.println("_currentSceneNeedsInit = true");
            }

        } else 
        {
            if (_currentScene > -1) {
                if (up) {
                    _children[_currentScene]->IndexScroll(false);
                } else if (down) {
                    _children[_currentScene]->IndexScroll(true);
                }

                if (left) {
                    _children[_currentScene]->ValueScroll(false);
                } else if (right) {
                    _children[_currentScene]->ValueScroll(true);
                }

                if (_currentSceneNeedsInit) {
                    _currentSceneNeedsInit = false;
                    _children[_currentScene]->Initialize();
                }

                _children[_currentScene]->Update(milliseconds);
            }
        
        }
    }

    void Init() {
        _instances.push_back(this);
    }

    static void handleNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
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
    TButton &_button;
    TButton &_button2;
    TButton &_button3;
    Encoder &_encoderUpDown;
    Encoder &_encoderLeftRight;
    midi::MidiInterface<TMidiTransport>&_midi;
    // encoder stuff
    long Position = 0, oldPosition = 0;
    long PositionY = 0, oldPositionY = 0;
    static std::vector<SceneHostControl*> _instances;
};

template<class TEncoder, class TButton, class TMidiTransport>
std::vector<SceneHostControl<TEncoder, TButton, TMidiTransport>*> SceneHostControl<TEncoder, TButton, TMidiTransport>::_instances;

#endif  //TEENSY_SCENE_CONTROLLER_SCENECONTROLLER_H
