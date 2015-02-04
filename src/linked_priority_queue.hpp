#ifndef LINKED_PRIORITY_QUEUE_HPP_
#define LINKED_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "priority_queue.hpp"
#include "array_stack.hpp"


namespace ics {

template<class T> class LinkedPriorityQueue : public PriorityQueue<T>  {
  using PriorityQueue<T>::gt;  //Required because of templated classes
  public:
    LinkedPriorityQueue() = delete;
    explicit LinkedPriorityQueue(bool (*agt)(const T& a, const T& b));
    LinkedPriorityQueue(const LinkedPriorityQueue<T>& to_copy);
    LinkedPriorityQueue(std::initializer_list<T> il,bool (*agt)(const T& a, const T& b));
    LinkedPriorityQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop,bool (*agt)(const T& a, const T& b));
    virtual ~LinkedPriorityQueue();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual T&   peek       () const;
    virtual std::string str () const;

    virtual int  enqueue (const T& element);
    virtual T    dequeue ();
    virtual void clear   ();

    virtual int enqueue (ics::Iterator<T>& start, const ics::Iterator<T>& stop);

    virtual LinkedPriorityQueue<T>& operator = (const LinkedPriorityQueue<T>& rhs);
    virtual bool operator == (const PriorityQueue<T>& rhs) const;
    virtual bool operator != (const PriorityQueue<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T2>& s);

  private:
    class LN;

  public:
    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(LinkedPriorityQueue<T>* fof, LN* initial);
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
        LN*                     prev;     //if header, then current is at front of list
        LN*                     current;  //if can_erase is false, this value is unusable
        LinkedPriorityQueue<T>* ref_pq;
        int                     expected_mod_count;
        bool                    can_erase = true;
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

    //See base class PriorityQueue
    //bool (*gt)(const T& a, const T& b);// gt(a,b) = true iff a has higher priority than b
    int used      =  0;
    LN* front     =  new LN();
    int mod_count =  0;                  //For sensing concurrent modification
    void delete_list(LN*& front);        //Recycle storage, set front's argument to nullptr;
  };

/**
 * delete_list. Given a starting node position, function will
 * delete a node and all subsequent nodes. Given the structure
 * of the code, this function can also delete pieces of a linked list,
 * say for example half
 */
template<class T>
void LinkedPriorityQueue<T>::delete_list(LN*& front) {
  while (front != nullptr) {
    LN *temp = front;
    front = front->next;
    delete temp;
  }
}

//See code in array_priority_queue.hpp and linked_queue.hpp

/**
 * Default constuctor. Not much to do here
 */
template<class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(bool (*agt)(const T& a, const T& b)) : PriorityQueue<T>(agt) {}

/**
 * Copy constuctor. Given a linked priority queue, enqueue all the elements
 * in the argument queue to make the current queue equal. See equals
 * for definition of equality
 *
 * @param const LinkedPriorityQueue<T>& to_copy
 *  The queue to copy
 */
template <class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(const LinkedPriorityQueue<T>& to_copy) 
  : PriorityQueue<T>(to_copy) {
  enqueue(to_copy.ibegin(), to_copy.iend());
  gt = to_copy.gt;
}

/**
 * Constuctor w/ std::initializer_list. Given a initial list of 
 * elements, enqueue each in order to construct our queue. All elements are
 * added in order and first assigned a priority
 *
 * @param std::initialize_list<T> il
 *  The initial values of the priority queue
 */
template <class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(std::initializer_list<T> il,bool (*agt)(const T& a, const T& b))
  : PriorityQueue<T>(agt) {
  gt = agt;
  for (T val : il)
    enqueue(val);
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
LinkedPriorityQueue<T>::LinkedPriorityQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop,bool (*agt)(const T& a, const T& b))
	: PriorityQueue<T>(agt){
  gt = agt;
  enqueue(start, stop);
}

/**
 * Deconstructor
 */
template <class T>
LinkedPriorityQueue<T>::~LinkedPriorityQueue() { delete_list(front->next); } 

/**
 * Enqueue. Function will add an element the list given a determined priority.
 * This function can have large mutations on the queue, since the
 * two interesting spots in a queue are the front and rear. If the
 * queue is empty, we initialize the queue, if not we grow it.
 *
 * @return int
 *  1, a guarentee that we will insert.
 */
