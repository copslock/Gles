
#ifndef __LIST__
#define __LIST__
#ifndef _MSC_VER
#include <iostream>
#endif


template <class T>
class List
{
    struct Item
    {
        T data;
        Item *next;
    };

public:
    List();
    ~List();

    bool isEmpty() const;
    int length() const;
    const T &at(int k) const;
    int search(const T &v);
    void remove(int k);
    void insert(int k, const T &v);
    void output();

private:
    Item *header;
};

template <class T>
List<T>::List()
{
    header = 0;
}

template <class T>
List<T>::~List()
{
    Item *tmp, *i = header;
    while (i)
    {
        tmp = i;
        i = i->next;
        delete tmp;
    }
}

template <class T>
bool List<T>::isEmpty() const
{
    return header == 0;
}

template <class T>
int List<T>::length() const
{
    int len = 0;
    Item *i = header;
    while (i)
    {
        len++;
        i = i->next;
    }

    return len;
}

template <class T>
const T &List<T>::at(int k) const
{
    do
    {
        if (k < 0)
            break;

        Item *it = header;
        if (it == 0)
            break;

        int i;
        for (i = 0; it && i < k; i++)
            it = it->next;

        if (i != k)
            break;

        return it->data;
    }
    while (0);

    T *ret = new T;
    return *ret;
}

template <class T>
int List<T>::search(const T &v)
{
    int k = 0;
    Item *i = header;
    while (i)
    {
        if (i->data == v)
            return k;

        k++;
        i = i->next;
    }

    return -1;
}

template <class T>
void List<T>::remove(int k)
{
    if (k < 0)
        return;

    Item *prev = 0, *it = header;
    for (int i = 0; i < k; i++)
    {
        if (it == 0)
            return;

        prev = it;
        it = it->next;
    }

    if (prev != 0)
        prev->next = it->next;
    else
        header = it->next;

    delete it;
    return;
}

template <class T>
void List<T>::insert(int k, const T &v)
{
    Item *t = new Item;
    t->data = v;
    t->next = 0;

    if (header == 0)
    {
        header = t;
        return;
    }

    Item *it = header;
    bool flag = false;
    if (k < 0)
        flag = true;

    for (int i = 0; it->next != 0 && (flag || i < k); i++)
        it = it->next;

    t->next = it->next;
    it->next = t;
}

template <class T>
void List<T>::output()
{
    Item *i = header;
    while (i)
    {
        //std::cout << i->data << std::endl;
        i = i->next;
    }
}

#endif
