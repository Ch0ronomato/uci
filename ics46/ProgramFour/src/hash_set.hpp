#ifndef HASH_SET_HPP_
#define HASH_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "pair.hpp"
#include "iterator.hpp"
#include "set.hpp"


namespace ics {

template<class T> class HashSet : public Set<T>	{
  public:
    HashSet() = delete;
    HashSet(int (*ahash)(const T& element), double the_load_factor = 1.0);
    HashSet(int initial_bins, int (*ahash)(const T& element), double the_load_factor = 1.0);
    HashSet(const HashSet<T>& to_copy);
    HashSet(std::initializer_list<T> il, int (*ahash)(const T& element), double the_load_factor = 1.0);
    HashSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop, int (*ahash)(const T& element), double the_load_factor = 1.0);
    virtual ~HashSet();

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

    virtual HashSet<T>& operator = (const HashSet<T>& rhs);
    virtual bool operator == (const Set<T>& rhs) const;
    virtual bool operator != (const Set<T>& rhs) const;
    virtual bool operator <= (const Set<T>& rhs) const;
    virtual bool operator <  (const Set<T>& rhs) const;
    virtual bool operator >= (const Set<T>& rhs) const;
    virtual bool operator >  (const Set<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const HashSet<T2>& s);

    virtual ics::Iterator<T>& ibegin () const;
    virtual ics::Iterator<T>& iend   () const;

  private:
    class LN;

  public:
    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(HashSet<T>* iterate_over, bool begin);
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
        ics::pair<int,LN*> current; //Bin Index/Cursor; stop: LN* == nullptr
        HashSet<T>*        ref_set;
        int                expected_mod_count;
        bool               can_erase = true;
        void advance_cursors();
    };

    virtual Iterator begin () const;
    virtual Iterator end   () const;

  private:
    class LN {
      public:
        LN ()                      : next(nullptr){}
        LN (const LN& ln)          : value(ln.value), next(ln.next){}
        LN (T v,  LN* n = nullptr) : value(v), next(n){}

        T   value;
        LN* next;
    };

    LN** set      = nullptr;
    int (*hash)(const T& element);
    double load_factor;//used/bins <= load_factor
    int bins      = 1; //# bins available in the array
    int used      = 0; //# of key->value pairs in the hash table
    int mod_count = 0; //For sensing concurrent modification
    int   hash_compress (const T& element) const;
    void  ensure_load_factor(int new_used);
    LN*   find_element (int bin, const T& element) const;
    LN*   copy_list(LN*   l) const;
    LN**  copy_hash_table(LN** ht, int bins) const;
    void  delete_hash_table(LN**& ht, int bins);
  };





template<class T>
HashSet<T>::HashSet(int (*ahash)(const T& element), double the_load_factor)
    : hash(ahash), load_factor(the_load_factor) {
  //write code here
}

template<class T>
HashSet<T>::HashSet(int initial_bins, int (*ahash)(const T& element), double the_load_factor)
    : bins(initial_bins), hash(ahash), load_factor(the_load_factor) {
  //write code here
}

template<class T>
HashSet<T>::HashSet(const HashSet<T>& to_copy)
    : hash(to_copy.hash), load_factor(to_copy.load_factor), bins(to_copy.bins), used(to_copy.used) {
  //write code here
}

template<class T>
HashSet<T>::HashSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop, int (*ahash)(const T& element), double the_load_factor)
    : hash(ahash), load_factor(the_load_factor) {
  //write code here
}

template<class T>
HashSet<T>::HashSet(std::initializer_list<T> il,int (*ahash)(const T& k), double the_load_factor)
    : hash(ahash), load_factor(the_load_factor) {
  //write code here
}

template<class T>
HashSet<T>::~HashSet() {
  //write code here
}


template<class T>
inline bool HashSet<T>::empty() const {
  //write code here
}

template<class T>
int HashSet<T>::size() const {
  //write code here
}

template<class T>
bool HashSet<T>::contains (const T& element) const {
  //write code here
}

template<class T>
std::string HashSet<T>::str() const {
  //write code here
}

template<class T>
bool HashSet<T>::contains(ics::Iterator<T>& start, const ics::Iterator<T>& stop) const {
  //write code here
}

template<class T>
int HashSet<T>::insert(const T& element) {
  //write code here
}

template<class T>
int HashSet<T>::erase(const T& element) {
  //write code here
}

template<class T>
void HashSet<T>::clear() {
  //write code here
}

template<class T>
int HashSet<T>::insert(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  //write code here
}

