#pragma once

#include "platform/CCAccelerometerDelegate.h"

NS_CC_BEGIN

class CC_DLL CCAccelerometer
{
public:
    CCAccelerometer();
    ~CCAccelerometer();

    void setDelegate(CCAccelerometerDelegate* pDelegate);
    void update( double x,double y,double z,double timestamp );
private:
    CCAcceleration m_obAccelerationValue;
    CCAccelerometerDelegate* m_pAccelDelegate;
};

NS_CC_END
