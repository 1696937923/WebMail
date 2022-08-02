/****************************************************************************
** Meta object code from reading C++ file 'UserUi.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "UserUi.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UserUi.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UserUi_t {
    QByteArrayData data[11];
    char stringdata0[152];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UserUi_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UserUi_t qt_meta_stringdata_UserUi = {
    {
QT_MOC_LITERAL(0, 0, 6), // "UserUi"
QT_MOC_LITERAL(1, 7, 11), // "ShowEmailUi"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 11), // "const char*"
QT_MOC_LITERAL(4, 32, 10), // "ShowMainUi"
QT_MOC_LITERAL(5, 43, 16), // "ReciveUserUiShow"
QT_MOC_LITERAL(6, 60, 16), // "ReciveBackUiShow"
QT_MOC_LITERAL(7, 77, 14), // "ReciveNewEmail"
QT_MOC_LITERAL(8, 92, 18), // "on_SendBtn_clicked"
QT_MOC_LITERAL(9, 111, 20), // "on_LogoutBtn_clicked"
QT_MOC_LITERAL(10, 132, 19) // "on_EmailBtn_clicked"

    },
    "UserUi\0ShowEmailUi\0\0const char*\0"
    "ShowMainUi\0ReciveUserUiShow\0"
    "ReciveBackUiShow\0ReciveNewEmail\0"
    "on_SendBtn_clicked\0on_LogoutBtn_clicked\0"
    "on_EmailBtn_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UserUi[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    0,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    4,   58,    2, 0x08 /* Private */,
       6,    0,   67,    2, 0x08 /* Private */,
       7,    0,   68,    2, 0x08 /* Private */,
       8,    0,   69,    2, 0x08 /* Private */,
       9,    0,   70,    2, 0x08 /* Private */,
      10,    0,   71,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 3, 0x80000000 | 3, QMetaType::Int,    2,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UserUi::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        UserUi *_t = static_cast<UserUi *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ShowEmailUi((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 1: _t->ShowMainUi(); break;
        case 2: _t->ReciveUserUiShow((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const char*(*)>(_a[2])),(*reinterpret_cast< const char*(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 3: _t->ReciveBackUiShow(); break;
        case 4: _t->ReciveNewEmail(); break;
        case 5: _t->on_SendBtn_clicked(); break;
        case 6: _t->on_LogoutBtn_clicked(); break;
        case 7: _t->on_EmailBtn_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UserUi::*)(const char * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserUi::ShowEmailUi)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UserUi::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserUi::ShowMainUi)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UserUi::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_UserUi.data,
      qt_meta_data_UserUi,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *UserUi::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UserUi::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UserUi.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int UserUi::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
    return _id;
}

// SIGNAL 0
void UserUi::ShowEmailUi(const char * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UserUi::ShowMainUi()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
