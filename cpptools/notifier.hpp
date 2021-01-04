#ifndef CPPTOOLS__NOTIFIER_HPP
#define CPPTOOLS__NOTIFIER_HPP

#include <functional>
#include <unordered_map>

namespace CppTools
{

template <typename ...ArgTypes>
class Notifier
{
    public:
        using ListenerType = std::function<void(ArgTypes...)>;

    private:
        std::unordered_map<unsigned int, ListenerType> _listeners;
        unsigned int _count;

    public:
        Notifier();

        unsigned int addSubscriber(ListenerType callback);
        void deleteSubscriber(unsigned int id);

        void notify(ArgTypes... args) const;
};

template<typename ...ArgTypes>
Notifier<ArgTypes...>::Notifier() :
    _listeners(),
    _count(0)
{

}

template<typename ...ArgTypes>
unsigned int Notifier<ArgTypes...>::addSubscriber(std::function<void(ArgTypes...)> callback)
{
    _listeners[_count] = callback;
    return _count++;
}

template<typename ...ArgTypes>
void Notifier<ArgTypes...>::deleteSubscriber(unsigned int id)
{
    _listeners.erase(id);
}

template<typename ...ArgTypes>
void Notifier<ArgTypes...>::notify(ArgTypes... args) const
{
    for (auto it = _listeners.begin(); it != _listeners.end(); it++)
    {
        ListenerType listener = it->second;
        listener(std::forward<ArgTypes>(args)...);
    }
}

}//namespace CppTools

#endif //TOOLS__NOTIFIER_HPP