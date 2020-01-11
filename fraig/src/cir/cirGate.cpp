/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::_gmark =0;

void
CirGate::reportGate() const
{
   cout << "==================================================" << endl;
   stringstream ss;
   ss << "= " + getTypeStr() << '(' << _id << ")";
   if (_name != "") {
      ss << "\"" << _name << "\"";
   }
   ss << ", line " << getLineNo();
   cout << setw(49) << left << ss.str() << "=" << endl;
   cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   _gmark++;
   dfs_fanin(level, 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   _gmark++;
   dfs_fanout(level, 0); 
}

void
CirGate::dfs_fanin(int level, int cur) const
{
   if(level != -1) if(cur > level) return;
   int _cur = cur;
   if(_fanin.size()) _mark = _gmark;
   if(_cur == 0) {
      cout << getTypeStr() << " " << _id << endl;
      _mark = _gmark;
      ++_cur;
   }
   for(int n = 0; n < _fanin.size(); ++n) {
      for(int m = 0; m < _cur; ++m) cout << "  ";
      if(_invert[n]) cout << "!";
      cout << _fanin[n]->getTypeStr() << " " << _fanin[n]->_id;
      if(_fanin[n]->_mark == _gmark) { cout << " (*)" << endl; }
      else {
         cout << endl;
         _fanin[n]->dfs_fanin( level, _cur + 1);
      }
   }
}

void 
CirGate::dfs_fanout(int level, int cur) const
{
   if(level != -1) if(cur > level) return;
   int _cur = cur;
   if(_fanout.size()) _mark = _gmark;
   if(_cur == 0) {
      cout << getTypeStr() << " " << _id << endl;
      _mark = _gmark;
      ++_cur;
   }
   for(int n = 0; n < _fanout.size(); ++n) {
      for(int m = 0; m < _cur; ++m) cout << "  ";
      if(_outinvert[n]) cout << "!";
      cout << _fanout[n]->getTypeStr() << " " << _fanout[n]->_id;
      if(_fanout[n]->_mark == _gmark) { cout << " (*)" << endl; }
      else {
         cout << endl;
         _fanout[n]->dfs_fanout(level, _cur + 1);
      }
   }
   
}

