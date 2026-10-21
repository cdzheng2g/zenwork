/****************************************************************************
** Meta object code from reading C++ file 'KeyboardContainer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../third_party/QVirtualKeyboard/qt5softkeyboard/KeyboardContainer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KeyboardContainer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KeyboardContainer_t {
    QByteArrayData data[25];
    char stringdata0[306];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KeyboardContainer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KeyboardContainer_t qt_meta_stringdata_KeyboardContainer = {
    {
QT_MOC_LITERAL(0, 0, 17), // "KeyboardContainer"
QT_MOC_LITERAL(1, 18, 14), // "changeLanguage"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 12), // "changeSymbol"
QT_MOC_LITERAL(4, 47, 12), // "hideKeyboard"
QT_MOC_LITERAL(5, 60, 10), // "keyPressed"
QT_MOC_LITERAL(6, 71, 7), // "Qt::Key"
QT_MOC_LITERAL(7, 79, 3), // "key"
QT_MOC_LITERAL(8, 83, 4), // "text"
QT_MOC_LITERAL(9, 88, 21), // "Qt::KeyboardModifiers"
QT_MOC_LITERAL(10, 110, 9), // "modifiers"
QT_MOC_LITERAL(11, 120, 15), // "chooseCandidate"
QT_MOC_LITERAL(12, 136, 2), // "id"
QT_MOC_LITERAL(13, 139, 16), // "setCandidateList"
QT_MOC_LITERAL(14, 156, 5), // "texts"
QT_MOC_LITERAL(15, 162, 15), // "setLanguageName"
QT_MOC_LITERAL(16, 178, 4), // "name"
QT_MOC_LITERAL(17, 183, 14), // "onChangeSymbol"
QT_MOC_LITERAL(18, 198, 12), // "onHideSymbol"
QT_MOC_LITERAL(19, 211, 13), // "animationHide"
QT_MOC_LITERAL(20, 225, 13), // "animationShow"
QT_MOC_LITERAL(21, 239, 19), // "setInputMethodHints"
QT_MOC_LITERAL(22, 259, 20), // "Qt::InputMethodHints"
QT_MOC_LITERAL(23, 280, 5), // "hints"
QT_MOC_LITERAL(24, 286, 19) // "onAnimationFinished"

    },
    "KeyboardContainer\0changeLanguage\0\0"
    "changeSymbol\0hideKeyboard\0keyPressed\0"
    "Qt::Key\0key\0text\0Qt::KeyboardModifiers\0"
    "modifiers\0chooseCandidate\0id\0"
    "setCandidateList\0texts\0setLanguageName\0"
    "name\0onChangeSymbol\0onHideSymbol\0"
    "animationHide\0animationShow\0"
    "setInputMethodHints\0Qt::InputMethodHints\0"
    "hints\0onAnimationFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KeyboardContainer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,
       3,    0,   80,    2, 0x06 /* Public */,
       4,    0,   81,    2, 0x06 /* Public */,
       5,    3,   82,    2, 0x06 /* Public */,
      11,    1,   89,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    1,   92,    2, 0x0a /* Public */,
      15,    1,   95,    2, 0x0a /* Public */,
      17,    0,   98,    2, 0x0a /* Public */,
      18,    0,   99,    2, 0x0a /* Public */,
      19,    0,  100,    2, 0x0a /* Public */,
      20,    0,  101,    2, 0x0a /* Public */,
      21,    1,  102,    2, 0x0a /* Public */,
      24,    0,  105,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6, QMetaType::QString, 0x80000000 | 9,    7,    8,   10,
    QMetaType::Void, QMetaType::Int,   12,

 // slots: parameters
    QMetaType::Void, QMetaType::QStringList,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void,

       0        // eod
};

void KeyboardContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KeyboardContainer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->changeLanguage(); break;
        case 1: _t->changeSymbol(); break;
        case 2: _t->hideKeyboard(); break;
        case 3: _t->keyPressed((*reinterpret_cast< Qt::Key(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< Qt::KeyboardModifiers(*)>(_a[3]))); break;
        case 4: _t->chooseCandidate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->setCandidateList((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 6: _t->setLanguageName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onChangeSymbol(); break;
        case 8: _t->onHideSymbol(); break;
        case 9: _t->animationHide(); break;
        case 10: _t->animationShow(); break;
        case 11: _t->setInputMethodHints((*reinterpret_cast< Qt::InputMethodHints(*)>(_a[1]))); break;
        case 12: _t->onAnimationFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KeyboardContainer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KeyboardContainer::changeLanguage)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KeyboardContainer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KeyboardContainer::changeSymbol)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (KeyboardContainer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KeyboardContainer::hideKeyboard)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (KeyboardContainer::*)(Qt::Key , const QString & , Qt::KeyboardModifiers );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KeyboardContainer::keyPressed)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (KeyboardContainer::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KeyboardContainer::chooseCandidate)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KeyboardContainer::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KeyboardContainer.data,
    qt_meta_data_KeyboardContainer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KeyboardContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KeyboardContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KeyboardContainer.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KeyboardContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void KeyboardContainer::changeLanguage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KeyboardContainer::changeSymbol()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void KeyboardContainer::hideKeyboard()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void KeyboardContainer::keyPressed(Qt::Key _t1, const QString & _t2, Qt::KeyboardModifiers _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void KeyboardContainer::chooseCandidate(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
