/****************************************************************************
** Meta object code from reading C++ file 'IRTemperatureWrapper.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../IRTemperatureWrapper.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'IRTemperatureWrapper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RuleTemper_t {
    QByteArrayData data[15];
    char stringdata0[173];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RuleTemper_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RuleTemper_t qt_meta_stringdata_RuleTemper = {
    {
QT_MOC_LITERAL(0, 0, 10), // "RuleTemper"
QT_MOC_LITERAL(1, 11, 11), // "dataChanged"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 14), // "temperatureMax"
QT_MOC_LITERAL(4, 39, 9), // "meterType"
QT_MOC_LITERAL(5, 49, 6), // "ruleId"
QT_MOC_LITERAL(6, 56, 14), // "resultTypeMask"
QT_MOC_LITERAL(7, 71, 14), // "temperatureAve"
QT_MOC_LITERAL(8, 86, 14), // "temperatureStd"
QT_MOC_LITERAL(9, 101, 14), // "temperatureMin"
QT_MOC_LITERAL(10, 116, 14), // "temperatureMid"
QT_MOC_LITERAL(11, 131, 9), // "hotPointX"
QT_MOC_LITERAL(12, 141, 9), // "hotPointY"
QT_MOC_LITERAL(13, 151, 10), // "coldPointX"
QT_MOC_LITERAL(14, 162, 10) // "coldPointY"

    },
    "RuleTemper\0dataChanged\0\0temperatureMax\0"
    "meterType\0ruleId\0resultTypeMask\0"
    "temperatureAve\0temperatureStd\0"
    "temperatureMin\0temperatureMid\0hotPointX\0"
    "hotPointY\0coldPointX\0coldPointY"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RuleTemper[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
      12,   26, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       3,    0,   25,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Float,

 // properties: name, type, flags
       4, QMetaType::Int, 0x00495001,
       5, QMetaType::UInt, 0x00495001,
       6, QMetaType::UInt, 0x00495001,
       7, QMetaType::Float, 0x00495001,
       8, QMetaType::Float, 0x00495001,
       3, QMetaType::Float, 0x00495001,
       9, QMetaType::Float, 0x00495001,
      10, QMetaType::Float, 0x00495001,
      11, QMetaType::Int, 0x00495001,
      12, QMetaType::Int, 0x00495001,
      13, QMetaType::Int, 0x00495001,
      14, QMetaType::Int, 0x00495001,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,

       0        // eod
};

void RuleTemper::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RuleTemper *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataChanged(); break;
        case 1: { float _r = _t->temperatureMax();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RuleTemper::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RuleTemper::dataChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<RuleTemper *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->meterType(); break;
        case 1: *reinterpret_cast< uint*>(_v) = _t->ruleId(); break;
        case 2: *reinterpret_cast< uint*>(_v) = _t->resultTypeMask(); break;
        case 3: *reinterpret_cast< float*>(_v) = _t->temperatureAve(); break;
        case 4: *reinterpret_cast< float*>(_v) = _t->temperatureStd(); break;
        case 5: *reinterpret_cast< float*>(_v) = _t->temperatureMax(); break;
        case 6: *reinterpret_cast< float*>(_v) = _t->temperatureMin(); break;
        case 7: *reinterpret_cast< float*>(_v) = _t->temperatureMid(); break;
        case 8: *reinterpret_cast< int*>(_v) = _t->hotPointX(); break;
        case 9: *reinterpret_cast< int*>(_v) = _t->hotPointY(); break;
        case 10: *reinterpret_cast< int*>(_v) = _t->coldPointX(); break;
        case 11: *reinterpret_cast< int*>(_v) = _t->coldPointY(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject RuleTemper::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_RuleTemper.data,
    qt_meta_data_RuleTemper,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RuleTemper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RuleTemper::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RuleTemper.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RuleTemper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 12;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void RuleTemper::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_HotColdInfo_t {
    QByteArrayData data[9];
    char stringdata0[88];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HotColdInfo_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HotColdInfo_t qt_meta_stringdata_HotColdInfo = {
    {
QT_MOC_LITERAL(0, 0, 11), // "HotColdInfo"
QT_MOC_LITERAL(1, 12, 11), // "dataChanged"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 9), // "hotTemper"
QT_MOC_LITERAL(4, 35, 9), // "hotPointX"
QT_MOC_LITERAL(5, 45, 9), // "hotPointY"
QT_MOC_LITERAL(6, 55, 10), // "coldTemper"
QT_MOC_LITERAL(7, 66, 10), // "coldPointX"
QT_MOC_LITERAL(8, 77, 10) // "coldPointY"

    },
    "HotColdInfo\0dataChanged\0\0hotTemper\0"
    "hotPointX\0hotPointY\0coldTemper\0"
    "coldPointX\0coldPointY"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HotColdInfo[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       6,   20, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::Float, 0x00495001,
       4, QMetaType::Int, 0x00495001,
       5, QMetaType::Int, 0x00495001,
       6, QMetaType::Float, 0x00495001,
       7, QMetaType::Int, 0x00495001,
       8, QMetaType::Int, 0x00495001,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       0,
       0,

       0        // eod
};

void HotColdInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HotColdInfo *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HotColdInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HotColdInfo::dataChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<HotColdInfo *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< float*>(_v) = _t->hotTemper(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->hotPointX(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->hotPointY(); break;
        case 3: *reinterpret_cast< float*>(_v) = _t->coldTemper(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->coldPointX(); break;
        case 5: *reinterpret_cast< int*>(_v) = _t->coldPointY(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject HotColdInfo::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_HotColdInfo.data,
    qt_meta_data_HotColdInfo,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HotColdInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HotColdInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HotColdInfo.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HotColdInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void HotColdInfo::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_IRTool_t {
    QByteArrayData data[18];
    char stringdata0[156];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IRTool_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IRTool_t qt_meta_stringdata_IRTool = {
    {
QT_MOC_LITERAL(0, 0, 6), // "IRTool"
QT_MOC_LITERAL(1, 7, 18), // "getRectTemperature"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 2), // "x1"
QT_MOC_LITERAL(4, 30, 2), // "y1"
QT_MOC_LITERAL(5, 33, 2), // "x2"
QT_MOC_LITERAL(6, 36, 2), // "y2"
QT_MOC_LITERAL(7, 39, 11), // "RuleTemper*"
QT_MOC_LITERAL(8, 51, 7), // "wrapper"
QT_MOC_LITERAL(9, 59, 18), // "getLineTemperature"
QT_MOC_LITERAL(10, 78, 19), // "getPointTemperature"
QT_MOC_LITERAL(11, 98, 1), // "x"
QT_MOC_LITERAL(12, 100, 1), // "y"
QT_MOC_LITERAL(13, 102, 6), // "float*"
QT_MOC_LITERAL(14, 109, 4), // "temp"
QT_MOC_LITERAL(15, 114, 12), // "getPointTemp"
QT_MOC_LITERAL(16, 127, 15), // "getHotColdPoint"
QT_MOC_LITERAL(17, 143, 12) // "HotColdInfo*"

    },
    "IRTool\0getRectTemperature\0\0x1\0y1\0x2\0"
    "y2\0RuleTemper*\0wrapper\0getLineTemperature\0"
    "getPointTemperature\0x\0y\0float*\0temp\0"
    "getPointTemp\0getHotColdPoint\0HotColdInfo*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IRTool[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    5,   39,    2, 0x02 /* Public */,
       9,    5,   50,    2, 0x02 /* Public */,
      10,    3,   61,    2, 0x02 /* Public */,
      15,    2,   68,    2, 0x02 /* Public */,
      16,    1,   73,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::Bool, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, 0x80000000 | 7,    3,    4,    5,    6,    8,
    QMetaType::Bool, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, 0x80000000 | 7,    3,    4,    5,    6,    8,
    QMetaType::Bool, QMetaType::Int, QMetaType::Int, 0x80000000 | 13,   11,   12,   14,
    QMetaType::Float, QMetaType::Int, QMetaType::Int,   11,   12,
    QMetaType::Bool, 0x80000000 | 17,    8,

       0        // eod
};

void IRTool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<IRTool *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { bool _r = _t->getRectTemperature((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< RuleTemper*(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 1: { bool _r = _t->getLineTemperature((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< RuleTemper*(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->getPointTemperature((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< float*(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { float _r = _t->getPointTemp((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 4: { bool _r = _t->getHotColdPoint((*reinterpret_cast< HotColdInfo*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 4:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< RuleTemper* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 4:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< RuleTemper* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< HotColdInfo* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject IRTool::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_IRTool.data,
    qt_meta_data_IRTool,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *IRTool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IRTool::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_IRTool.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int IRTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
