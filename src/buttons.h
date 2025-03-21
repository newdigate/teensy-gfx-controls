#ifndef TEENSY_BUTTONS_H
#define TEENSY_BUTTONS_H
#include "teensy_controls.h"

class TeensyButton : public TeensyControl {
public:
    TeensyButton(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        TeensyControl (view, nullptr, width, height, x, y),
        _depressed(false),
        _highlighted(false)
    {
        _needsRedraw = true;
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
    void ForceRedraw() override {
        _needsRedraw = true;
    }

    void Update(unsigned milliseconds) override {
      if (_needsRedraw) {
            const uint16_t color1 = _depressed? colors[3] : colors[1];
            const uint16_t color2 = _depressed? colors[1] : colors[3];
          const uint16_t colorbg = _depressed? colors[1] : colors[2];

          _view.fillRect(_left + 1, _top + 1, _width - 2, _height - 2, colorbg);
          _view.drawFastHLine(_left + 1, _top, _width - 2, color2 );
          _view.drawFastVLine(_left, _top + 1, _height - 3, color2 );
          _view.drawFastHLine(_left + 1, _top + _height - 2, _width - 2, color1 );
          _view.drawFastVLine(_left + _width - 1, _top + 1, _height - 3, color1 );

          if (_highlighted) {
              _view.fillRoundRect(_left + 2, _top + 2, _width - 4, _height - 5, 2, 0x255A );
          }
          DrawButton();

        _needsRedraw = false;
      }
    }

    virtual void DrawButton() {
    }

    void ValueScroll(bool forward) override {
        _depressed = !_depressed;
        _needsRedraw = true;
    }

protected:
    bool _depressed;
    bool _highlighted;
    uint16_t colors[4] = {0x0000,0x6B4D,0xBDF7,0xFFFF};
};

class TeensyButtonRecord : public TeensyButton {
public:
    TeensyButtonRecord(View& view, bool &isRecording) : TeensyButton(view, 13, 16, 0, 0),
    _lastMilliseconds(0),
    _indicatorOn(true),
    _isRecording(isRecording)
    {
    }

    void Update(unsigned milliseconds) override {
        if (_lastIsRecordingValue != _isRecording){
            _needsRedraw = true;
        }
        TeensyButton::Update(milliseconds);
        if (!_isRecording) return;
        if (milliseconds > _lastMilliseconds + 500) {
            _indicatorOn = !_indicatorOn;
            _needsRedraw = true;
            _lastMilliseconds = milliseconds;
        }
    }

    void DrawButton() override {
        if (!_isRecording) {
            _view.drawCircle(_left + 6, _top + 7, 4, 0xD105);
            return;
        }
        if (_indicatorOn)
            _view.fillCircle(_left + 6, _top + 7, 4, 0xD105);
        else
            _view.fillCircle(_left + 6, _top + 7, 4, 0);
    }

    void ValueScroll(const bool forward) override {
        //_isRecording = !_isRecording;
        //_needsRedraw = true;
    }

    void ButtonDown(uint8_t buttonNumber) override {
        //_isRecording = !_isRecording;
        //_needsRedraw = true;
    }

private:
    unsigned _lastMilliseconds;
    bool _indicatorOn;
    bool &_isRecording;
    bool _lastIsRecordingValue = false;
};

class TeensyButtonStop : public TeensyButton {
public:
    TeensyButtonStop(View& view) : TeensyButton(view, 16, 16, 0, 0)
    {
    }

    void DrawButton() override {
        _view.fillRect(_left + 4, _top + 4, _width - 8, _height - 9, 0);
    }
};

class TeensyButtonPause : public TeensyButton {
public:
    TeensyButtonPause(View& view) : TeensyButton(view, 16, 16, 0, 0)
    {
    }

    void DrawButton() override {
        _view.fillRect(_left + 3, _top + 4, 4, _height - 9, 0);
        _view.fillRect(_left + 9, _top + 4, 4, _height - 9, 0);
    }
};


class TeensyButtonPlay : public TeensyButton {
public:
    TeensyButtonPlay(View& view) : TeensyButton(view, 16, 16, 0, 0)
    {
    }

    void DrawButton() override {
        const int16_t left = _left + 6;
        const int16_t top = _top + 3;
        _view.fillTriangle(
            left, top,
            left, top + 8,
            left + 4, top + 4,
            0);
    }
};

class TeensyButtonRewind : public TeensyButton {
public:
    TeensyButtonRewind(View& view) : TeensyButton(view, 16, 16, 0, 0)
    {
    }

    void DrawButton() override {
        int16_t left = _left + 7;
        const int16_t top = _top + 3;
        _view.fillTriangle(
            left, top,
            left, top + 8,
            left - 4, top + 4,
            0);
        left += 5;
        _view.fillTriangle(
            left, top,
            left, top + 8,
            left - 4, top + 4,
            0);
    }
};

class TeensyButtonFastfwd : public TeensyButton {
public:
    TeensyButtonFastfwd(View& view) : TeensyButton(view, 16, 16, 0, 0)
    {
    }

    void DrawButton() override {
        int16_t left = _left + 3;
        const int16_t top = _top + 3;
        _view.fillTriangle(
            left, top,
            left, top + 8,
            left + 4, top + 4,
            0);
        left += 5;
        _view.fillTriangle(
            left, top,
            left, top + 8,
            left + 4, top + 4,
            0);
    }
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

    void ButtonDown(uint8_t buttonNumber) override {
        if (_selectedIndex > 0 && _selectedIndex < _children.size()) {
            _children[_selectedIndex]->ButtonDown(buttonNumber);
        }
    }
    void ValueScroll(const bool forward) override {
        if (_selectedIndex > 0 && _selectedIndex < _children.size()) {
            _children[_selectedIndex]->ValueScroll(forward);
        }
        //ForceRedraw();
    }
    
    void IndexScroll(const bool forward) override {
        if (forward && (_selectedIndex == -1 || _selectedIndex < _children.size() - 1)) {
            if (_selectedIndex >= 0) {
                auto *selectedButton = static_cast<TeensyButton *>(_children[_selectedIndex]);
                selectedButton->SetHighlighted(false);
            }
            _selectedIndex += 1;
            auto *selectedButton = static_cast<TeensyButton*>(_children[_selectedIndex]);
            selectedButton->SetHighlighted(true);
        } else if (!forward && _selectedIndex > 0) {
            if (_selectedIndex >= 0 && _selectedIndex < _children.size()) {
                auto *selectedButton = static_cast<TeensyButton*>(_children[_selectedIndex]);
                selectedButton->SetHighlighted(false);
            }
            _selectedIndex -= 1;
            auto *selectedButton = static_cast<TeensyButton*>(_children[_selectedIndex]);
            selectedButton->SetHighlighted(true);
        }
    }

    void ForceRedraw() override {
        for(auto &child:_children) {
            TeensyButton *button = static_cast<TeensyButton*>(child);
            button->ForceRedraw();
        }
    }

    int GetSelectedIndex() const {
        return _selectedIndex;
    }
protected:
    unsigned int _totalWidth;
    int _selectedIndex;

};


class TeensyMediaButtonBar : public TeensyButtonBar {
public:
    TeensyMediaButtonBar(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y, bool &isRecording) :
                            TeensyButtonBar (view, width, height, x, y),
                            _initialized(false),
                            button_rewind(view),
                            button_play(view),
                            button_pause(view),
                            button_stop(view),
                            button_fastfwd(view),
                            button_record(view, isRecording)
    {
    }

    virtual ~TeensyMediaButtonBar() {
    }

    void Init() {
        if (!_initialized) {
            _initialized = true;
            AddButton(&button_rewind);
            AddButton(&button_play);
            AddButton(&button_pause);
            AddButton(&button_stop);
            AddButton(&button_fastfwd);
            AddButton(&button_record);
        } else
            ForceRedraw();
    }

protected:
    bool _initialized;
    TeensyButtonRewind button_rewind;
    TeensyButtonPlay button_play;
    TeensyButtonPause button_pause;
    TeensyButtonStop button_stop;
    TeensyButtonFastfwd button_fastfwd;
    TeensyButtonRecord button_record;
};
#endif  //TEENSY_CONTROLS_H
