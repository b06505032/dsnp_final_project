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
      /*
      // --------GET THE KEY
      cout<<endl<<"get the key: ";
      cout<<endl<<g->getTypeStr()<<" "<<g->getID()<<" key:"<<g->getkey()<<endl;
      
      // --------PRINT OUT GATE FANIN
      cout<<endl<<"before merging... GATE: ";
      g->printGate();
      if (g->_fanin[0]->_type == UNDEF_GATE) cout << '*';
      if (g->_invert[0]) cout << '!';
      cout<< g->_fanin[0]->_id <<' ';
      if (g->_fanin[1]->_type == UNDEF_GATE) cout << '*';
      if (g->_invert[1]) cout << '!';
      cout<< g->_fanin[1]->_id;
      cout << endl;
      // --------PRINT OUT GATE FANIN
      */
      
      int p = myhash.insert(g);
      // if(myhash.insert(g)!=-1)
      if( p != -1 ) { //cannot insert
        // CirGate*& p_ = _Gatelist[p];
        // myhash.query(p_);
				merge( g->_id, (unsigned)p, false, "str");
        
        /*
        // --------PRINT OUT GATE FANIN
        cout<<endl<<"after merging... GATE: ";
        _Gatelist[p]->printGate();
        if (_Gatelist[p]->_fanin[0]->_type == UNDEF_GATE) cout << '*';
        if (_Gatelist[p]->_invert[0]) cout << '!';
        cout<< _Gatelist[p]->_fanin[0]->_id <<' ';
        if (_Gatelist[p]->_fanin[1]->_type == UNDEF_GATE) cout << '*';
        if (_Gatelist[p]->_invert[1]) cout << '!';
        cout<< _Gatelist[p]->_fanin[1]->_id;
        cout << endl;
        // --------PRINT OUT GATE FANIN
        */
      } 
    }
    else continue;
  }
  
  /*
  for(size_t b = 0; b < myhash.numBuckets();b++){
    cout<<"["<<b<<"]: ";
    for (size_t i = 0; i < myhash[b].size(); ++i)
      cout<<myhash[b][i]->_id<<" ";
    cout<<endl;
  }
  cout<<endl<<"---------"<<endl;
  
  for(int b = 0;b <myhash.numBuckets();b++)
  {
    while(myhash[b].size()>=2){
      cout << "Strashing: " << myhash[b][myhash[b].size()-1]->_id << " merging " << myhash[b][myhash[b].size()-2]->_id << "..." << endl;
      merge(myhash[b][myhash[b].size()-1]->_id, myhash[b][myhash[b].size()-2]->_id);
      myhash[b].pop_back();
      myhash[b].pop_back();
    }
  }
  */
  
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
