/****************************************************************************
** Meta object code from reading C++ file 'ElaProgressRingPrivate.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/private/ElaProgressRingPrivate.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaProgressRingPrivate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaProgressRingPrivate_t {
    QByteArrayData data[8];
    char stringdata0[127];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaProgressRingPrivate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaProgressRingPrivate_t qt_meta_stringdata_ElaProgressRingPrivate = {
    {
QT_MOC_LITERAL(0, 0, 22), // "ElaProgressRingPrivate"
QT_MOC_LITERAL(1, 23, 17), // "pBusyIndexChanged"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 20), // "pBusyStartDegChanged"
QT_MOC_LITERAL(4, 63, 22), // "pBusyContentDegChanged"
QT_MOC_LITERAL(5, 86, 10), // "pBusyIndex"
QT_MOC_LITERAL(6, 97, 13), // "pBusyStartDeg"
QT_MOC_LITERAL(7, 111, 15) // "pBusyContentDeg"

    },
    "ElaProgressRingPrivate\0pBusyIndexChanged\0"
    "\0pBusyStartDegChanged\0pBusyContentDegChanged\0"
    "pBusyIndex\0pBusyStartDeg\0pBusyContentDeg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaProgressRingPrivate[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       3,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,
       4,    0,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       5, QMetaType::Int, 0x00495003,
       6, QMetaType::Int, 0x00495003,
       7, QMetaType::Int, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       2,

       0        // eod
};

void ElaProgressRingPrivate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaProgressRingPrivate *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pBusyIndexChanged(); break;
        case 1: _t->pBusyStartDegChanged(); break;
        case 2: _t->pBusyContentDegChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ElaProgressRingPrivate::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaProgressRingPrivate::pBusyIndexChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ElaProgressRingPrivate::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaProgressRingPrivate::pBusyStartDegChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ElaProgressRingPrivate::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaProgressRingPrivate::pBusyContentDegChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ElaProgressRingPrivate *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->_pBusyIndex; break;
        case 1: *reinterpret_cast< int*>(_v) = _t->_pBusyStartDeg; break;
        case 2: *reinterpret_cast< int*>(_v) = _t->_pBusyContentDeg; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ElaProgressRingPrivate *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->_pBusyIndex != *reinterpret_cast< int*>(_v)) {
                _t->_pBusyIndex = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->pBusyIndexChanged();
            }
            break;
        case 1:
            if (_t->_pBusyStartDeg != *reinterpret_cast< int*>(_v)) {
                _t->_pBusyStartDeg = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->pBusyStartDegChanged();
            }
            break;
        case 2:
            if (_t->_pBusyContentDeg != *reinterpret_cast< int*>(_v)) {
                _t->_pBusyContentDeg = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->pBusyContentDegChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject ElaProgressRingPrivate::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ElaProgressRingPrivate.data,
    qt_meta_data_ElaProgressRingPrivate,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaProgressRingPrivate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaProgressRingPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaProgressRingPrivate.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ElaProgressRingPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ElaProgressRingPrivate::pBusyIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ElaProgressRingPrivate::pBusyStartDegChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ElaProgressRingPrivate::pBusyContentDegChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
