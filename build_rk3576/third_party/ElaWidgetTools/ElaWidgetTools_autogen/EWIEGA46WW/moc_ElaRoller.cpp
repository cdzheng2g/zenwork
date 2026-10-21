/****************************************************************************
** Meta object code from reading C++ file 'ElaRoller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/ElaRoller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaRoller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaRoller_t {
    QByteArrayData data[18];
    char stringdata0[272];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaRoller_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaRoller_t qt_meta_stringdata_ElaRoller = {
    {
QT_MOC_LITERAL(0, 0, 9), // "ElaRoller"
QT_MOC_LITERAL(1, 10, 20), // "pBorderRadiusChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 16), // "pItemListChanged"
QT_MOC_LITERAL(4, 49, 18), // "pItemHeightChanged"
QT_MOC_LITERAL(5, 68, 23), // "pMaxVisibleItemsChanged"
QT_MOC_LITERAL(6, 92, 20), // "pCurrentIndexChanged"
QT_MOC_LITERAL(7, 113, 19), // "pIsContainerChanged"
QT_MOC_LITERAL(8, 133, 20), // "pIsEnableLoopChanged"
QT_MOC_LITERAL(9, 154, 18), // "currentDataChanged"
QT_MOC_LITERAL(10, 173, 4), // "data"
QT_MOC_LITERAL(11, 178, 13), // "pBorderRadius"
QT_MOC_LITERAL(12, 192, 9), // "pItemList"
QT_MOC_LITERAL(13, 202, 11), // "pItemHeight"
QT_MOC_LITERAL(14, 214, 16), // "pMaxVisibleItems"
QT_MOC_LITERAL(15, 231, 13), // "pCurrentIndex"
QT_MOC_LITERAL(16, 245, 12), // "pIsContainer"
QT_MOC_LITERAL(17, 258, 13) // "pIsEnableLoop"

    },
    "ElaRoller\0pBorderRadiusChanged\0\0"
    "pItemListChanged\0pItemHeightChanged\0"
    "pMaxVisibleItemsChanged\0pCurrentIndexChanged\0"
    "pIsContainerChanged\0pIsEnableLoopChanged\0"
    "currentDataChanged\0data\0pBorderRadius\0"
    "pItemList\0pItemHeight\0pMaxVisibleItems\0"
    "pCurrentIndex\0pIsContainer\0pIsEnableLoop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaRoller[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       7,   64, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    0,   56,    2, 0x06 /* Public */,
       5,    0,   57,    2, 0x06 /* Public */,
       6,    0,   58,    2, 0x06 /* Public */,
       7,    0,   59,    2, 0x06 /* Public */,
       8,    0,   60,    2, 0x06 /* Public */,
       9,    1,   61,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,

 // properties: name, type, flags
      11, QMetaType::Int, 0x00495003,
      12, QMetaType::QStringList, 0x00495003,
      13, QMetaType::Int, 0x00495003,
      14, QMetaType::Int, 0x00495003,
      15, QMetaType::Int, 0x00495003,
      16, QMetaType::Bool, 0x00495003,
      17, QMetaType::Bool, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,
       5,
       6,

       0        // eod
};

void ElaRoller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaRoller *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pBorderRadiusChanged(); break;
        case 1: _t->pItemListChanged(); break;
        case 2: _t->pItemHeightChanged(); break;
        case 3: _t->pMaxVisibleItemsChanged(); break;
        case 4: _t->pCurrentIndexChanged(); break;
        case 5: _t->pIsContainerChanged(); break;
        case 6: _t->pIsEnableLoopChanged(); break;
        case 7: _t->currentDataChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ElaRoller::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRoller::pBorderRadiusChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ElaRoller::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRoller::pItemListChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ElaRoller::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRoller::pItemHeightChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ElaRoller::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRoller::pMaxVisibleItemsChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ElaRoller::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRoller::pCurrentIndexChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ElaRoller::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRoller::pIsContainerChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ElaRoller::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRoller::pIsEnableLoopChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ElaRoller::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaRoller::currentDataChanged)) {
                *result = 7;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ElaRoller *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getBorderRadius(); break;
        case 1: *reinterpret_cast< QStringList*>(_v) = _t->getItemList(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getItemHeight(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->getMaxVisibleItems(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->getCurrentIndex(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->getIsContainer(); break;
        case 6: *reinterpret_cast< bool*>(_v) = _t->getIsEnableLoop(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ElaRoller *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBorderRadius(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setItemList(*reinterpret_cast< QStringList*>(_v)); break;
        case 2: _t->setItemHeight(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setMaxVisibleItems(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setCurrentIndex(*reinterpret_cast< int*>(_v)); break;
        case 5: _t->setIsContainer(*reinterpret_cast< bool*>(_v)); break;
        case 6: _t->setIsEnableLoop(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ElaRoller::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ElaRoller.data,
    qt_meta_data_ElaRoller,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaRoller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaRoller::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaRoller.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ElaRoller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ElaRoller::pBorderRadiusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ElaRoller::pItemListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ElaRoller::pItemHeightChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ElaRoller::pMaxVisibleItemsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ElaRoller::pCurrentIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ElaRoller::pIsContainerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ElaRoller::pIsEnableLoopChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ElaRoller::currentDataChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
