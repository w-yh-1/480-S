#include "IRTemperatureWrapper.h"
#include <string.h>

RuleTemper::RuleTemper(QObject *parent) : QObject(parent)
{
    memset(&m_data, 0, sizeof(m_data));
}

USBSDK_RuleTemper* RuleTemper::nativePtr()
{
    return &m_data;
}

void RuleTemper::notifyChanged()
{
    emit dataChanged();
}

int RuleTemper::meterType() const { return m_data.meterType; }
uint RuleTemper::ruleId() const { return m_data.ruleId; }
uint RuleTemper::resultTypeMask() const { return m_data.resultTypeMask; }
float RuleTemper::temperatureAve() const { return m_data.temperatureAve; }
float RuleTemper::temperatureStd() const { return m_data.temperatureStd; }
float RuleTemper::temperatureMax() const { return m_data.temperatureMax; }
float RuleTemper::temperatureMin() const { return m_data.temperatureMin; }
float RuleTemper::temperatureMid() const { return m_data.temperatureMid; }
int RuleTemper::hotPointX() const { return m_data.hotPoint.x; }
int RuleTemper::hotPointY() const { return m_data.hotPoint.y; }
int RuleTemper::coldPointX() const { return m_data.coldPoint.x; }
int RuleTemper::coldPointY() const { return m_data.coldPoint.y; }

// =====================================================

IRTool::IRTool(QObject *parent) : QObject(parent)
{
   // m_ir.IR_init();
}

bool IRTool::getRectTemperature(int x1, int y1, int x2, int y2, RuleTemper *wrapper)
{
    if (!wrapper) return false;
    bool ok = m_ir.IR_RectangleTemperature(x1, y1, x2, y2, wrapper->nativePtr());
    wrapper->notifyChanged();
    return ok;
}

bool IRTool::getLineTemperature(int x1, int y1, int x2, int y2, RuleTemper *wrapper)
{
    if (!wrapper) return false;
    bool ok = m_ir.IR_LineTemperature(x1, y1, x2, y2, wrapper->nativePtr());
    wrapper->notifyChanged();
    return ok;
}

bool IRTool::getPointTemperature(int x, int y, float *temp)
{
    return m_ir.IR_PointTemperature(x, y, temp);
}

float IRTool::getPointTemp(int x, int y)
{
    float temp = 0.0f;
    m_ir.IR_PointTemperature(x, y, &temp);
    return temp;
}

bool IRTool::getHotColdPoint(HotColdInfo *wrapper)
{
    if (!wrapper) return false;
    bool ok = m_ir.IR_getHotColdPoint(wrapper->nativePtr());
    wrapper->notifyChanged();
    return ok;
}

// =====================================================

HotColdInfo::HotColdInfo(QObject *parent) : QObject(parent)
{
    memset(&m_data, 0, sizeof(m_data));
}

USBSDK_HotAndColdInfo* HotColdInfo::nativePtr()
{
    return &m_data;
}

void HotColdInfo::notifyChanged()
{
    emit dataChanged();
}

float HotColdInfo::hotTemper() const { return m_data.hotTemper; }
int HotColdInfo::hotPointX() const { return m_data.hotPoint.x; }
int HotColdInfo::hotPointY() const { return m_data.hotPoint.y; }
float HotColdInfo::coldTemper() const { return m_data.coldTemper; }
int HotColdInfo::coldPointX() const { return m_data.coldPoint.x; }
int HotColdInfo::coldPointY() const { return m_data.coldPoint.y; }

// #include "moc_IRTemperatureWrapper.cpp"
