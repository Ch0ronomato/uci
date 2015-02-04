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
template <class T>
LinkedQueue<T>::LinkedQueue() : front(nullptr), rear(front) {
}

template <class T>
LinkedQueue<T>::LinkedQueue(const LinkedQueue<T>& to_copy) {
	enqueue(to_copy.ibegin(), to_copy.iend());
}

template <class T>
LinkedQueue<T>::LinkedQueue(std::initializer_list<T> il) {
	for (T each : il) enqueue(each);
}

template <class T>
LinkedQueue<T>::LinkedQueue(ics::Iterator<T>& start, const ics::Iterator<T>& end) {
	front = rear;
	enqueue(start, end);
}

template <class T>
LinkedQueue<T>::~LinkedQueue() { delete_list(front); }

template <class T>
bool LinkedQueue<T>::empty() const {
	return used == 0;
}

template <class T>
int LinkedQueue<T>::size() const {
	return used;
}

template <class T>
T& LinkedQueue<T>::peek() const {
	if (empty()) throw EmptyError("LinkedQueue::peek the queue is empty");
	return front->value;
}

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

template <class T>
T LinkedQueue<T>::dequeue() {
	// bounds check
	if (front == nullptr)
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

template <class T>
void LinkedQueue<T>::clear() {
  delete_list(front);
  rear = nullptr;
}
template <class T>
void LinkedQueue<T>::delete_list(LN *&node) {
	LN *to_delete = node;
	while (node != nullptr) {
		node = node->next;
		delete to_delete;
		to_delete = node;
    used--;
    mod_count++;
	}
}

template <class T>
LinkedQueue<T>& LinkedQueue<T>::operator = (const LinkedQueue<T>& rhs) {
	if (!empty()) clear();
	enqueue(rhs.ibegin(), rhs.iend());
	return *this;
}

// Two queues are equal iff they contain the same elements in the same order.
template <class T>
bool LinkedQueue<T>::operator == (const Queue<T>& rhs) const {
	if (this == &rhs) return true;
	if (size() != rhs.size()) return false;
	auto &lhs_iter = ibegin(), &rhs_iter = rhs.ibegin();
	for (int i=0; i < used; i++, lhs_iter++, rhs_iter++) {
		if (*lhs_iter != *rhs_iter) return false;
	}
	return true;
}

template <class T>
bool LinkedQueue<T>::operator != (const Queue<T>& rhs) const {
	return !((*this) == rhs);
}

template<class T2>
std::ostream& operator << (std::ostream& outs, const LinkedQueue<T2>& s) {
	outs << "queue[";
	int i = 0;
	for (auto & iter = s.ibegin(); i < s.used; iter++, i++) {
		if (i < s.size() - 1) outs << *iter << ",";
		else outs << *iter;
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

template<class T>
std::string LinkedQueue<T>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_queue->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}

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
