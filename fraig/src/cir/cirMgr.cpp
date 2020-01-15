/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <algorithm>

using namespace std;

// TODO: Implement memeber functions for class CirMgr
/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

// unsigned CirMgr::_globalRef =0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   // 1. READ IN ENTIRE FILE
   ifstream infile;
   infile.open("./" + fileName);
   if (infile.fail()) {
      cerr<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
      return false;
   }
   stringstream strStream;
   strStream << infile.rdbuf();
   string aagstring = strStream.str();
   // 2. SPLIT THE STRING WITH \n
   string::size_type pos1, pos2;
   string c = "\n";
   pos2 = aagstring.find(c);
   pos1 = 0;
   while(string::npos != pos2)
   {
      l.push_back(aagstring.substr(pos1, pos2-pos1));
      pos1 = pos2 + c.size();
      pos2 = aagstring.find(c, pos1);
   }
   if(pos1 != aagstring.length())
      l.push_back(aagstring.substr(pos1));
   // 3. read the GATE
   readHeader();
   _Gatelist[0] = new CirConstGate();
   readInput();
   readOutput();
   readAig();
   readComment();
   connection();
   DFS();
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI    " << setw(8) << right << _in.size()  << endl;
   cout << "  PO    " << setw(8) << right << _out.size() << endl;
   cout << "  AIG   " << setw(8) << right << _aig.size() << endl;
   cout << "------------------" << endl;
   cout << "  Total " << setw(8) << right << _in.size()+_out.size()+_aig.size() << endl;
}

