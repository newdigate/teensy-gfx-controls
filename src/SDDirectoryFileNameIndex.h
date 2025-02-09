#ifndef TEENSY_GFX_CONTROLS_DIRECTORYFILENAMECACHE_H
#define TEENSY_GFX_CONTROLS_DIRECTORYFILENAMECACHE_H

#include <Arduino.h>
#include <SD.h>
#include "teensy_controls.h"

namespace newdigate {

    class SDDirectoryFileNameIndex {
    public:
        SDDirectoryFileNameIndex(SDClass &sd, std::vector<String> &fileExtensions) :
            _sd(sd),
            _fileExtensions(fileExtensions)
        {
        }

        virtual ~SDDirectoryFileNameIndex() = default;

        char* getFilenameAtIndex(unsigned searchIndex, const char *directory = nullptr) {
            char *result = nullptr;
            File dir = directory? _sd.open(directory) : _sd.open("/");
            unsigned int index = 0;
            while (true) { 

                File files =  dir.openNextFile();
                if (!files) {
                    //If no more files, break out.
                    break;
                }

                bool match = false;
                String curfile = files.name(); //put file in string
                for (auto & filenameExtension : _fileExtensions) {
                    int m = curfile.lastIndexOf(filenameExtension);
                    if (m > 0) match = true;
                }
                if (!match) continue;
                int underscore = curfile.indexOf("_");
                int dot = curfile.indexOf(".");

                if ((underscore != 0) && (dot != 0)) {
                    if (index == searchIndex) {
                        char *filename = new char[curfile.length()+1] {0};
                        memcpy(filename, curfile.c_str(), curfile.length());
                        result = filename;
                        break;
                    }

                    index++;
                }

                files.close();
            }
            dir.close();
            return result;
        }

        unsigned getNumFiles(const char *directory = nullptr) {
            unsigned result = 0;
            File dir = directory? _sd.open(directory) : _sd.open("/");
            while (true) {

                File files =  dir.openNextFile();
                if (!files) {
                    break;
                }

                bool match = false;
                String curfile = files.name(); //put file in string
                for (auto & filenameExtension : _fileExtensions) {
                    int m = curfile.lastIndexOf(filenameExtension);
                    if (m > 0) match = true;
                }
                if (!match) continue;
                int underscore = curfile.indexOf("_");
                int dot = curfile.indexOf(".");

                if ((underscore != 0) && (dot != 0)) {
                    result++;
                }

                files.close();
            }
            dir.close();
            return result;
        }

    private:
        SDClass& _sd;
        std::vector<String> &_fileExtensions;
    };

    class SDFileSelectorMenuItem : public TeensyMenuItem {
        SDFileSelectorMenuItem(SDClass &sd, TeensyMenu &menu, std::vector<String> &fileExtensions, const String &label) :
                    TeensyMenuItem (menu, nullptr, 10, nullptr, nullptr, nullptr, nullptr),
                    _sd(sd),
                    _sdDirectoryFileNameIndex(sd, fileExtensions),
                    _label(label),
                    _selectedIndex(0),
                    _enabled(false),
                    _fileCount(0)
        {
        }

        ~SDFileSelectorMenuItem() override = default;

        void Initialize() {
            if (!_enabled) {
                bool connected = CheckIfMicroSDisConnected();
                _fileCount = _sdDirectoryFileNameIndex.getNumFiles();
                _enabled = true;
            }
        }

        bool CheckIfMicroSDisConnected() {
            File root = _sd.open( ".");
            return (bool)root;
        }

        void Update(unsigned milliseconds) override {
            if (!_enabled) Initialize();
            if (_needsRedraw) {
                setTextSize(1);
                drawString(_label.c_str(), 0, 1);//+_top-_yOffset);
                if (_enabled && _fileCount > 0 && _selectedIndex >= -0) {
                    if (currentFilename == nullptr) {
                        currentFilename = _sdDirectoryFileNameIndex.getFilenameAtIndex(_selectedIndex);
                    }
                    drawString(currentFilename, 60, 1);
                }
                _needsRedraw = false;
            }
        }

        void ButtonDown(unsigned char buttonNumber) override {
        }

    private:
        SDClass &_sd;
        SDDirectoryFileNameIndex _sdDirectoryFileNameIndex;
        const String &_label;
        int _selectedIndex;
        bool _enabled;
        int _fileCount;
        char *currentFilename;
    };

}
#endif
