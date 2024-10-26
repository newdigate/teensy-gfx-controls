#ifndef TEENSY_BUTTONS_H
#define TEENSY_BUTTONS_H
#include "teensy_controls.h"

class TeensyButton : public TeensyControl {
public:
    TeensyButton(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        TeensyControl (view, nullptr, width, height, x, y),
        _needsRedraw(true),
        _depressed(false)
    {
    }

    virtual ~TeensyButton() {
    }

    void SetDepressed(bool depressed) {
      if (_depressed != depressed) {
          _depressed = depressed;
          _needsRedraw = true;
      }
    }

    virtual void Update() {
      if (_needsRedraw) {
            const uint16_t color1 = _depressed? colors[3] : colors[1];
            const uint16_t color2 = _depressed? colors[1] : colors[3];

          _display.drawPixel(_left, _top, 0);
          _display.drawPixel(_left + _width-1, _top, 0);
          _display.drawPixel(_left, _top + _height-1, 0);
          _display.drawPixel(_left + _width-1, _top + _height-1, 0);

          _display.fillRect(_left+1, _top+1, _width-2, _height-2, colors[0]);
          _display.drawFastHLine(_left+1, _top, _width - 2, color2 );
          _display.drawFastVLine(_left, _top+1, _height - 3, color2 );
          _display.drawFastHLine(_left + 1, _top + _height - 2, _width - 2, color1 );
          _display.drawFastVLine(_left + _width - 1, _top + 1, _height - 3, color1 );
          DrawButton();
        _needsRedraw = false;
      }
    }

    virtual void DrawButton() {
    }

    virtual void ValueScroll(bool forward) {
    }

    virtual void ButtonDown(uint8_t buttonNumber) {
    }

    virtual void IncreaseSelectedIndex() {
    }

    virtual void DecreaseSelectedIndex() {
    }

protected:
    bool _needsRedraw;
    bool _depressed;
    uint16_t colors[4] = {0x0000,0x6B4D,0xBDF7,0xFFFF};
};

class TeensyButtonBar : public TeensyControl {
public:
    TeensyButtonBar(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        TeensyControl (view, nullptr, width, height, x, y),
        _totalWidth(0)
        {
        }

    virtual ~TeensyButtonBar() {
    }

    void AddButton(TeensyButton *button) {
        button->SetLeft(_totalWidth);
        _totalWidth += button->width();
        _children.push_back(button);
    }

    virtual void Update() {
        for (auto && child : _children){
            child->Update();
        }
    }

    virtual void ValueScroll(bool forward) { 
    }
    
    virtual void ButtonDown(uint8_t buttonNumber) {
    }

    virtual void IncreaseSelectedIndex() {
    }

    virtual void DecreaseSelectedIndex() {
    }

protected:
    unsigned int _totalWidth;
};

#endif  //TEENSY_CONTROLS_H
