/****************************************************************************
** Meta object code from reading C++ file 'ElaRollerPickerContainer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/DeveloperComponents/ElaRollerPickerContainer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaRollerPickerContainer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaRollerPickerContainer_t {
    QByteArrayData data[6];
    char stringdata0[113];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaRollerPickerContainer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaRollerPickerContainer_t qt_meta_stringdata_ElaRollerPickerContainer = {
    {
QT_MOC_LITERAL(0, 0, 24), // "ElaRollerPickerContainer"
QT_MOC_LITERAL(1, 25, 27), // "pAnimationPixOffsetYChanged"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 17), // "overButtonClicked"
QT_MOC_LITERAL(4, 72, 19), // "cancelButtonClicked"
QT_MOC_LITERAL(5, 92, 20) // "pAnimationPixOffsetY"

    },
    "ElaRollerPickerContainer\0"
    "pAnimationPixOffsetYChanged\0\0"
    "overButtonClicked\0cancelButtonClicked\0"
    "pAnimationPixOffsetY"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaRollerPickerContainer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       1,   32, // properties
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

 // properties: notify_signal_id
       0,

       0        // eod
};

void ElaRollerPickerContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaRollerPickerContainer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pAnimationPixOffsetYChanged(); break;
        case 1: _t->overButtonClicked(); break;
        case 2: _t->cancelButtonClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ElaRollerPickerContainer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRollerPickerContainer::pAnimationPixOffsetYChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ElaRollerPickerContainer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRollerPickerContainer::overButtonClicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ElaRollerPickerContainer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRollerPickerContainer::cancelButtonClicked)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ElaRollerPickerContainer *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->_pAnimationPixOffsetY; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ElaRollerPickerContainer *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->_pAnimationPixOffsetY != *reinterpret_cast< int*>(_v)) {
                _t->_pAnimationPixOffsetY = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->pAnimationPixOffsetYChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject ElaRollerPickerContainer::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ElaRollerPickerContainer.data,
    qt_meta_data_ElaRollerPickerContainer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaRollerPickerContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaRollerPickerContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaRollerPickerContainer.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ElaRollerPickerContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ElaRollerPickerContainer::pAnimationPixOffsetYChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ElaRollerPickerContainer::overButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ElaRollerPickerContainer::cancelButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
