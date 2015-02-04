// Ian Schweer (Unique ID: 660942)
// Shaun McThomas (Unique ID: 307523)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming:
//   primarily that both partners worked on all parts together.

#ifndef LINKED_QUEUE_HPP_
#define LINKED_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "queue.hpp"


namespace ics {

template<class T> class LinkedQueue : public Queue<T>  {
  public:
    LinkedQueue();
    LinkedQueue(const LinkedQueue<T>& to_copy);
    LinkedQueue(std::initializer_list<T> il);
    LinkedQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop);
    virtual ~LinkedQueue();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual T&   peek       () const;
    virtual std::string str () const;

    virtual int  enqueue (const T& element);
    virtual T    dequeue ();
    virtual void clear   ();

    virtual int enqueue (ics::Iterator<T>& start, const ics::Iterator<T>& stop);

    virtual LinkedQueue<T>& operator = (const LinkedQueue<T>& rhs);
    virtual bool operator == (const Queue<T>& rhs) const;
    virtual bool operator != (const Queue<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinkedQueue<T2>& s);

  private:
    class LN;

  public:
    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(LinkedQueue<T>* fof, LN* initial);
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
        LN*             prev = nullptr;  //if nullptr, current at front of list
        LN*             current;         //if can_erase is false, this value is unusable
        LinkedQueue<T>* ref_queue;
        int             expected_mod_count;
        bool            can_erase = true;
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
        LN* next = nullptr;
    };

    int used      =  0;
    LN* front     =  nullptr;
    LN* rear      =  nullptr;
    int mod_count =  0;            //For sensing of concurrent modification
    void delete_list(LN*& front);  //Recycle storage, set front's argument to nullptr;
};



//See code in array_queue.hpp

//Write the constructors, methods, and operators here for LinkedQueue

/**
 * Default constuctor. Not much to do here
 */
template <class T>
LinkedQueue<T>::LinkedQueue() {
}

/**
 * Copy constuctor. Given a linked queue, enqueue all the elements
 * in the argument queue to make the current queue equal. See equals
 * for definition of equality
 *
 * @param const LinkedQueue<T>& to_copy
 *  The queue to copy
 */
template <class T>
LinkedQueue<T>::LinkedQueue(const LinkedQueue<T>& to_copy) {
    enqueue(to_copy.ibegin(), to_copy.iend());
}

/**
 * Constuctor w/ std::initializer_list. Given a initial list of 
 * elements, enqueue each to construct our queue. All elements are
 * added in order and processed in order.
 *
 * @param std::initialize_list<T> il
 *  The initial values of the queue
 */
template <class T>
LinkedQueue<T>::LinkedQueue(std::initializer_list<T> il) {
    for (T each : il) enqueue(each);
}

/**
 * Constuctor w/ iterators. Given two ics iterators, enqueue all
 * elements between the iterators. We assume both iterators point
 * to the same list of values.
 *
 * @param ics::Iterator<T>& start
 *  The starting iterator
 * @param const ics::Iterator<T>& end
 *  The ending iterator
 */
template <class T>
LinkedQueue<T>::LinkedQueue(ics::Iterator<T>& start, const ics::Iterator<T>& end) {
    enqueue(start, end);
}

/**
 * Deconstructor
 */
template <class T>
LinkedQueue<T>::~LinkedQueue() { 
  delete_list(front); 
}

/**
 * Empty. Function will determine if the queue is empty.
 * Note: An alternative way to do this would be to say front==nullptr
 * however, that then requires the code further down to enforce that 
 * requirement, where as used is a highly used integer. This is easier.
 */
template <class T>
bool LinkedQueue<T>::empty() const {
    return used == 0;
}

/**
 * Size. Function will return the size of the queue. 
 */
template <class T>
int LinkedQueue<T>::size() const {
    return used;
}

/**
 * Peek. Function will return the first value in the queue
 * WITHOUT dequeueing the value. Useful for consumer code.
 */
template <class T>
T& LinkedQueue<T>::peek() const {
    if (empty()) throw EmptyError("LinkedQueue::peek the queue is empty");
    return front->value;
}

/**
 * Str. A debugging print method.
 */
template <class T>
std::string LinkedQueue<T>::str() const {
    std::ostringstream str_stream;
    if (front == nullptr)
        str_stream << "queue[]";
    else {
        str_stream << "queue[";
        LN *node = front;
        while (node != nullptr) {
            str_stream << node->value;
            if (node != rear) str_stream<< ",";
            else str_stream << "]";
            node = node->next;
        }
    }
    str_stream << "(used=" << used << ", mod_count=" << mod_count;
    if (front != nullptr) str_stream << ", front=" << front->value;
    if (rear != nullptr) str_stream << ", rear=" << rear->value;
    str_stream << ")";
    return str_stream.str();
}

/**
 * Enqueue. Function will add an element to the rear of the list.
 * This function can have large mutations on the queue, since the
 * two interesting spots in a queue are the front and rear. If the
 * queue is empty, we initialize the queue, if not we grow it.
 *
 * @return int
 *  1, a guarentee that we will insert.
 */
