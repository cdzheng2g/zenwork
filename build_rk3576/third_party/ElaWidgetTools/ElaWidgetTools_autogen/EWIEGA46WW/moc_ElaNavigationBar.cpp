/****************************************************************************
** Meta object code from reading C++ file 'ElaNavigationBar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/ElaWidgetTools/ElaNavigationBar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElaNavigationBar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ElaNavigationBar_t {
    QByteArrayData data[19];
    char stringdata0[350];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElaNavigationBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElaNavigationBar_t qt_meta_stringdata_ElaNavigationBar = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ElaNavigationBar"
QT_MOC_LITERAL(1, 17, 21), // "pIsTransparentChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 34), // "pIsAllowPageOpenInNewWindowCh..."
QT_MOC_LITERAL(4, 75, 26), // "pNavigationBarWidthChanged"
QT_MOC_LITERAL(5, 102, 19), // "pageOpenInNewWindow"
QT_MOC_LITERAL(6, 122, 7), // "nodeKey"
QT_MOC_LITERAL(7, 130, 19), // "userInfoCardClicked"
QT_MOC_LITERAL(8, 150, 21), // "navigationNodeClicked"
QT_MOC_LITERAL(9, 172, 37), // "ElaNavigationType::Navigation..."
QT_MOC_LITERAL(10, 210, 8), // "nodeType"
QT_MOC_LITERAL(11, 219, 11), // "isRouteBack"
QT_MOC_LITERAL(12, 231, 19), // "navigationNodeAdded"
QT_MOC_LITERAL(13, 251, 8), // "QWidget*"
QT_MOC_LITERAL(14, 260, 4), // "page"
QT_MOC_LITERAL(15, 265, 21), // "navigationNodeRemoved"
QT_MOC_LITERAL(16, 287, 14), // "pIsTransparent"
QT_MOC_LITERAL(17, 302, 27), // "pIsAllowPageOpenInNewWindow"
QT_MOC_LITERAL(18, 330, 19) // "pNavigationBarWidth"

    },
    "ElaNavigationBar\0pIsTransparentChanged\0"
    "\0pIsAllowPageOpenInNewWindowChanged\0"
    "pNavigationBarWidthChanged\0"
    "pageOpenInNewWindow\0nodeKey\0"
    "userInfoCardClicked\0navigationNodeClicked\0"
    "ElaNavigationType::NavigationNodeType\0"
    "nodeType\0isRouteBack\0navigationNodeAdded\0"
    "QWidget*\0page\0navigationNodeRemoved\0"
    "pIsTransparent\0pIsAllowPageOpenInNewWindow\0"
    "pNavigationBarWidth"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElaNavigationBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       3,   80, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    0,   56,    2, 0x06 /* Public */,
       5,    1,   57,    2, 0x06 /* Public */,
       7,    0,   60,    2, 0x06 /* Public */,
       8,    3,   61,    2, 0x06 /* Public */,
      12,    3,   68,    2, 0x06 /* Public */,
      15,    2,   75,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9, QMetaType::QString, QMetaType::Bool,   10,    6,   11,
    QMetaType::Void, 0x80000000 | 9, QMetaType::QString, 0x80000000 | 13,   10,    6,   14,
    QMetaType::Void, 0x80000000 | 9, QMetaType::QString,   10,    6,

 // properties: name, type, flags
      16, QMetaType::Bool, 0x00495003,
      17, QMetaType::Bool, 0x00495003,
      18, QMetaType::Int, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       2,

       0        // eod
};

void ElaNavigationBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElaNavigationBar *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pIsTransparentChanged(); break;
        case 1: _t->pIsAllowPageOpenInNewWindowChanged(); break;
        case 2: _t->pNavigationBarWidthChanged(); break;
        case 3: _t->pageOpenInNewWindow((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->userInfoCardClicked(); break;
        case 5: _t->navigationNodeClicked((*reinterpret_cast< ElaNavigationType::NavigationNodeType(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 6: _t->navigationNodeAdded((*reinterpret_cast< ElaNavigationType::NavigationNodeType(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QWidget*(*)>(_a[3]))); break;
        case 7: _t->navigationNodeRemoved((*reinterpret_cast< ElaNavigationType::NavigationNodeType(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ElaNavigationBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBar::pIsTransparentChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ElaNavigationBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBar::pIsAllowPageOpenInNewWindowChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ElaNavigationBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBar::pNavigationBarWidthChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ElaNavigationBar::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBar::pageOpenInNewWindow)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ElaNavigationBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBar::userInfoCardClicked)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ElaNavigationBar::*)(ElaNavigationType::NavigationNodeType , QString , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBar::navigationNodeClicked)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ElaNavigationBar::*)(ElaNavigationType::NavigationNodeType , QString , QWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBar::navigationNodeAdded)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ElaNavigationBar::*)(ElaNavigationType::NavigationNodeType , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ElaNavigationBar::navigationNodeRemoved)) {
                *result = 7;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ElaNavigationBar *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->getIsTransparent(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->getIsAllowPageOpenInNewWindow(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getNavigationBarWidth(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ElaNavigationBar *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setIsTransparent(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setIsAllowPageOpenInNewWindow(*reinterpret_cast< bool*>(_v)); break;
        case 2: _t->setNavigationBarWidth(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ElaNavigationBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ElaNavigationBar.data,
    qt_meta_data_ElaNavigationBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElaNavigationBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElaNavigationBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElaNavigationBar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ElaNavigationBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
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
void ElaNavigationBar::pIsTransparentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ElaNavigationBar::pIsAllowPageOpenInNewWindowChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ElaNavigationBar::pNavigationBarWidthChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ElaNavigationBar::pageOpenInNewWindow(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ElaNavigationBar::userInfoCardClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ElaNavigationBar::navigationNodeClicked(ElaNavigationType::NavigationNodeType _t1, QString _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ElaNavigationBar::navigationNodeAdded(ElaNavigationType::NavigationNodeType _t1, QString _t2, QWidget * _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ElaNavigationBar::navigationNodeRemoved(ElaNavigationType::NavigationNodeType _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
