// Ian Schweer (Unique ID: 660942)
// Shaun McThomas (Unique ID: 307523)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming:
//   primarily that both partners worked on all parts together.

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

template<class T> class LinkedSet : public Set<T>   {
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
LinkedSet<T>::LinkedSet(const LinkedSet<T>& to_copy) : Set<T>() {
	insert(to_copy.ibegin(), to_copy.iend());
}

/**
 * Constructor with initializer_list. Initializer lists do not have
 * a uniqueness constraint, so we can't assume anything.
 */
template <class T>
LinkedSet<T>::LinkedSet(std::initializer_list<T> il) : Set<T>() {
	for (T val : il)
		insert(val);
}

/**
 * Constructor given two iterators. Since it's impossible to determine
 * size given two ics::Iterators, we cannot assume a size.
 */
template <class T>
LinkedSet<T>::LinkedSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop)
	: Set<T>() {
	insert(start, stop);
}

/**
 * Deconstructor
 */
template <class T>
LinkedSet<T>::~LinkedSet() {
	delete_list(front);
}

/**
 * Empty. Function will determine if the set is empty.
 * Note: An alternative way to do this would be to say front==nullptr
 * however, that then requires the code further down to enforce that 
 * requirement, where as used is a highly used integer. This is easier.
 */
template <class T>
bool LinkedSet<T>::empty() const {
	return used == 0;
}

/**
 * Size. Function will return the size of the queue. 
 */
template <class T>
int  LinkedSet<T>::size() const {
	return used;
}

/**
 * Contains. Function will determine if an element is in a set. This
 * function will take Linear Time.
 *
 * @param const T& element
 *  The element to check for existence.
 *
 * @return bool
 *  Return if the element is or isn't in the set.
 */
template <class T>
bool LinkedSet<T>::contains(const T& element) const {
	bool to_return = false;
	for (LN *node = front; node != trailer  && !to_return; node = node->next)
		if (node->value == element) 
			to_return = true;
	return to_return;
}

/**
 * Str. A debugging print method.
 */
template <class T>
std::string LinkedSet<T>::str() const {
	std::ostringstream out;
	out << *this << "(length=" << size() << ",mod_count=" << mod_count << ")";
	return out.str();
}

/**
 * Contains. Function will determine if a set contains all elements specified by the
 * iterators. Function will consume the result of the simpiler contains method
 */
template<class T>
bool LinkedSet<T>::contains (ics::Iterator<T>& start, const ics::Iterator<T>& stop) const {
	bool found = true;
	for (; found && start != stop; start++) 
		found = contains(*start);
	return found;
}

/**
 * Insert. Function will insert an element into a set given it doesn't already exist.
 * Since this is trailered, we can just extened the trailered element.
 *
 * @return int
 *  The number of insertions, which is one or zero.
 */
template<class T>
int  LinkedSet<T>::insert (const T& element) {
	int to_return = 0;
	if (!contains(element)) {
		used++;        
		mod_count++;
		to_return++;
		trailer->value = element;
		trailer = trailer->next = new LN();
	}
	return to_return;
}

/**
 * Erase. Function will find and erase a current element. This function 
 * will not do any of the actual deleting, but merely call the erase_at
 * function.
 *
 * @param const T& element
 *  The element to erase.
 *
 * @return int
 *  The number of elements erase. Either one or zero.
 */
template<class T>
int LinkedSet<T>::erase(const T& element) {
	for (LN* temp = front; temp != trailer; temp = temp->next)
		if (temp->value == element)
			return erase_at(temp);
	return 0;
}

/**
 * Clear. Given the set isn't empty, erase all elements (except the trailer).
 */
template<class T>
void LinkedSet<T>::clear() {
	if (!empty()) 
	{
	   delete_list(front);
		front = trailer;
	}
}

/**
 * Insert w/ iterators. Given a start and stop iterator, insert all the elements
 * between the two iterators, given the element does not exist. Function will
 * consume the insert function.
 *
 * @param ics::Iterator<T>& start
 *  The starting iterator
 *
 * @param const ics::Iterator<T>& stop
 *  The ending iterator
 *
 * @return int
 *  The number of inserts.
 */