template <class T>
int LinkedQueue<T>::enqueue (const T& element) {
    // is our queue empty?
    if (front == nullptr) {
        front = rear = new LN(element);
    } else {
        // simply increase our rear
        rear = rear->next = new LN(element);
    }

    // house keeping and return
    mod_count++;
    used++;
    return 1;
}

/**
 * Enqueue w/ iterators. Function will enqueue multiple items given
 * a start and end iterators. Function will consume the result of the 
 * simplier enqueue method (see Enqueue).
 *
 * @param ics::Iterator<T>& start
 *  The starting iterator
 * @param const ics::Iterator<T>& stop
 *  The ending iterator
 *
 * @return int
 *  The number of insertions to the queue.
 */
template <class T>
int LinkedQueue<T>::enqueue (ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
    int insert = 0;
    // for each value, call the enqueue.
    while (start != stop) {
        insert += enqueue(*start);
        ++start;
    }
    return insert;
}

/**
 * Dequeue. Function will "pop" the top element of the queue for processing.
 * The function will also move the front pointer through the list, checking 
 * boundaries as it goes. It will return the dequeue value and manage deletion.
 *
 * @return T
 *  The value to process in the queue
 *
 * @throw EmptyError 
 *  If we are out of bounds, we will error
 */
template <class T>
T LinkedQueue<T>::dequeue() {
    // bounds check
    if (empty())
        throw EmptyError("Linked Queue is empty");

    // Get current head.
    LN *val = front;

    // increment head.
    if (front->next == nullptr) {
        front = rear = nullptr;
    } else {
        front = front->next;
    }

    // get the return value;
    T return_value = val->value;

    // delete the un-needed node;
    delete val;

    // house keeping and return
    mod_count++;
    used--;
    return return_value;
}

/**
 * Clear. Function will clear the entire queue. See delete_list for
 * details
 */
template <class T>
void LinkedQueue<T>::clear() {
  delete_list(front);
  rear = nullptr;
}

/**
 * delete_list. Given a starting node position, function will
 * delete a node and all subsequent nodes. Given the structure
 * of the code, this function can also delete pieces of a linked list,
 * say for example half
 */
template <class T>
void LinkedQueue<T>::delete_list(LN *&node) {
    LN *to_delete = node;
    for ( ;node != nullptr; used--, mod_count++) 
    {   
        node = node->next;    
        delete to_delete;
        to_delete = node;
   }
}

/**
 * Assignment. Function will become equal to a argument queue.
 * See equality operator for precise definition. In the event
 * that the queue has already been used previously, the queue
 * will be cleared.
 */
template <class T>
LinkedQueue<T>& LinkedQueue<T>::operator = (const LinkedQueue<T>& rhs) {
    if (!empty()) clear();
    enqueue(rhs.ibegin(), rhs.iend());
    return *this;
}

/**
 * Equality. Function will determine if two queues are equal. Two
 * queues are equal if and only if the sizes are equal, and if the
 * two queues are in the SAME order.
 *
 * @param const Queue<T>&
 *  The argument queue. Notice the type is not a linked queue, that
 *  is becase we use iterators to check element and order equality.
 *  This allows for cross compabilities between data structures
 *  as long as they are the same data type.
 */
template <class T>
bool LinkedQueue<T>::operator == (const Queue<T>& rhs) const {
    if (this == &rhs) return true;
    if (size() != rhs.size()) return false;
    
    for (auto &lhs_iter = ibegin(), &rhs_iter = rhs.ibegin();lhs_iter != iend() ;lhs_iter++, rhs_iter++) 
        if (*lhs_iter != *rhs_iter) return false;
    return true;
}

/**
 * Non-Equality. Function will determine if two queues are NOT equal. See
 * equality for definition.
 *
 * @param const Queue<T>&
 *  The argument queue. Notice the type is not a linked queue, that
 *  is becase we use iterators to check element and order equality.
 *  This allows for cross compabilities between data structures
 *  as long as they are the same data type.
 */
template <class T>
bool LinkedQueue<T>::operator != (const Queue<T>& rhs) const {
    return !((*this) == rhs);
}

/**
 * Out stream. Function will print out the queue.
 */
template<class T2>
std::ostream& operator << (std::ostream& outs, const LinkedQueue<T2>& s) {
    outs << "queue[";
    int i = 0;
    for (auto & iter = s.ibegin(); i < s.used; iter++, i++) {
        (i < s.size() - 1) ? outs << *iter << "," : outs << *iter;
    }
    outs << "]:rear";
    return outs;
}

//Fill in the missing parts of the erase method and ++ operators
//  for LinkedQueue's Iterator



//KLUDGE: memory-leak
template<class T>
auto LinkedQueue<T>::ibegin () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinkedQueue<T>*>(this),front));
}

//KLUDGE: memory-leak
template<class T>
auto LinkedQueue<T>::iend () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinkedQueue<T>*>(this),nullptr));
}

