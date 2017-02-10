#pragma once

#include "../Configurator/configurator.h"

struct SubConfig2:public codepi::Configurator{
  int k;

  CFG_HEADER(SubConfig2)
  CFG_ENTRY2(k,9)
  CFG_TAIL
};

struct SubConfig1:public SubConfig2{
  int i,j;
  
  CFG_HEADER(SubConfig1)
  CFG_ENTRY2(i,7)
  CFG_ENTRY1(j)
  CFG_PARENT(SubConfig2)
  CFG_TAIL
};

struct TestConfig:public codepi::Configurator{

  int jjj;
  std::vector<int> k;
  std::array<int,10> arr;
  std::set<int> intSet;
  std::pair<int, std::string> pair;
  std::pair<std::string, float> pair2;
  std::map<std::string, int> map;
  std::string n;
  SubConfig1 s;
  std::vector<SubConfig1> t;
  std::vector<std::string> strList;
  SubConfig1 u;
  bool b;
  codepi::Optional<int> opt1, opt2, opt3;
  codepi::Optional< std::vector<int> > optvec;

  CFG_HEADER(TestConfig)
  CFG_ENTRY2(jjj,12)
  CFG_ENTRY1(k)
  CFG_ENTRY1(arr)
  CFG_ENTRY1(intSet)
  CFG_ENTRY1(pair)
  CFG_ENTRY1(pair2)
  CFG_ENTRY1(map)
  CFG_ENTRY2(n,"hello")
  CFG_ENTRY1(s)
  CFG_ENTRY1(t)
  CFG_ENTRY1(strList)
  CFG_ENTRY1(u)
  CFG_ENTRY1(b)
  CFG_ENTRY1(opt1)
  CFG_ENTRY1(opt2)
  CFG_ENTRY1(opt3)
  CFG_ENTRY1(optvec)
  CFG_TAIL
};