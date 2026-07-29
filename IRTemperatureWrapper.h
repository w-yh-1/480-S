#ifndef IRTEMPERATUREWRAPPER_H
#define IRTEMPERATUREWRAPPER_H

#include <QObject>
#include "ir_camera_usb.h"

class RuleTemper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int meterType        READ meterType        NOTIFY dataChanged)
    Q_PROPERTY(uint ruleId          READ ruleId          NOTIFY dataChanged)
    Q_PROPERTY(uint resultTypeMask  READ resultTypeMask  NOTIFY dataChanged)
    Q_PROPERTY(float temperatureAve READ temperatureAve  NOTIFY dataChanged)
    Q_PROPERTY(float temperatureStd READ temperatureStd  NOTIFY dataChanged)
    Q_PROPERTY(float temperatureMax READ temperatureMax  NOTIFY dataChanged)
    Q_PROPERTY(float temperatureMin READ temperatureMin  NOTIFY dataChanged)
    Q_PROPERTY(float temperatureMid READ temperatureMid  NOTIFY dataChanged)
    Q_PROPERTY(int hotPointX        READ hotPointX       NOTIFY dataChanged)
    Q_PROPERTY(int hotPointY        READ hotPointY       NOTIFY dataChanged)
    Q_PROPERTY(int coldPointX       READ coldPointX      NOTIFY dataChanged)
    Q_PROPERTY(int coldPointY       READ coldPointY      NOTIFY dataChanged)

public:
    explicit RuleTemper(QObject *parent = nullptr);
    USBSDK_RuleTemper* nativePtr();
    void notifyChanged();

    int meterType() const;
    uint ruleId() const;
    uint resultTypeMask() const;
    float temperatureAve() const;
    float temperatureStd() const;
    Q_INVOKABLE float temperatureMax() const;
    float temperatureMin() const;
    float temperatureMid() const;
    int hotPointX() const;
    int hotPointY() const;
    int coldPointX() const;
    int coldPointY() const;

signals:
    void dataChanged();

private:
    USBSDK_RuleTemper m_data;
};

class HotColdInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float hotTemper READ hotTemper NOTIFY dataChanged)
    Q_PROPERTY(int hotPointX READ hotPointX NOTIFY dataChanged)
    Q_PROPERTY(int hotPointY READ hotPointY NOTIFY dataChanged)
    Q_PROPERTY(float coldTemper READ coldTemper NOTIFY dataChanged)
    Q_PROPERTY(int coldPointX READ coldPointX NOTIFY dataChanged)
    Q_PROPERTY(int coldPointY READ coldPointY NOTIFY dataChanged)

public:
    explicit HotColdInfo(QObject *parent = nullptr);
    USBSDK_HotAndColdInfo* nativePtr();
    void notifyChanged();

    float hotTemper() const;
    int hotPointX() const;
    int hotPointY() const;
    float coldTemper() const;
    int coldPointX() const;
    int coldPointY() const;

signals:
    void dataChanged();

private:
    USBSDK_HotAndColdInfo m_data;
};


class IRTool : public QObject
{
    Q_OBJECT
public:
    explicit IRTool(QObject *parent = nullptr);

    Q_INVOKABLE bool getRectTemperature(int x1, int y1, int x2, int y2, RuleTemper *wrapper);
    Q_INVOKABLE bool getLineTemperature(int x1, int y1, int x2, int y2, RuleTemper *wrapper);
    Q_INVOKABLE bool getPointTemperature(int x, int y, float *temp);
    Q_INVOKABLE float getPointTemp(int x, int y);
    Q_INVOKABLE bool getHotColdPoint(HotColdInfo *wrapper);
private:
    IRCameraUSB m_ir;
};

#endif
