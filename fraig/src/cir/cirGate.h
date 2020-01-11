/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
   friend class CirMgr; // I added

   CirGate(GateType type, unsigned id, unsigned lineNo): _type(type), _id(id), _lineNo(lineNo) , _fanin(0), _fanout(0), _name("") {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { switch (_type) {
      case UNDEF_GATE: return "UNDEF";
      case PI_GATE:    return "PI";
      case PO_GATE:    return "PO";
      case AIG_GATE:   return "AIG";
      case CONST_GATE: return "CONST";
      default:         return "";
    } 
   }
   unsigned getLineNo() const { return _lineNo; }
   virtual bool isAig() const { return false; } // TODO

   // Printing functions
   virtual void printGate() const {}
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   // for DFS in Mgr
   mutable unsigned _ref;

   // for DFS -fanin -fanout
   static unsigned _gmark;
   mutable unsigned _mark;
   void dfs_fanin(int level, int cur) const;
   void dfs_fanout(int level, int cur) const;
   
private:

protected:
  GateType _type;
  unsigned _id;
  unsigned _lineNo;
  GateList _fanin;
  GateList _fanout;
  vector<bool> _invert;
  vector<bool> _outinvert;
  string _name;
};

class CirConstGate: public CirGate  {
public:
  CirConstGate(): CirGate(CONST_GATE, 0, 0) {}
  void printGate() const { cout << "CONST0"; } 
};

class CirPiGate: public CirGate  {
public:
  CirPiGate(unsigned id, unsigned lineNo): CirGate(PI_GATE, id, lineNo) {}
  void printGate() const { 
    cout << "PI  " << _id;
    if(_name!="") cout << " (" << _name << ")";
  }
};

class CirPoGate: public CirGate  {
public:
  CirPoGate(unsigned id, unsigned lineNo): CirGate(PO_GATE, id, lineNo) {}
  void printGate() const { cout << "PO  " << _id << " "; }
};

class CirAigGate: public CirGate {
public:
  CirAigGate(unsigned id, unsigned lineNo): CirGate(AIG_GATE, id, lineNo) {}
  void printGate() const { cout << "AIG " << _id << " "; }
};

class CirUndefGate: public CirGate  {
public:
  CirUndefGate(unsigned id): CirGate(UNDEF_GATE, id, 0) {}
  void printGate() const { cout << "UNDEF " << _id; }
};

#endif // CIR_GATE_H
