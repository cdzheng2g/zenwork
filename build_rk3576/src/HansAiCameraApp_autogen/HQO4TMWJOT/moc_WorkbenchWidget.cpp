/****************************************************************************
** Meta object code from reading C++ file 'WorkbenchWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/ui/workbench/WorkbenchWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkbenchWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WorkbenchWidget_t {
    QByteArrayData data[14];
    char stringdata0[198];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WorkbenchWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WorkbenchWidget_t qt_meta_stringdata_WorkbenchWidget = {
    {
QT_MOC_LITERAL(0, 0, 15), // "WorkbenchWidget"
QT_MOC_LITERAL(1, 16, 15), // "switchTopModule"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 2), // "id"
QT_MOC_LITERAL(4, 36, 18), // "switchWorkflowStep"
QT_MOC_LITERAL(5, 55, 12), // "createScheme"
QT_MOC_LITERAL(6, 68, 17), // "editCurrentScheme"
QT_MOC_LITERAL(7, 86, 12), // "selectScheme"
QT_MOC_LITERAL(8, 99, 10), // "schemeName"
QT_MOC_LITERAL(9, 110, 17), // "saveEditingScheme"
QT_MOC_LITERAL(10, 128, 19), // "cancelEditingScheme"
QT_MOC_LITERAL(11, 148, 19), // "deleteCurrentScheme"
QT_MOC_LITERAL(12, 168, 19), // "showSchemeFilePanel"
QT_MOC_LITERAL(13, 188, 9) // "exporting"

    },
    "WorkbenchWidget\0switchTopModule\0\0id\0"
    "switchWorkflowStep\0createScheme\0"
    "editCurrentScheme\0selectScheme\0"
    "schemeName\0saveEditingScheme\0"
    "cancelEditingScheme\0deleteCurrentScheme\0"
    "showSchemeFilePanel\0exporting"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WorkbenchWidget[] = {

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
       1,    1,   59,    2, 0x08 /* Private */,
       4,    1,   62,    2, 0x08 /* Private */,
       5,    0,   65,    2, 0x08 /* Private */,
       6,    0,   66,    2, 0x08 /* Private */,
       7,    1,   67,    2, 0x08 /* Private */,
       9,    0,   70,    2, 0x08 /* Private */,
      10,    0,   71,    2, 0x08 /* Private */,
      11,    0,   72,    2, 0x08 /* Private */,
      12,    1,   73,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,

       0        // eod
};

void WorkbenchWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WorkbenchWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->switchTopModule((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->switchWorkflowStep((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->createScheme(); break;
        case 3: _t->editCurrentScheme(); break;
        case 4: _t->selectScheme((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->saveEditingScheme(); break;
        case 6: _t->cancelEditingScheme(); break;
        case 7: _t->deleteCurrentScheme(); break;
        case 8: _t->showSchemeFilePanel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WorkbenchWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_WorkbenchWidget.data,
    qt_meta_data_WorkbenchWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WorkbenchWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkbenchWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WorkbenchWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int WorkbenchWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
