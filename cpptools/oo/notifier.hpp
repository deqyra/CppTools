#ifndef CPPTOOLS__OO__NOTIFIER_HPP
#define CPPTOOLS__OO__NOTIFIER_HPP

#include <functional>
#include <unordered_map>

namespace tools::oo
{

template <typename ...ArgTypes>
class notifier
{
public:
    using listener_fun = void (*)(ArgTypes&&...);

private:
    std::unordered_map<unsigned int, listener_fun> _listeners;
    unsigned int _count;

public:
    notifier() :
        _listeners(),
        _count(0)
    {

    }

    unsigned int add_subscriber(listener_fun callback)
    {
        _listeners[_count] = callback;
        return _count++;
    }

    void delete_subscriber(unsigned int id)
    {
        _listeners.erase(id);
    }

    void notify(ArgTypes&&... args) const
    {
        for (auto it = _listeners.begin(); it != _listeners.end(); it++)
        {
            it->second(std::forward<ArgTypes&&>(args)...);
        }
    }
};

} // namespace tools::oo

#endif //CPPTOOLS__NOTIFIER_HPP