#include "CCAccelerometer.h"
#include "ccMacros.h"

NS_CC_BEGIN

CCAccelerometer::CCAccelerometer() : 
    m_pAccelDelegate(NULL)
{
    memset(&m_obAccelerationValue, 0, sizeof(m_obAccelerationValue));
}

CCAccelerometer::~CCAccelerometer() 
{
}

void CCAccelerometer::setDelegate(CCAccelerometerDelegate* pDelegate) 
{
    m_pAccelDelegate = pDelegate;
}

void CCAccelerometer::update( double x,double y,double z,double timestamp ) 
{
    if (m_pAccelDelegate)
    {
        m_obAccelerationValue.x            = x;
        m_obAccelerationValue.y            = y;
        m_obAccelerationValue.z            = z;
        m_obAccelerationValue.timestamp = timestamp;

        // Delegate
        m_pAccelDelegate->didAccelerate(&m_obAccelerationValue);
    }    
}

NS_CC_END

