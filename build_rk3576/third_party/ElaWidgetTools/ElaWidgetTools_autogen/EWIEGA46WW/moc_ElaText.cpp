/****************************************************************************
** Meta object code from reading C++ file 'ElaText.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/ElaText.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaText.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaText_t {
    QByteArrayData data[14];
    char stringdata0[221];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaText_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaText_t qt_meta_stringdata_ElaText = {
    {
QT_MOC_LITERAL(0, 0, 7), // "ElaText"
QT_MOC_LITERAL(1, 8, 22), // "pIsWrapAnywhereChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 21), // "pTextPixelSizeChanged"
QT_MOC_LITERAL(4, 54, 21), // "pTextPointSizeChanged"
QT_MOC_LITERAL(5, 76, 17), // "pTextStyleChanged"
QT_MOC_LITERAL(6, 94, 15), // "pElaIconChanged"
QT_MOC_LITERAL(7, 110, 15), // "pIsWrapAnywhere"
QT_MOC_LITERAL(8, 126, 14), // "pTextPixelSize"
QT_MOC_LITERAL(9, 141, 14), // "pTextPointSize"
QT_MOC_LITERAL(10, 156, 10), // "pTextStyle"
QT_MOC_LITERAL(11, 167, 22), // "ElaTextType::TextStyle"
QT_MOC_LITERAL(12, 190, 8), // "pElaIcon"
QT_MOC_LITERAL(13, 199, 21) // "ElaIconType::IconName"

    },
    "ElaText\0pIsWrapAnywhereChanged\0\0"
    "pTextPixelSizeChanged\0pTextPointSizeChanged\0"
    "pTextStyleChanged\0pElaIconChanged\0"
    "pIsWrapAnywhere\0pTextPixelSize\0"
    "pTextPointSize\0pTextStyle\0"
    "ElaTextType::TextStyle\0pElaIcon\0"
    "ElaIconType::IconName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaText[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       5,   44, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    0,   41,    2, 0x06 /* Public */,
       5,    0,   42,    2, 0x06 /* Public */,
       6,    0,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       7, QMetaType::Bool, 0x00495003,
       8, QMetaType::Int, 0x00495003,
       9, QMetaType::Int, 0x00495003,
      10, 0x80000000 | 11, 0x0049500b,
      12, 0x80000000 | 13, 0x0049500b,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,

       0        // eod
};

void ElaText::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaText *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pIsWrapAnywhereChanged(); break;
        case 1: _t->pTextPixelSizeChanged(); break;
        case 2: _t->pTextPointSizeChanged(); break;
        case 3: _t->pTextStyleChanged(); break;
        case 4: _t->pElaIconChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ElaText::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaText::pIsWrapAnywhereChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ElaText::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaText::pTextPixelSizeChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ElaText::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaText::pTextPointSizeChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ElaText::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaText::pTextStyleChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ElaText::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaText::pElaIconChanged)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ElaText *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->getIsWrapAnywhere(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getTextPixelSize(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getTextPointSize(); break;
        case 3: *reinterpret_cast< ElaTextType::TextStyle*>(_v) = _t->getTextStyle(); break;
        case 4: *reinterpret_cast< ElaIconType::IconName*>(_v) = _t->getElaIcon(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ElaText *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setIsWrapAnywhere(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setTextPixelSize(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setTextPointSize(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setTextStyle(*reinterpret_cast< ElaTextType::TextStyle*>(_v)); break;
        case 4: _t->setElaIcon(*reinterpret_cast< ElaIconType::IconName*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

static const QMetaObject::SuperData qt_meta_extradata_ElaText[] = {
    QMetaObject::SuperData::link<ElaTextType::staticMetaObject>(),
    QMetaObject::SuperData::link<ElaIconType::staticMetaObject>(),
    nullptr
};

QT_INIT_METAOBJECT const QMetaObject ElaText::staticMetaObject = { {
    QMetaObject::SuperData::link<QLabel::staticMetaObject>(),
    qt_meta_stringdata_ElaText.data,
    qt_meta_data_ElaText,
    qt_static_metacall,
    qt_meta_extradata_ElaText,
    nullptr
} };


const QMetaObject *ElaText::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaText::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaText.stringdata0))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int ElaText::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ElaText::pIsWrapAnywhereChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ElaText::pTextPixelSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ElaText::pTextPointSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ElaText::pTextStyleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ElaText::pElaIconChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
