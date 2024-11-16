#ifndef TEENSY_CONTROLS_H
#define TEENSY_CONTROLS_H
#include <queue>
#include <stack>
#include <functional>
#include <Encoder.h>
#include "View.h"
#include "VirtualView.h"
#include "rect.h"
#include <Bounce2.h>
#include <iostream>

class TeensyControl : public VirtualView {
public:
    TeensyControl(View &view, std::function<void()> updateFn, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        VirtualView (view,  x, y, width, height),
        f_update(updateFn),
        _children(),
        _needsRedraw(false)
        {
    }

    virtual ~TeensyControl() {
    }

    virtual void Update(unsigned millis) {
        if (f_update != nullptr) {
            f_update();
        }
        for (auto && child : _children){
            child->Update(millis);
        }
    }

    virtual void ForceRedraw() {
        _needsRedraw = true;
        for(auto &child:_children) {
            child->ForceRedraw();
        }
    }

    virtual void ValueScroll(const bool forward) {
    }
    
    virtual void ButtonDown(uint8_t buttonNumber) {
    }

    virtual void IndexScroll(const bool forward) {
    }

    virtual void NoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) { }
    virtual void NoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) { }
    virtual void AfterTouchPoly(byte inChannel, byte inNote, byte inValue) { }
    virtual void ControlChange(byte inChannel, byte inNumber, byte inValue) { }
    virtual void ProgramChange(byte inChannel, byte inNumber) { }
    virtual void AfterTouchChannel(byte inChannel, byte inPressure) { }
    virtual void PitchBend(byte inChannel, int inValue) { }
    virtual void SysEx(byte* inData, unsigned inSize) {}
    virtual void MtcQuarterFrame(byte inData) {}
    virtual void SongPosition(unsigned inBeats) {}
    virtual void SongSelect(byte inSongNumber) {}
    virtual void TuneRequest() {}
    virtual void Clock() {}
    virtual void Start() {}
    virtual void Continue() {}
    virtual void Stop() {}
    virtual void ActiveSensing() {}
    virtual void SystemReset() {}
    virtual void Tick() {}
protected:
    std::function<void()> f_update = nullptr;
    std::vector<TeensyControl *> _children;
    bool _needsRedraw;
};

template< typename TDisplay >
class ViewController {
public:
    ViewController(TDisplay &display, Encoder &encoderUpDown, Encoder &encoderLeftRight, Bounce2::Button *button = nullptr, std::function< void() > fnButtonPressed = nullptr) : 
        _display(display),
        _controls(),
        _dialogs(),
        _encoderUpDown(encoderUpDown),
        _encoderLeftRight(encoderLeftRight),
        _button(button),
        _fnButtonPressed(fnButtonPressed)
    {
    }

    virtual ~ViewController() {
    }

    void SetActive(bool active) {
        if (active == _active)
            return;
        _active = active;
    }
    
    bool Active() {
        return _active;
    }

    void Refresh(unsigned millis) {
        if (_dialogs.size() > 0){
            _dialogs.top()->Update(millis);
            return;
        }
        if (!_needsRefresh) 
            return;
        for (auto && ctrl : _controls) {
            ctrl->Update(millis);
        }
        _needsRefresh = false;
    }

    void Update(unsigned millis) {
        Process(millis);
        Refresh(millis);
    }

    void Process(unsigned millis)
    {
        {
            bool left = false, right = false, up = false, down = false;

            Position = _encoderLeftRight.read();
            if ((Position - oldPosition) < 0) {
                left = true;
            }
            if ((Position - oldPosition) > 0) {
                right = true;
            }

            PositionY = _encoderUpDown.read();
            if ((PositionY - oldPositionY) > 0) {
                up = true;
            }
            if ((PositionY - oldPositionY) < 0) {
                down = true;
            }
            
            if (up) {
                _display.YOffsetDelta(- 2);
                _needsRefresh = true;
            } else if (down) {
                _display.YOffsetDelta( 2);
                _needsRefresh = true;
            } 

            if (left) {
                    _display.XOffsetDelta(- 2);
                _needsRefresh = true;
            } else if (right) {
                    _display.XOffsetDelta( 2);
                _needsRefresh = true;
            } 
            
            oldPosition = Position;
            oldPositionY = PositionY;
        }
        
        if (_button != nullptr && _fnButtonPressed != nullptr) {
            _button->update();

            if (_button->changed() && !_button->fell()) {
                // button has been pressed...
                _fnButtonPressed();
            }
        }
    }

