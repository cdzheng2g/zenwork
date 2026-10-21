/****************************************************************************
** Meta object code from reading C++ file 'CameraSettingsPage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/ui/pages/CameraSettingsPage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CameraSettingsPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CameraSettingsPage_t {
    QByteArrayData data[13];
    char stringdata0[178];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CameraSettingsPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CameraSettingsPage_t qt_meta_stringdata_CameraSettingsPage = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CameraSettingsPage"
QT_MOC_LITERAL(1, 19, 21), // "onAutoExposureToggled"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 7), // "checked"
QT_MOC_LITERAL(4, 50, 14), // "onApplyClicked"
QT_MOC_LITERAL(5, 65, 21), // "onResetDefaultClicked"
QT_MOC_LITERAL(6, 87, 21), // "scheduleRealtimeApply"
QT_MOC_LITERAL(7, 109, 19), // "applyRealtimeParams"
QT_MOC_LITERAL(8, 129, 12), // "onFrameReady"
QT_MOC_LITERAL(9, 142, 7), // "cv::Mat"
QT_MOC_LITERAL(10, 150, 5), // "frame"
QT_MOC_LITERAL(11, 156, 13), // "onStreamError"
QT_MOC_LITERAL(12, 170, 7) // "message"

    },
    "CameraSettingsPage\0onAutoExposureToggled\0"
    "\0checked\0onApplyClicked\0onResetDefaultClicked\0"
    "scheduleRealtimeApply\0applyRealtimeParams\0"
    "onFrameReady\0cv::Mat\0frame\0onStreamError\0"
    "message"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CameraSettingsPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x08 /* Private */,
       4,    0,   52,    2, 0x08 /* Private */,
       5,    0,   53,    2, 0x08 /* Private */,
       6,    0,   54,    2, 0x08 /* Private */,
       7,    0,   55,    2, 0x08 /* Private */,
       8,    1,   56,    2, 0x08 /* Private */,
      11,    1,   59,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, QMetaType::QString,   12,

       0        // eod
};

void CameraSettingsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CameraSettingsPage *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onAutoExposureToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->onApplyClicked(); break;
        case 2: _t->onResetDefaultClicked(); break;
        case 3: _t->scheduleRealtimeApply(); break;
        case 4: _t->applyRealtimeParams(); break;
        case 5: _t->onFrameReady((*reinterpret_cast< const cv::Mat(*)>(_a[1]))); break;
        case 6: _t->onStreamError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CameraSettingsPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CameraSettingsPage.data,
    qt_meta_data_CameraSettingsPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CameraSettingsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CameraSettingsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CameraSettingsPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CameraSettingsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
