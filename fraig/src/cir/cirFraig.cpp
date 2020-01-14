/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
// #include "myHashMap.h"
#include "myHashSet.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
  HashSet<CirGate*> myhash(M);
  // cout<<endl<<"myhashnum: "<<myhash.numBuckets()<<endl;
  for(int i=0;i<_dfsList.size();i++) {
    CirGate*& g = _Gatelist[_dfsList[i]->_id];
    if(g->_type==AIG_GATE) {
      cout<<g->getTypeStr()<<" "<<g->getID()<<" key:"<<g->getkey()<<endl;
      int p = myhash.insert(g);
      // if(myhash.insert(g)!=-1){
      if( p != -1 ) { //cannot insert
        // CirGate*& p_ = _Gatelist[p];
        // myhash.query(p_);
        strashed = true;
        cout << "Strashing: " << (unsigned)p << " merging " << g->_id << "..." << endl;
				// merge( g->_id, (unsigned)p, false);
        replace(g, _Gatelist[p]);
      } 
    }
    else continue;
  }
  
  // for(size_t b = 0; b < myhash.numBuckets();b++){
  //   cout<<"["<<b<<"]: ";
  //   for (size_t i = 0; i < myhash[b].size(); ++i)
  //     cout<<myhash[b][i]->_id<<" ";
  //   cout<<endl;
  // }
  // cout<<endl<<"---------"<<endl;
  
  // for(int b = 0;b <myhash.numBuckets();b++)
  // {
  //   while(myhash[b].size()>=2){
  //     cout << "Strashing: " << myhash[b][myhash[b].size()-1]->_id << " merging " << myhash[b][myhash[b].size()-2]->_id << "..." << endl;
  //     merge(myhash[b][myhash[b].size()-1]->_id, myhash[b][myhash[b].size()-2]->_id);
  //     myhash[b].pop_back();
  //     myhash[b].pop_back();
  //   }
  // }
  
  if(strashed)
  {
    _aig.clear();
    for (unsigned i = 0; i < _Gatelist.size(); i++)
      if(_Gatelist[i]->_type == AIG_GATE && !_Gatelist[i]->deleted) _aig.push_back(_Gatelist[i]);
    DFS();
  }
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

void
CirMgr::replace(CirGate*& a, CirGate* b, bool inv)
{
	// Detach a's inputs
	for(unsigned i = 0; i < a->_fanin.size(); ++i) {
		for(unsigned j = 0; j < a->_fanin[i]->_fanout.size(); ++j) {
			if(a == a->_fanin[i]->_fanout[j]) {
				a->_fanin[i]->_fanout.erase(a->_fanin[i]->_fanout.begin() + j);
				a->_fanin[i]->_outinvert.erase(a->_fanin[i]->_outinvert.begin() + j);
				break;
			}
		}
	}

	// Attach a's output to b
	if(b) {
		for(unsigned n = 0; n < a->_fanout.size(); ++n) {
			b->_fanout.push_back(a->_fanout[n]);
			b->_outinvert.push_back(inv != a->_outinvert[n]);
		}
	}

	// Reconnect a's output
	for(unsigned i = 0; i < a->_fanout.size(); ++i) {
		for(unsigned j = 0; j < a->_fanout[i]->_fanin.size(); ++j) {
			if(a->_fanout[i]->_fanin[j] == a) {
				if(b) {
					a->_fanout[i]->_fanin[j] = b;
					a->_fanout[i]->_invert[j] = (inv != a->_outinvert[i]);
				}
				else {
					a->_fanout[i]->_fanin.erase(a->_fanout[i]->_fanin.begin() + j);
					a->_fanout[i]->_invert.erase(a->_fanout[i]->_invert.begin() + j);
				}
				break;
			}
		}
	}

	// Delete a, also remove from _idGlist
	a->deleted = true;
  delete a;
	// a = 0;
	// --_header[4];
}