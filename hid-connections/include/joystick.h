#pragma once

#include <string>
#include <switch.h>

class Joystick
{
  public:
    Joystick()
    {}

    Joystick(size_t index);

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
        return static_cast<HidNpadIdType>(HidNpadIdType_No1 + this->index);
    }

  private:
    size_t index;
    PadState state;
    HidNpadStyleTag styleTag;

    std::string name;
};
