#pragma once

class IGraphicObject
{
protected:
    IGraphicObject(void) {}
public:
    virtual ~IGraphicObject(void) {}

public:
    void virtual OnLostDevice() = 0;
    void virtual OnResetDevice() = 0;

    void virtual OnDestroyDevice() = 0; // Clean up;
};
