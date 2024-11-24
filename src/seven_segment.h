#ifndef TEENSY_SEVENSEGMENT_H
#define TEENSY_SEVENSEGMENT_H
#include "teensy_controls.h"

class TeensySevenSegment : public TeensyControl {
public:
    TeensySevenSegment(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        TeensyControl (view, nullptr, width, height, x, y),
        _needsRedraw(true),
        _highlighted(false),
        _digit(0)
    {
    }

    virtual ~TeensySevenSegment() {
    }

    void SetDigit(uint8_t digit) {
        if (digit != _digit){
            _digit = digit;
            _needsRedraw = true;
        }
    }

    void ForceRedraw() override {
        _needsRedraw = true;
    }

    void Update(unsigned milliseconds) override {
        if (_needsRedraw) {
            const int16_t point1x = _left,               point1y = _top;
            const int16_t point2x = _left + _width - 1,  point2y = _top;
            const int16_t point3x = _left,               point3y = _top + _height/2;
            const int16_t point4x = _left + _width - 1,  point4y = _top + _height/2;
            const int16_t point5x = _left,               point5y = _top + _height;
            const int16_t point6x = _left + _width - 1,  point6y = _top + _height;
            const int16_t swidth = _width - 2;
            const int16_t sheight = _height/2 - 1;

            bool segments[7] = {false,false,false,false,false,false,false};
            switch (_digit % 10) {
                case 0: {
                    segments[0] = true;
                    segments[1] = true;
                    segments[2] = true;
                    segments[3] = true;
                    segments[4] = true;
                    segments[5] = true;
                    segments[6] = false;
                    break;
                }
                case 1: {
                    segments[0] = false;
                    segments[1] = true;
                    segments[2] = true;
                    segments[3] = false;
                    segments[4] = false;
                    segments[5] = false;
                    segments[6] = false;
                    break;
                }
                case 2: {
                    segments[0] = true;
                    segments[1] = true;
                    segments[2] = false;
                    segments[3] = true;
                    segments[4] = true;
                    segments[5] = false;
                    segments[6] = true;
                    break;
                }
                case 3: {
                    segments[0] = true;
                    segments[1] = true;
                    segments[2] = true;
                    segments[3] = true;
                    segments[4] = false;
                    segments[5] = false;
                    segments[6] = true;
                    break;
                }
                case 4: {
                    segments[0] = false;
                    segments[1] = true;
                    segments[2] = true;
                    segments[3] = false;
                    segments[4] = false;
                    segments[5] = true;
                    segments[6] = true;
                    break;
                }
                case 5: {
                    segments[0] = true;
                    segments[1] = false;
                    segments[2] = true;
                    segments[3] = true;
                    segments[4] = false;
                    segments[5] = true;
                    segments[6] = true;
                    break;
                }
                case 6: {
                    segments[0] = true;
                    segments[1] = false;
                    segments[2] = true;
                    segments[3] = true;
                    segments[4] = true;
                    segments[5] = true;
                    segments[6] = true;
                    break;
                }
                case 7: {
                    segments[0] = true;
                    segments[1] = true;
                    segments[2] = true;
                    segments[3] = false;
                    segments[4] = false;
                    segments[5] = false;
                    segments[6] = false;
                    break;
                }
                case 8: {
                    segments[0] = true;
                    segments[1] = true;
                    segments[2] = true;
                    segments[3] = true;
                    segments[4] = true;
                    segments[5] = true;
                    segments[6] = true;
                    break;
                }
                case 9: {
                    segments[0] = true;
                    segments[1] = true;
                    segments[2] = true;
                    segments[3] = false;
                    segments[4] = false;
                    segments[5] = true;
                    segments[6] = true;
                    break;
                }
                default: break;
            }

            if (_highlighted) {
                _display.fillRoundRect(_left, _top, _width, _height, 2, 0x255A );
            }
            _display.drawFastHLine(point1x + 1,   point1y,        swidth,                    segments[0]? colors[3] : colors[1]);
            _display.drawFastVLine(point2x,       point2y + 1,    sheight,                   segments[1]? colors[3] : colors[1]);
            _display.drawFastVLine(point4x,       point4y + 1,    sheight,                   segments[2]? colors[3] : colors[1]);
            _display.drawFastHLine(point5x + 1,   point5y,        swidth,                    segments[3]? colors[3] : colors[1]);
            _display.drawFastVLine(point3x,       point3y + 1,    sheight,                   segments[4]? colors[3] : colors[1]);
            _display.drawFastVLine(point1x,       point1y + 1,    sheight,                   segments[5]? colors[3] : colors[1]);
            _display.drawFastHLine(point3x + 1,   point3y,        swidth,                    segments[6]? colors[3] : colors[1]);

            _needsRedraw = false;
        }
    }

    virtual void DrawButton() {
    }

    void ValueScroll(bool forward) override {
    }

    void ButtonDown(uint8_t buttonNumber) override {
    }

    virtual void IncreaseSelectedIndex() {
    }

    virtual void DecreaseSelectedIndex() {
    }

protected:
    uint8_t _digit;
    bool _needsRedraw;
    bool _highlighted;
    uint16_t colors[4] = {0x0000,0x3A2A,0xBDF7,0xFFFF};
};

class TeensySevenSegmentSeparator : public TeensySevenSegment {
public:
    TeensySevenSegmentSeparator(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        TeensySevenSegment (view, width, height, x, y)
    {
    }

    virtual ~TeensySevenSegmentSeparator() {
    }

    void Update(unsigned millis) override {
        if (millis > _lastMilliseconds + 500) {
            _indictorOn = !_indictorOn;
            _needsRedraw = true;
            _lastMilliseconds = millis;
        }
        if (_needsRedraw) {
            const int16_t point1x = _left + _width/2 -1,               point1y = _top + _height/4;
            const int16_t point2x = _left + _width/2 - 1,              point2y = _top + _height*3/4;
            _display.drawFastVLine(point1x, point1y, 2, _indictorOn? colors[2]:colors[1]);
            _display.drawFastVLine(point2x, point2y, 2, _indictorOn? colors[2]:colors[1]);
            _needsRedraw = false;
        }
    }
private:
    bool _indictorOn = false;
    unsigned _lastMilliseconds = 0;
};

class TeensyTimeIndicator : public TeensyControl {
public:
    TeensyTimeIndicator(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        TeensyControl (view, nullptr, width, height, x, y),
        _numSegments(10),
        _needsRedraw(true),
        _highlighted(false),
        _milliseconds(0)
    {
    }

    void Init() {
        AddSegment();
        AddSegment();
        AddSegment();
        AddSegment();
        AddSeparator();
        AddSegment();
        AddSegment();
        AddSeparator();
        AddSegment();
        AddSegment();
        AddSeparator();
        AddSegment();
        AddSegment();
    }

    virtual ~TeensyTimeIndicator() {
    }

    void SetTime(const unsigned milliseconds) {
        if (milliseconds != _milliseconds) {
            _milliseconds = milliseconds;
            _needsRedraw = true;
            unsigned seconds = milliseconds / 1000;
            unsigned mseconds = milliseconds % 1000;
            unsigned minutes = seconds / 60;
            unsigned hours = minutes / 60;
            minutes = minutes % 60;
            seconds = seconds % 60;
            uint8_t hoursThousands = hours / 1000;
            uint8_t hoursHundreds = hours / 100;
            uint8_t hoursTens = hours / 10;
            uint8_t hoursOnes = hours % 10;
            uint8_t minutesTens = minutes / 10;
            uint8_t minutesOnes = minutes % 10;
            uint8_t secondsTens = seconds / 10;
            uint8_t secondsOnes = seconds % 10;
            uint8_t millisTenths = mseconds / 100;
            uint8_t millisHundredths = mseconds / 10;
            uint8_t digits[] = {hoursThousands, hoursHundreds, hoursTens, hoursOnes, minutesTens, minutesOnes, secondsTens, secondsOnes, millisTenths, millisHundredths};
            uint8_t index = 0;
            for(auto && child:_segments) {
                child->SetDigit(digits[index]);
                index++;
            }
        }
    }

    void ForceRedraw() override {
        _needsRedraw = true;
        for (auto && child: _children) {
            child->ForceRedraw();
        }
    }

    void Update(unsigned millis) override {
        if (_needsRedraw) {
            for (auto && child: _children) {
                child->Update(millis);
            }
            _needsRedraw = false;
        }
    }

protected:
    uint8_t _numSegments;
    bool _needsRedraw;
    bool _highlighted;
    uint16_t _offsetx;
    unsigned _milliseconds;
    std::vector<TeensySevenSegment*> _segments;

    void AddSegment() {
        auto && segment = new TeensySevenSegment(_display, 5, _height, _left + _offsetx, _top);
        _children.push_back(segment);
        _segments.push_back(segment);
        _offsetx += 7;
    }

    void AddSeparator() {
        auto && separator = new TeensySevenSegmentSeparator(_display, 2, _height, _left + _offsetx, _top);
        _children.push_back(separator);
        _offsetx += 3;
    }
};

class TeensyBarsAndBeatsIndicator : public TeensyControl {
public:
    TeensyBarsAndBeatsIndicator(View &view, unsigned int width, unsigned int height, unsigned int x, unsigned int y) :
        TeensyControl (view, nullptr, width, height, x, y),
        _needsRedraw(true),
        _highlighted(false),
        _bars(0),
        _beats(0)
    {
        AddSegment();
        AddSegment();
        AddSegment();
        AddSegment();
        AddSeparator();
        AddSegment();
        AddSegment();
    }

