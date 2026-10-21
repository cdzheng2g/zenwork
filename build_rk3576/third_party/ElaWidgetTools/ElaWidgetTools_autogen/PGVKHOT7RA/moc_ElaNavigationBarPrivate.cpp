/****************************************************************************
** Meta object code from reading C++ file 'ElaNavigationBarPrivate.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/private/ElaNavigationBarPrivate.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaNavigationBarPrivate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaNavigationBarPrivate_t {
    QByteArrayData data[8];
    char stringdata0[153];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaNavigationBarPrivate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaNavigationBarPrivate_t qt_meta_stringdata_ElaNavigationBarPrivate = {
    {
QT_MOC_LITERAL(0, 0, 23), // "ElaNavigationBarPrivate"
QT_MOC_LITERAL(1, 24, 27), // "pNavigationViewWidthChanged"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 25), // "pUserButtonSpacingChanged"
QT_MOC_LITERAL(4, 79, 25), // "onNavigationOpenNewWindow"
QT_MOC_LITERAL(5, 105, 7), // "nodeKey"
QT_MOC_LITERAL(6, 113, 20), // "pNavigationViewWidth"
QT_MOC_LITERAL(7, 134, 18) // "pUserButtonSpacing"

    },
    "ElaNavigationBarPrivate\0"
    "pNavigationViewWidthChanged\0\0"
    "pUserButtonSpacingChanged\0"
    "onNavigationOpenNewWindow\0nodeKey\0"
    "pNavigationViewWidth\0pUserButtonSpacing"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaNavigationBarPrivate[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       2,   34, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   31,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    5,

 // properties: name, type, flags
       6, QMetaType::Int, 0x00495003,
       7, QMetaType::Int, 0x00495003,

 // properties: notify_signal_id
       0,
       1,

       0        // eod
};

void ElaNavigationBarPrivate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaNavigationBarPrivate *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pNavigationViewWidthChanged(); break;
        case 1: _t->pUserButtonSpacingChanged(); break;
        case 2: _t->onNavigationOpenNewWindow((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ElaNavigationBarPrivate::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBarPrivate::pNavigationViewWidthChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ElaNavigationBarPrivate::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBarPrivate::pUserButtonSpacingChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ElaNavigationBarPrivate *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->_pNavigationViewWidth; break;
        case 1: *reinterpret_cast< int*>(_v) = _t->_pUserButtonSpacing; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ElaNavigationBarPrivate *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->_pNavigationViewWidth != *reinterpret_cast< int*>(_v)) {
                _t->_pNavigationViewWidth = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->pNavigationViewWidthChanged();
            }
            break;
        case 1:
            if (_t->_pUserButtonSpacing != *reinterpret_cast< int*>(_v)) {
                _t->_pUserButtonSpacing = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->pUserButtonSpacingChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ElaNavigationBarPrivate::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ElaNavigationBarPrivate.data,
    qt_meta_data_ElaNavigationBarPrivate,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaNavigationBarPrivate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaNavigationBarPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaNavigationBarPrivate.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ElaNavigationBarPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ElaNavigationBarPrivate::pNavigationViewWidthChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ElaNavigationBarPrivate::pUserButtonSpacingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
