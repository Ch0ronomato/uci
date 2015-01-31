#ifndef LINKED_SET_HPP_
#define LINKED_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "set.hpp"


namespace ics {

template<class T> class LinkedSet : public Set<T>	{
  public:
	LinkedSet();
	LinkedSet(const LinkedSet<T>& to_copy);
	LinkedSet(std::initializer_list<T> il);
    LinkedSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop);
	virtual ~LinkedSet();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual bool contains   (const T& element) const;
    virtual std::string str () const;

    virtual bool contains (ics::Iterator<T>& start, const ics::Iterator<T>& stop) const;

    virtual int  insert (const T& element);
    virtual int  erase  (const T& element);
    virtual void clear  ();

    virtual int insert (ics::Iterator<T>& start, const ics::Iterator<T>& stop);
    virtual int erase  (ics::Iterator<T>& start, const ics::Iterator<T>& stop);
    virtual int retain (ics::Iterator<T>& start, const ics::Iterator<T>& stop);

    virtual LinkedSet<T>& operator = (const LinkedSet<T>& rhs);
    virtual bool operator == (const Set<T>& rhs) const;
    virtual bool operator != (const Set<T>& rhs) const;
    virtual bool operator <= (const Set<T>& rhs) const;
    virtual bool operator <  (const Set<T>& rhs) const;
    virtual bool operator >= (const Set<T>& rhs) const;
    virtual bool operator >  (const Set<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinkedSet<T2>& s);

  private:
    class LN;

  public:
    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(LinkedSet<T>* fof, LN* initial);
        virtual ~Iterator();
        virtual T           erase();
        virtual std::string str  () const;
        virtual const ics::Iterator<T>& operator ++ ();
        virtual const ics::Iterator<T>& operator ++ (int);
        virtual bool operator == (const ics::Iterator<T>& rhs) const;
        virtual bool operator != (const ics::Iterator<T>& rhs) const;
        virtual T& operator *  () const;
        virtual T* operator -> () const;
      private:
        LN*           current;  //if can_erase is false, this value is unusable
        LinkedSet<T>* ref_set;
        int           expected_mod_count;
        bool          can_erase = true;
    };

    //For explicit use: Iterator<...>& it = c.ibegin(); ... or for (Iterator<...>& it = c.ibegin(); it != c.iend(); ++it)...
    virtual ics::Iterator<T>& ibegin () const;
    virtual ics::Iterator<T>& iend   () const;

    //For implicit use: for (... i : c)...
    virtual Iterator begin () const;
    virtual Iterator end   () const;

  private:
    class LN {
      public:
        LN ()                      {}
        LN (const LN& ln)          : value(ln.value), next(ln.next){}
        LN (T v,  LN* n = nullptr) : value(v), next(n){}

        T   value;
        LN* next   = nullptr;
    };

    int used      = 0;             //Cache of # values in set
    LN* front     = new LN();
    LN* trailer   = front;
    int mod_count = 0;             //For sensing concurrent modification
    int erase_at(LN* p);
    void delete_list(LN*& front);  //Recycle storage, set front's argument to nullptr;
  };



//See code in array_set.hpp and linked_queue.hpp and linked_priority_queue.hpp

//Write the constructors, methods, and operators here for LinkedSet

/**
 * Default constructor
 */
template <class T>
LinkedSet<T>::LinkedSet() : Set<T>() {

}

/**
 * Copy constructor. Since we are coping a set, we can assume the size
 * of the set will stay the same.
 */
template <class T>
LinkedSet<T>::LinkedSet(const LinkedSet<T>& to_copy) : Set<T>(), used(to_copy.size()) {
    insert(to_copy.ibegin(), to_copy.iend());
}

/**
 * Constructor with initializer_list. Initializer lists do not have
 * a uniqueness constraint, so we can't assume anything.
 */
template <class T>
LinkedSet<T>::LinkedSet(std::initializer_list<T> il) : Set<T>() {
    for (T val : il) 
        used += insert(val);
}

