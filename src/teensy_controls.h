#ifndef TEENSY_CONTROLS_H
#define TEENSY_CONTROLS_H
#include <queue>
#include <functional>
#include <Encoder.h>
#include "View.h"
#include "VirtualView.h"
#include "rect.h"

class TeensyControl : public VirtualView {
public:
    TeensyControl(View &view, std::function<void()> updateFn, unsigned int width, unsigned int height, unsigned int x, unsigned int y) : 
        VirtualView (view,  x, y, width, height),
        f_update(updateFn),
        _children()
        {
        }

    virtual ~TeensyControl() {
    }

    void Update() {
        if (f_update != nullptr) {
            f_update();
        }
        for (auto && child : _children){
            child->Update();
        }
    }

    virtual void ValueScroll(bool forward) { 
    }
    
    virtual bool MidiNoteEvent(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity) { 
        return false;
    }

    virtual bool MidiCCEvent(uint8_t channel, uint8_t data1, uint8_t data2) {
        return false;
    }

protected:
    std::function<void()> f_update = nullptr;
    std::vector<TeensyControl *> _children;
};

template< typename TDisplay >
class ViewController {
public:
    ViewController(TDisplay &display, Encoder &encoderUpDown, Encoder &encoderLeftRight) : 
        _display(display),
        _controls(),
        _encoderUpDown(encoderUpDown),
        _encoderLeftRight(encoderLeftRight)
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

    void Refresh() {
        if (!_needsRefresh) 
            return;
        for (auto && ctrl : _controls) {
            ctrl->Update();
        }
        _needsRefresh = false;
    }

    void Update() {
        Process();
        Refresh();
    }

    void Process() {
        
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

    void AddControl(TeensyControl *control){
        _controls.push_back(control);
        _totalHeight += control->height();
    }

protected:
    TDisplay &_display;
    bool _active = false;
    bool _needsRefresh = true;
    std::vector< TeensyControl* > _controls;
    int _selectedControl = -1;

    //signed short _xOffset = 0;
    //signed short _yOffset = 0;

    unsigned int _totalHeight = 0;
    
    Encoder &_encoderUpDown;
    Encoder &_encoderLeftRight;
    
    // encoder stuff
    long Position = 0, oldPosition = 0;
    long PositionY = 0, oldPositionY = 0;

};

class TeensyMenu : public TeensyControl {
public:
    TeensyMenu(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y, uint16_t colorMenuItemBackground, uint16_t colorMenuItemBackgroundSelected) : 
        TeensyControl (view, nullptr, x, y, width, height),
        _colorMenuItemBackground(colorMenuItemBackground),
        _colorMenuItemBackgroundSelected(colorMenuItemBackgroundSelected)
        {
        }

    virtual ~TeensyMenu() {
    }

    virtual void Update() {
        if (NeedsUpdate) {
            //fillRect(0, 0, _width, _height, _colorMenuItemBackground);
            _display.fillRect(_left, _top, _width, _height, _colorMenuItemBackground);

            if (_selectedIndex > -1 && _selectedIndex < _children.size() ) {
                fillRect(0, _children[_selectedIndex]->Top(), _width, _children[_selectedIndex]->Height(), _colorMenuItemBackgroundSelected);
            }
            TeensyControl::Update();
            NeedsUpdate = false;
        }
    }

    void AddControl(TeensyControl *control) {
        control->SetTop( _currentTop );
        _currentTop += control->Height();
        _children.push_back(control);
    }

    void IncreaseSelectedIndex() {
        if (_selectedIndex < _children.size() -1) {
            _selectedIndex++;
            NeedsUpdate = true;
            ScrollIfNeeded();
        }
    }
    void DecreaseSelectedIndex() {
        if (_selectedIndex > 0 ) {
            _selectedIndex--;
            NeedsUpdate = true;
            ScrollIfNeeded();
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
    
    void ValueScroll(bool forward) override { 
        if (_selectedIndex < 0 || _selectedIndex > _children.size() -1)
            return;
        
        _children[_selectedIndex]->ValueScroll(forward);
        NeedsUpdate = true;
    }

    bool MidiNoteEvent(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity) override { 
        if (_selectedIndex < 0 || _selectedIndex > _children.size() -1)
            return false;
        bool result = _children[_selectedIndex]->MidiNoteEvent(noteDown, channel, pitch, velocity);
        NeedsUpdate = true;   
        return result;
    }

    bool MidiCCEvent(uint8_t channel, uint8_t data1, uint8_t data2) override {
        if (_selectedIndex < 0 || _selectedIndex > _children.size() -1)
            return false;
        bool result = _children[_selectedIndex]->MidiCCEvent(channel, data1, data2);
        NeedsUpdate = true;   
        return result;
    }

    bool NeedsUpdate = true;
     
protected:
    int _currentTop = 0;
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
            std::function<bool(uint8_t channel, uint8_t data1, uint8_t data2)> menuMidiCCEvent = nullptr
        ) : 
        TeensyControl (view, std::bind(&TeensyMenuItem::MenuItemUpdate, this), 128, height, 0, 0),
        _updateWithView(updateWithView),
        _menuValueScroll(menuValueScroll),
        _menuMidiNoteEvent(menuMidiNoteEvent),
        _menuMidiCCEvent(menuMidiCCEvent)
    {
    }

    virtual ~TeensyMenuItem() {
    }

    virtual void MenuItemUpdate() { 
        if (_updateWithView != nullptr) {
            _updateWithView(this);
        }
    }
    
    virtual void ValueScroll(bool forward) override { 
        if (_menuValueScroll != nullptr) {
            _menuValueScroll(forward);
        }
    }

    bool MidiNoteEvent(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity) override {
        if (_menuMidiNoteEvent != nullptr) {
            return _menuMidiNoteEvent(noteDown, channel, pitch, velocity);
        }
        return false;
    }

    bool MidiCCEvent(uint8_t channel, uint8_t data1, uint8_t data2) override {
        if (_menuMidiCCEvent != nullptr) {
            return _menuMidiCCEvent(channel, data1, data2);
        }
        return false;
    }

private:
    std::function<void(View*)> _updateWithView;
    std::function<void(bool forward)> _menuValueScroll;
    std::function<bool(bool noteDown, uint8_t channel, uint8_t pitch, uint8_t velocity)> _menuMidiNoteEvent;
    std::function<bool(uint8_t channel, uint8_t data1, uint8_t data2)> _menuMidiCCEvent;
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
