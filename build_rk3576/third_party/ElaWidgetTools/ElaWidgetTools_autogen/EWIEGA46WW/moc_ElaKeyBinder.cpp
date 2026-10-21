/****************************************************************************
** Meta object code from reading C++ file 'ElaKeyBinder.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/ElaKeyBinder.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaKeyBinder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaKeyBinder_t {
    QByteArrayData data[12];
    char stringdata0[216];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaKeyBinder_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaKeyBinder_t qt_meta_stringdata_ElaKeyBinder = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ElaKeyBinder"
QT_MOC_LITERAL(1, 13, 20), // "pBorderRadiusChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 21), // "pBinderKeyTextChanged"
QT_MOC_LITERAL(4, 57, 30), // "pNativeVirtualBinderKeyChanged"
QT_MOC_LITERAL(5, 88, 20), // "binderKeyTextChanged"
QT_MOC_LITERAL(6, 109, 13), // "binderKeyText"
QT_MOC_LITERAL(7, 123, 29), // "nativeVirtualBinderKeyChanged"
QT_MOC_LITERAL(8, 153, 9), // "binderKey"
QT_MOC_LITERAL(9, 163, 13), // "pBorderRadius"
QT_MOC_LITERAL(10, 177, 14), // "pBinderKeyText"
QT_MOC_LITERAL(11, 192, 23) // "pNativeVirtualBinderKey"

    },
    "ElaKeyBinder\0pBorderRadiusChanged\0\0"
    "pBinderKeyTextChanged\0"
    "pNativeVirtualBinderKeyChanged\0"
    "binderKeyTextChanged\0binderKeyText\0"
    "nativeVirtualBinderKeyChanged\0binderKey\0"
    "pBorderRadius\0pBinderKeyText\0"
    "pNativeVirtualBinderKey"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaKeyBinder[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       3,   48, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    0,   41,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,
       7,    1,   45,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::UInt,    8,

 // properties: name, type, flags
       9, QMetaType::Int, 0x00495003,
      10, QMetaType::QString, 0x00495003,
      11, QMetaType::UInt, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       2,

       0        // eod
};

void ElaKeyBinder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaKeyBinder *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pBorderRadiusChanged(); break;
        case 1: _t->pBinderKeyTextChanged(); break;
        case 2: _t->pNativeVirtualBinderKeyChanged(); break;
        case 3: _t->binderKeyTextChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->nativeVirtualBinderKeyChanged((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ElaKeyBinder::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaKeyBinder::pBorderRadiusChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ElaKeyBinder::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaKeyBinder::pBinderKeyTextChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ElaKeyBinder::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaKeyBinder::pNativeVirtualBinderKeyChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ElaKeyBinder::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaKeyBinder::binderKeyTextChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ElaKeyBinder::*)(quint32 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaKeyBinder::nativeVirtualBinderKeyChanged)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ElaKeyBinder *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getBorderRadius(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->getBinderKeyText(); break;
        case 2: *reinterpret_cast< quint32*>(_v) = _t->getNativeVirtualBinderKey(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ElaKeyBinder *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBorderRadius(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setBinderKeyText(*reinterpret_cast< QString*>(_v)); break;
        case 2: _t->setNativeVirtualBinderKey(*reinterpret_cast< quint32*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ElaKeyBinder::staticMetaObject = { {
    QMetaObject::SuperData::link<QLabel::staticMetaObject>(),
    qt_meta_stringdata_ElaKeyBinder.data,
    qt_meta_data_ElaKeyBinder,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaKeyBinder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaKeyBinder::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaKeyBinder.stringdata0))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int ElaKeyBinder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
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
void ElaKeyBinder::pBorderRadiusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ElaKeyBinder::pBinderKeyTextChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ElaKeyBinder::pNativeVirtualBinderKeyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ElaKeyBinder::binderKeyTextChanged(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ElaKeyBinder::nativeVirtualBinderKeyChanged(quint32 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
