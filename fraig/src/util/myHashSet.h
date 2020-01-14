/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data> 
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(size_t bucketid, const size_t numBuckets, vector<Data> *b) : _bucketid(bucketid), _numBuckets(numBuckets), _index(0), _node(&b[bucketid][0]), _buckets(b) {}
      ~iterator() {}
      const Data& operator * () const { return *(_node); }
      iterator& operator ++ () { 
         if(_buckets[_bucketid].size() >= 2 && _index < _buckets[_bucketid].size() - 1)
         {
            _index++;
            _node = &_buckets[_bucketid][_index];
         }
         else
         {
            _bucketid++;
            while(_bucketid < _numBuckets) 
            {
               if(_buckets[_bucketid].empty()) 
                  _bucketid++; 
               else break;
            }
            _index = 0;
            _node = &_buckets[_bucketid][_index];
         }
         return *(this);
      } 
      iterator operator ++ (int) { iterator it = (*this); (*this)++; return it; }
      iterator& operator -- () { 
         if(_buckets[_bucketid].size() >= 2 && _index > 0)
         {
            _index--;
            _node = &_buckets[_bucketid][_index];
         }
         else
         {
            _bucketid++;
            while (_buckets[_bucketid].empty()) { _bucketid++; }
            _index = _buckets[_bucketid].size() - 1;
            _node = &_buckets[_bucketid][_index];
         }
         return *(this);
      }
      iterator operator -- (int) { iterator it = (*this); (*this)--; return it; }
      iterator& operator = (const iterator& i) { 
         this->_node = i._node;
         this->_numBuckets = i._numBuckets;
         this->_bucketid = i._bucketid;
         this->_index = i._index;
         return (*this); 
      } 
      bool operator == (const iterator& i) const { 
         return (i._node == this->_node && i._bucketid == this->_bucketid && i._index == this->_index); 
      }
      bool operator != (const iterator& i) const {
         return !(i == *this);
      } 
   private:
      Data* _node;
      size_t _numBuckets, _bucketid, _index;
      vector<Data>* _buckets;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      size_t id = 0;
      while( id < _numBuckets )
      {
         if(_buckets[id].size()) {
            return iterator(id, _numBuckets, _buckets);
         }
         id++;
      }
      return end();  
   }
   // Pass the end
   iterator end() const { return iterator(_numBuckets, _numBuckets, _buckets); }
   // return true if no valid data
   bool empty() const { return begin() == end(); }
   // number of valid data
   size_t size() const { 
      size_t s = 0; 
      for(iterator b = begin(), e = end(); b != e; ++b)
         s++;
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
      // size_t id = bucketNum(d);
      size_t id = d->getkey()% _numBuckets;
      if (_buckets[id].empty())
         return false;
      for(size_t index = 0; index != _buckets[id].size(); index++)
         if(_buckets[id][index] == d) return true;
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
      // if(!check(d)) 
         // return false;
      // else {
         // size_t id = bucketNum(d);
         // size_t id = d->getkey();
         size_t id = d->getkey() % _numBuckets;
         for(size_t index = 0; index < _buckets[id].size(); index++) {
            if(_buckets[id][index]->getkey() == d->getkey()) {
               d = _buckets[id][index];
               return true;
            }
         }
         return false;
      // }
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      // size_t id = bucketNum(d);
      size_t id = d->getkey()% _numBuckets;
      if(!check(d)) {
         _buckets[id].push_back(d);
         return false;
      }
      else {
         for (size_t index = 0; index < _buckets[id].size(); index++) {
            if(_buckets[id][index] == d) {
               _buckets[id][index] = d;
               return true;
            }
         }
      }
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   int insert(const Data& d) { 
      // if(check(d)) return false;
      size_t id = d->getkey() % _numBuckets;
      // _buckets[id].push_back(d);
      // return true;
      if(_buckets[id].size()==0) {
         _buckets[id].push_back(d);
         cout << "inserted: " << d->getTypeStr() <<" "<< d->getID() << endl;
         return -1;
      }
      else {
         int i = (int)_buckets[id][0]->getID();
         _buckets[id].pop_back();
         return i;
      }
      
      // _buckets[id].push_back(d);
      // return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      if(!check(d)) 
         return false;
      else {
         size_t id = d->getkey()% _numBuckets;
         for (size_t index = 0; index < _buckets[id].size(); index++) {
            if(_buckets[id][index] == d) {
               _buckets[id][index] = _buckets[id][_buckets[id].size()-1];
               _buckets[id].pop_back();
               return true;
            }
         }
      }
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
