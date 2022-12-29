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
    void Update() {
        if (f_update != nullptr) {
            f_update();
        }
    }
private:
    unsigned int _iconWidth;
    unsigned int _iconHeight;
    std::function<void()> f_update = nullptr;

    const uint16_t * _iconOn;
    const uint16_t * _iconOff;
};

template< typename TDisplay, typename TEncoder >
class SceneController {
public:
    SceneController(TDisplay &display, TEncoder &encoderUpDown, TEncoder &encoderLeftRight) : 
        _display(display),
        _scenes(),
        _encoderUpDown(encoderUpDown),
        _encoderLeftRight(encoderLeftRight)
    {
    }

    virtual ~SceneController() {
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

    void Process() {
        if (_active) {
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

            if (right) {
                if (_currentScene < _scenes.size()-1) {
                    _currentScene++;
                } else 
                    _currentScene = 0;
                DrawSceneMenu();
            } else if (left) {
                if (_currentScene > 0) {
                    _currentScene--;
                } else 
                    _currentScene = _scenes.size()-1;
            } 

            if (left | right | up | down){
                DrawSceneMenu();
            }
            oldPosition = Position;
            oldPositionY = PositionY;
        }
    }

    void DrawSceneMenu() {
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
    }

    void AddScene(Scene *scene){
        _scenes.push_back(scene);
    }


    void DrawIcon(const uint16_t * icon, int x, int y, int iconWidth, int iconHeight) {
        for (int i=0; i < iconWidth; i++)
            for (int j=0; j < iconHeight; j++)
                _display.drawPixel(x+i, y+j, icon[i + (j * iconWidth)]);
    }
protected:
    TDisplay &_display;
    bool _active = false;
    std::vector< Scene* > _scenes;
    int _currentScene = -1;
    
    Encoder &_encoderUpDown;
    Encoder &_encoderLeftRight;
    
    // encoder stuff
    long Position = 0, oldPosition = 0;
    long PositionY = 0, oldPositionY = 0;

};

#endif  //TEENSY_SCENE_CONTROLLER_SCENECONTROLLER_H
