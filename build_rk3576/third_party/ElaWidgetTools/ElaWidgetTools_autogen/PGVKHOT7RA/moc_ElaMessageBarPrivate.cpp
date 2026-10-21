/****************************************************************************
** Meta object code from reading C++ file 'ElaMessageBarPrivate.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/private/ElaMessageBarPrivate.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaMessageBarPrivate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaMessageBarPrivate_t {
    QByteArrayData data[7];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaMessageBarPrivate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaMessageBarPrivate_t qt_meta_stringdata_ElaMessageBarPrivate = {
    {
QT_MOC_LITERAL(0, 0, 20), // "ElaMessageBarPrivate"
QT_MOC_LITERAL(1, 21, 15), // "pOpacityChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 19), // "pTimePercentChanged"
QT_MOC_LITERAL(4, 58, 13), // "messageBarEnd"
QT_MOC_LITERAL(5, 72, 8), // "pOpacity"
QT_MOC_LITERAL(6, 81, 12) // "pTimePercent"

    },
    "ElaMessageBarPrivate\0pOpacityChanged\0"
    "\0pTimePercentChanged\0messageBarEnd\0"
    "pOpacity\0pTimePercent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaMessageBarPrivate[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       2,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   31,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
       5, QMetaType::QReal, 0x00495003,
       6, QMetaType::QReal, 0x00495003,

 // properties: notify_signal_id
       0,
       1,

       0        // eod
};

void ElaMessageBarPrivate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaMessageBarPrivate *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pOpacityChanged(); break;
        case 1: _t->pTimePercentChanged(); break;
        case 2: _t->messageBarEnd(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ElaMessageBarPrivate::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaMessageBarPrivate::pOpacityChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ElaMessageBarPrivate::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaMessageBarPrivate::pTimePercentChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ElaMessageBarPrivate *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< qreal*>(_v) = _t->_pOpacity; break;
        case 1: *reinterpret_cast< qreal*>(_v) = _t->_pTimePercent; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ElaMessageBarPrivate *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->_pOpacity != *reinterpret_cast< qreal*>(_v)) {
                _t->_pOpacity = *reinterpret_cast< qreal*>(_v);
                Q_EMIT _t->pOpacityChanged();
            }
            break;
        case 1:
            if (_t->_pTimePercent != *reinterpret_cast< qreal*>(_v)) {
                _t->_pTimePercent = *reinterpret_cast< qreal*>(_v);
                Q_EMIT _t->pTimePercentChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject ElaMessageBarPrivate::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ElaMessageBarPrivate.data,
    qt_meta_data_ElaMessageBarPrivate,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaMessageBarPrivate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaMessageBarPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaMessageBarPrivate.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ElaMessageBarPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ElaMessageBarPrivate::pOpacityChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ElaMessageBarPrivate::pTimePercentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
