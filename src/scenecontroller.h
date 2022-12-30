#ifndef TEENSY_SCENE_CONTROLLER_SCENECONTROLLER_H
#define TEENSY_SCENE_CONTROLLER_SCENECONTROLLER_H
#include <queue>
#include <functional>

class Scene {
public:
    Scene(const uint16_t * iconOn, const uint16_t * iconOff, unsigned int iconWidth, unsigned int iconHeight) : 
        _iconWidth(iconWidth),
        _iconHeight(iconHeight),
        _iconOn(iconOn), 
        _iconOff(iconOff) {
    }

    virtual ~Scene() {
    }

    const uint16_t * GetIcon(bool on) {
        return (on)? _iconOn : _iconOff;
    }
    unsigned int GetIconWidth() { return _iconWidth; }
    unsigned int GetIconHeight() { return _iconHeight; }

    void SetUpdateFunction(std::function<void()> updateFn) {
        f_update = updateFn;
    }

    void SetButtonPressedFunction(std::function<void()> buttonPressedFn) {
        f_buttonPressed = buttonPressedFn;
    }

    void SetRotary1ChangedFunction(std::function<void(bool)> rotaryChangedFn) {
        f_rotary1Changed = rotaryChangedFn;
    }

    void SetRotary2ChangedFunction(std::function<void(bool)> rotaryChangedFn) {
        f_rotary2Changed = rotaryChangedFn;
    }

    void Update() {
        if (f_update != nullptr) {
            f_update();
        }
    }

    void ButtonPressed() {
        if (f_buttonPressed != nullptr) {
            f_buttonPressed();
        }
    }

    void Rotary1Changed(bool forward) {
        if (f_rotary1Changed != nullptr) {
            f_rotary1Changed(forward);
        }
    }

    void Rotary2Changed(bool forward) {
        if (f_rotary2Changed != nullptr) {
            f_rotary2Changed(forward);
        }
    }

private:
    unsigned int _iconWidth;
    unsigned int _iconHeight;

    std::function<void()> f_update = nullptr;
    std::function<void()> f_buttonPressed = nullptr;
    std::function<void(bool)> f_rotary1Changed = nullptr;
    std::function<void(bool)> f_rotary2Changed = nullptr;

    const uint16_t * _iconOn;
    const uint16_t * _iconOff;
};

class BaseSceneController {
public:
    BaseSceneController() : _scenes()
    {
    }

    virtual ~BaseSceneController() {
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

    void SetCurrentSceneIndex(int scene) {
        _currentScene = scene;
    }

    void Update() {
        if (!_active && _currentScene > -1) {
            _scenes[_currentScene]->Update();
        }
    }

    virtual void Process() = 0;

    virtual void FillRect(int x, int y, int width, int height, uint16_t color) = 0;

    virtual void DrawPixel(int x, int y, uint16_t color) = 0;

    void DrawSceneMenu() {
        if (!_active)
        {
            FillRect(0, 0, 128, 16, ST7735_BLACK);
            return;
        }

        int sceneIndex = 0;

        int x = _scenes.size() * 16;
        FillRect(x, 0, 128, 16, ST7735_WHITE);
        
        x = 0;
        for(auto&& scene : _scenes) {
            const bool sceneSelected = sceneIndex == _currentScene;
            const uint16_t * icon = scene->GetIcon(sceneSelected);
            DrawIcon(icon, x, 0, scene->GetIconWidth(), scene->GetIconHeight());
            x+=16;
            sceneIndex++;
        }
    }

    void AddScene(Scene *scene){
        _scenes.push_back(scene);
    }
    
    void DrawIcon(const uint16_t * icon, int x, int y, int iconWidth, int iconHeight) {
        for (int i=0; i < iconWidth; i++)
            for (int j=0; j < iconHeight; j++)
                DrawPixel(x+i, y+j, icon[i + (j * iconWidth)]);
    }
protected:
    bool _active = false;
    std::vector< Scene* > _scenes;
    int _currentScene = -1;
};

template< typename TDisplay, typename TEncoder, typename TButton >
class SceneController : public BaseSceneController {
public:
    SceneController(TDisplay &display, TEncoder &encoderUpDown, TEncoder &encoderLeftRight, TButton &button) : 
        BaseSceneController(),
        _display(display),
        _encoderUpDown(encoderUpDown),
        _encoderLeftRight(encoderLeftRight),
        _button(button)
    {
    }

    virtual ~SceneController() {
    }

    virtual void Process() override {
        _button.update();

        if (_button.changed() && !_button.fell()) {
            //Serial.println("TOGGLED");
            if (!_active) {
                _active = true;
                DrawSceneMenu();
            } else {
                _active = false;
            }
        }

        if (!_active) {
            Update();
        }

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

        oldPosition = Position;
        oldPositionY = PositionY;
    
        if (_active) {

            if (right) {
                if (_currentScene < _scenes.size()-1) {
                    _currentScene++;
                } else 
                    _currentScene = 0;
            } else if (left) {
                if (_currentScene > 0) {
                    _currentScene--;
                } else 
                    _currentScene = _scenes.size()-1;
            } 

            if (left | right | up | down) {
                DrawSceneMenu();
            }
        } else 
        {
            if (_currentScene > -1) {
                if (up) {
                    _scenes[_currentScene]->Rotary1Changed(false);
                } else if (down) {
                    _scenes[_currentScene]->Rotary1Changed(true);
                }

                if (left) {
                    _scenes[_currentScene]->Rotary2Changed(false);
                } else if (right) {
                    _scenes[_currentScene]->Rotary2Changed(true);
                }              
            }
        }
    }

    virtual void FillRect(int x, int y, int width, int height, uint16_t color) override {
        _display.fillRect(x, y, width, height, color);
    };

    virtual void DrawPixel(int x, int y, uint16_t color) override {
        _display.Pixel(x, y, color);
    };

protected:
    TDisplay &_display;
    TButton &_button;
    Encoder &_encoderUpDown;
    Encoder &_encoderLeftRight;
    
    // encoder stuff
    long Position = 0, oldPosition = 0;
    long PositionY = 0, oldPositionY = 0;
};

#endif  //TEENSY_SCENE_CONTROLLER_SCENECONTROLLER_H