    void AddControl(TeensyControl *control){
        _controls.push_back(control);
        _totalHeight += control->height();
    }

    void AddDialog(TeensyControl *control){
        _dialogs.push(control);
    }

protected:
    TDisplay &_display;
    bool _active = false;
    bool _needsRefresh = true;
    std::vector< TeensyControl* > _controls;
    std::stack< TeensyControl* > _dialogs;
    int _selectedControl = -1;
    std::function< void() > _fnButtonPressed;

    //signed short _xOffset = 0;
    //signed short _yOffset = 0;

    unsigned int _totalHeight = 0;
    
    Encoder &_encoderUpDown;
    Encoder &_encoderLeftRight;
    Bounce2::Button *_button;
    // encoder stuff
    long Position = 0, oldPosition = 0;
    long PositionY = 0, oldPositionY = 0;

};

class TeensyMenu : public TeensyControl {
public:
    TeensyMenu(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y, uint16_t colorMenuItemBackground, uint16_t colorMenuItemBackgroundSelected) : 
        TeensyControl(
            view, 
            nullptr,//std::bind(& TeensyMenu:: DrawBackground, this),
            width, height, x, y),
        _colorMenuItemBackground(colorMenuItemBackground),
        _colorMenuItemBackgroundSelected(colorMenuItemBackgroundSelected)
        {
        }

    virtual ~TeensyMenu() {
    }

    void DrawBackground() {
        _display.fillRect(_left, _top, _width, _height, _colorMenuItemBackground);

        if (_selectedIndex > -1 && _selectedIndex < _children.size() ) {
            fillRect(0, _children[_selectedIndex]->Top(), _width, _children[_selectedIndex]->Height(), _colorMenuItemBackgroundSelected);
        }
    }

    void Update(const unsigned millis) override {
        if (_needsRedraw) {
            //fillRect(0, 0, _width, _height, _colorMenuItemBackground);
            DrawBackground();
            _needsRedraw = false;
        }
        TeensyControl::Update(millis);
    }

    void AddControl(TeensyControl *control) {
        control->SetTop( _currentTop );
        _currentTop += control->Height();
        _children.push_back(control);
    }

    void IndexScroll(const bool forward) override {
        if (forward && _selectedIndex < _children.size() -1) {
            const int previousSelectedIndex = _selectedIndex;
            _selectedIndex++;
            _needsRedraw = true;
            ScrollIfNeeded();
            _children[previousSelectedIndex]->ForceRedraw();
            _children[_selectedIndex]->ForceRedraw();
        } else if (!forward && _selectedIndex > 0) {
            const int previousSelectedIndex = _selectedIndex;
            _selectedIndex--;
            _needsRedraw = true;
            ScrollIfNeeded();
            _children[previousSelectedIndex]->ForceRedraw();
            _children[_selectedIndex]->ForceRedraw();
        }
    }

    void ScrollIfNeeded() {
        if (_currentTop > _height) {

            int top = 0;
            for (int j = 0; j < _selectedIndex; j++)
                top += _children[j]->Height();

            int newYOffset = _yOffset;
            if (top >= _currentTop - (_height / 2) ) {
                newYOffset = _currentTop - _height;
            }  else if (top > _height/2) {
                newYOffset = top - _height/2; 
            } else if (top <= _height/2) {
                newYOffset = 0;
            }

            if (newYOffset != _yOffset) {
                _yOffset = newYOffset;
                _displayclipy2 = _height - 1 + _yOffset;
                //_displayclipy1 = _yOffset;

                _display.fillRect(_left, _top, _width, _height, _colorMenuItemBackground);
            }
        }
    }
    
    void ValueScroll(const bool forward) override {
        if (_selectedIndex < 0 || _selectedIndex > _children.size() -1 || _children.size() < 1)
            return;
        
        _children[_selectedIndex]->ValueScroll(forward);
    }

