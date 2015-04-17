#ifndef BST_MAP_HPP_
#define BST_MAP_HPP_

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

template<class KEY,class T> class BSTMap : public Map<KEY,T>	{
 public:
   	typedef ics::pair<KEY,T> Entry;
	BSTMap();
	BSTMap(const BSTMap<KEY,T>& to_copy);
	BSTMap(std::initializer_list<Entry> il);
   	BSTMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop);
	virtual ~BSTMap();

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
	virtual BSTMap<KEY,T>& operator = (const BSTMap<KEY,T>& rhs);
	virtual bool operator == (const Map<KEY,T>& rhs) const;
	virtual bool operator != (const Map<KEY,T>& rhs) const;

	template<class KEY2,class T2>
	friend std::ostream& operator << (std::ostream& outs, const BSTMap<KEY2,T2>& m);

	virtual ics::Iterator<Entry>& ibegin () const;
	virtual ics::Iterator<Entry>& iend   () const;

  private:
    class TN;

  public:
    class Iterator : public ics::Iterator<Entry> {
      public:
       //KLUDGE should be callable only in begin/end
       Iterator(BSTMap<KEY,T>* iterate_over, bool begin);
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
       ics::ArrayQueue<Entry> it;          //Iterator (as a Queue) for Map
       BSTMap<KEY,T>*         ref_map;
       int                    expected_mod_count;
       bool                   can_erase = true;
   };

   virtual Iterator begin () const;
   virtual Iterator end   () const;
   //KLUDGE: define
   //virtual ics::Iterator<KEY>&  begin_key   () const;
   //virtual ics::Iterator<KEY>&  end_key     () const;
   //virtual ics::Iterator<T>&    begin_value () const;
   //virtual ics::Iterator<T>&    end_value   () const;

   private:
	class TN {
	public:
		TN ()                     : left(nullptr), right(nullptr){}
		TN (const TN& tn)         : value(tn.value), left(tn.left), right(tn.right){}
		TN (Entry v, TN* l = nullptr,
		          TN* r = nullptr) : value(v), left(l), right(r){}

		Entry value;
		TN*   left;
		TN*   right;
	};

	TN* map       = nullptr;
	int used      = 0; //Amount of array used
	int mod_count = 0; //For sensing concurrent modification
	TN*  			 find_key      (TN*  root, const KEY& key) const;
	bool 			 find_value    (TN*  root, const T& value) const;
	T&    			 insert        (TN*& root, const KEY& key, const T& value);
	ics::pair<KEY,T> remove_closest(TN*& root);
	T    			 remove        (TN*& root, const KEY& key);
	TN*  			 copy          (TN*  root)                 const;
	void 			 copy_to_queue (TN* root, ArrayQueue<Entry>& q) const;
	void 			 delete_BST    (TN*& root);
	bool 			 equals        (TN*  root, const Map<KEY,T>& other) const;
	std::string 	 string_rotated(TN* root, std::string indent) const;
};


template<class KEY,class T>
BSTMap<KEY,T>::BSTMap() {}


template<class KEY,class T>
BSTMap<KEY,T>::BSTMap(const BSTMap<KEY,T>& to_copy) : used(to_copy.used) {
	map = copy(to_copy.map);	
}


template<class KEY,class T>
BSTMap<KEY,T>::BSTMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop) {
	put(start,stop);
}


template<class KEY,class T>
BSTMap<KEY,T>::BSTMap(std::initializer_list<Entry> il) {
 	for (Entry entry : il)
 		put(entry.first, entry.second);
}


template<class KEY,class T>
BSTMap<KEY,T>::~BSTMap() {
	// delete_BST(map);
}


template<class KEY,class T>
inline bool BSTMap<KEY,T>::empty() const {
	return used == 0;
}


template<class KEY,class T>
int BSTMap<KEY,T>::size() const {
	return used;
}


template<class KEY,class T>
bool BSTMap<KEY,T>::has_key (const KEY& element) const {
  return find_key(map, element) != nullptr;
}


