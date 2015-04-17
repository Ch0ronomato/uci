#ifndef HASH_MAP_HPP_
#define HASH_MAP_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "pair.hpp"
#include "map.hpp"
#include "array_queue.hpp"   //For traversal


namespace ics {

template<class KEY,class T> class HashMap : public Map<KEY,T> {
  public:
    typedef ics::pair<KEY,T> Entry;
    HashMap() = delete;
    HashMap(int (*ahash)(const KEY& k), double the_load_factor = 1.0);
    HashMap(int initial_bins, int (*ahash)(const KEY& k), double the_load_factor = 1.0);
    HashMap(const HashMap<KEY,T>& to_copy);
    HashMap(std::initializer_list<Entry> il, int (*ahash)(const KEY& k), double the_load_factor = 1.0);
    HashMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop, int (*ahash)(const KEY& k), double the_load_factor = 1.0);
    virtual ~HashMap();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual bool has_key    (const KEY& key) const;
    virtual bool has_value  (const T& value) const;
    virtual std::string str () const;

    virtual T    put   (const KEY& key, const T& value);
    virtual T    erase (const KEY& key);
    virtual void clear ();

    virtual int put   (ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop);

    virtual T&       operator [] (const KEY&);
    virtual const T& operator [] (const KEY&) const;
    virtual HashMap<KEY,T>& operator = (const HashMap<KEY,T>& rhs);
    virtual bool operator == (const Map<KEY,T>& rhs) const;
    virtual bool operator != (const Map<KEY,T>& rhs) const;

    template<class KEY2,class T2>
    friend std::ostream& operator << (std::ostream& outs, const HashMap<KEY2,T2>& m);

    virtual ics::Iterator<Entry>& ibegin () const;
    virtual ics::Iterator<Entry>& iend   () const;

   private:
     class LN;

   public:
     class Iterator : public ics::Iterator<Entry> {
       public:
        //KLUDGE should be callable only in begin/end
        Iterator(HashMap<KEY,T>* iterate_over, bool begin);
        Iterator(const Iterator& i);
       virtual ~Iterator();
        virtual Entry       erase();
        virtual std::string str  () const;
        virtual const ics::Iterator<Entry>& operator ++ ();
        virtual const ics::Iterator<Entry>& operator ++ (int);
        virtual bool operator == (const ics::Iterator<Entry>& rhs) const;
        virtual bool operator != (const ics::Iterator<Entry>& rhs) const;
        virtual Entry& operator *  () const;
        virtual Entry* operator -> () const;
      private:
        ics::pair<int,LN*> current; //Bin Index/Cursor; stop: LN* == nullptr
        HashMap<KEY,T>*    ref_map;
        int                expected_mod_count;
        bool               can_erase = true;
        void advance_cursors();
    };

    virtual Iterator begin () const;
    virtual Iterator end   () const;
    //KLUDGE: define
    //virtual ics::Iterator<KEY>&  begin_key   () const;
    //virtual ics::Iterator<KEY>&  end_key     () const;
    //virtual ics::Iterator<T>&    begin_value () const;
    //virtual ics::Iterator<T>&    end_value   () const;

    private:
      class LN {
        public:
          LN ()                         : next(nullptr){}
          LN (const LN& ln)             : value(ln.value), next(ln.next){}
          LN (Entry v, LN* n = nullptr) : value(v), next(n){}

          Entry value;
          LN*   next;
      };

      LN** map      = nullptr;
      int (*hash)(const KEY& k);
      double load_factor;//used/bins <= load_factor
      int bins      = 1; //# bins available in the array
      int used      = 0; //# of key->value pairs in the hash table
      int mod_count = 0; //For sensing concurrent modification
      int   hash_compress (const KEY& key) const;
      void  ensure_load_factor(int new_used);
      LN*   find_key (int bin, const KEY& key) const;
      bool  find_value (const T& value) const;
      LN*   copy_list(LN*   l) const;
      LN**  copy_hash_table(LN** ht, int bins) const;
      void  delete_hash_table(LN**& ht, int bins);
  };





