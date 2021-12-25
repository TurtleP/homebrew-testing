#pragma once

#include <memory>
#include <string>

#include "guid.h"

#include <switch.h>

class Joystick
{
  public:
    Joystick()
    {}

    Joystick(size_t index);

    ~Joystick()
    {
        this->Close();
    };

    void Close();

    bool Open(size_t index);

    bool IsConnected() const
    {
        return padIsConnected(&this->state);
    }

    void Update()
    {
        padUpdate(&this->state);
    }

    HidNpadIdType Split();

    bool Merge(const Joystick* other);

    bool IsDown(size_t button);

    std::string GetName() const
    {
        return this->name;
    };

    HidNpadStyleTag GetStyleTag() const;

    HidNpadIdType GetID() const
    {
        return this->instanceID;
    }

    std::string GetGUID() const
    {
        return this->guid;
    }

    void* GetHandle() const
    {
        return this->fakeHandle;
    }

  private:
    size_t index;

    PadState state;
    HidNpadStyleTag styleTag;

    HidNpadIdType instanceID;

    void* fakeHandle;

    std::string name;
    std::string guid;
};
