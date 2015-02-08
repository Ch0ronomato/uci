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




template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(bool (*agt)(const T& a, const T& b)) : PriorityQueue<T>(agt) {
  gt = agt;
  pq = new T[length];
}


template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(int initial_length, bool (*agt)(const T& a, const T& b))
  : PriorityQueue<T>(agt), length(initial_length) {
  if (length < 0)
    length = 0;
  pq = new T[length];
}


template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(const HeapPriorityQueue<T>& to_copy)
  : PriorityQueue<T>(to_copy.gt), length(to_copy.length), used(to_copy.used) {
  pq = new T[length];
  for (int i=0; i<to_copy.used; ++i)
    pq[i] = to_copy.pq[i];
}


template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop, bool (*agt)(const T& a, const T& b))
  : PriorityQueue<T>(agt) {
  pq = new T[length];
  enqueue(start,stop);
}


template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(std::initializer_list<T> il, bool (*agt)(const T& a, const T& b))
  : PriorityQueue<T>(agt) {
  pq = new T[length];
  for (T pq_elem : il)
    enqueue(pq_elem);
}


template<class T>
HeapPriorityQueue<T>::~HeapPriorityQueue() {
  delete [] pq;
}


template<class T>
inline bool HeapPriorityQueue<T>::empty() const {
  return used == 0;
}


template<class T>
int HeapPriorityQueue<T>::size() const {
  return used;
}


template<class T>
T& HeapPriorityQueue<T>::peek () const {
  if (empty())
    throw EmptyError("HeapPriorityQueue::peek");
  return pq[0];
}


template<class T>
std::string HeapPriorityQueue<T>::str() const {
  std::ostringstream answer;
  answer << *this << "(length=" << length << ",used=" << used << ",mod_count=" << mod_count << ")";
  return answer.str();
}


template<class T>
int HeapPriorityQueue<T>::enqueue(const T& element) {
  this->ensure_length(used + 1);
  pq[used] = element;
  // re heapify
  percolate_up(used++);
  return 1;
}


template<class T>
T HeapPriorityQueue<T>::dequeue() {
  if (this->empty())
    throw EmptyError("HeapPriorityQueue::dequeue");

  ++mod_count;
}


template<class T>
void HeapPriorityQueue<T>::clear() {
  used = 0;
  ++mod_count;
}


template<class T>
int HeapPriorityQueue<T>::enqueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  int count = 0;
  for (; start != stop; ++start)
    count += enqueue(*start);

  return count;
}


template<class T>
HeapPriorityQueue<T>& HeapPriorityQueue<T>::operator = (const HeapPriorityQueue<T>& rhs) {
  if (rhs == *this)
    return *this;
  this->ensure_length(rhs.used);
  gt   = rhs.gt;  //gt is in the base class
  used = rhs.used;
  
  ++mod_count;
  return *this;
}


template<class T>
  bool HeapPriorityQueue<T>::operator == (const PriorityQueue<T>& rhs) const {
  if (this == &rhs)
    return true;
  if (used != rhs.size())
    return false;
  if (gt != rhs.gt)
    return false;
  if (used != rhs.size())
    return false;
  //KLUDGE: should check for same == function used to prioritize, but cannot unless
  //  it is made part of the PriorityQueue class (should it be? protected)?
  ics::Iterator<T>& rhs_i = rhs.ibegin();
  
  return true;
}


template<class T>
bool HeapPriorityQueue<T>::operator != (const PriorityQueue<T>& rhs) const {
  return !(*this == rhs);
}


template<class T>
std::ostream& operator << (std::ostream& outs, const HeapPriorityQueue<T>& p) {
  outs << "priority_queue[";

  if (!p.empty()) {
  }

  outs << "]:highest";
  return outs;
}


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

template<class T>
void HeapPriorityQueue<T>::ensure_length(int new_length) {
  if (length >= new_length)
    return;
  T*  old_pq  = pq;
  length = std::max(new_length,2*length);
  pq = new T[length];
  for (int i=0; i<used; ++i)
    pq[i] = old_pq[i];

  delete [] old_pq;
}


template<class T>
HeapPriorityQueue<T>::Iterator::Iterator(HeapPriorityQueue<T> *iterate_over, bool begin) : ref_pq(iterate_over), it(*iterate_over) { }

template<class T>
HeapPriorityQueue<T>::Iterator::Iterator(const Iterator& i) : ref_pq(i.ref_pq), it(i.it) { }


template<class T>
HeapPriorityQueue<T>::Iterator::~Iterator() {}


template<class T>
T HeapPriorityQueue<T>::Iterator::erase() {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("HeapPriorityQueue::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("HeapPriorityQueue::Iterator::erase Iterator cursor already erased");

  can_erase = false;
  expected_mod_count = ref_pq->mod_count;
}


template<class T>
std::string HeapPriorityQueue<T>::Iterator::str() const {
  std::ostringstream answer;
}


template<class T>
const ics::Iterator<T>& HeapPriorityQueue<T>::Iterator::operator ++ () {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ++");

  return *this;
}


//KLUDGE: can create garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& HeapPriorityQueue<T>::Iterator::operator ++ (int) {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ++(int)");
}


template<class T>
bool HeapPriorityQueue<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HeapPriorityQueue::Iterator::operator ==");
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ==");
  if (ref_pq != rhsASI->ref_pq)
    throw ComparingDifferentIteratorsError("HeapPriorityQueue::Iterator::operator ==");

}


template<class T>
bool HeapPriorityQueue<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HeapPriorityQueue::Iterator::operator !=");
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator !=");
  if (ref_pq != rhsASI->ref_pq)
    throw ComparingDifferentIteratorsError("HeapPriorityQueue::Iterator::operator !=");
}


template<class T>
T& HeapPriorityQueue<T>::Iterator::operator *() const {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator *");
}


template<class T>
T* HeapPriorityQueue<T>::Iterator::operator ->() const {
  if (expected_mod_count !=  ref_pq->mod_count)
    throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ->");
}

/**
 * HELPER FUNCTIONS
 */
template<class T> 
int HeapPriorityQueue<T>::left_child(int i) {
  return (i * 2) + 1;
}

template<class T> 
int HeapPriorityQueue<T>::right_child(int i) {
  return (i * 2) + 2;
}

template<class T> 
int HeapPriorityQueue<T>::parent(int i) {
  return ((i % 2 == 1 ? i + 1 : i) / 2) - 1;
}

template<class T>
bool HeapPriorityQueue<T>::is_root(int i) {
  return i == 0;
}

template<class T>
bool HeapPriorityQueue<T>::in_heap(int i) {
  return i < used;
}

template<class T>
void HeapPriorityQueue<T>::percolate_up(int i) {
  // find parent and go up.
  int parentIndex = parent(i);
  if (is_root(i) || !gt(pq[i], pq[parentIndex])) return;
  std::swap(pq[parentIndex], pq[i]);
  percolate_up(parentIndex); // parentIndex now become's i.
}

template<class T>
void HeapPriorityQueue<T>::percolate_down(int i) {

}

}
#endif /* HEAP_PRIORITY_QUEUE_HPP_ */