template<class KEY,class T>
HashMap<KEY,T>::HashMap(int (*ahash)(const KEY& k), double the_load_factor) : hash(ahash), load_factor(the_load_factor) {
  //write code here
  hash = ahash;
  std::hash<std::string> hasher;
  std::cout << hasher("a") << std::endl;
}

template<class KEY,class T>
HashMap<KEY,T>::HashMap(int initial_bins, int (*ahash)(const KEY& k), double the_load_factor) : bins(initial_bins), hash(ahash), load_factor(the_load_factor) {
  //write code here
}

template<class KEY,class T>
HashMap<KEY,T>::HashMap(const HashMap<KEY,T>& to_copy) : hash(to_copy.hash), load_factor(to_copy.load_factor), bins(to_copy.bins), used(to_copy.used) {
  //write code here
}

template<class KEY,class T>
HashMap<KEY,T>::HashMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop, int (*ahash)(const KEY& k), double the_load_factor) : hash(ahash), load_factor(the_load_factor) {
  //write code here
}

template<class KEY,class T>
HashMap<KEY,T>::HashMap(std::initializer_list<Entry> il,int (*ahash)(const KEY& k), double the_load_factor) : hash(ahash), load_factor(the_load_factor) {
  //write code here
}

template<class KEY,class T>
HashMap<KEY,T>::~HashMap() {
  //write code here
}


template<class KEY,class T>
inline bool HashMap<KEY,T>::empty() const {
  //write code here
  return used == 0;
}

template<class KEY,class T>
int HashMap<KEY,T>::size() const {
  //write code here
  return used;
}

template<class KEY,class T>
bool HashMap<KEY,T>::has_key (const KEY& key) const {
  //write code here
  int khash = hash(key);
  std::cout << khash << std::endl;
  bool found = false;
  for (LN *bucket = map[khash]; bucket != nullptr; bucket = bucket->next) {
    std::cout << "Hey" << std::endl;
    if (bucket->value.first == key) { found = true; break; }
  }

  return found;
}

template<class KEY,class T>
bool HashMap<KEY,T>::has_value (const T& value) const {
  //write code here
}

template<class KEY,class T>
std::string HashMap<KEY,T>::str() const {
  //write code here
}

template<class KEY,class T>
T HashMap<KEY,T>::put(const KEY& key, const T& value) {
  //write code here
}

template<class KEY,class T>
T HashMap<KEY,T>::erase(const KEY& key) {
  //write code here
}

template<class KEY,class T>
void HashMap<KEY,T>::clear() {
  //write code here
}

template<class KEY,class T>
int HashMap<KEY,T>::put (ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop) {
  //write code here
}

template<class KEY,class T>
T& HashMap<KEY,T>::operator [] (const KEY& key) {
  //write code here
}

template<class KEY,class T>
const T& HashMap<KEY,T>::operator [] (const KEY& key) const {
  //write code here
}

template<class KEY,class T>
bool HashMap<KEY,T>::operator == (const Map<KEY,T>& rhs) const {
  //write code here
}

template<class KEY,class T>
HashMap<KEY,T>& HashMap<KEY,T>::operator = (const HashMap<KEY,T>& rhs) {
  //write code here
}

template<class KEY,class T>
bool HashMap<KEY,T>::operator != (const Map<KEY,T>& rhs) const {
  //write code here
}


template<class KEY,class T>
std::ostream& operator << (std::ostream& outs, const HashMap<KEY,T>& m) {
  //write code here
}

//KLUDGE: memory-leak
template<class KEY,class T>
auto HashMap<KEY,T>::ibegin () const -> ics::Iterator<Entry>& {
  return *(new Iterator(const_cast<HashMap<KEY,T>*>(this),true));
}

//KLUDGE: memory-leak
template<class KEY,class T>
auto HashMap<KEY,T>::iend () const -> ics::Iterator<Entry>& {
  return *(new Iterator(const_cast<HashMap<KEY,T>*>(this),false));
}