template <class T>
int LinkedSet<T>::insert (ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	int to_return = 0;
	for (; start != stop; to_return += insert(*start++));    
	return to_return;
}

/**
 * Erase. Function will erase all elements in a set between a start and stop
 * iterator. Function will consume the simplier erase method.
 *
 * @param ics::Iterator<T> start
 *  The starting iterator.
 *
 * @param const ics::Iterator<T> stop
 *  The ending iterator.
 *
 * @return int
 *  The number of removals from the list.
 */
template <class T>
int LinkedSet<T>::erase  (ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	int to_return = 0;
	for(; start != stop; to_return += erase(*start++));
	return to_return;
}

/** 
 * Retain. Function will compute the intersection of two sets. Function will
 * construct a new linked set given two iterators, and then determine the 
 * elements in common.
 *
 * @param ics::Iterator<T>& start
 *  The starting iterator
 *  
 * @param const ics::Iterator<T>& end
 *  The ending iterator.
 *
 * @return int
 *  The number of elements "retained" in the set.
 */
template <class T>
int LinkedSet<T>::retain (ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	LinkedSet<T> s;
	for (; start != stop; start++)
		if (contains(*start))
			s.insert(*start);   

	*this = s;
	return used;
}

/**
 * Assignment. Function will become equal to a argument set.
 * See equality operator for precise definition. In the event
 * that the set has already been used previously, the set
 * will be cleared.
 */
template <class T>
LinkedSet<T>& LinkedSet<T>::operator = (const LinkedSet<T>& rhs) {
	if (this == &rhs)
		return *this;
	clear();
	insert(rhs.ibegin(), rhs.iend());
	++mod_count;
	return *this;
}

/**
 * Equality. Function will determine if two set are equal. Two
 * set are equal if and only if the sizes are equal, and if all
 * the elements in the right hand set are in the current set, but
 * not in the same order
 *
 * @param const Set<T>&
 *  The argument set. Notice the type is not a linked set, that
 *  is becase we use iterators to check element equality.
 *  This allows for cross compabilities between data structures
 *  as long as they are the same data type.
 */
template<class T>
bool LinkedSet<T>::operator == (const Set<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (used != rhs.size())
		return false;
	for (LN *temp = front; temp != trailer; temp = temp->next)
		if (!rhs.contains(temp->value))
			return false;

	return true;
}

/**
 * Non-Equality. Function will determine if two sets are NOT equal. See
 * equality for definition.
 *
 * @param const Set<T>&
 *  The argument queue. Notice the type is not a linked queue, that
 *  is becase we use iterators to check element and order equality.
 *  This allows for cross compabilities between data structures
 *  as long as they are the same data type.
 */
template<class T>
bool LinkedSet<T>::operator != (const Set<T>& rhs) const {
	return !(*this == rhs);
}

template<class T>
bool LinkedSet<T>::operator <= (const Set<T>& rhs) const {
	// if (this == &rhs)
	//     return true;
	// if (used > rhs.size())
	//     return false;
	// for (LN *temp = front; temp != trailer; temp = temp->next)
	//     if (!rhs.contains(temp->value))
	//         return false;

	// return true;
	return (*this == rhs || *this < rhs);
}

template<class T>
bool LinkedSet<T>::operator <  (const Set<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (used >= rhs.size())
		return false;
	for (LN *temp = front; temp != trailer; temp = temp->next)
		if (!rhs.contains(temp->value))
			return false;

	return true;
}

template<class T>
bool LinkedSet<T>::operator >= (const Set<T>& rhs) const {
	return rhs <= *this;
}

template<class T>
bool LinkedSet<T>::operator >  (const Set<T>& rhs) const {
	return rhs < *this;
}

template<class T>
LinkedSet<T>::Iterator::Iterator(LinkedSet<T>* fof, LN* initial) : current(initial), ref_set(fof) {
	expected_mod_count = ref_set->mod_count;
}

template<class T>
LinkedSet<T>::Iterator::~Iterator() {

}

