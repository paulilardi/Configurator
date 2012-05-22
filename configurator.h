// Copyright (C) 2011 Paul Ilardi (configurator@pilardi.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, unconditionally.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.
// 
// Tested with gcc 4.1, gcc 4.3, vs2005, vs2010.
// NOTE: On Windows, recommend compiling with /we4002 compile flag.  
//       This will catch "incorrect number of args in macro" errors.

#pragma once

#include<string>
#include<vector>
#include<sstream>
#include<fstream>
#include<stdexcept>
#include<iomanip>

#include"type_traits_helper.h"

#ifdef _MSC_VER // if Visual Studio
#pragma warning( error : 4002 ) // treat macros with incorrect number of args as error
#endif

//////////////////////////////////////////////////////////////////
// Configurator - virtual base class

/// Virtual class for serializing and deserializing config files.
class Configurator{
public:
	/// read and parse file / stream / string
	void readFile(const std::string& filename);
	void readStream(std::istream& stream);
	void readString(const std::string& str);
	void readString(const char* str, int size);
	void readString(const char* str);

	/// write contents of struct to file / stream / string
	void writeToFile(const std::string& filename);
	void writeToStream(std::ostream& stream,int indent=0);
	void writeToString(std::string& str);
	int  writeToString(char* str, int maxSize); //returns bytes used
	std::string toString();

	/// equality
	bool operator==(Configurator& other);
	bool operator!=(Configurator& other);

	/// set varname = val
	void set(const std::string& varName, const std::string& val);
	/// set varname based on contents of stream
	void set(const std::string& varName, std::istream& stream);
	/// return name of current struct
	virtual std::string getStructName()=0;
	///virtualized destructor for proper inheritance
	virtual ~Configurator(){}   

protected:
	enum MFType{INIT_ALL,SET_STREAM,WRITE_ALL,IS_EQUAL};

	/// Helper method that is called by all of the public methods above.
	///   This method is automatically generated in subclass using macros below
	///   Returns the number of variables matched
	virtual int multiFunction(MFType mfType, std::string* str, std::string* subVar,
		std::istream* streamIn, std::ostream* streamOut, int indent, 
		Configurator* other)=0;

	/// return i*2 spaces, for printing
	static std::string indentBy(int i);
	/// returns default value of type T
	template <typename T> static T getDefaultVal(const T&var){ return T();}
	/// overridable method called on parse error
	virtual void throwError(std::string error){ throw std::runtime_error(error); }

	//////////////////////////////////////////////////////////////////
	// setFromStream(stream, val, subVar)
	// Used internally by multiFunction
	// Sets value of val based on contents of stream
	// subVar is for '.' separated nested structs, e.g. "a.b=5"
	// Five definitions below: Configurator, string, vector, bool, and other

	/// setFromStream for strings.  
	/// by default, operator>> will only read one word at a time
	/// this one will read an entire line
	static void setFromStream(std::istream& ss, std::string& str, const std::string& subVar="");

	/// setFromStream for Configurator descendants
	static void setFromStream(std::istream& ss, Configurator& cfg, const std::string& subVar="");

	/// setFromStream for bool (allows (t,true,1,f,false,0))
	static void setFromStream(std::istream& ss, bool& b, const std::string& subVar="");

	/// setFromString for vectors
	/// Parses vector from stream of format: "[1 2 3 4 5]" (commas required for strings, optional for others)
	template <typename T>
	static void setFromStream(std::istream& is, std::vector<T>& vec, const std::string& subVar="");

	/// setFromStream for all other types
	/// the enable_if is required to prevent it from matching on Configurator descendents
	/// note: this is defined inline because vs2005 has trouble compiling otherwise
	template <typename T>
	static typename TTHelper::enable_if<!TTHelper::is_configurator<T>::value,void>::type
		setFromStream(std::istream& ss, T& val, const std::string& subVar=""){
			if(!subVar.empty()) { //subVar should be empty
				ss.setstate(std::ios::failbit); //set fail bit to trigger error handling
				return;
			}
			ss>>std::setbase(0)>>val;
	}

	//////////////////////////////////////////////////////////////////
	// writeToStreamHelper(stream, val, indent)
	// Used internally by multiFunction
	// Writes the contents of val to the stream
	// Three definitions below: Configurator, vector, and other

	/// writeToStreamHelper for descendents of Configurator
	static void writeToStreamHelper(std::ostream& stream, Configurator& cfg, int indent);

	/// writeToStreamHelper for bool (writes true/false)
	static void writeToStreamHelper(std::ostream& stream, bool& b, int indent);

	/// writeToStreamHelper for string (writes true/false)
	static void writeToStreamHelper(std::ostream& stream, std::string& str, int indent);

	/// writeToStreamHelper for vectors
	/// Prints vector to stream in format: "[1,2,3,4,5]"
	template <typename T>
	static void writeToStreamHelper(std::ostream& stream, std::vector<T>& vec, int indent);

