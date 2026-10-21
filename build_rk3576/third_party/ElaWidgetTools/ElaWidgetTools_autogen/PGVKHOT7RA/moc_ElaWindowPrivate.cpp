/****************************************************************************
** Meta object code from reading C++ file 'ElaWindowPrivate.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/private/ElaWindowPrivate.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaWindowPrivate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaWindowPrivate_t {
    QByteArrayData data[23];
    char stringdata0[409];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaWindowPrivate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaWindowPrivate_t qt_meta_stringdata_ElaWindowPrivate = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ElaWindowPrivate"
QT_MOC_LITERAL(1, 17, 25), // "onNavigationButtonClicked"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 18), // "onThemeReadyChange"
QT_MOC_LITERAL(4, 63, 18), // "onThemeModeChanged"
QT_MOC_LITERAL(5, 82, 23), // "ElaThemeType::ThemeMode"
QT_MOC_LITERAL(6, 106, 9), // "themeMode"
QT_MOC_LITERAL(7, 116, 26), // "onWindowDisplayModeChanged"
QT_MOC_LITERAL(8, 143, 23), // "onNavigationNodeClicked"
QT_MOC_LITERAL(9, 167, 37), // "ElaNavigationType::Navigation..."
QT_MOC_LITERAL(10, 205, 8), // "nodeType"
QT_MOC_LITERAL(11, 214, 7), // "nodeKey"
QT_MOC_LITERAL(12, 222, 11), // "isRouteBack"
QT_MOC_LITERAL(13, 234, 21), // "onNavigationNodeAdded"
QT_MOC_LITERAL(14, 256, 8), // "QWidget*"
QT_MOC_LITERAL(15, 265, 4), // "page"
QT_MOC_LITERAL(16, 270, 23), // "onNavigationNodeRemoved"
QT_MOC_LITERAL(17, 294, 30), // "onNavigationRouterStateChanged"
QT_MOC_LITERAL(18, 325, 10), // "domainName"
QT_MOC_LITERAL(19, 336, 38), // "ElaActionCommanderType::Comma..."
QT_MOC_LITERAL(20, 375, 5), // "state"
QT_MOC_LITERAL(21, 381, 22), // "onWMWindowClickedEvent"
QT_MOC_LITERAL(22, 404, 4) // "data"

    },
    "ElaWindowPrivate\0onNavigationButtonClicked\0"
    "\0onThemeReadyChange\0onThemeModeChanged\0"
    "ElaThemeType::ThemeMode\0themeMode\0"
    "onWindowDisplayModeChanged\0"
    "onNavigationNodeClicked\0"
    "ElaNavigationType::NavigationNodeType\0"
    "nodeType\0nodeKey\0isRouteBack\0"
    "onNavigationNodeAdded\0QWidget*\0page\0"
    "onNavigationNodeRemoved\0"
    "onNavigationRouterStateChanged\0"
    "domainName\0ElaActionCommanderType::CommanderState\0"
    "state\0onWMWindowClickedEvent\0data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaWindowPrivate[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a /* Public */,
       3,    0,   60,    2, 0x0a /* Public */,
       4,    1,   61,    2, 0x0a /* Public */,
       7,    0,   64,    2, 0x0a /* Public */,
       8,    3,   65,    2, 0x0a /* Public */,
      13,    3,   72,    2, 0x0a /* Public */,
      16,    2,   79,    2, 0x0a /* Public */,
      17,    2,   84,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
      21,    1,   89,    2, 0x02 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9, QMetaType::QString, QMetaType::Bool,   10,   11,   12,
    QMetaType::Void, 0x80000000 | 9, QMetaType::QString, 0x80000000 | 14,   10,   11,   15,
    QMetaType::Void, 0x80000000 | 9, QMetaType::QString,   10,   11,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 19,   18,   20,

 // methods: parameters
    QMetaType::Void, QMetaType::QVariantMap,   22,

       0        // eod
};

void ElaWindowPrivate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaWindowPrivate *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onNavigationButtonClicked(); break;
        case 1: _t->onThemeReadyChange(); break;
        case 2: _t->onThemeModeChanged((*reinterpret_cast< ElaThemeType::ThemeMode(*)>(_a[1]))); break;
        case 3: _t->onWindowDisplayModeChanged(); break;
        case 4: _t->onNavigationNodeClicked((*reinterpret_cast< ElaNavigationType::NavigationNodeType(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 5: _t->onNavigationNodeAdded((*reinterpret_cast< ElaNavigationType::NavigationNodeType(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QWidget*(*)>(_a[3]))); break;
        case 6: _t->onNavigationNodeRemoved((*reinterpret_cast< ElaNavigationType::NavigationNodeType(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 7: _t->onNavigationRouterStateChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< ElaActionCommanderType::CommanderState(*)>(_a[2]))); break;
        case 8: _t->onWMWindowClickedEvent((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ElaWindowPrivate::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ElaWindowPrivate.data,
    qt_meta_data_ElaWindowPrivate,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaWindowPrivate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaWindowPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaWindowPrivate.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ElaWindowPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
