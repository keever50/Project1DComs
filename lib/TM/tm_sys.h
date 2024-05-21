#pragma once

class Tm_sys
{
public:
    void init();
    void yield();
};

extern Tm_sys tm_sys;