template<class KEY,class T>
bool BSTMap<KEY,T>::has_value (const T& element) const {
  ics::ArrayQueue<Entry> queue;
  copy_to_queue(map, queue);
  for (Entry entry : queue) {
    if (entry.second == element)
      return true;
  }
  return false;
}


template<class KEY,class T>
std::string BSTMap<KEY,T>::str() const {
	return string_rotated(map, "");
}


template<class KEY,class T>
T BSTMap<KEY,T>::put(const KEY& key, const T& value) {
   used += has_key(key)? 0 : 1;
  mod_count++;
  return insert(map, key, value);
}

template<class KEY,class T>
T BSTMap<KEY,T>::erase(const KEY& key) {
	// erase will already throw an error
	// so we'll let remove do that.
	if (find_key(map, key) != nullptr) {
		mod_count--;
		used--;
		return remove(map, key);
	} else
		throw KeyError("BSTMap::remove no node");
}


template<class KEY,class T>
void BSTMap<KEY,T>::clear() {
	delete_BST(map);
}


template<class KEY,class T>
int BSTMap<KEY,T>::put (ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop) {
	int count;
	for (count = 0;start != stop;count++,start++) 
		put((*start).first, (*start).second);
	return count;
}

// we really don't want to return nullptr here.
template<class KEY,class T>
T& BSTMap<KEY,T>::operator [] (const KEY& key) { // for calls like map[key] = some value
  //write code here
  if (find_key(map, key) == nullptr) 
    put(key, T());
  return find_key(map, key)->value.second;
}


template<class KEY,class T>
const T& BSTMap<KEY,T>::operator [] (const KEY& key) const { // for calls like some value = map[key]
  //write code here
  return find_key(map, key)->value.second;
}

template<class KEY,class T>
bool BSTMap<KEY,T>::operator == (const Map<KEY,T>& rhs) const {
	return equals(map, rhs);
}


template<class KEY,class T>
BSTMap<KEY,T>& BSTMap<KEY,T>::operator = (const BSTMap<KEY,T>& rhs) {
	if (!empty()) clear();
	put(rhs.ibegin(), rhs.iend());
	return (*this);
}


template<class KEY,class T>
bool BSTMap<KEY,T>::operator != (const Map<KEY,T>& rhs) const {
	return !((*this) == rhs);
}


template<class KEY,class T>
std::ostream& operator << (std::ostream& outs, const BSTMap<KEY,T>& m) {
	outs << "map[";
	if (!m.empty()) 
		outs << m.str();
	outs << "]";
	return outs;
}


//KLUDGE: memory-leak
template<class KEY,class T>
auto BSTMap<KEY,T>::ibegin () const -> ics::Iterator<Entry>& {
 return *(new Iterator(const_cast<BSTMap<KEY,T>*>(this),true));
}


//KLUDGE: memory-leak
template<class KEY,class T>
auto BSTMap<KEY,T>::iend () const -> ics::Iterator<Entry>& {
 return *(new Iterator(const_cast<BSTMap<KEY,T>*>(this),false));
}


template<class KEY,class T>
auto BSTMap<KEY,T>::begin () const -> BSTMap<KEY,T>::Iterator {
 return Iterator(const_cast<BSTMap<KEY,T>*>(this),true);
}


template<class KEY,class T>
auto BSTMap<KEY,T>::end () const -> BSTMap<KEY,T>::Iterator {
 return Iterator(const_cast<BSTMap<KEY,T>*>(this),false);
}


template<class KEY,class T>
typename BSTMap<KEY,T>::TN* BSTMap<KEY,T>::find_key (TN* root, const KEY& key) const {
  //write code here
  for (;root != nullptr && root->value.first != key; 
    root = (root->value.first > key ? root->left : root->right)); 
  return root;
}


template<class KEY,class T>
bool BSTMap<KEY,T>::find_value (TN* root, const T& value) const {
	ArrayQueue<Entry> q;
	copy_to_queue(root, q);
	while (!q.empty() && q.dequeue().second != value)
		; 
	return !q.empty();
}


