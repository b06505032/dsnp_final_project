/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  for(unsigned j = 0; j<_dfsList.size();j++)
    _dfsList[j]->sweep = false;

  for (unsigned aigid=0; aigid<_Gatelist.size();aigid++)
  {
    // cout<<"id: "<<_Gatelist[aigid]->_id<<", type: "<<_Gatelist[aigid]->_type<<" , sweep: "<<_Gatelist[aigid]->sweep<<endl;
    if(_Gatelist[aigid]->sweep == 0) continue; // false
    else
    {
      if(_Gatelist[aigid]->_type == PI_GATE) continue;
      else if(_Gatelist[aigid]->_type == CONST_GATE) continue;
      else
      {
        cout<<"Sweeping: "<<_Gatelist[aigid]->getTypeStr()<<"("<<_Gatelist[aigid]->_id<<") removed..."<<endl;
        if(_Gatelist[aigid]->_type==AIG_GATE) {
          A=A-1; _Gatelist[aigid]->_type = UNDEF_GATE;
        }
        if(!_Gatelist[aigid]->_fanin.empty())
        {
          unsigned fi0, fi1;
          fi0 = _Gatelist[aigid]->_fanin[0]->_id;
          fi1 = _Gatelist[aigid]->_fanin[1]->_id;
          _Gatelist[aigid]->_fanin.pop_back();
          _Gatelist[aigid]->_fanin.pop_back();
          _Gatelist[aigid]->_invert.pop_back();
          _Gatelist[aigid]->_invert.pop_back();
          _Gatelist[fi0]->_fanout.pop_back();
          _Gatelist[fi1]->_fanout.pop_back();
          _Gatelist[fi0]->_outinvert.pop_back();
          _Gatelist[fi1]->_outinvert.pop_back();
        }
        if(!_Gatelist[aigid]->_fanout.empty())
        {
          unsigned fo;
          fo = _Gatelist[aigid]->_fanout[0]->_id;
          _Gatelist[aigid]->_fanout.pop_back();
          _Gatelist[aigid]->_outinvert.pop_back();
          _Gatelist[fo]->_fanin.pop_back();
          _Gatelist[fo]->_invert.pop_back();
        }
      }
    }
  }

  // for (unsigned id=0; id<_Gatelist.size();id++)
    // cout<<endl<<"_Gatelist["<<id<<"]"<<_Gatelist[id]->getTypeStr()<<endl;
  
  // cout<<"sweep before: ";
  // cout<<"size: "<<_aig.size()<<", id: ";
  // for(int i = 0;i!=_aig.size();i++)
  //   cout<<_aig[i]->_id<<" ";
  
  // cout<<endl<<"sweep after: ";
  _aig.clear();
  for (unsigned i = 0; i < _Gatelist.size(); i++)
    if(_Gatelist[i]->_type == AIG_GATE) _aig.push_back(_Gatelist[i]);
  
  // cout<<"size: "<<_aig.size()<<", id: ";
  // for(unsigned i = 0; i<_aig.size(); i++)
      // cout<<_aig[i]->_id<<" ";
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