/**
 * Constructor given two iterators. Since it's impossible to determine
 * size given two ics::Iterators, we cannot assume a size.
 */
template <class T>
LinkedSet<T>::LinkedSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop)
    : Set<T>() {
    used = insert(start, stop);
}

template <class T>
LinkedSet<T>::~LinkedSet() {
    delete_list(front);
}

template <class T>
bool LinkedSet<T>::empty() const {
    // for (auto front = ibegin(); front != iend(); front++)
    //     erase(*front);
}

template <class T>
int  LinkedSet<T>::size() const {
    return used;
}

template <class T>
bool LinkedSet<T>::contains(const T& element) const {
    bool to_return = false;
    for (LN *node = front; node != trailer; node = node->next) 
        if (node->value == element) {
            to_return = true;
        }

    return to_return;
}

template <class T>
std::string LinkedSet<T>::str() const {

}

template<class T>
bool LinkedSet<T>::contains (ics::Iterator<T>& start, const ics::Iterator<T>& stop) const {

}

template<class T>
int  LinkedSet<T>::insert (const T& element) {

}

template<class T>
int  LinkedSet<T>::erase  (const T& element) {

}

template<class T>
void LinkedSet<T>::clear  () {

}


template <class T>
int LinkedSet<T>::insert (ics::Iterator<T>& start, const ics::Iterator<T>& stop) {

}
template <class T>
int LinkedSet<T>::erase  (ics::Iterator<T>& start, const ics::Iterator<T>& stop) {

}
template <class T>
int LinkedSet<T>::retain (ics::Iterator<T>& start, const ics::Iterator<T>& stop) {

}

template <class T>
LinkedSet<T>& LinkedSet<T>::operator = (const LinkedSet<T>& rhs) {

}

template<class T>
bool LinkedSet<T>::operator == (const Set<T>& rhs) const {

}

template<class T>
bool LinkedSet<T>::operator != (const Set<T>& rhs) const {

}

template<class T>
bool LinkedSet<T>::operator <= (const Set<T>& rhs) const {

}

template<class T>
bool LinkedSet<T>::operator <  (const Set<T>& rhs) const {

}

template<class T>
bool LinkedSet<T>::operator >= (const Set<T>& rhs) const {

}

template<class T>
bool LinkedSet<T>::operator >  (const Set<T>& rhs) const {

}

template<class T>
LinkedSet<T>::Iterator::Iterator(LinkedSet<T>* fof, LN* initial) {

}

template<class T>
LinkedSet<T>::Iterator::~Iterator() {

}

template<class T>
T LinkedSet<T>::Iterator::erase() {

}

template<class T>
std::string LinkedSet<T>::Iterator::str  () const {

}

template<class T>
const ics::Iterator<T>& LinkedSet<T>::Iterator::operator ++ () {

}

template<class T>
const ics::Iterator<T>& LinkedSet<T>::Iterator::operator ++ (int) {

}

template<class T>
bool LinkedSet<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {

}

template<class T>
bool LinkedSet<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {

}

template<class T>
T& LinkedSet<T>::Iterator::operator *  () const {

}

template<class T>
T* LinkedSet<T>::Iterator::operator -> () const {

}

template<class T2>
std::ostream& operator << (std::ostream& outs, const LinkedSet<T2>& s) {

}

template<class T>
auto LinkedSet<T>::ibegin() const -> ics::Iterator<T>&{

}

template<class T>
auto LinkedSet<T>::iend() const -> ics::Iterator<T>&{

}

template<class T>
auto LinkedSet<T>::begin() const -> LinkedSet<T>::Iterator {

}

template<class T>
auto LinkedSet<T>::end() const -> LinkedSet<T>::Iterator {

}


template<class T>
void LinkedSet<T>::delete_list(LN*& node) {

}

}

#endif /* LINKED_SET_HPP_ */