template<class KEY,class T>
auto HashMap<KEY,T>::begin () const -> HashMap<KEY,T>::Iterator {
  return Iterator(const_cast<HashMap<KEY,T>*>(this),true);
}

template<class KEY,class T>
auto HashMap<KEY,T>::end () const -> HashMap<KEY,T>::Iterator {
  return Iterator(const_cast<HashMap<KEY,T>*>(this),false);
}

template<class KEY,class T>
int HashMap<KEY,T>::hash_compress (const KEY& key) const {
  //write code here
}

template<class KEY,class T>
void HashMap<KEY,T>::ensure_load_factor(int new_used) {
  //write code here
}

template<class KEY,class T>
typename HashMap<KEY,T>::LN* HashMap<KEY,T>::find_key (int bin, const KEY& key) const {
  //write code here
}

template<class KEY,class T>
bool HashMap<KEY,T>::find_value (const T& value) const {
  //write code here
}

template<class KEY,class T>
typename HashMap<KEY,T>::LN* HashMap<KEY,T>::copy_list (LN* l) const {
  //write code here
}

template<class KEY,class T>
typename HashMap<KEY,T>::LN** HashMap<KEY,T>::copy_hash_table (LN** ht, int bins) const {
  //write code here
}

template<class KEY,class T>
void HashMap<KEY,T>::delete_hash_table (LN**& ht, int bins) {
  //write code here
}


template<class KEY,class T>
void HashMap<KEY,T>::Iterator::advance_cursors(){
  //write code here
}

template<class KEY,class T>
HashMap<KEY,T>::Iterator::Iterator(HashMap<KEY,T>* iterate_over, bool begin) : ref_map(iterate_over) {
  //write code here
}

template<class KEY,class T>
HashMap<KEY,T>::Iterator::Iterator(const Iterator& i) :
    current(i.current), ref_map(i.ref_map), expected_mod_count(i.expected_mod_count), can_erase(i.can_erase) {}

template<class KEY,class T>
HashMap<KEY,T>::Iterator::~Iterator()
{}

template<class KEY,class T>
auto HashMap<KEY,T>::Iterator::erase() -> Entry {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("HashMap::Iterator::erase Iterator cursor already erased");
  if (current.second == nullptr)
    throw CannotEraseError("HashMap::Iterator::erase Iterator cursor beyond data structure");

  //write code here
}

template<class KEY,class T>
std::string HashMap<KEY,T>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_map->str() << "(current=" << current.first << "/" << current.second << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}

//KLUDGE: cannot use Entry
template<class KEY,class T>
auto  HashMap<KEY,T>::Iterator::operator ++ () -> const ics::Iterator<ics::pair<KEY,T>>& {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator ++");

  //write code here
}

//KLUDGE: creates garbage! (can return local value!)
template<class KEY,class T>
auto HashMap<KEY,T>::Iterator::operator ++ (int) -> const ics::Iterator<ics::pair<KEY,T>>&{
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator ++(int)");

  //write code here
}

template<class KEY,class T>
bool HashMap<KEY,T>::Iterator::operator == (const ics::Iterator<Entry>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashMap::Iterator::operator ==");
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator ==");
  if (ref_map != rhsASI->ref_map)
    throw ComparingDifferentIteratorsError("HashMap::Iterator::operator ==");

  //write code here
}


template<class KEY,class T>
bool HashMap<KEY,T>::Iterator::operator != (const ics::Iterator<Entry>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashMap::Iterator::operator !=");
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator !=");
  if (ref_map != rhsASI->ref_map)
    throw ComparingDifferentIteratorsError("HashMap::Iterator::operator !=");

  //write code here
}

template<class KEY,class T>
ics::pair<KEY,T>& HashMap<KEY,T>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashMap::Iterator::operator * Iterator illegal: exhausted");

  //write code here
}

template<class KEY,class T>
ics::pair<KEY,T>* HashMap<KEY,T>::Iterator::operator ->() const {
  if (expected_mod_count !=
      ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashMap::Iterator::operator -> Iterator illegal: exhausted");

  //write code here
}

}

#endif /* HASH_MAP_HPP_ */
