/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <stdlib.h>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cstring>
#include <fstream>
#include <sstream>


using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  // 1. READ IN ENTIRE FILE
  stringstream strStream;
  strStream << patternFile.rdbuf();
  string patternstring = strStream.str();
  patternstring.erase(remove(patternstring.begin(), patternstring.end(), ' '), patternstring.end());
  // 2. SPLIT THE STRING WITH \n
  string::size_type pos1, pos2;
  string c = "\n";
  vector<string> patternline; // each line of the pattern string
  pos2 = patternstring.find(c);
  pos1 = 0;
  while(string::npos != pos2)
  {
    patternline.push_back(patternstring.substr(pos1, pos2-pos1));
    pos1 = pos2 + c.size();
    pos2 = patternstring.find(c, pos1);
  }
  if(pos1 != patternstring.length())
    patternline.push_back(patternstring.substr(pos1));

  
  for(int li = 0; li<patternline.size();li++)
  {
    vector<unsigned> patterns;
    if(patternline[li] == "") continue;
    if (!validpattern(patternline[li])) break; // illegal patterns    
    
  }

}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

bool
CirMgr::validpattern(string& string)
{
  unsigned psize = 0;
  psize = string.size();
  if (psize != I)
  {
    cerr << endl<<"Error: Pattern(" << string << ") length(" << psize << ") does not match the number of inputs(" << I << ") in a circuit!!"<<endl;
    return false;
  }
  for (int i = 0; i < psize; ++i)
  {
    if (string[i] == '0' or string[i] == '1') continue;
    else 
    {
      cerr << endl << "Error: Pattern(" << string << ") contains a non-0/1 character('"<< string[i] << "')."<<endl;
      return false;
    }
  }
  return true;
}
