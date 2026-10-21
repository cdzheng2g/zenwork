/****************************************************************************
** Meta object code from reading C++ file 'ElaTabWidgetPrivate.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/private/ElaTabWidgetPrivate.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaTabWidgetPrivate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaDragMonitor_t {
    QByteArrayData data[1];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaDragMonitor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaDragMonitor_t qt_meta_stringdata_ElaDragMonitor = {
    {
QT_MOC_LITERAL(0, 0, 14) // "ElaDragMonitor"

    },
    "ElaDragMonitor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaDragMonitor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void ElaDragMonitor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject ElaDragMonitor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ElaDragMonitor.data,
    qt_meta_data_ElaDragMonitor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaDragMonitor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaDragMonitor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaDragMonitor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ElaDragMonitor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_ElaTabWidgetPrivate_t {
    QByteArrayData data[10];
    char stringdata0[127];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaTabWidgetPrivate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaTabWidgetPrivate_t qt_meta_stringdata_ElaTabWidgetPrivate = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ElaTabWidgetPrivate"
QT_MOC_LITERAL(1, 20, 15), // "onTabDragCreate"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 10), // "QMimeData*"
QT_MOC_LITERAL(4, 48, 8), // "mimeData"
QT_MOC_LITERAL(5, 57, 14), // "onTabDragEnter"
QT_MOC_LITERAL(6, 72, 14), // "onTabDragLeave"
QT_MOC_LITERAL(7, 87, 13), // "onTabDragDrop"
QT_MOC_LITERAL(8, 101, 19), // "onTabCloseRequested"
QT_MOC_LITERAL(9, 121, 5) // "index"

    },
    "ElaTabWidgetPrivate\0onTabDragCreate\0"
    "\0QMimeData*\0mimeData\0onTabDragEnter\0"
    "onTabDragLeave\0onTabDragDrop\0"
    "onTabCloseRequested\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaTabWidgetPrivate[] = {

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

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       5,    1,   42,    2, 0x0a /* Public */,
       6,    1,   45,    2, 0x0a /* Public */,
       7,    1,   48,    2, 0x0a /* Public */,
       8,    1,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    9,

       0        // eod
};

void ElaTabWidgetPrivate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaTabWidgetPrivate *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onTabDragCreate((*reinterpret_cast< QMimeData*(*)>(_a[1]))); break;
        case 1: _t->onTabDragEnter((*reinterpret_cast< QMimeData*(*)>(_a[1]))); break;
        case 2: _t->onTabDragLeave((*reinterpret_cast< QMimeData*(*)>(_a[1]))); break;
        case 3: _t->onTabDragDrop((*reinterpret_cast< QMimeData*(*)>(_a[1]))); break;
        case 4: _t->onTabCloseRequested((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ElaTabWidgetPrivate::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ElaTabWidgetPrivate.data,
    qt_meta_data_ElaTabWidgetPrivate,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaTabWidgetPrivate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaTabWidgetPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaTabWidgetPrivate.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ElaTabWidgetPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