template<class T>
T LinkedSet<T>::Iterator::erase() {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::erase");
	if (!can_erase)
		throw CannotEraseError("LinkedSet::Iterator::erase Iterator cursor already erased");
	if (current == nullptr || current->next == nullptr)
		throw CannotEraseError("LinkedSet::Iterator::erase Iterator cursor beyond data structure");

	can_erase = false;
	T to_return = current->value;
	LN *to_erase = current;
	current = current->next;
	ref_set->erase_at(to_erase);
	expected_mod_count = ref_set->mod_count;
	return to_return;
}

template<class T>
std::string LinkedSet<T>::Iterator::str  () const {
	std::ostringstream answer;
	answer << ref_set->str() << "(current=" << current->value << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
	return answer.str();
}

template<class T>
const ics::Iterator<T>& LinkedSet<T>::Iterator::operator ++ () {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator ++");
	if (current->next == nullptr) {
		return *this;
	}
	if (!can_erase) can_erase = true;
	else {
		current = current->next;
	}
	return *this;
}

template<class T>
const ics::Iterator<T>& LinkedSet<T>::Iterator::operator ++ (int) {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator ++(int)");
	if (current->next == nullptr) {
		return *this;
	}

	Iterator *to_return = new Iterator(this->ref_set, current);
	if (!can_erase) can_erase = true;
	else {
		current = current->next;
	}
	return *to_return;
}

template<class T>
bool LinkedSet<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("ArraySet::Iterator::operator ==");
	if (expected_mod_count != ref_set->mod_count)
		 throw ConcurrentModificationError("ArraySet::Iterator::operator ==");
	if (ref_set != rhsASI->ref_set)
		throw ComparingDifferentIteratorsError("ArraySet::Iterator::operator ==");

	return current == rhsASI->current;
}

template<class T>
bool LinkedSet<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
	return !(*this == rhs);
}

template<class T>
T& LinkedSet<T>::Iterator::operator *  () const {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator *");
	if (!can_erase || current == nullptr) {
		std::ostringstream where;
		where << current << " when size = " << ref_set->size();
		throw IteratorPositionIllegal("LinkedSet::Iterator::operator * Iterator illegal: " + where.str());
	}
	return current->value;
}

template<class T>
T* LinkedSet<T>::Iterator::operator -> () const {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("ArraySet::Iterator::operator ->");
	if (!can_erase || current->next == nullptr || current == nullptr) {
		std::ostringstream where;
		where << current << " when size = " << ref_set->size();
		throw IteratorPositionIllegal("ArraySet::Iterator::operator -> Iterator illegal: "+where.str());
	}

	return &current->value;
}

template<class T2>
std::ostream& operator << (std::ostream& outs, const LinkedSet<T2>& s) {
	outs << "set[";
	if (!s.empty()) {
		int i = 0;
		for (auto &val = s.ibegin(); val != s.iend(); ++val) {
			outs << *val;
			if (++i < s.size()) outs << ",";
		}
	}
	outs << "]";
	return outs;
}

template<class T>
auto LinkedSet<T>::ibegin() const -> ics::Iterator<T>&{
	return *(new Iterator(const_cast<LinkedSet<T>*>(this), front));
}

template<class T>
auto LinkedSet<T>::iend() const -> ics::Iterator<T>&{
	return *(new Iterator(const_cast<LinkedSet<T>*>(this), trailer));
}

template<class T>
auto LinkedSet<T>::begin() const -> LinkedSet<T>::Iterator {
	return Iterator(const_cast<LinkedSet<T>*>(this), front);
}

template<class T>
auto LinkedSet<T>::end() const -> LinkedSet<T>::Iterator {
	return Iterator(const_cast<LinkedSet<T>*>(this), trailer);
}


template<class T>
void LinkedSet<T>::delete_list(LN*& node) {
	for (LN *temp = node; temp != trailer; temp = temp->next) {
		erase_at(temp);
	}
}

template<class T>
int LinkedSet<T>::erase_at(LN* node) {
	LN* temp = node;
	node = node->next;
	if (temp != front) {
		// update back references.
		LN *prev = front;
		for (; prev->next != temp && prev->next != nullptr; prev = prev->next);
		prev->next = node;
	} else {
		front = node;
	}
	delete temp;
	used--;
	mod_count++;
	return 1;
}

}

#endif /* LINKED_SET_HPP_ */

