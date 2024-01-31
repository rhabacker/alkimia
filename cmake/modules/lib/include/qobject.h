#ifndef QT4_QOBJECT_H
#define QT4_QOBJECT_H

#define WRAPPED_QOBJECT_DEFS_H
#include_next  <qobject.h>

#include <QAtomicInt>
#define Q_DISABLE_COPY_MOVE(a)
#define QOBJECTDEFS_H
#include "qobjectdefs_impl.h"

//Connect a signal to a pointer to qobject member function
template <typename Func1, typename Func2>
static inline /*QMetaObject::Connection */ void *_connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                 const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                                 Qt::ConnectionType type = Qt::AutoConnection)
{
    typedef QtPrivate::FunctionPointer<Func1> SignalType;
    typedef QtPrivate::FunctionPointer<Func2> SlotType;
#if 0
    Q_STATIC_ASSERT_X(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value,
                      "No Q_OBJECT in the class with the signal");

    //compilation error if the arguments does not match.
    Q_STATIC_ASSERT_X(int(SignalType::ArgumentCount) >= int(SlotType::ArgumentCount),
                      "The slot requires more arguments than the signal provides.");
    Q_STATIC_ASSERT_X((QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::value),
                      "Signal and slot arguments are not compatible.");
    Q_STATIC_ASSERT_X((QtPrivate::AreArgumentsCompatible<typename SlotType::ReturnType, typename SignalType::ReturnType>::value),
                      "Return type of the slot is not compatible with the return type of the signal.");

    const int *types = nullptr;
    if (type == Qt::QueuedConnection || type == Qt::BlockingQueuedConnection)
        types = QtPrivate::ConnectionTypes<typename SignalType::Arguments>::types();

    return connectImpl(sender, reinterpret_cast<void **>(&signal),
                       receiver, reinterpret_cast<void **>(&slot),
                       new QtPrivate::QSlotObject<Func2, typename QtPrivate::List_Left<typename SignalType::Arguments, SlotType::ArgumentCount>::Value,
                                       typename SignalType::ReturnType>(slot),
                        type, types, &SignalType::Object::staticMetaObject);
#endif
    return 0;
}
#endif // QT4_QOBJECT_H