template<class KEY,class T>
T& BSTMap<KEY,T>::insert (TN*& root, const KEY& key, const T& value) {
  if (root == nullptr) {
    root = new TN(Entry(key, value));    
    return root->value.second;
  }
  if (root->value.first == key) {
    T temp = root->value.second;
    root->value.second = value;
    return temp;
  }
  if (key < root->value.first)
      return insert(root->left, key, value);  
   return insert(root->right, key, value);
}


template<class KEY,class T>
ics::pair<KEY,T> BSTMap<KEY,T>::remove_closest(TN*& root) {
 if (root->right == nullptr) {
   ics::pair<KEY,T> to_return = root->value;
   TN* to_delete = root;
   root = root->left;
   delete to_delete;
   return to_return;
 }else
   return remove_closest(root->right);
}


template<class KEY,class T>
T BSTMap<KEY,T>::remove (TN*& root, const KEY& key) {
 if (root == nullptr) {
   std::ostringstream answer;
   answer << "BSTMap::erase: key(" << key << ") not in Map";
   throw KeyError(answer.str());
 }else
   if (key == root->value.first) {
     T to_return = root->value.second;
     if (root->left == nullptr) {
       TN* to_delete = root;
       root = root->right;
       delete to_delete;
     }else if (root->right == nullptr) {
       TN* to_delete = root;
       root = root->left;
       delete to_delete;
     }else
       root->value = remove_closest(root->left);
     return to_return;
   }else
     return remove( (key < root->value.first ? root->left : root->right), key);
}


template<class KEY,class T>
typename BSTMap<KEY,T>::TN* BSTMap<KEY,T>::copy (TN* root) const {
	//write code here
	if (root == nullptr) 
		return nullptr;
	else {
		TN *to_return(root);
		to_return->left = copy(root->left);
		to_return->right = copy(root->right);
		return to_return;
	}
}


template<class KEY,class T>
void BSTMap<KEY,T>::copy_to_queue (TN* root, ArrayQueue<Entry>& q) const {
  if (root == nullptr) return;
  q.enqueue(root->value);
  copy_to_queue(root->left, q);
  copy_to_queue(root->right, q);
}


template<class KEY,class T>
void BSTMap<KEY,T>::delete_BST (TN*& root) {
	while (!empty()) {
		erase(map->value.first);
	}
}

/**
 * 
 * Equality of two maps (NOT BST's) depends on existence of keys
 * and values. Two maps are equal if they contain the same keys and 
 * values. This is structure agnostic. 
 */
template<class KEY,class T>
bool BSTMap<KEY,T>::equals (TN*  root, const Map<KEY,T>& other) const {
	if (size() != other.size())
		return false;
	for (auto &iter = other.ibegin(); iter != other.iend(); iter++) {
		TN *node = find_key(map, iter->first);
		if (node == nullptr || node->value.second != iter->second) 
			return false;
	}
	return true;
}


template<class KEY,class T>
std::string BSTMap<KEY,T>::string_rotated(TN* root, std::string indent) const {
	if (root == nullptr)
    return "";
  else {
  	std::stringstream outs;
    outs << string_rotated(root->right, indent+"..");
    outs << indent << root->value.first << "->" << root->value.second;
    outs << string_rotated(root->left, indent+"..");
    return outs.str();
  }
}


template<class KEY,class T>
BSTMap<KEY,T>::Iterator::Iterator(BSTMap<KEY,T>* iterate_over, bool begin) : it(), ref_map(iterate_over)
	, expected_mod_count(iterate_over->mod_count) {
	if (begin) 
		iterate_over->copy_to_queue(iterate_over->map, it);
}


template<class KEY,class T>
BSTMap<KEY,T>::Iterator::Iterator(const Iterator& i) :
   it(i.it), ref_map(i.ref_map), expected_mod_count(i.expected_mod_count), can_erase(i.can_erase)
{}