void
CirMgr::printNetlist() const
{
   /*
      cout << endl;
      for (unsigned i = 0, n = _dfsList.size(); i < n; ++i) {
         cout << "[" << i << "] ";
         _dfsList[i]->printGate();
      }
   */
   unsigned n = 0;
   cout<<endl;
   for (size_t i = 0; i < _dfsList.size(); i++) {
      // if(_dfsList[i]==NULL) cout<<"dfs "<< i <<"is null!"<<endl; continue;
      if (_dfsList[i]->_type == PI_GATE)
      {
         cout << "[" << i-n << "] ";
         _dfsList[i]->printGate();
         cout << endl;
      }
      else if (_dfsList[i]->_type == PO_GATE) 
      {
         cout << "[" << i-n << "] ";
         _dfsList[i]->printGate();
         if (_dfsList[i]->_fanin[0]->_type == UNDEF_GATE) cout << '*';
         if (_dfsList[i]->_invert[0]) cout << '!';
         cout << _dfsList[i]->_fanin[0]->_id;
         if(_dfsList[i]->_name!="") cout << " (" << _dfsList[i]->_name << ")";
         cout << endl;
      }
      else if(_dfsList[i]->_type == AIG_GATE)
      {
         cout << "[" << i-n << "] ";
         _dfsList[i]->printGate();
         if (_dfsList[i]->_fanin[0]->_type == UNDEF_GATE) cout << '*';
         if (_dfsList[i]->_invert[0]) cout << '!';
         cout<< _dfsList[i]->_fanin[0]->_id <<' ';
         if (_dfsList[i]->_fanin[1]->_type == UNDEF_GATE) cout << '*';
         if (_dfsList[i]->_invert[1]) cout << '!';
         cout<< _dfsList[i]->_fanin[1]->_id;
         cout << endl;
      }
      else if(_dfsList[i]->_type == CONST_GATE)
      {
         cout << "[" << i-n << "] ";
         _dfsList[i]->printGate();
         cout << endl;
      }
      else
      {
         n++;
      }
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (unsigned i = 0; i < _in.size(); i++) 
     cout << ' ' << _in[i]->_id;
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (unsigned i = 0; i < _out.size(); i++)
     cout << ' ' << _out[i]->_id;
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   /*
   IdList undef;
   IdList unused;
   map<unsigned, CirGate*>::const_iterator i, n;
   for (i = _Gatelist.begin(), n = _Gatelist.end(); i != n; i++) {
      if (i->second->_type == UNDEF_GATE) continue;
      if (i->second->_type == CONST_GATE) continue;
      if (i->second->_type != PO_GATE && i->second->_fanout.empty())
      {
         unused.push_back(i->first);
         continue;
      }
      for (unsigned j = 0; j < i->second->_fanin.size(); j++)
      {
         if (i->second->_fanin[j]->_type == UNDEF_GATE)
         {
            undef.push_back(i->first);
            break;
         }
      }
   }
   if (!undef.empty())
   {
      cout << "Gates with floating fanin(s):";
      for (unsigned i = 0; i < undef.size(); i++)
         cout << ' ' << undef[i];
      cout << endl;
   }
   if (!unused.empty())
   {
      cout << "Gates defined but not used  :";
      for (unsigned i = 0; i < unused.size(); i++)
         cout << ' ' << unused[i];
      cout << endl;
   }
   */
   

   bool a1=true,a2=true;
   map<unsigned, CirGate*>::const_iterator i, n;
   for(i = _Gatelist.begin(), n = _Gatelist.end(); i != n; i++){
      if(!i->second) continue;
      if (i->second->_type == CONST_GATE) continue;
      if(!i->second->deleted and i->second->_type == AIG_GATE){
         for(int j = 0; j<i->second->_fanin.size();j++) {
            if(i->second->_fanin[j]->_type == UNDEF_GATE){
               if(a1){cout<<"Gates with floating fanin(s):";a1=false;}
               cout<<" "<< i->first;
               break;
            }
         }
         // if(i->second->_fanin[0]->_type == UNDEF_GATE or i->second->_fanin[1]->_type==UNDEF_GATE){
         //    if(a1){cout<<"Gates with floating fanin(s):";a1=false;}
         //    cout<<" "<< i->first;
         // }
      }
      if(i->second->_type==PO_GATE) {
         if(i->second->_fanin[0]->_type == UNDEF_GATE) {
            if(a1){cout<<"Gates with floating fanin(s):";a1=false;}
            cout<<" "<< i->first;
         }
      }
   }
   if(!a1){cout<<endl;}
   for(i = _Gatelist.begin(), n = _Gatelist.end(); i != n; i++) {
      if(!i->second) continue;
      if (i->second->_type == CONST_GATE) continue;
      if (!i->second->deleted and i->second->_fanout.size()==0 and i->second->_type!=PO_GATE) {
         if(a2){cout<<"Gates defined but not used  :";a2=false;}
         cout<<" "<<i->first;
      }
   }
   if(!a2){cout<<endl;}
}

void
CirMgr::printFECPairs() const 
{
   // TODO
}

void
CirMgr::writeAag(ostream& outfile) const
{
   int dfs_A=0;
   for (size_t i = 0; i < _dfsList.size(); i++)
      if (_dfsList[i]->_type == AIG_GATE) dfs_A++;
   outfile << "aag "<<M<<" "<<I<<" "<<L<<" "<<O<<" "<<dfs_A<<endl;
   for (unsigned i = 0; i < I; i++) {
      outfile << l[i+1];
      outfile << endl;
   }
   for (unsigned i = 0; i < _out.size(); i++) {
      if(opted) outfile << _out[i]->_id;
      else if (strashed) outfile << _out[i]->_id;
      else outfile << l[i+1+I];
      outfile << endl;
   }
   for (size_t i = 0; i < _dfsList.size(); i++) {
      if (_dfsList[i]->_type != AIG_GATE) continue;
      outfile << l[(_dfsList[i]->_lineNo)-1];  
      outfile << endl;
   }
   int comment = I+O+A+1;
   while (comment < l.size())
   {
      if (l[comment] == "c") break;
      outfile << l[comment];
      outfile << endl;
      comment++;
   }
   outfile<<"c"<<endl;
   // outfile<<"AAG output by Chung-Yang (Ric) Huang"<<endl;
   outfile<<"AAG output by Chien-Ying (Catherine) Yang"<<endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
   // TODO
}

void
CirMgr::readHeader()
{
   vector<string> header;
   string::size_type pos1_, pos2_;
   string d = " ";
   pos2_ = l[0].find(d);
   pos1_ = 0;
   while(string::npos != pos2_)
   {
      header.push_back(l[0].substr(pos1_, pos2_-pos1_));
      pos1_ = pos2_ + d.size();
      pos2_ = l[0].find(d, pos1_);
   }
   if(pos1_ != l[0].length())
      header.push_back(l[0].substr(pos1_));
   M = atof(header[1].c_str()); // store in miloa
   I = atof(header[2].c_str());
   L = atof(header[3].c_str());
   O = atof(header[4].c_str());
   A = atof(header[5].c_str());
   _in.resize(I);
   _out.resize(O);
   _aig.resize(A);
}

void
CirMgr::readInput()
{
   for (unsigned i = 0; i < I; i++)
   {
      unsigned id = atof(l[i+1].c_str())/2;
      unsigned lineNo = i+2;
      _in[i] = new CirPiGate(id, lineNo);
      _Gatelist[id] = _in[i];
   }
}

void
CirMgr::readOutput()
{
   for (unsigned i = 0; i < O; i++)
   {
      unsigned id = M+i+1;
      unsigned lineNo = i+2+I;
      _out[i] = new CirPoGate(id, lineNo);
      _Gatelist[id] = _out[i];
   }
}

void
CirMgr::readAig()
{
   // AIG_GATE
   for (unsigned i = 0; i < A; i++)
   {
      vector<string> Aigs; //parse AIG | INPUT1 | INPUT2
      if (!lexOptions(l[i+1+I+O], Aigs)) return;
      unsigned id = atof(Aigs[0].c_str())/2;
      unsigned lineNo = i+2+I+O;
      _aig[i] = new CirAigGate(id, lineNo);
      _Gatelist[id] = _aig[i];
   }
}

void
CirMgr::readComment()
{
   int line = I+O+A+1;
   while (line < l.size())
   {
      if (l[line] == "c") break;
      vector<string> name;
      if (!lexOptions(l[line], name)) return ;
      if (l[line][0] == 'i') {
         int index;
         string s = name[0];
         stringstream ss(s);
         ss.ignore(1);
         ss >> index;
         _in[index]->_name = name[1];
      }
      else if (l[line][0] == 'o') {
         int index;
         string s = name[0];
         stringstream ss(s);
         ss.ignore(1);
         ss >> index;
         _out[index]->_name = name[1];
      }
      line++;
   }
}

void
CirMgr::connection()
{
   // DEAL WITH AIG_GATE's FAN_IN FAN_OUT
   for (unsigned i = 0; i < A; i++) {
      // parse _aig[i] | INPUT1 | INPUT2
      // Aigs    [0]      [1]      [2]
      vector<string> Aigs; 
      if (!lexOptions(l[i+1+I+O], Aigs)) return;
      unsigned aigid = atof(Aigs[0].c_str())/2;
      for (int count = 1; count != 3; count++) {
         if ((int)atof(Aigs[count].c_str()) % 2 != 0)
         { // invert
            unsigned id = (atof(Aigs[count].c_str())-1)/2;
            map<unsigned, CirGate*>::iterator it = _Gatelist.find(id);
            if (it == _Gatelist.end())
               _Gatelist[id] = new CirUndefGate(id);
            _Gatelist[aigid]->_fanin.push_back(_Gatelist[id]);
            _Gatelist[aigid]->_invert.push_back(true);
            _Gatelist[id]->_fanout.push_back(_Gatelist[aigid]);
            _Gatelist[id]->_outinvert.push_back(true);
         }
         else
         {
            unsigned id = atof(Aigs[count].c_str())/2;
            map<unsigned, CirGate*>::iterator it = _Gatelist.find(id);
            if (it == _Gatelist.end())
               _Gatelist[id] = new CirUndefGate(id);
            _Gatelist[aigid]->_fanin.push_back(_Gatelist[id]);
            _Gatelist[aigid]->_invert.push_back(false);
            _Gatelist[id]->_fanout.push_back(_Gatelist[aigid]);
            _Gatelist[id]->_outinvert.push_back(false);
         }
      }
   }
   // DEAL WITH PO's FANIN
   for (unsigned i = 0; i < O; i++) {
      unsigned outid = M+i+1;
      if ((int)atof(l[i+1+I].c_str()) % 2 != 0)
      {  // invert
         unsigned id = (atof(l[i+1+I].c_str())-1)/2;
         map<unsigned, CirGate*>::iterator it = _Gatelist.find(id);
         if (it == _Gatelist.end())
            _Gatelist[id] = new CirUndefGate(id);
         _Gatelist[outid]->_fanin.push_back(_Gatelist[id]);
         _Gatelist[outid]->_invert.push_back(true);
         _Gatelist[id]->_fanout.push_back(_Gatelist[outid]);
         _Gatelist[id]->_outinvert.push_back(true);
      }
      else
      {
         unsigned id = atof(l[i+1+I].c_str())/2;
         map<unsigned, CirGate*>::iterator it = _Gatelist.find(id);
         if (it == _Gatelist.end())
            _Gatelist[id] = new CirUndefGate(id);
         _Gatelist[outid]->_fanin.push_back(_Gatelist[id]);
         _Gatelist[outid]->_invert.push_back(false);
         _Gatelist[id]->_fanout.push_back(_Gatelist[outid]);
         _Gatelist[id]->_outinvert.push_back(false);
      }
   }
}

bool
CirMgr::lexOptions(const string& option, vector<string>& tokens) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   return true;
}

void
CirMgr::DFS()
{          
   if (_dfsList.size()>0) _dfsList.clear();
   _globalRef++;
   for (unsigned i = 0; i < _out.size(); i++)
      DFSVisit(_out[i]->_id);
}

void
CirMgr::DFSVisit(unsigned vertex)
{
   if (_Gatelist[vertex]->_fanin.size() > 0) {
      for (size_t i = 0; i < _Gatelist[vertex]->_fanin.size(); i++) {
         if(_Gatelist[vertex]->_fanin[i]->_ref != _globalRef) {
            _Gatelist[vertex]->_fanin[i]->_ref = _globalRef;
            DFSVisit(_Gatelist[vertex]->_fanin[i]->_id);
         }
      }
   }
   // bool exist = false;
   // for(int i=0;i < _dfsList.size();i++) {
   //    if(_Gatelist[vertex]->_id == _dfsList[i]->_id) {
   //       exist = true;
   //       break;
   //    }
   // }
   // if(exist==false) _dfsList.push_back(_Gatelist[vertex]);
   _dfsList.push_back(_Gatelist[vertex]);
}
