/****************************************************************************
** Meta object code from reading C++ file 'LiveMonitorWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/ui/workbench/LiveMonitorWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LiveMonitorWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LiveMonitorWidget_t {
    QByteArrayData data[31];
    char stringdata0[436];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LiveMonitorWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LiveMonitorWidget_t qt_meta_stringdata_LiveMonitorWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "LiveMonitorWidget"
QT_MOC_LITERAL(1, 18, 19), // "editSchemeRequested"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 18), // "newSchemeRequested"
QT_MOC_LITERAL(4, 58, 21), // "importSchemeRequested"
QT_MOC_LITERAL(5, 80, 21), // "exportSchemeRequested"
QT_MOC_LITERAL(6, 102, 21), // "deleteSchemeRequested"
QT_MOC_LITERAL(7, 124, 24), // "schemeSelectionRequested"
QT_MOC_LITERAL(8, 149, 10), // "schemeName"
QT_MOC_LITERAL(9, 160, 16), // "runOnceRequested"
QT_MOC_LITERAL(10, 177, 16), // "runToolRequested"
QT_MOC_LITERAL(11, 194, 6), // "toolId"
QT_MOC_LITERAL(12, 201, 13), // "pausedChanged"
QT_MOC_LITERAL(13, 215, 6), // "paused"
QT_MOC_LITERAL(14, 222, 10), // "setSchemes"
QT_MOC_LITERAL(15, 233, 7), // "schemes"
QT_MOC_LITERAL(16, 241, 13), // "currentScheme"
QT_MOC_LITERAL(17, 255, 21), // "setCurrentSchemeTools"
QT_MOC_LITERAL(18, 277, 17), // "QList<SchemeTool>"
QT_MOC_LITERAL(19, 295, 5), // "tools"
QT_MOC_LITERAL(20, 301, 12), // "onFrameReady"
QT_MOC_LITERAL(21, 314, 7), // "cv::Mat"
QT_MOC_LITERAL(22, 322, 5), // "frame"
QT_MOC_LITERAL(23, 328, 13), // "onStreamError"
QT_MOC_LITERAL(24, 342, 7), // "message"
QT_MOC_LITERAL(25, 350, 18), // "onFolderImageReady"
QT_MOC_LITERAL(26, 369, 5), // "image"
QT_MOC_LITERAL(27, 375, 8), // "fileName"
QT_MOC_LITERAL(28, 384, 20), // "switchToCameraSource"
QT_MOC_LITERAL(29, 405, 20), // "switchToFolderSource"
QT_MOC_LITERAL(30, 426, 9) // "toggleRun"

    },
    "LiveMonitorWidget\0editSchemeRequested\0"
    "\0newSchemeRequested\0importSchemeRequested\0"
    "exportSchemeRequested\0deleteSchemeRequested\0"
    "schemeSelectionRequested\0schemeName\0"
    "runOnceRequested\0runToolRequested\0"
    "toolId\0pausedChanged\0paused\0setSchemes\0"
    "schemes\0currentScheme\0setCurrentSchemeTools\0"
    "QList<SchemeTool>\0tools\0onFrameReady\0"
    "cv::Mat\0frame\0onStreamError\0message\0"
    "onFolderImageReady\0image\0fileName\0"
    "switchToCameraSource\0switchToFolderSource\0"
    "toggleRun"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LiveMonitorWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x06 /* Public */,
       3,    0,  100,    2, 0x06 /* Public */,
       4,    0,  101,    2, 0x06 /* Public */,
       5,    0,  102,    2, 0x06 /* Public */,
       6,    0,  103,    2, 0x06 /* Public */,
       7,    1,  104,    2, 0x06 /* Public */,
       9,    0,  107,    2, 0x06 /* Public */,
      10,    1,  108,    2, 0x06 /* Public */,
      12,    1,  111,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    2,  114,    2, 0x0a /* Public */,
      17,    1,  119,    2, 0x0a /* Public */,
      20,    1,  122,    2, 0x08 /* Private */,
      23,    1,  125,    2, 0x08 /* Private */,
      25,    2,  128,    2, 0x08 /* Private */,
      28,    0,  133,    2, 0x08 /* Private */,
      29,    0,  134,    2, 0x08 /* Private */,
      30,    0,  135,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::Bool,   13,

 // slots: parameters
    QMetaType::Void, QMetaType::QStringList, QMetaType::QString,   15,   16,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void, 0x80000000 | 21,   22,
    QMetaType::Void, QMetaType::QString,   24,
    QMetaType::Void, 0x80000000 | 21, QMetaType::QString,   26,   27,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void LiveMonitorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LiveMonitorWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->editSchemeRequested(); break;
        case 1: _t->newSchemeRequested(); break;
        case 2: _t->importSchemeRequested(); break;
        case 3: _t->exportSchemeRequested(); break;
        case 4: _t->deleteSchemeRequested(); break;
        case 5: _t->schemeSelectionRequested((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->runOnceRequested(); break;
        case 7: _t->runToolRequested((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->pausedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setSchemes((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 10: _t->setCurrentSchemeTools((*reinterpret_cast< const QList<SchemeTool>(*)>(_a[1]))); break;
        case 11: _t->onFrameReady((*reinterpret_cast< const cv::Mat(*)>(_a[1]))); break;
        case 12: _t->onStreamError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->onFolderImageReady((*reinterpret_cast< const cv::Mat(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 14: _t->switchToCameraSource(); break;
        case 15: _t->switchToFolderSource(); break;
        case 16: _t->toggleRun(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LiveMonitorWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LiveMonitorWidget::editSchemeRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LiveMonitorWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LiveMonitorWidget::newSchemeRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LiveMonitorWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LiveMonitorWidget::importSchemeRequested)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (LiveMonitorWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LiveMonitorWidget::exportSchemeRequested)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (LiveMonitorWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LiveMonitorWidget::deleteSchemeRequested)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (LiveMonitorWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LiveMonitorWidget::schemeSelectionRequested)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (LiveMonitorWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LiveMonitorWidget::runOnceRequested)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (LiveMonitorWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LiveMonitorWidget::runToolRequested)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (LiveMonitorWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LiveMonitorWidget::pausedChanged)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LiveMonitorWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_LiveMonitorWidget.data,
    qt_meta_data_LiveMonitorWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LiveMonitorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LiveMonitorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LiveMonitorWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int LiveMonitorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void LiveMonitorWidget::editSchemeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void LiveMonitorWidget::newSchemeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void LiveMonitorWidget::importSchemeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void LiveMonitorWidget::exportSchemeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void LiveMonitorWidget::deleteSchemeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void LiveMonitorWidget::schemeSelectionRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void LiveMonitorWidget::runOnceRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void LiveMonitorWidget::runToolRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void LiveMonitorWidget::pausedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