template <class T>
int LinkedPriorityQueue<T>::enqueue (const T& element) {
  // empty
  if (front->next == nullptr) {
    front->next = new LN(element, nullptr);
  }

  else if (gt != nullptr && gt(element, front->next->value)) {
    front->next = new LN(element, front->next);
  }

  // anywhere else.
  else 
    for (LN *node = front->next, *prev = front; node != nullptr; node = node->next, prev = prev->next) {
      if (gt != nullptr && gt(element, node->value)) {
        prev->next = new LN(element, node);
        break;
      }
      else if (node->next == nullptr) {
        node->next = new LN(element);
        break;
      } 
    }
  mod_count++;
  used++;
  return 1;
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
T LinkedPriorityQueue<T>::dequeue() {
  LN *to_return = front->next;
  T val = to_return->value;
  front->next = front->next->next;
  delete to_return;

  --used;
  mod_count++;
  return val;
}

/**
 * Empty. Function will determine if the queue is empty.
 * Note: An alternative way to do this would be to say front->next==nullptr
 * which is enforced later on in the code since this is a header linked list but
 * for simplicity, we do this check.
 */
template <class T>
bool LinkedPriorityQueue<T>::empty() const {
  return used == 0;
}

/**
 * Size. Function will return the size of the queue. 
 */
template <class T>
int LinkedPriorityQueue<T>::size () const {
  return used;
}

/**
 * Peek. Function will return the first value in the queue
 * WITHOUT dequeueing the value. Useful for consumer code.
 */
template <class T>
T& LinkedPriorityQueue<T>::peek () const {
  if (front->next != nullptr) return front->next->value;
  throw EmptyError("LinkedPriorityQueue::peek empty queue error");
}

/**
 * Str. A debugging print method.
 */
template <class T>
std::string LinkedPriorityQueue<T>::str() const {
  int i = 0;
  std::stringstream string_value(""), temp("");
  for (T val : *this) {
    if (i == 0) string_value << val;
    else {
      temp.str(string_value.str());
      string_value.str(std::string());
      string_value << val << "," << temp.str();
    }
    i++;
  }
  string_value << "(used = " << used << ", mod_count = " << ")";
  return string_value.str();
}

/**
 * Clear. Function will clear the entire queue. See delete_list for
 * details
 */
template <class T>
void LinkedPriorityQueue<T>::clear() {
  if (used > 0) delete_list(front->next); 
  used = 0;
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
int LinkedPriorityQueue<T>::enqueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  int total = 0;
  for (; start != stop; start++)
    total += enqueue(*start);
  return 0;
}

/**
 * Assignment. Function will become equal to a argument priority queue.
 * See equality operator for precise definition. In the event
 * that the queue has already been used previously, the queue
 * will be cleared.
 */
template <class T>
LinkedPriorityQueue<T>& LinkedPriorityQueue<T>::operator = (const LinkedPriorityQueue<T>& rhs) {
  if (!empty()) clear();
  enqueue(rhs.ibegin(), rhs.iend());
  gt = rhs.gt;
}

/**
 * Equality. Function will determine if two priority queues are equal. Two
 * priority queues are equal if and only if the sizes are equal, and if the
 * two queues are in the SAME order and have the same priority functions.
 *
 * @param const PriorityQueue<T>&
 *  The argument priority queue. Notice the type is not a linked priority queue, that
 *  is becase we use iterators to check element and order equality.
 *  This allows for cross compabilities between data structures
 *  as long as they are the same data type.
 */
template <class T>
bool LinkedPriorityQueue<T>::operator == (const PriorityQueue<T>& rhs) const {
  // are the sizes the same?
  if (used != rhs.size()) return false;

  // do the priority functions equal?
  if (gt != rhs.gt) return false;

  // element equals
  for (auto &lhs_iter = ibegin(), &rhs_iter = rhs.ibegin(); lhs_iter != iend(); lhs_iter++, rhs_iter++) {
    if (*lhs_iter != *rhs_iter) return false;
  }
  return true;
}

/**
 * Non-Equality. Function will determine if two priority queues are NOT equal. See
 * equality for definition.
 *
 * @param const PriorityQueue<T>&
 *  The argument priority queue. Notice the type is not a linked priority queue, that
 *  is becase we use iterators to check element and order equality.
 *  This allows for cross compabilities between data structures
 *  as long as they are the same data type.
 */
template <class T>
bool LinkedPriorityQueue<T>::operator != (const PriorityQueue<T>& rhs) const {
  return !((*this) == rhs);
}

/**
 * Out stream. Function will print out the queue.
 */
template<class T2>
std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T2>& s) {
  int i = 0;
  std::stringstream string_value(""), temp("");
  for (T2 val : s) {
    if (i == 0) string_value << val;
    else {
      temp.str(string_value.str());
      string_value.str(std::string());
      string_value << val << "," << temp.str();
    }
    i++;
  }
  outs << "priority_queue[" << string_value.str() << "]:highest";
  return outs;
}

