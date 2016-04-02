#ifndef COEUS_EVENT_HPP
#define COEUS_EVENT_HPP

#include <vector>
#include <functional>

using std::vector;
using std::function;

template <class T>
class Event
{
public:

    void AddListener(function<void(T)> callback)
    {
        m_Callbacks.push_back(callback);
    }

    void Dispatch(T data)
    {
        for (auto func : m_Callbacks)
        {
            func(data);
        }
    }

private:

    vector<function<void(T)>> m_Callbacks;
};

#endif // COEUS_EVENT_HPP
