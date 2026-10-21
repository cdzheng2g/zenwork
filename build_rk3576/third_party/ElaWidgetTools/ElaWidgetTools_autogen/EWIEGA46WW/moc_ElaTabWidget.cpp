/****************************************************************************
** Meta object code from reading C++ file 'ElaTabWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/ElaTabWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaTabWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaTabWidget_t {
    QByteArrayData data[8];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaTabWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaTabWidget_t qt_meta_stringdata_ElaTabWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ElaTabWidget"
QT_MOC_LITERAL(1, 13, 24), // "pIsTabTransparentChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 30), // "pIsContainerAcceptDropsChanged"
QT_MOC_LITERAL(4, 70, 15), // "pTabSizeChanged"
QT_MOC_LITERAL(5, 86, 17), // "pIsTabTransparent"
QT_MOC_LITERAL(6, 104, 23), // "pIsContainerAcceptDrops"
QT_MOC_LITERAL(7, 128, 8) // "pTabSize"

    },
    "ElaTabWidget\0pIsTabTransparentChanged\0"
    "\0pIsContainerAcceptDropsChanged\0"
    "pTabSizeChanged\0pIsTabTransparent\0"
    "pIsContainerAcceptDrops\0pTabSize"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaTabWidget[] = {

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
       5, QMetaType::Bool, 0x00495003,
       6, QMetaType::Bool, 0x00495003,
       7, QMetaType::QSize, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       2,

       0        // eod
};

void ElaTabWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaTabWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pIsTabTransparentChanged(); break;
        case 1: _t->pIsContainerAcceptDropsChanged(); break;
        case 2: _t->pTabSizeChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ElaTabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaTabWidget::pIsTabTransparentChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ElaTabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaTabWidget::pIsContainerAcceptDropsChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ElaTabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaTabWidget::pTabSizeChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ElaTabWidget *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->getIsTabTransparent(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->getIsContainerAcceptDrops(); break;
        case 2: *reinterpret_cast< QSize*>(_v) = _t->getTabSize(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ElaTabWidget *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setIsTabTransparent(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setIsContainerAcceptDrops(*reinterpret_cast< bool*>(_v)); break;
        case 2: _t->setTabSize(*reinterpret_cast< QSize*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject ElaTabWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QTabWidget::staticMetaObject>(),
    qt_meta_stringdata_ElaTabWidget.data,
    qt_meta_data_ElaTabWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaTabWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaTabWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaTabWidget.stringdata0))
        return static_cast<void*>(this);
    return QTabWidget::qt_metacast(_clname);
}

int ElaTabWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
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
void ElaTabWidget::pIsTabTransparentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ElaTabWidget::pIsContainerAcceptDropsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ElaTabWidget::pTabSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