//Write the constructors, methods, and operators here for LinkedPriorityQueue
//Fill in the missing parts of the erase method and ++ operators
//  for LinkedPriorityQueue's Iterator


//KLUDGE: memory-leak
template<class T>
auto LinkedPriorityQueue<T>::ibegin () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinkedPriorityQueue<T>*>(this),front->next));
}

//KLUDGE: memory-leak
template<class T>
auto LinkedPriorityQueue<T>::iend () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinkedPriorityQueue<T>*>(this),nullptr));
}

template<class T>
auto LinkedPriorityQueue<T>::begin () const -> LinkedPriorityQueue<T>::Iterator {
  return Iterator(const_cast<LinkedPriorityQueue<T>*>(this),front->next);
}

template<class T>
auto LinkedPriorityQueue<T>::end () const -> LinkedPriorityQueue<T>::Iterator {
  return Iterator(const_cast<LinkedPriorityQueue<T>*>(this),nullptr);
}



template<class T>
LinkedPriorityQueue<T>::Iterator::Iterator(LinkedPriorityQueue<T>* fof, LN* initial) : current(initial), ref_pq(fof) {
  prev = ref_pq->front;
  expected_mod_count = ref_pq->mod_count;
}

template<class T>
LinkedPriorityQueue<T>::Iterator::~Iterator() {}

/**
 * Erase. Function will erase the current iterator position. After an
 * erase call, we will return the value of deleted node. Erase will mutate
 * the under-line reference priority queue. 
 *
 * @throw ConcurrentModificationError given conflicting modification counts
 *        CannotEraseError given either false permission or the current position is out of bounds
 */
template<class T>
T LinkedPriorityQueue<T>::Iterator::erase() {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor already erased");
  if (current == nullptr)
    throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor beyond data structure");

  T val = current->value;
  prev->next = current->next;
  delete current;
  current = prev->next;
  can_erase = false;
  ref_pq->used--;
  expected_mod_count = --ref_pq->mod_count;
  return val;
}

/**
 * str. Just a debug string function.
 */
template<class T>
std::string LinkedPriorityQueue<T>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_pq->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
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
const ics::Iterator<T>& LinkedPriorityQueue<T>::Iterator::operator ++ () {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ++");

  if (!can_erase) can_erase = true;
  else {
    prev = current;
    if (current != nullptr) 
      current = current->next;
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
const ics::Iterator<T>& LinkedPriorityQueue<T>::Iterator::operator ++ (int) {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ++(int)");

  if (current == nullptr)
    return *this;

  if (!can_erase) can_erase = true;
  else {
    prev = current;
    current = current->next;
  }
  return *(new Iterator(ref_pq, prev));
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
bool LinkedPriorityQueue<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator ==");
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ==");
  if (ref_pq != rhsASI->ref_pq)
    throw ComparingDifferentIteratorsError("LinkedPriorityQueue::Iterator::operator ==");

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
bool LinkedPriorityQueue<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator !=");
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator !=");
  if (ref_pq != rhsASI->ref_pq)
    throw ComparingDifferentIteratorsError("LinkedPriorityQueue::Iterator::operator !=");

  return current != rhsASI->current;
}

template<class T>
T& LinkedPriorityQueue<T>::Iterator::operator *() const {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator *");
  if (!can_erase || current == nullptr) {
    std::ostringstream where;
    where << current
          << " when front = " << ref_pq->front;
    throw IteratorPositionIllegal("LinkedPriorityQueue::Iterator::operator * Iterator illegal: "+where.str());
  }

  return current->value;
}

template<class T>
T* LinkedPriorityQueue<T>::Iterator::operator ->() const {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator *");
  if (!can_erase || current == nullptr) {
    std::ostringstream where;
    where << current
          << " when front = " << ref_pq->front;
    throw IteratorPositionIllegal("LinkedPriorityQueue::Iterator::operator * Iterator illegal: "+where.str());
  }

  return &(current->value);
}

}

#endif /* LINKED_PRIORITY_QUEUE_HPP_ */
