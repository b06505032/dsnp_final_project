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
  if(sweeped == true) return;
  for(unsigned j = 0; j<_dfsList.size();j++)
    _dfsList[j]->sweep = false;

  for (unsigned aigid=0; aigid<_Gatelist.size();aigid++)
  {
    // cout<<"id: "<<_Gatelist[aigid]->_id<<", type: "<<_Gatelist[aigid]->_type<<" , sweep: "<<_Gatelist[aigid]->sweep<<endl;
    if(_Gatelist[aigid]->sweep == false) continue; // false
    else
    {
      if(_Gatelist[aigid]->_type == PI_GATE) {_Gatelist[aigid]->sweep=false; continue;}
      else if(_Gatelist[aigid]->_type == CONST_GATE) {_Gatelist[aigid]->sweep=false; continue;}
      else
      {
        cout<<"Sweeping: "<<_Gatelist[aigid]->getTypeStr()<<"("<<_Gatelist[aigid]->_id<<") removed..."<<endl;
        if(_Gatelist[aigid]->_type==AIG_GATE) {
          // A=A-1;
          _Gatelist[aigid]->_type = UNDEF_GATE;
        }
        if(!_Gatelist[aigid]->_fanin.empty())
        {
          for(unsigned k = 0; k < _Gatelist[aigid]->_fanin.size();k++){
            unsigned fi = _Gatelist[aigid]->_fanin[k]->_id;
            for(int j = 0; j<_Gatelist[fi]->_fanout.size();j++) {
              if(_Gatelist[fi]->_fanout[j]->_id == aigid) _Gatelist[fi]->_fanout.erase( _Gatelist[fi]->_fanout.begin()+j);
              if(_Gatelist[fi]->_fanout[j]->_id == aigid) _Gatelist[fi]->_outinvert.erase( _Gatelist[fi]->_outinvert.begin()+j);
            }
            // _Gatelist[fi]->_fanout.pop_back();
            // _Gatelist[fi]->_outinvert.pop_back();
          }
          _Gatelist[aigid]->_fanin.clear();
          _Gatelist[aigid]->_invert.clear();
        }
        if(!_Gatelist[aigid]->_fanout.empty())
        {
          for(unsigned k = 0; k < _Gatelist[aigid]->_fanout.size();k++){
            unsigned fo = _Gatelist[aigid]->_fanout[k]->_id;
            for(int j = 0; j<_Gatelist[fo]->_fanin.size();j++) {
              if(_Gatelist[fo]->_fanin[j]->_id == aigid) _Gatelist[fo]->_fanin.erase( _Gatelist[fo]->_fanin.begin()+j);
              if(_Gatelist[fo]->_fanin[j]->_id == aigid) _Gatelist[fo]->_invert.erase( _Gatelist[fo]->_invert.begin()+j);
            }
            // _Gatelist[fo]->_fanin.pop_back();
            // _Gatelist[fo]->_invert.pop_back();
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
  _aig.clear();
  for (unsigned i = 0; i < _Gatelist.size(); i++)
    if(_Gatelist[i]->_type == AIG_GATE) _aig.push_back(_Gatelist[i]);
  sweeped = true;
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
  CirGate::_gmark++;
  for (unsigned i = 0; i < _dfsList.size(); i++) {
    // opt(_dfsList[i]->_id);
    // opt(_out[i]->_id);
    reconnect(_dfsList[i]->_id);
  }
  DFS();
  // for (unsigned id=0; id<_Gatelist.size();id++)
    // cout<<endl<<"_Gatelist["<<id<<"]"<<" ,delete: "<<_Gatelist[id]->deleted<<endl;
  
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/

void
CirMgr::opt(unsigned vertex)
{
  // if(_Gatelist[vertex]->_mark == CirGate::_gmark) return;
	if(_Gatelist[vertex]->_type == UNDEF_GATE) return;
  if(_Gatelist[vertex]->_type == PI_GATE) return;
  if(_Gatelist[vertex]->_type == CONST_GATE) return;
  if(_Gatelist[vertex]->_fanin.size()>0)
  {
    // for (size_t i = 0; i < _Gatelist[vertex]->_fanin.size(); i++) 
      // opt(_Gatelist[vertex]->_fanin[i]->_id);
    if(_Gatelist[vertex]->_fanin.size()==2)
    {
      if(_Gatelist[vertex]->_fanin[0]->_id == _Gatelist[vertex]->_fanin[1]->_id)
      {
        if (_Gatelist[vertex]->_invert[0] == _Gatelist[vertex]->_invert[1])
        {
          cout<<vertex<<" has same input"<<endl;
          updateIO(vertex, _Gatelist[vertex]->_fanin[0]->_id, _Gatelist[vertex]->_invert[0]);
        }
        else
        {
          cout<<vertex<<" has inverse input"<<endl;
          updateIO(vertex, 0, 0);
        }
      }
      else if(_Gatelist[vertex]->_fanin[0]->_type == CONST_GATE && _Gatelist[vertex]->_fanin[1]->_type == CONST_GATE)
      {
        if(_Gatelist[vertex]->_invert[0] && _Gatelist[vertex]->_invert[1])
				{
          cout<<vertex<<" has const1 input == "<<endl;
          updateIO(vertex, 0, 1);
        }
				else
				{
          cout<<vertex<<" has const0 input =="<<endl;
          updateIO(vertex, 0, 0);
        }
      }
      else if(_Gatelist[vertex]->_fanin[0]->_type == CONST_GATE){
        if (_Gatelist[vertex]->_invert[0]){
          cout<<vertex<<" has const1 input at [0]"<<endl;
          updateIO(vertex, _Gatelist[vertex]->_fanin[1]->_id, _Gatelist[vertex]->_invert[1]);
        }
        else {
          cout<<vertex<<" has const0 input at [0]"<<endl; 
          updateIO(vertex, 0, 0);
        }
      }
      else if(_Gatelist[vertex]->_fanin[1]->_type == CONST_GATE){
        if (_Gatelist[vertex]->_invert[1])
        {
          cout<<vertex<<" has const1 input at[1]"<<endl;
          updateIO(vertex, _Gatelist[vertex]->_fanin[0]->_id, _Gatelist[vertex]->_invert[0]);
        }
        else
        {
          cout<<vertex<<" has const0 input at [1]"<<endl;
          updateIO(vertex, 0, 0);
        }
      }
    }
  }
  _Gatelist[vertex]->_mark = CirGate::_gmark;
}

void
CirMgr::updateIO(unsigned del_id, unsigned fi_id, bool inputInv)
{
  cout<< del_id<<" gate deleting before"<<endl;
  for(int i=0 ;i<_Gatelist[del_id]->_fanin.size();i++)
    cout<<" fa"<<i<<" "<<_Gatelist[del_id]->_fanin[i]->_id;
  cout<<endl;
  for(unsigned i = 0; i<_Gatelist[del_id]->_fanout.size();i++) {
    unsigned fo = _Gatelist[del_id]->_fanout[i]->_id;
    // GateType tp = _Gatelist[del_id]->_fanout[i]->_type;
    // if(tp!=PO_GATE){
      // for(int j = 0; j<_Gatelist[fo]->_fanin.size();j++) {
      //   if(_Gatelist[fo]->_fanin[j]->_id == del_id) _Gatelist[fo]->_fanin.erase( _Gatelist[fo]->_fanin.begin()+j);
      //   if(_Gatelist[fo]->_fanin[j]->_id == del_id) _Gatelist[fo]->_invert.erase( _Gatelist[fo]->_invert.begin()+j);
      // }
      _Gatelist[fo]->eraseFanIn(del_id);
      _Gatelist[fo]->_fanin.push_back(_Gatelist[fi_id]);
      _Gatelist[fo]->_invert.push_back(inputInv);
    // }
    // else
    // {
    //   for(int j =0;j<_out.size();j++)
    //     if(_out[j]->_id == fo) {
    //       for(int k = 0; k<_out[j]->_fanin.size();k++) {
    //         if(_out[j]->_fanin[k]->_id == del_id) _out[j]->_fanin.erase(_out[j]->_fanin.begin()+k);
    //         if(_out[j]->_fanin[k]->_id == del_id) _out[j]->_invert.erase(_out[j]->_invert.begin()+k);
    //       }
    //       _out[j]->_fanin.push_back(_Gatelist[fi_id]);
    //       _out[j]->_invert.push_back(inputInv);
    //     }
    // }
  }
  delete _Gatelist[del_id];
  _Gatelist[del_id]->deleted = true;
}


void
CirMgr::reconnect(unsigned del_id)
{
  CirGate *&cur = _Gatelist[del_id], *pre = 0;
  bool inv = false;
  if(cur->_type!=AIG_GATE) return;
  // Fanin const 1
	if((cur->_fanin[0]->_type == CONST_GATE && cur->_invert[0]))
	{ pre = cur->_fanin[1]; inv = cur->_invert[1]; }
	else if((cur->_fanin[1]->_type == CONST_GATE && cur->_invert[1]))
	{ pre = cur->_fanin[0]; inv = cur->_invert[0]; }
  // Fanin const 0
	else if((cur->_fanin[0]->_type == CONST_GATE && !(cur->_invert[0])) || (cur->_fanin[1]->_type == CONST_GATE && !(cur->_invert[1])) )
	{ pre =  _Gatelist[0]; inv = false; }
  else if(cur->_fanin[0] == cur->_fanin[1]) {
		// Identical
		if(cur->_invert[0] == cur->_invert[1])
			{ pre = cur->_fanin[0]; inv = cur->_invert[0]; }
		// Inverted
		else { pre = _Gatelist[0]; inv = false; }
	}
  if(pre) {
			cout << "Simplifying: " << pre->_id << " merging " << (inv ? "!" : "") << cur->_id << "..." << endl;
			replace(cur, pre, inv);
	}

}

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
			if(a == a->_fanout[i]->_fanin[j]) {
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
	delete a;
	a = 0;
	// --_header[4];
}