template<class T>
auto LinkedQueue<T>::begin () const -> LinkedQueue<T>::Iterator {
  return Iterator(const_cast<LinkedQueue<T>*>(this),front);
}

template<class T>
auto LinkedQueue<T>::end () const -> LinkedQueue<T>::Iterator {
  return Iterator(const_cast<LinkedQueue<T>*>(this),nullptr);
}



template<class T>
LinkedQueue<T>::Iterator::Iterator(LinkedQueue<T>* fof, LN* initial) : current(initial), ref_queue(fof) {
  expected_mod_count = ref_queue->mod_count;
}

template<class T>
LinkedQueue<T>::Iterator::~Iterator() {}

/**
 * Erase. Function will erase the current iterator position. After an
 * erase call, we will return the value of deleted node. Erase will mutate
 * the under-line reference queue. 
 *
 * @throw ConcurrentModificationError given conflicting modification counts
 *        CannotEraseError given either false permission or the current position is out of bounds
 */
template<class T>
T LinkedQueue<T>::Iterator::erase() {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("LinkedQueue::Iterator::erase Iterator cursor already erased");
  if (current == nullptr)
    throw CannotEraseError("LinkedQueue::Iterator::erase Iterator cursor beyond data structure");

  T val = current->value;
  LN *to_delete = current;
  if (current == ref_queue->front) 
    ref_queue->front = ref_queue->front->next;
  else
    prev->next = current->next;
  current = current->next;
  delete to_delete;
  can_erase = false;
  expected_mod_count = ref_queue->mod_count;
  ref_queue->used--;
  return val;
}

/**
 * str. Just a debug string function.
 */
template<class T>
std::string LinkedQueue<T>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_queue->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}

/**
 * Pre-fix increment. Function will increment the current position of the
 * the iterator given valid erase permissons. Prefix will return the newly 
 * update current position of the iterator.
 *
 * @throw ConcurrentModificationError given conflicting modification counts
 */
template<class T>
const ics::Iterator<T>& LinkedQueue<T>::Iterator::operator ++ () {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator ++");

  if (!can_erase) can_erase = true;
  else {
      if (current != nullptr) {
      prev = current;
      current = current->next;
    }
  }  
  return *this;
}

//KLUDGE: can create garbage! (can return local value!)

/**
 * Post-fix increment. Function will increment the current position of the
 * the iterator given valid erase permissons. Post-fix will return the previous 
 * position of the iterator.
 *
 * @throw ConcurrentModificationError given conflicting modification counts
 */
template<class T>
const ics::Iterator<T>& LinkedQueue<T>::Iterator::operator ++ (int) {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator ++(int)");
  
  if (current == nullptr) return *this;
  if (!can_erase) can_erase = true;
  else {
    prev = current;
    current = current->next;
  }
  return *(new Iterator(ref_queue, prev));
}

/**
 * Iterator Equality. Function will determine if two iterators are equal.
 * Two iterators are equal if they both point to the same spot in the data
 * structure.
 *
 * @throw ConcurrentModificationError given conflicting modification counts
 *        IteratorTypeError given the right hand sider iterator is null
 *        ComparingDifferentIteratorsError given the two reference queues don't
 *          match
 */
template<class T>
bool LinkedQueue<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("LinkedQueue::Iterator::operator ==");
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator ==");
  if (ref_queue != rhsASI->ref_queue)
    throw ComparingDifferentIteratorsError("LinkedQueue::Iterator::operator ==");
  return current == rhsASI->current;
}

/**
 * Iterator Non-Equality. Function will determine if two iterators are NOT equal.
 * See equality for definition.
 *
 * @throw ConcurrentModificationError given conflicting modification counts
 *        IteratorTypeError given the right hand sider iterator is null
 *        ComparingDifferentIteratorsError given the two reference queues don't
 *          match
 */
template<class T>
bool LinkedQueue<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("LinkedQueue::Iterator::operator !=");
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator !=");
  if (ref_queue != rhsASI->ref_queue)
    throw ComparingDifferentIteratorsError("LinkedQueue::Iterator::operator !=");
  return current != rhsASI->current;
}

template<class T>
T& LinkedQueue<T>::Iterator::operator *() const {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator *");
  if (!can_erase || current == nullptr) {
    std::ostringstream where;
    where << current
          << " when front = " << ref_queue->front << " and "
          << " and rear = " << ref_queue->rear;
    throw IteratorPositionIllegal("LinkedQueue::Iterator::operator * Iterator illegal: "+where.str());
  }
  return current->value;
}

template<class T>
T* LinkedQueue<T>::Iterator::operator ->() const {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator *");
  if (!can_erase || current == nullptr) {
    std::ostringstream where;
    where << current
          << " when front = " << ref_queue->front << " and "
          << " and rear = " << ref_queue->rear;
    throw IteratorPositionIllegal("LinkedQueue::Iterator::operator * Iterator illegal: "+where.str());
  }

  return &(current->value);
}

}

#endif
