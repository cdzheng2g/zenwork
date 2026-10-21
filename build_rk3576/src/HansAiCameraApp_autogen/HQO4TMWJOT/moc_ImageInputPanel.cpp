/****************************************************************************
** Meta object code from reading C++ file 'ImageInputPanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/ui/workbench/ImageInputPanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageInputPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ImageInputPanel_t {
    QByteArrayData data[17];
    char stringdata0[208];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ImageInputPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ImageInputPanel_t qt_meta_stringdata_ImageInputPanel = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ImageInputPanel"
QT_MOC_LITERAL(1, 16, 20), // "cameraSourceSelected"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 20), // "folderSourceSelected"
QT_MOC_LITERAL(4, 59, 10), // "imageReady"
QT_MOC_LITERAL(5, 70, 7), // "cv::Mat"
QT_MOC_LITERAL(6, 78, 5), // "image"
QT_MOC_LITERAL(7, 84, 8), // "fileName"
QT_MOC_LITERAL(8, 93, 19), // "sourceStatusChanged"
QT_MOC_LITERAL(9, 113, 6), // "status"
QT_MOC_LITERAL(10, 120, 15), // "onSourceChanged"
QT_MOC_LITERAL(11, 136, 5), // "index"
QT_MOC_LITERAL(12, 142, 15), // "selectDirectory"
QT_MOC_LITERAL(13, 158, 12), // "showPrevious"
QT_MOC_LITERAL(14, 171, 8), // "showNext"
QT_MOC_LITERAL(15, 180, 14), // "togglePlayback"
QT_MOC_LITERAL(16, 195, 12) // "playbackTick"

    },
    "ImageInputPanel\0cameraSourceSelected\0"
    "\0folderSourceSelected\0imageReady\0"
    "cv::Mat\0image\0fileName\0sourceStatusChanged\0"
    "status\0onSourceChanged\0index\0"
    "selectDirectory\0showPrevious\0showNext\0"
    "togglePlayback\0playbackTick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ImageInputPanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    2,   66,    2, 0x06 /* Public */,
       8,    1,   71,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   74,    2, 0x08 /* Private */,
      12,    0,   77,    2, 0x08 /* Private */,
      13,    0,   78,    2, 0x08 /* Private */,
      14,    0,   79,    2, 0x08 /* Private */,
      15,    0,   80,    2, 0x08 /* Private */,
      16,    0,   81,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, QMetaType::QString,    6,    7,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ImageInputPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ImageInputPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->cameraSourceSelected(); break;
        case 1: _t->folderSourceSelected(); break;
        case 2: _t->imageReady((*reinterpret_cast< const cv::Mat(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->sourceStatusChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onSourceChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->selectDirectory(); break;
        case 6: _t->showPrevious(); break;
        case 7: _t->showNext(); break;
        case 8: _t->togglePlayback(); break;
        case 9: _t->playbackTick(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ImageInputPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageInputPanel::cameraSourceSelected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ImageInputPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageInputPanel::folderSourceSelected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ImageInputPanel::*)(const cv::Mat & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageInputPanel::imageReady)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ImageInputPanel::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageInputPanel::sourceStatusChanged)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ImageInputPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ImageInputPanel.data,
    qt_meta_data_ImageInputPanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ImageInputPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImageInputPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ImageInputPanel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ImageInputPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ImageInputPanel::cameraSourceSelected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ImageInputPanel::folderSourceSelected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ImageInputPanel::imageReady(const cv::Mat & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ImageInputPanel::sourceStatusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
