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
    if(_Gatelist[aigid]->sweep == false) continue;
    else
    {
      if(_Gatelist[aigid]->_type == PI_GATE) {_Gatelist[aigid]->sweep=false; continue;}
      else if(_Gatelist[aigid]->_type == CONST_GATE) {_Gatelist[aigid]->sweep=false; continue;}
      else
      {
        if (_Gatelist[aigid]-> deleted) continue;
        sweeped = true;
        cout<<"Sweeping: "<<_Gatelist[aigid]->getTypeStr()<<"("<<_Gatelist[aigid]->_id<<") removed..."<<endl;
        _Gatelist[aigid]->deleted = true;
        if(_Gatelist[aigid]->_type==AIG_GATE) {
          // A=A-1;
          _Gatelist[aigid]->_type = UNDEF_GATE;
        }
        if(!_Gatelist[aigid]->_fanin.empty())
        {
          for(unsigned k = 0; k < _Gatelist[aigid]->_fanin.size();k++){
            unsigned fi = _Gatelist[aigid]->_fanin[k]->_id;
            _Gatelist[fi]->eraseFanOut(aigid);
            // for(int j = 0; j<_Gatelist[fi]->_fanout.size();j++) {
            //   if(_Gatelist[fi]->_fanout[j]->_id == aigid) _Gatelist[fi]->_fanout.erase( _Gatelist[fi]->_fanout.begin()+j);
            //   if(_Gatelist[fi]->_fanout[j]->_id == aigid) _Gatelist[fi]->_outinvert.erase( _Gatelist[fi]->_outinvert.begin()+j);
            // }
          }
          _Gatelist[aigid]->_fanin.clear();
          _Gatelist[aigid]->_invert.clear();
        }
        if(!_Gatelist[aigid]->_fanout.empty())
        {
          for(unsigned k = 0; k < _Gatelist[aigid]->_fanout.size();k++){
            unsigned fo = _Gatelist[aigid]->_fanout[k]->_id;
            _Gatelist[fo]->eraseFanIn(aigid);
            // for(int j = 0; j<_Gatelist[fo]->_fanin.size();j++) {
            //   if(_Gatelist[fo]->_fanin[j]->_id == aigid) _Gatelist[fo]->_fanin.erase( _Gatelist[fo]->_fanin.begin()+j);
            //   if(_Gatelist[fo]->_fanin[j]->_id == aigid) _Gatelist[fo]->_invert.erase( _Gatelist[fo]->_invert.begin()+j);
            // }
          }
          _Gatelist[aigid]->_fanout.clear();
          _Gatelist[aigid]->_outinvert.clear();
        }
      }
    }
  }

  // for (unsigned id=0; id<_Gatelist.size();id++)
    // cout<<endl<<"_Gatelist["<<id<<"]"<<_Gatelist[id]->getTypeStr()<<" ,sweep:"<<_Gatelist[id]->sweep<<endl;
  
  // cout<<"sweep before: ";
  // cout<<"size: "<<_aig.size()<<", id: ";
  // for(int i = 0;i!=_aig.size();i++)
  //   cout<<_aig[i]->_id<<" ";
  
  // cout<<endl<<"sweep after: ";
  if(sweeped)
  {
    _aig.clear();
    for (unsigned i = 0; i < _Gatelist.size(); i++)
      if(_Gatelist[i]->_type == AIG_GATE) _aig.push_back(_Gatelist[i]);
  }
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
  for (unsigned i = 0; i < _dfsList.size(); i++)
    opt(_dfsList[i]->_id);
  // for (unsigned id=0; id<_Gatelist.size();id++)
    // cout<<endl<<"_Gatelist["<<id<<"]"<<" ,delete: "<<_Gatelist[id]->deleted<<endl;
  if(opted)
  {
    // for(int i = 0 ;i<_Gatelist.size();i++){
    //   if(_Gatelist[i]->_type == AIG_GATE && _Gatelist[i]->_fanout.size()==0 && !_Gatelist[i]->deleted)
    //     _Gatelist[i]->_type = PO_GATE;
    // }
    _aig.clear();
    for (unsigned i = 0; i < _Gatelist.size(); i++)
      if(_Gatelist[i]->_type == AIG_GATE && !_Gatelist[i]->deleted) _aig.push_back(_Gatelist[i]);
    _out.clear();
    for (unsigned i = 0; i < _Gatelist.size(); i++){
      if(_Gatelist[i]->_type == PO_GATE && !_Gatelist[i]->deleted) _out.push_back(_Gatelist[i]);
    }
    DFS();
  }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/

