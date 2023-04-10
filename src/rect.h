#ifndef TEENSY_CONTROLS_RECT_H
#define TEENSY_CONTROLS_RECT_H

class rect {
public:
    int _x1, 
        _y1, 
        _x2, 
        _y2;

    rect(int x1, int y1, int x2, int y2) :
        _x1(x1),
        _y1(y1),
        _x2(x2),
        _y2(y2)
    {
    }
} ;

#endif  //TEENSY_CONTROLS_RECT_H
