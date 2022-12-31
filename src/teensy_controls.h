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
    TeensyMenu(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) : 
        TeensyControl (view, nullptr, x, y, width, height)
        {
        }

    virtual ~TeensyMenu() {
    }

    void AddControl(TeensyControl *control) {
        control->SetTop( _currentTop );
        _currentTop += control->Height();
        _children.push_back(control);
    }
     
protected:
    int _currentTop = 0;
    int _selectedIndex = 0;
};

#endif  //TEENSY_CONTROLS_H