template<class T>
int HashSet<T>::erase(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  //write code here
}

template<class T>
int HashSet<T>::retain(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  //write code here
}

template<class T>
HashSet<T>& HashSet<T>::operator = (const HashSet<T>& rhs) {
  //write code here
}

template<class T>
bool HashSet<T>::operator == (const Set<T>& rhs) const {
  //write code here
}

template<class T>
bool HashSet<T>::operator != (const Set<T>& rhs) const {
  //write code here
}

template<class T>
bool HashSet<T>::operator <= (const Set<T>& rhs) const {
  //write code here
}

template<class T>
bool HashSet<T>::operator < (const Set<T>& rhs) const {
  //write code here
}

template<class T>
bool HashSet<T>::operator >= (const Set<T>& rhs) const {
  //write code here
}

template<class T>
bool HashSet<T>::operator > (const Set<T>& rhs) const {
  //write code here
}

template<class T>
std::ostream& operator << (std::ostream& outs, const HashSet<T>& s) {
  //write code here
}

//KLUDGE: memory-leak
template<class T>
auto HashSet<T>::ibegin () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<HashSet<T>*>(this),true));
}

//KLUDGE: memory-leak
template<class T>
auto HashSet<T>::iend () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<HashSet<T>*>(this),false));
}

template<class T>
auto HashSet<T>::begin () const -> HashSet<T>::Iterator {
  return Iterator(const_cast<HashSet<T>*>(this),true);
}

template<class T>
auto HashSet<T>::end () const -> HashSet<T>::Iterator {
  return Iterator(const_cast<HashSet<T>*>(this),false);
}

template<class T>
int HashSet<T>::hash_compress (const T& element) const {
  //write code here
}

template<class T>
void HashSet<T>::ensure_load_factor(int new_used) {
  //write code here
}

template<class T>
typename HashSet<T>::LN* HashSet<T>::find_element (int bin, const T& element) const {
  for (LN* c = set[bin]; c->next!=nullptr; c=c->next)
    if (element == c->value)
      return c;

  return nullptr;
}

template<class T>
typename HashSet<T>::LN* HashSet<T>::copy_list (LN* l) const {
  if (l == nullptr)
    return nullptr;
  else
    return new LN(l->value, copy_list(l->next));
}

template<class T>
typename HashSet<T>::LN** HashSet<T>::copy_hash_table (LN** ht, int bins) const {
  //write code here
}

template<class T>
void HashSet<T>::delete_hash_table (LN**& ht, int bins) {
  //write code here
}


template<class T>
void HashSet<T>::Iterator::advance_cursors(){
  //write code here
}




template<class T>
HashSet<T>::Iterator::Iterator(HashSet<T>* iterate_over, bool begin) : ref_set(iterate_over) {
  //write code here
}


template<class T>
HashSet<T>::Iterator::Iterator(const Iterator& i) :
    current(i.current), ref_set(i.ref_set), expected_mod_count(i.expected_mod_count), can_erase(i.can_erase) {}

template<class T>
HashSet<T>::Iterator::~Iterator() {}

template<class T>
T HashSet<T>::Iterator::erase() {
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("HashSet::Iterator::erase Iterator cursor already erased");
  if (current.second == nullptr)
    throw CannotEraseError("HashSet::Iterator::erase Iterator cursor beyond data structure");

  //write code here
}

template<class T>
std::string HashSet<T>::Iterator::str() const {
  //write code here
}

template<class T>
const ics::Iterator<T>& HashSet<T>::Iterator::operator ++ () {
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator ++");

  //write code here
}

//KLUDGE: creates garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& HashSet<T>::Iterator::operator ++ (int) {
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator ++(int)");

  //write code here
}

template<class T>
bool HashSet<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashSet::Iterator::operator ==");
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator ==");
  if (ref_set != rhsASI->ref_set)
    throw ComparingDifferentIteratorsError("HashSet::Iterator::operator ==");

  //write code here
}


template<class T>
bool HashSet<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashSet::Iterator::operator !=");
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator !=");
  if (ref_set != rhsASI->ref_set)
    throw ComparingDifferentIteratorsError("HashSet::Iterator::operator !=");

  //write code here
}

template<class T>
T& HashSet<T>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashSet::Iterator::operator * Iterator illegal: exhausted");

  //write code here
}

template<class T>
T* HashSet<T>::Iterator::operator ->() const {
  if (expected_mod_count !=
      ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashSet::Iterator::operator * Iterator illegal: exhausted");

  //write code here
}

}

#endif /* HASH_SET_HPP_ */