	/// writeToStreamHelper for all other types
	/// the enable_if is required to prevent it from matching on Configurator descendents
	/// note: this is defined inline because vs2005 has trouble compiling otherwise
	template <typename T>
	static typename TTHelper::enable_if<!TTHelper::is_configurator<T>::value,void>::type
		writeToStreamHelper(std::ostream& stream, T& val, int indent){
			stream<<val;
	}

	/////////////////////////////////////////////////////////////////////////////
	// cfgCompareHelper(a, b)
	// returns 0 if same, >0 if different

	/// cfgCompareHelper for Configurator
	static int cfgCompareHelper(Configurator& a, Configurator& b);

	template <typename T>
	static int cfgCompareHelper(std::vector<T>& a, std::vector<T>& b){
		int retVal = 0;
		if(a.size()!=b.size()) return 1;
		for(size_t i=0;i<a.size();i++){
			retVal+=cfgCompareHelper(a[i],b[i]);
		}
		return retVal;
	}
	
	/// cfgCompareHelper for any type with defined operator==
	template <typename T>
	static typename TTHelper::enable_if<!TTHelper::is_configurator<T>::value,int>::type
		cfgCompareHelper(T& a, T& b){
			return !(a==b);
	}
	
	/// Used by TTHelper::is_configurator to identify decendents of Configurator
	typedef void is_configurator;

};

//////////////////////////////////////////////////////////////////
// Implementation of templated static methods from Configurator

// setFromString for vectors
// Parses vector from stream of format: "[1 2 3 4 5]" (commas required for strings, optional for others)
template <typename T>
void Configurator::setFromStream(std::istream& is, std::vector<T>& vec, const std::string& subVar){
	if(!subVar.empty()) { //subVar should be empty
		is.setstate(std::ios::failbit); //set fail bit to trigger error handling
		return;
	}
	vec.clear();
	std::string line;
	// find first element
	while(isspace(is.peek())||is.peek()=='[') is.ignore();
	while(is.good()){
		// check for each of vector
		if(is.peek()==']'){
			is.ignore();
			break;
		}

		// read element and add to vector
		T val;
		setFromStream(is,val);
		if(is) vec.push_back(val);

		// push to next element, removing comments
		while(isspace(is.peek())||is.peek()==','||is.peek()=='#') {
			std::string dumpStr;
			if(is.peek()=='#') getline(is,dumpStr);
			else is.ignore();
		}
	}
}

// writeToStreamHelper for vectors
// Prints vector to stream in format: "[1,2,3,4,5]"
template <typename T>
void Configurator::writeToStreamHelper(std::ostream& stream, std::vector<T>& vec, int indent){
	stream<<"[";
	for(size_t i=0;i<vec.size();i++){
		if(i>0) stream<<",";
		writeToStreamHelper(stream,vec[i],indent);
	}
	stream<<"]";
}

//////////////////////////////////////////////////////////////////
// Macros to automatically generate the multiFunction method in
// descendant classes.

// automatically generates subclass constructor and begins multiFunction method
#define CFG_HEADER(structName) \
	structName() { multiFunction(INIT_ALL,NULL,NULL,NULL,NULL,0,NULL); } \
	std::string getStructName() { return #structName; } \
	int multiFunction(MFType mfType, std::string* str, std::string* subVar, \
		std::istream* streamIn, std::ostream* streamOut,int indent,Configurator*other){ \
		int retVal=0; \
		structName* otherPtr; \
		if(mfType==IS_EQUAL) {otherPtr = dynamic_cast<structName*>(other); \
			if(!otherPtr) return 1; /*dynamic cast failed, types different*/ }

// continues multiFunction method, called for each member varible in struct 
#define CFG_ENTRY2(varName, defaultVal) \
	if(mfType==INIT_ALL) {varName = defaultVal;retVal++;} \
	else if(mfType==SET_STREAM && #varName==*str) { setFromStream(*streamIn,varName,*subVar);retVal++;}\
	else if(mfType==WRITE_ALL) {*streamOut<<indentBy(indent)<<#varName<<"="; \
		writeToStreamHelper(*streamOut,varName,indent); \
		*streamOut<<std::endl;retVal++; \
		if(streamOut->fail()) \
		throwError("Configurator ("+getStructName()+") error, can't write variable: "+#varName);} \
	else if(mfType==IS_EQUAL) { \
		retVal+=cfgCompareHelper(this->varName,otherPtr->varName); }	

// alternative to CFG_ENTRY2 used when default defaultVal is sufficient
#define CFG_ENTRY1(varName) CFG_ENTRY2(varName, getDefaultVal(varName))

// calls multiFunction method of parent
// allows for inheritance
#define CFG_PARENT(parentName) \
	int rc=parentName::multiFunction(mfType,str,subVar,streamIn,streamOut,indent,other); \
	retVal+=rc;
	

// closes out multiFunction method
#define CFG_TAIL return retVal; }