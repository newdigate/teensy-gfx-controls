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
    TeensyControl(View &view, std::function<void(int16_t,int16_t)> updateFn, unsigned int width, unsigned int height, unsigned int x, unsigned int y) : 
        VirtualView (view,  x, y, width, height),
        f_update(updateFn),
        _children()
        {
        }

    virtual ~TeensyControl() {
    }

    void Update(uint16_t x, uint16_t y) {
        if (f_update != nullptr) {
            f_update(x, y);
        }
        for (auto && child : _children){
            child->Update(x, y);
        }
    }

private:
    std::function<void(int16_t,int16_t)> f_update = nullptr;
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
        DrawSceneMenu();
    }
    
    bool Active() {
        return _active;
    }

    void Refresh() {
        if (!_needsRefresh) 
            return;
        for (auto && ctrl : _controls) {
            ctrl->Update(0, 0);
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
                _display.YOffsetDelta(- 5);
                _needsRefresh = true;
            } else if (down) {
                _display.YOffsetDelta( 5);
                _needsRefresh = true;
            } 

            if (left) {
                 _display.XOffsetDelta(- 5);
                _needsRefresh = true;
            } else if (right) {
                 _display.XOffsetDelta( 5);
                _needsRefresh = true;
            } 
            
            oldPosition = Position;
            oldPositionY = PositionY;
    }

    void DrawSceneMenu() {
        /*
        if (!_active)
        {
            _display.fillRect(0, 0, 128, 16, ST7735_BLACK);
            return;
        }

        int sceneIndex = 0;

        int x = _scenes.size() * 16;
        _display.fillRect(x, 0, 128, 16, ST7735_WHITE);
        
        x = 0;
        for(auto&& scene : _scenes) {
            const bool sceneSelected = sceneIndex == _currentScene;
            const uint16_t * icon = scene->GetIcon(sceneSelected);
            DrawIcon(icon, x, 0, scene->GetIconWidth(), scene->GetIconHeight());
            x+=16;
            sceneIndex++;
        }
        */
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

#endif  //TEENSY_CONTROLS_H
