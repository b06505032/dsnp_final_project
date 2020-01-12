/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() {}
   ~CirMgr() {
      // for(size_t i=0;i!=_Gatelist.size();i++) {
      //    delete _Gatelist[i];
      // }
   } 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
      map<unsigned, CirGate*>::const_iterator it = _Gatelist.find(gid);
      if (it == _Gatelist.end()) return 0;
      return it->second;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

   // for DFS
   static unsigned _globalRef;
   void DFS();
   void DFSVisit(unsigned vertex);

private:
   ofstream           *_simLog;

   // M, maximum index
   // I, #inputs
   // L, #latches = 0
   // O, #outputs
   // A, #AND gates
   unsigned M,I,L,O,A;
   vector<string> l;
   GateList _in;
   GateList _out;
   GateList _aig;
   map<unsigned, CirGate*> _Gatelist;
   GateList _dfsList;

   // Helper function
   void readHeader();
   void readInput();
   void readOutput();
   void readAig();
   void readComment();
   void connection();
   bool lexOptions(const string& option, vector<string>& tokens) const;

};

#endif // CIR_MGR_H
