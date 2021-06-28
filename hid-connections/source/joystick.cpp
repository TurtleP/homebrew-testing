#include "joystick.h"

#include "log.h"

static constexpr HidNpadStyleTag INVALID_STYLE_TAG = static_cast<HidNpadStyleTag>(-1);

Joystick::Joystick(size_t index) : index(index)
{}

HidNpadIdType Joystick::Split()
{
    /* only allow splitting dual joycon */
    if (this->styleTag != HidNpadStyleTag_NpadJoyDual)
        return this->GetID();

    HidNpadIdType nextId = static_cast<HidNpadIdType>(this->index + 1);

    Result result = hidSetNpadJoyAssignmentModeSingle(this->GetID(), HidNpadJoyDeviceType_Left);

    if (R_SUCCEEDED(result))
        result = hidSetNpadJoyAssignmentModeSingle(nextId, HidNpadJoyDeviceType_Right);

    return nextId;
}

bool Joystick::Merge(const Joystick* other)
{
    if (this->styleTag == HidNpadStyleTag_NpadJoyLeft &&
        other->GetStyleTag() != HidNpadStyleTag_NpadJoyRight)
        return false;

    Result result = hidMergeSingleJoyAsDualJoy(this->GetID(), other->GetID());

    if (R_SUCCEEDED(result))
        result = hidSetNpadJoyAssignmentModeDual(this->GetID());

    return R_SUCCEEDED(result);
}

bool Joystick::Open(size_t index)
{
    if (index == 0)
        padInitializeDefault(&this->state);
    else
        padInitialize(&this->state, this->GetID());

    padUpdate(&this->state);

    this->styleTag = this->GetStyleTag();
    LOG("StyleTag is %x", this->styleTag);

    switch (this->styleTag)
    {
        case HidNpadStyleTag_NpadFullKey:
            this->name = "Pro Controller";
            break;
        case HidNpadStyleTag_NpadHandheld:
            this->name = "Nintendo Switch";
            break;
        case HidNpadStyleTag_NpadJoyDual:
            this->name = "Dual Joy-Con";
            break;
        case HidNpadStyleTag_NpadJoyLeft:
            this->name = "Left Joy-Con";
            break;
        case HidNpadStyleTag_NpadJoyRight:
            this->name = "Right Joy-Con";
            break;
        default:
            /* not supported */
            LOG("Not Supported!");
            return false;
    }

    return padIsConnected(&this->state);
}

HidNpadStyleTag Joystick::GetStyleTag() const
{
    uint32_t styleSet = padGetStyleSet(&this->state);
    LOG("padStyleSet %x", styleSet);

    if (styleSet & HidNpadStyleTag_NpadFullKey)
        return HidNpadStyleTag_NpadFullKey;
    else if (styleSet & HidNpadStyleTag_NpadHandheld)
        return HidNpadStyleTag_NpadHandheld;
    else if (styleSet & HidNpadStyleTag_NpadJoyDual)
        return HidNpadStyleTag_NpadJoyDual;
    else if (styleSet & HidNpadStyleTag_NpadJoyLeft)
        return HidNpadStyleTag_NpadJoyLeft;
    else if (styleSet & HidNpadStyleTag_NpadJoyRight)
        return HidNpadStyleTag_NpadJoyRight;

    return INVALID_STYLE_TAG;
}

bool Joystick::IsDown(size_t button)
{
    uint64_t buttons = padGetButtonsDown(&this->state);

    if (buttons & button)
        return true;

    return false;
}
