/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

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

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

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

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const { return false; }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const { return false; }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) { return false; }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) { return true; }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) { return false; }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
