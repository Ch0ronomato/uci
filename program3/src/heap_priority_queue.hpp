#ifndef HEAP_PRIORITY_QUEUE_HPP_
#define HEAP_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "priority_queue.hpp"
#include <utility>              //For std::swap function
#include "array_stack.hpp"


namespace ics {

template<class T> class HeapPriorityQueue : public PriorityQueue<T>  {
  using PriorityQueue<T>::gt;  //Required because of templated classes
  public:
    HeapPriorityQueue() = delete;
    explicit HeapPriorityQueue(bool (*agt)(const T& a, const T& b));
    HeapPriorityQueue(int initialLength,bool (*agt)(const T& a, const T& b));
    HeapPriorityQueue(const HeapPriorityQueue<T>& to_copy);
    HeapPriorityQueue(std::initializer_list<T> il,bool (*agt)(const T& a, const T& b));
    HeapPriorityQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop,bool (*agt)(const T& a, const T& b));
    virtual ~HeapPriorityQueue();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual T&   peek       () const;
    virtual std::string str () const;

    virtual int  enqueue (const T& element);
    virtual T    dequeue ();
    virtual void clear   ();

    virtual int enqueue (ics::Iterator<T>& start, const ics::Iterator<T>& stop);

    virtual HeapPriorityQueue<T>& operator = (const HeapPriorityQueue<T>& rhs);
    virtual bool operator == (const PriorityQueue<T>& rhs) const;
    virtual bool operator != (const PriorityQueue<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const HeapPriorityQueue<T2>& p);

    virtual ics::Iterator<T>& ibegin() const;
    virtual ics::Iterator<T>& iend  () const;

    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(HeapPriorityQueue<T>* iterate_over, bool begin);
        Iterator(const Iterator& i);
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
        HeapPriorityQueue<T>  it;          //Copy of pq to use as iterator via dequeue
        HeapPriorityQueue<T>* ref_pq;
        int                   expected_mod_count;
        bool                  can_erase = true;
    };

    virtual Iterator begin() const;
    virtual Iterator end  () const;

  private:
    //See base class PriorityQueue
    //bool (*gt)(const T& a, const T& b);// gt(a,b) = true iff a has higher priority than b
    T*  pq;
    int  length    = 0;                  //Physical length of the array (must be > .size()
    int  used      = 0;                  //Amount of array used
    int  mod_count = 0;                  //For sensing concurrent modification
    void ensure_length(int new_length);
    int  left_child     (int i);         //Useful abstractions for heaps as arrays
    int  right_child    (int i);
    int  parent         (int i);
    bool is_root        (int i);
    bool in_heap        (int i);
    void percolate_up   (int i);
    void percolate_down (int i);
  };




//Insert constructor/methods here: see array_priority_queue.hpp

//Insert heap helper methods here.


//KLUDGE: memory-leak
template<class T>
auto HeapPriorityQueue<T>::ibegin () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<HeapPriorityQueue<T>*>(this),true));
}

//KLUDGE: memory-leak
template<class T>
auto HeapPriorityQueue<T>::iend () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<HeapPriorityQueue<T>*>(this),false));
}

template<class T>
auto HeapPriorityQueue<T>::begin () const -> HeapPriorityQueue<T>::Iterator {
  return Iterator(const_cast<HeapPriorityQueue<T>*>(this),true);
}

template<class T>
auto HeapPriorityQueue<T>::end () const -> HeapPriorityQueue<T>::Iterator {
  return Iterator(const_cast<HeapPriorityQueue<T>*>(this),false);
}




//Insert Iterator constructor/methods here: see array_priority_queue.hpp



}

#endif /* HEAP_PRIORITY_QUEUE_HPP_ */
