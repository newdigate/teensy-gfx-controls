#ifndef TEENSY_BUTTONS_H
#define TEENSY_BUTTONS_H
#include "teensy_controls.h"

class TeensyButton : public TeensyControl {
public:
    TeensyButton(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        TeensyControl (view, nullptr, width, height, x, y),
        _needsRedraw(true),
        _depressed(false),
        _highlighted(false)
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
    void SetHighlighted(bool highlighted) {
        if (_highlighted != highlighted) {
            _highlighted = highlighted;
            _needsRedraw = true;
        }
    }

    virtual void Update() {
      if (_needsRedraw) {
            const uint16_t color1 = _depressed? colors[3] : colors[1];
            const uint16_t color2 = _depressed? colors[1] : colors[3];
          const uint16_t colorbg = _depressed? colors[1] : colors[2];

          _display.drawPixel(_left, _top, 0);
          _display.drawPixel(_left + _width-1, _top, 0);
          _display.drawPixel(_left, _top + _height-1, 0);
          _display.drawPixel(_left + _width-1, _top + _height-1, 0);

          _display.fillRect(_left+1, _top+1, _width-2, _height-2, colorbg);
          _display.drawFastHLine(_left+1, _top, _width - 2, color2 );
          _display.drawFastVLine(_left, _top+1, _height - 3, color2 );
          _display.drawFastHLine(_left + 1, _top + _height - 2, _width - 2, color1 );
          _display.drawFastVLine(_left + _width - 1, _top + 1, _height - 3, color1 );

          if (_highlighted) {
              _display.fillRect(_left + 2, _top + 2, _width -4, _height-5, 0x255A );
              _display.drawRoundRect(_left + 1, _top + 1, _width -2, _height-3, 2, 0x255A );
          }
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
    bool _highlighted;
    uint16_t colors[4] = {0x0000,0x6B4D,0xBDF7,0xFFFF};
};

class TeensyButtonRecord : public TeensyButton {
public:
    TeensyButtonRecord(View& view) : TeensyButton(view, 13, 16, 0, 0),
    _lastMilliseconds(0),
    _indicatorOn(true),
    _isRecording(false)
    {
    }

    void SetRecording(bool recording) {
        if (_isRecording != recording) {
            _isRecording = recording;
            _needsRedraw = true;
        }
    }

    void Update() override {
        TeensyButton::Update();
        if (!_isRecording) return;
        if (unsigned currentMillis = millis(); currentMillis > _lastMilliseconds + 500) {
            _indicatorOn = !_indicatorOn;
            _needsRedraw = true;
            _lastMilliseconds = currentMillis;
        }
    }

    void DrawButton() override {
        if (!_isRecording) {
            _display.drawCircle(_left + 6, _top + 7, 4, 0xD105);
            return;
        }
        if (_indicatorOn)
            _display.fillCircle(_left + 6, _top + 7, 4, 0xD105);
        else
            _display.fillCircle(_left + 6, _top + 7, 4, 0);
    }

    void ValueScroll(bool forward) override {

    }

    void IncreaseSelectedIndex() override {

    }

    void DecreaseSelectedIndex() override {

    }
private:
    unsigned _lastMilliseconds;
    bool _indicatorOn;
    bool _isRecording;
};

class TeensyButtonBar : public TeensyControl {
public:
    TeensyButtonBar(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        TeensyControl (view, nullptr, width, height, x, y),
        _totalWidth(0),
        _selectedIndex(-1)
    {
    }

    virtual ~TeensyButtonBar() {
    }

    void AddButton(TeensyButton *button) {
        button->SetLeft(_totalWidth);
        _totalWidth += button->width() + 1;
        _children.push_back(button);
    }

    void Update() override {
        for (auto && child : _children){
            child->Update();
        }
    }
    void ButtonDown(uint8_t buttonNumber) override {

    }
    void ValueScroll(bool forward) override {
    }
    
    void IncreaseSelectedIndex() override {
        if (_selectedIndex == -1 || _selectedIndex < _children.size() - 1) {
            if (_selectedIndex >= 0) {
                auto *selectedButton = dynamic_cast<TeensyButton*>(_children[_selectedIndex]);
                selectedButton->SetHighlighted(false);
            }
            _selectedIndex += 1;
            auto *selectedButton = dynamic_cast<TeensyButton*>(_children[_selectedIndex]);
            selectedButton->SetHighlighted(true);
        }
    }

    void DecreaseSelectedIndex() override {
        if (_selectedIndex > 0) {
            if (_selectedIndex >= 0 && _selectedIndex < _children.size()) {
                auto *selectedButton = dynamic_cast<TeensyButton*>(_children[_selectedIndex]);
                selectedButton->SetHighlighted(false);
            }
            _selectedIndex -= 1;
            auto *selectedButton = dynamic_cast<TeensyButton*>(_children[_selectedIndex]);
            selectedButton->SetHighlighted(true);
        }
    }

protected:
    unsigned int _totalWidth;
    int _selectedIndex;

};

#endif  //TEENSY_CONTROLS_H
