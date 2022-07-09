#ifndef TEENSY_CONTROLS_H
#define TEENSY_CONTROLS_H
#include <queue>
#include <functional>
#include <Encoder.h>
#include "View.h"

class rect {
public:
    int x1 = 0, 
        y1 = 0, 
        x2 = 0, 
        y2 = 0;
} ;

class VirtualView : public View {
public:
    VirtualView(View &actualDisplay, int16_t left, int16_t top, uint16_t width, uint16_t height)
    :   View(width, height),
        _display(actualDisplay),
        _left(left), 
        _top(top), 
        _width(width), 
        _height(height)
    {
        _displayclipx1 = 0;
        _displayclipx2 = _width-1;

        _displayclipy1 = 0;
        _displayclipy2 = _height-1;

        _clipWindowX1 = 0;
        _clipWindowY1 = 0;
        _clipWindowX2 = _width-1;
        _clipWindowY2 = _height-1;
    }

    virtual ~VirtualView() {
    }

    int16_t XOffset() {
        return _xOffset;
    }

    void XOffset(int16_t xOffset) {
        _xOffset = xOffset;
    }

    void XOffsetDelta(int16_t deltaXOffset) {
        _xOffset = deltaXOffset;
    }

    int16_t YOffset() {
        return _yOffset;
    }
    
    void YOffset(int16_t yOffset) {
        _yOffset = yOffset;
    }

    void YOffsetDelta(int16_t deltaYOffset) {
        _yOffset += deltaYOffset;
    }

    virtual void Pixel(int16_t x, int16_t y, uint16_t color) {
        x = x - _xOffset;
        y = y - _yOffset;
        if (_isClipping) {
            if ((x < _clipWindowX1) || (x > _clipWindowX2) || (y < _clipWindowY1) || (y > _clipWindowY2)) 
                return;
        }
        if (_isMasking) {
            for (auto && rect : _maskOutAreas) 
                if ((rect->x1 < x < rect->x2) && (rect->y1 < y < rect->y2)) 
                    return;
        }
        _display.drawPixel(x + _left, y + _top, color);
    }
    
    void fillRect(int x, int y, int width, int height, uint16_t color) {
        //if (_isClipping) {
            // TODO: check for no intersection
            //if (isInRect(x, y, _clipWindowX1, _clipWindowY1, _clipWindowX2, _clipWindowY2))
            //    return;
        //}
        if (x + width > _width)
            width = _width - x;
        if (y + height > _height)
            height = _height - y;
        View::fillRect(x, y, width, height, color);
    }

    void drawString(const char *str, int x, int y) {
         if (false && _isClipping) {
            int16_t left,top;
            uint16_t width, height;
            _display.getTextBounds(str, 0, 0, &left, &top, &width, &height);
            //if (isInRect(x, y, _clipWindowX1, _clipWindowY1, _clipWindowX2, _clipWindowY2))
            //    return;
        }
        View::drawString(str, x , y);
    }

protected:
    View &_display;
    int16_t _left;
    int16_t _top;
    int16_t _xOffset = 0;
    int16_t _yOffset = 0;
    uint16_t _width; 
    uint16_t _height;

    bool _isClipping = true;
    int _clipWindowX1 = 0, _clipWindowY1 = 0, _clipWindowX2 = 0, _clipWindowY2 = 0; 
    bool _isMasking = false;
    std::vector<rect *> _maskOutAreas = std::vector<rect *>();

    inline bool isInRect(int x, int y, int left, int top, int right, int bottom) {
        if (x < left) 
            return false;
        if (y < top) 
            return false;
        if (x > right) 
            return false;
        if (y > top) 
            return false;
        return true;
    }
};

class TeensyControl {
public:
    TeensyControl(std::function<void(int16_t,int16_t)> updateFn, unsigned int width, unsigned int height, unsigned int x, unsigned int y) : 
        f_update(updateFn),
        _width(width),
        _height(height),
        _x(x), 
        _y(y),
        _children() {
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
    int Width() {
        return _width;
    }

    int Height() {
        return _height;
    }
    
    int X() {
        return _x;
    }

    int Y() {
        return _y;
    }


private:
    std::function<void(int16_t,int16_t)> f_update = nullptr;
    unsigned int _width;
    unsigned int _height;
    unsigned int _x;
    unsigned int _y;
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

    void Update() {
        Process();
        if (!_needsRefresh) 
            return;
        for (auto && ctrl : _controls) {
            ctrl->Update(_xOffset, _yOffset);
        }
        _needsRefresh = false;
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
                SetYOffset(_yOffset - 1);
            } else if (down) {
                SetYOffset(_yOffset + 1);
            } 

            if (left) {
                SetXOffset(_xOffset - 1);
            } else if (right) {
                SetXOffset(_xOffset + 1);
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
        _totalHeight += control->Height();
    }
    signed short XOffset() {
        return _xOffset;
    }

    signed short YOffset() {
        return _yOffset;
    }

    void SetXOffset(signed short xoffset) {
        if (xoffset == _xOffset)
            return;
        _xOffset = xoffset;
        _needsRefresh = true;
    }
    
    void SetYOffset(signed short yoffset) {
        if (yoffset == _yOffset)
            return;
        _yOffset = yoffset;
        _needsRefresh = true;
    }

protected:
    TDisplay &_display;
    bool _active = false;
    bool _needsRefresh = true;
    std::vector< TeensyControl* > _controls;
    int _selectedControl = -1;

    signed short _xOffset = 0;
    signed short _yOffset = 0;

    unsigned int _totalHeight = 0;
    
    Encoder &_encoderUpDown;
    Encoder &_encoderLeftRight;
    
    // encoder stuff
    long Position = 0, oldPosition = 0;
    long PositionY = 0, oldPositionY = 0;

};

#endif  //TEENSY_CONTROLS_H