void
CirMgr::opt(unsigned vertex)
{
	if(_Gatelist[vertex]->_type == UNDEF_GATE) return;
  if(_Gatelist[vertex]->_type == PI_GATE) return;
  if(_Gatelist[vertex]->_type == CONST_GATE) return;
  if(_Gatelist[vertex]->_fanin.size()>0)
  {
    // for (size_t i = 0; i < _Gatelist[vertex]->_fanin.size(); i++) 
      // opt(_Gatelist[vertex]->_fanin[i]->_id);
    if(_Gatelist[vertex]->_fanin.size()==2)
    {
      // if(_Gatelist[vertex]->_fanin[0]->_type == CONST_GATE && _Gatelist[vertex]->_invert[0]){
      //   merge(vertex, _Gatelist[vertex]->_fanin[1]->_id, _Gatelist[vertex]->_invert[1]);        
      // }
      // else if(_Gatelist[vertex]->_fanin[1]->_type == CONST_GATE && _Gatelist[vertex]->_invert[1]){
      //   merge(vertex, _Gatelist[vertex]->_fanin[0]->_id, _Gatelist[vertex]->_invert[0]);
      // }
      // else if((_Gatelist[vertex]->_fanin[0]->_type == CONST_GATE && !(_Gatelist[vertex]->_invert[0])) || (_Gatelist[vertex]->_fanin[1]->_type == CONST_GATE && !(_Gatelist[vertex]->_invert[1]))){
      //   merge(vertex, 0, 0);
      // }
      // else if(_Gatelist[vertex]->_fanin[0] == _Gatelist[vertex]->_fanin[1]){
      //   if(_Gatelist[vertex]->_invert[0] == _Gatelist[vertex]->_invert[1])
      //   { merge(vertex, _Gatelist[vertex]->_fanin[0]->_id, _Gatelist[vertex]->_invert[0]); }
      //   else 
      //   { merge(vertex, 0, 0); }
      // }
      if(_Gatelist[vertex]->_fanin[0]->_type == CONST_GATE && _Gatelist[vertex]->_fanin[1]->_type == CONST_GATE)
      {
        if(_Gatelist[vertex]->_invert[0] && _Gatelist[vertex]->_invert[1])
        {
          // cout << vertex << " has const1 input == "<<endl;
          merge(vertex, 0, 1);
        }
				else
        {
          // cout << vertex << " has const1 input == "<<endl;
          merge(vertex, 0, 0);
        }
      }
      else if(_Gatelist[vertex]->_fanin[0]->_type == CONST_GATE)
      {
        if (_Gatelist[vertex]->_invert[0])
        {
          // cout<<vertex<<" has const1 input at [0]"<<endl;
          merge(vertex, _Gatelist[vertex]->_fanin[1]->_id, _Gatelist[vertex]->_invert[1]);
        }
        else
        {
          // cout<<vertex<<" has const0 input at [0]"<<endl;
          merge(vertex, 0, 0);
        }
      }
      else if(_Gatelist[vertex]->_fanin[1]->_type == CONST_GATE)
      {
        if (_Gatelist[vertex]->_invert[1])
        {
          // cout<<vertex<<" has const1 input at [1]"<<endl;
          merge(vertex, _Gatelist[vertex]->_fanin[0]->_id, _Gatelist[vertex]->_invert[0]);
        }
        else
        {
          // cout<<vertex<<" has const0 input at [1]"<<endl;
          merge(vertex, 0, 0);
        }
      }
      else if(_Gatelist[vertex]->_fanin[0]->_id == _Gatelist[vertex]->_fanin[1]->_id)
      {
        if (_Gatelist[vertex]->_invert[0] == _Gatelist[vertex]->_invert[1])
        {
          // cout<<vertex<<" has same input"<<endl;
          merge(vertex, _Gatelist[vertex]->_fanin[0]->_id, _Gatelist[vertex]->_invert[0]);
        }
        else
        {
          // cout<<vertex<<" has inverse input"<<endl;
          merge(vertex, 0, 0);
        }
      }
    }
  }
}

void
CirMgr::merge(unsigned del_id, unsigned fi_id, bool inputInv)
{
  cout << "Simplifying: " << fi_id << " merging " << (inputInv ? "!" : "") << del_id << "..." << endl;
  opted = true;
  // erase a-x----del_id 
  for(unsigned i = 0;i<_Gatelist[del_id]->_fanin.size();i++)
    _Gatelist[del_id]->_fanin[i]->eraseFanOut(del_id);
  // connect a-o-----(del_id)-------f1, f2, ..., fn
  if(_Gatelist[fi_id]){
    for(unsigned i=0; i<_Gatelist[del_id]->_fanout.size();i++) {
      _Gatelist[fi_id]->_fanout.push_back(_Gatelist[del_id]->_fanout[i]);
      _Gatelist[fi_id]->_outinvert.push_back(inputInv != _Gatelist[del_id]->_outinvert[i]);
    }
  }
  // del_id's fo's fi
  for(unsigned i = 0; i<_Gatelist[del_id]->_fanout.size();i++) {
    for(unsigned j = 0; j<_Gatelist[del_id]->_fanout[i]->_fanin.size();j++)
    {
      if(_Gatelist[del_id]->_fanout[i]->_fanin[j]->_id == del_id)
      {
        if(_Gatelist[fi_id])
        {
          // connect a------(del_id)-------o-f1,f2,...,fn
          _Gatelist[del_id]->_fanout[i]->_fanin[j] = _Gatelist[fi_id];
          _Gatelist[del_id]->_fanout[i]->_invert[j] = (inputInv != _Gatelist[del_id]->_outinvert[i]);
        }
        else
        {
          // if a is CONST  CONST-----------x-f1, f2, ..., fn
          _Gatelist[del_id]->_fanout[i]->eraseFanIn(del_id);
        }
        break;
      }
    }
  }
  _Gatelist[del_id]->_type = UNDEF_GATE;
  _Gatelist[del_id]->deleted = true;
  delete _Gatelist[del_id];
}
