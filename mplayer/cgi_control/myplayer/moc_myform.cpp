/****************************************************************************
** Meta object code from reading C++ file 'myform.h'
**
** Created: Sat Mar 30 14:52:23 2013
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "myform.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'myform.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyForm[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x0a,
      19,    7,    7,    7, 0x0a,
      38,    7,    7,    7, 0x0a,
      51,    7,    7,    7, 0x0a,
      62,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MyForm[] = {
    "MyForm\0\0do_pause()\0do_readUnusedMsg()\0"
    "do_readMsg()\0myAccept()\0processMsg()\0"
};

const QMetaObject MyForm::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MyForm,
      qt_meta_data_MyForm, 0 }
};

const QMetaObject *MyForm::metaObject() const
{
    return &staticMetaObject;
}

void *MyForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyForm))
        return static_cast<void*>(const_cast< MyForm*>(this));
    return QWidget::qt_metacast(_clname);
}

int MyForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: do_pause(); break;
        case 1: do_readUnusedMsg(); break;
        case 2: do_readMsg(); break;
        case 3: myAccept(); break;
        case 4: processMsg(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
