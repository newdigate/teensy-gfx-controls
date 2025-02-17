//
// Created by Nicholas Newdigate on 11/07/2022.
//

#ifndef TEENSY_GFX_CONTROLS_VIRTUALVIEW_H
#define TEENSY_GFX_CONTROLS_VIRTUALVIEW_H

#include "rect.h"
#include "View.h"

class VirtualView : public View {
public:
    VirtualView(View &actualDisplay, int16_t left, int16_t top, uint16_t width, uint16_t height)
    : View(width, height),
      _view(actualDisplay),
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

    virtual void ClearBackground() {
        fillRect(_left, _top, _width, _height, 0);
    }

    void XOffset(int16_t xOffset) {
        ClearBackground();
        _xOffset = xOffset;
    }

    void XOffsetDelta(int16_t deltaXOffset) {
        XOffset(_xOffset + deltaXOffset );
    }

    int16_t YOffset() {
        return _yOffset;
    }

    void YOffset(int16_t yOffset) {
        ClearBackground();
        _yOffset = yOffset;
    }

    void YOffsetDelta(int16_t deltaYOffset) {
        YOffset(_yOffset + deltaYOffset );
    }

    void Pixel(int16_t x, int16_t y, uint16_t color) override{

        x = x - _xOffset;
        y = y - _yOffset;
        _view.drawPixel(x + _left, y + _top, color);
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override
    {
        if((x < _displayclipx1) ||(x >= _displayclipx2) || (y < _displayclipy1 + _yOffset) || (y >= _displayclipy2 +_yOffset)) return;

        x += _originx;
        y += _originy;

        Pixel(x,y, color);
    }

    void fillRect(int x, int y, int width, int height, uint16_t color) {
        //if (_isClipping) {
            // TODO: check for no intersection
            //if (isInRect(x, y, _clipWindowX1, _clipWindowY1, _clipWindowX2, _clipWindowY2))
            //    return;
        //}
        if (x + width > _width)
            width = _width - x;
        if (y + height > _height + _yOffset)
            height = _height + _yOffset - y;
        View::fillRect(x, y, width, height, color);
    }

    void drawString(const char *str, int x, int y) {
        View::drawString(str, x , y);
    }

    int16_t Left() { return  _left; }
    int16_t Top()  { return  _top; }
    uint16_t Width() { return  _width; }
    uint16_t Height() { return  _height; }

    void SetLeft(int16_t left) { _left = left; }
    void SetTop(int16_t top) { _top = top; }
    void SetWidth(uint16_t width) { _width = width; }
    void SetHeight(uint16_t height) { _height = height; }
protected:
    View &_view;
    int16_t _left;
    int16_t _top;
    int16_t _xOffset = 0;
    int16_t _yOffset = 0;
    uint16_t _width;
    uint16_t _height;

    bool _isClipping = true;
    int _clipWindowX1 = 0, _clipWindowY1 = 0, _clipWindowX2 = 0, _clipWindowY2 = 0;

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

#endif //TEENSY_GFX_CONTROLS_VIRTUALVIEW_H