template<class KEY,class T>
BSTMap<KEY,T>::Iterator::~Iterator() {
	// delete ref_map;
}


template<class KEY,class T>
auto BSTMap<KEY,T>::Iterator::erase() -> Entry {
 if (expected_mod_count != ref_map->mod_count)
   throw ConcurrentModificationError("BSTMap::Iterator::erase");
 if (!can_erase)
   throw CannotEraseError("BSTMap::Iterator::erase Iterator cursor already erased");
 if (it.empty())
   throw CannotEraseError("BSTMap::Iterator::erase Iterator cursor beyond data structure");

	// delete the next node.
	can_erase = false;
	Entry to_delete = it.dequeue();
	ref_map->erase(to_delete.first);
	expected_mod_count = ref_map->mod_count;
	return to_delete;
}


template<class KEY,class T>
std::string BSTMap<KEY,T>::Iterator::str() const {
 std::ostringstream answer;
 answer << ref_map->str() << "(expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
 return answer.str();
}


//KLUDGE: cannot use Entry
template<class KEY,class T>
auto  BSTMap<KEY,T>::Iterator::operator ++ () -> const ics::Iterator<ics::pair<KEY,T>>& {
 if (expected_mod_count != ref_map->mod_count)
   throw ConcurrentModificationError("BSTMap::Iterator::operator ++");

  if (!can_erase) can_erase = true;
  else {
    if (!it.empty())
      it.dequeue();
  }

  return *this;
}


//KLUDGE: creates garbage! (can return local value!)
template<class KEY,class T>
auto BSTMap<KEY,T>::Iterator::operator ++ (int) -> const ics::Iterator<ics::pair<KEY,T>>&{
 if (expected_mod_count != ref_map->mod_count)
   throw ConcurrentModificationError("BSTMap::Iterator::operator ++(int)");

	Iterator *copy = new Iterator(*this);
  if (!can_erase) can_erase = true;
  else {
    if (!it.empty())
      it.dequeue();
  }
  return *copy;  
}


template<class KEY,class T>
bool BSTMap<KEY,T>::Iterator::operator == (const ics::Iterator<Entry>& rhs) const {
 const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
 if (rhsASI == 0)
   throw IteratorTypeError("BSTMap::Iterator::operator ==");
 if (expected_mod_count != ref_map->mod_count)
   throw ConcurrentModificationError("BSTMap::Iterator::operator ==");
 if (ref_map != rhsASI->ref_map)
   throw ComparingDifferentIteratorsError("BSTMap::Iterator::operator ==");
	return it == rhsASI->it;
}


template<class KEY,class T>
bool BSTMap<KEY,T>::Iterator::operator != (const ics::Iterator<Entry>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("BSTMap::Iterator::operator !=");
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("BSTMap::Iterator::operator !=");
	if (ref_map != rhsASI->ref_map)
		throw ComparingDifferentIteratorsError("BSTMap::Iterator::operator !=");

	return it != rhsASI->it;
}


template<class KEY,class T>
ics::pair<KEY,T>& BSTMap<KEY,T>::Iterator::operator *() const {
 if (expected_mod_count !=
     ref_map->mod_count)
   throw ConcurrentModificationError("BSTMap::Iterator::operator *");
 if (!can_erase || it.empty())
   throw IteratorPositionIllegal("BSTMap::Iterator::operator * Iterator illegal: exhausted");

	return it.peek();
}


template<class KEY,class T>
ics::pair<KEY,T>* BSTMap<KEY,T>::Iterator::operator ->() const {
 if (expected_mod_count !=
     ref_map->mod_count)
   throw ConcurrentModificationError("BSTMap::Iterator::operator *");
 if (!can_erase || it.empty())
   throw IteratorPositionIllegal("BSTMap::Iterator::operator -> Iterator illegal: exhausted");

 	//write code here
	return &it.peek();
}


}

#endif /* BST_MAP_HPP_ */