    void NoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) override {
        if (_selectedIndex < 0 || _selectedIndex > _children.size() -1 || _children.size() < 1)
            return;
        _children[_selectedIndex]->NoteOn(channel, pitch, velocity);
    }

    void ControlChange(uint8_t channel, uint8_t data1, uint8_t data2) override {
        if (_selectedIndex < 0 || _selectedIndex > _children.size() -1 || _children.size() < 1)
            return;
        _children[_selectedIndex]->ControlChange(channel, data1, data2);
    }

    void ButtonDown(uint8_t buttonNumber) override {
        if (_selectedIndex < 0 || _selectedIndex > _children.size() -1 || _children.size() < 1)
            return;
        _children[_selectedIndex]->ButtonDown(buttonNumber);
    }

    int GetSelectedIndex() const {
        return _selectedIndex;
    }

protected:
    unsigned int _currentTop = 0;
    int _selectedIndex = 0;
    uint16_t _colorMenuItemBackground;
    uint16_t _colorMenuItemBackgroundSelected;

};

class TeensyMenuItem : public TeensyControl {
public:
    TeensyMenuItem(
            View &view,
            std::function<void(View*)> updateWithView, 
            unsigned int height, 
            std::function<void(bool forward)> menuValueScroll = nullptr,
            std::function<bool(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity)> menuMidiNoteEvent = nullptr,
            std::function<bool(uint8_t channel, uint8_t data1, uint8_t data2)> menuMidiCCEvent = nullptr,
            std::function<void(uint8_t buttonNumber)> buttonDownEvent = nullptr
        ) : 
        TeensyControl (view, nullptr, 128, height, 0, 0),
        _updateWithView(updateWithView),
        _menuValueScroll(menuValueScroll),
        _menuMidiNoteEvent(menuMidiNoteEvent),
        _menuMidiCCEvent(menuMidiCCEvent),
        _buttonDownEvent(buttonDownEvent)
    {
    }

    ~TeensyMenuItem() override = default;

    void Update(const unsigned milliseconds) override {
        if (_updateWithView != nullptr) {
            _updateWithView(this);
        } else TeensyControl::Update(milliseconds);
    }
    
    void ValueScroll(const bool forward) override {
        if (_menuValueScroll != nullptr) {
            _menuValueScroll(forward);
        }
    }

    void NoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) override {
        if (_menuMidiNoteEvent != nullptr) {
            _menuMidiNoteEvent(true, channel, pitch, velocity);
        }
    }
    void NoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) override {
        if (_menuMidiNoteEvent != nullptr) {
            _menuMidiNoteEvent(false, channel, pitch, velocity);
        }
    }

    void ControlChange(uint8_t channel, uint8_t data1, uint8_t data2) override {
        if (_menuMidiCCEvent != nullptr) {
            _menuMidiCCEvent(channel, data1, data2);
        }
    }

    void ButtonDown(uint8_t buttonNumber) override {
        if (_buttonDownEvent != nullptr) {
            return _buttonDownEvent(buttonNumber);
        } 
    }

private:
    std::function<void(View*)> _updateWithView;
    std::function<void(bool forward)> _menuValueScroll;
    std::function<void(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity)> _menuMidiNoteEvent;
    std::function<void(uint8_t channel, uint8_t data1, uint8_t data2)> _menuMidiCCEvent;
    std::function<void(uint8_t buttonNumber)> _buttonDownEvent;

};
class TeensyStringMenuItem : public TeensyMenuItem {
public:
    TeensyStringMenuItem(
            TeensyMenu &menu,
            const String &label,
            const std::function<void(uint8_t buttonNumber)>& buttonDownEvent) :
        TeensyMenuItem (menu, nullptr, 10, nullptr, nullptr, nullptr, buttonDownEvent), //std::bind(&TeensyStringMenuItem::ButtonDown, this, std::placeholders::_1)),
        _label(label),
        buttonDownEvent(buttonDownEvent)
    {
    }

    ~TeensyStringMenuItem() override = default;

    void Update(unsigned milliseconds) override {
        setTextSize(1);
        drawString(_label.c_str(), 0, 1);//+_top-_yOffset);
    }

    void ButtonDown(unsigned char buttonNumber) override {
        if (buttonDownEvent) {
            buttonDownEvent(buttonNumber);
        }
    }

private:
    const String &_label;
    std::function<void(int)> buttonDownEvent;
};

class TeensyMenuController {
public:
    TeensyMenuController(TeensyMenu &teensyMenu) :
        _teensyMenu(teensyMenu)
        {
        }

    virtual ~TeensyMenuController() {
    }

protected:
    TeensyMenu &_teensyMenu;
};
#endif  //TEENSY_CONTROLS_H