    virtual ~TeensyBarsAndBeatsIndicator() {
    }

    void SetBarsAndBeats(const unsigned bars, unsigned beats) {
        if (_bars != bars || _beats != beats) {
            _bars = bars;
            _beats = beats;
            _needsRedraw = true;
            uint8_t barsThousands = _bars / 1000;
            uint8_t barsHundreds = _bars / 100;
            uint8_t barsTens = _bars / 10;
            uint8_t barsOnes = _bars % 10;
            uint8_t beatsTens = _beats / 10;
            uint8_t beatsOnes = _beats % 10;
            uint8_t digits[] = { barsThousands, barsHundreds, barsTens, barsOnes, beatsTens, beatsOnes };
            uint8_t index = 0;
            for(auto && child:_segments) {
                child->SetDigit(digits[index]);
                index++;
            }
        }
    }

    void ForceRedraw() override {
        _needsRedraw = true;
        for (auto && child: _children) {
            child->ForceRedraw();
        }
    }

    void Update(unsigned millis) override {
        if (_needsRedraw) {
            for (auto && child: _children) {
                child->Update(millis);
            }
            _needsRedraw = false;
        }
    }

protected:
    bool _needsRedraw;
    bool _highlighted;
    uint16_t _offsetx;
    unsigned _bars, _beats;
    std::vector<TeensySevenSegment*> _segments;

    void AddSegment() {
        auto && segment = new TeensySevenSegment(_display, 5, _height, _left + _offsetx, _top);
        _children.push_back(segment);
        _segments.push_back(segment);
        _offsetx += 7;
    }

    void AddSeparator() {
        auto && separator = new TeensySevenSegmentSeparator(_display, 2, _height, _left + _offsetx, _top);
        _children.push_back(separator);
        _offsetx += 3;
    }
};

#endif  //TEENSY_CONTROLS_H
