#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stdexcept>
#include <algorithm>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CDataType {
  public:
  virtual ~CDataType () {};
  virtual CDataType* clone () const = 0;
  virtual size_t getSize () const = 0;
  // my addition starts
  // type int = 1, double = 2, enum = 3, struct = 4;
  virtual int getType () const = 0;
  virtual void printType (ostream& oss) const = 0;
  // my addition ends
  virtual bool operator == (const CDataType & other) const {
    return (getType() == other.getType() );
  }
  virtual bool operator != ( const CDataType & other ) const {
    return (getType() != other.getType() );
  }
	friend ostream& operator<< (  ostream& out, const CDataType & rhs) {
    rhs.printType(out);
    return out; 
	}
};



class CDataTypeInt : public CDataType {
  public:
  CDataTypeInt() {};
  CDataType * clone() const override { return new CDataTypeInt(*this); }
  size_t getSize() const override { return 4; }
  int getType() const override { return 1; }
  void printType (ostream& oss) const override { oss << "int";}
};

class CDataTypeDouble : public CDataType{
  public:
  CDataTypeDouble() {};
  CDataType * clone() const override { return new CDataTypeDouble(*this); }
  size_t getSize() const override { return 8; }
  int getType() const override { return 2; }
  void printType (ostream& oss) const override { oss << "double";}

};

class CDataTypeEnum : public CDataType {
  public:
  CDataTypeEnum() {};
  CDataType * clone() const override { return new CDataTypeEnum(*this); }
  size_t getSize() const override { return 4; }
  int getType() const override { return 3; }

  bool operator == (const CDataType & other) const override {
    if ( getType() != other.getType() ) return false;
    //Check if the list of values are identical/in the same order
    CDataTypeEnum  const* x = dynamic_cast<CDataTypeEnum const *> (&other);
		if (mEnum.size() != x->mEnum.size() ) return false;

		auto lhs = mEnum.begin();
    auto rhs = x->mEnum.begin();
		for (; lhs != mEnum.end(); lhs++, rhs++) {
			if (*lhs != *rhs) return false;
		}
    return true;
  }

  bool operator != (const CDataType & other) const override {
    return !(*this == other );
  }
  
  CDataTypeEnum & add (const string & value) {
    auto it = find (mEnum.begin(), mEnum.end(), value);
    if (it != mEnum.end() ) throw invalid_argument("Duplicate enum value: " + value ); /// not sure if this will work
    mEnum.push_back(value);
    return *this;
  }

  
	void printType (ostream& oss) const override { 
		oss << "enum\n" << "{\n";
		for (auto it =  mEnum.begin(); it !=  mEnum.end(); it++) {
			oss << *it;
			if ((it + 1) == mEnum.end())  oss << "\n";
			else oss << ",\n";
		}
		oss << "}";
	}
  private:
  vector <string> mEnum;
};


class CDataTypeStruct : public CDataType {
  public:
  CDataTypeStruct() {};

  CDataTypeStruct(const CDataTypeStruct & other) {
		for (auto &it : other.fieldList) 
			fieldList.push_back(make_pair(it.first, it.second->clone() ));
		
	}
  
  ~CDataTypeStruct() {
    for (auto it = fieldList.begin(); it != fieldList.end(); it++) {
      delete (it->second);
    }
  }
  
  CDataType * clone() const override { return new CDataTypeStruct(*this); }
  size_t getSize () const override {
		size_t totalSize = 0;
		for (auto it = fieldList.begin(); it != fieldList.end(); it++) {
			totalSize += it->second->getSize();
		}
		return totalSize;
	}
  int getType() const override { return 4; }

  void printType(ostream& oss) const override {
		oss << "struct\n" << "{\n";
		for (auto it = fieldList.begin(); it != fieldList.end(); it++) {
			oss << *it->second << it->first << ";\n";
		}
		oss << "}";
	}

  CDataTypeStruct& addField (const string & name, const CDataType & type) {
		for (auto it = fieldList.begin(); it != fieldList.end(); it++) {
			if (it->first == name) throw invalid_argument("Duplicate field: " + name);
		}
    // auto * typeClone = type.clone();
    // cout <<"cloned: " << typeClone << endl;
		fieldList.push_back ( make_pair(name, type.clone() ));
    return *this;
  }
  CDataType & field (const string& name) const {
    auto it = fieldList.begin();
    for (; it < fieldList.end(); it++) {
      if (it->first == name ) break;
    }

    if (it == fieldList.end() ) throw invalid_argument("Unknown field: " + name);
    return *it->second;
  }

  bool operator == (const CDataType & other) const override {
    if ( getType() != other.getType() ) return false;
    //Check if the list of values are identical/in the same order
    CDataTypeStruct  const* x = dynamic_cast<CDataTypeStruct const *> (&other);

		if (fieldList.size() != x->fieldList.size() ) return false;

		auto lhs = fieldList.begin();
    auto rhs = x->fieldList.begin();
		for (; lhs != fieldList.end(); lhs++, rhs++) {
			if (*lhs->second != *rhs->second ) return false;
		}
    return true;
  }

  bool operator != (const CDataType & other) const override {
    return !(*this == other );
  }


  private:
  vector <pair <string, CDataType*> > fieldList;
};



#ifndef __PROGTEST__
static bool        whitespaceMatch                         ( const string    & a,
                                                             const string    & b )
{
  auto i = a.begin();
  auto j = b.begin();
  while (i != a.end()) {
    while (isspace(*i) ) i++;
    while (isspace(*j) ) j++;

    if (!*i || !*j ) return false;
    if ( *i++ != *j++ ) return false;
  }

  if ( i == a.end() && j == b.end() ) return true;
  
  return false;
}

template <typename T_>
static bool        whitespaceMatch                         ( const T_        & x,
                                                             const string    & ref )
{
  ostringstream oss;
  oss << x;
  return whitespaceMatch ( oss . str (), ref );
}
int main ( void )
{

  CDataTypeStruct  a = CDataTypeStruct () .
                        addField ( "m_Length", CDataTypeInt () ) .
                        addField ( "m_Status", CDataTypeEnum () . 
                          add ( "NEW" ) . 
                          add ( "FIXED" ) . 
                          add ( "BROKEN" ) . 
                          add ( "DEAD" ) ).
                        addField ( "m_Ratio", CDataTypeDouble () );
  
  CDataTypeStruct b = CDataTypeStruct () .
                        addField ( "m_Length", CDataTypeInt () ) .
                        addField ( "m_Status", CDataTypeEnum () . 
                          add ( "NEW" ) . 
                          add ( "FIXED" ) . 
                          add ( "BROKEN" ) . 
                          add ( "READY" ) ).
                        addField ( "m_Ratio", CDataTypeDouble () );
  
  CDataTypeStruct c =  CDataTypeStruct () .
                        addField ( "m_First", CDataTypeInt () ) .
                        addField ( "m_Second", CDataTypeEnum () . 
                          add ( "NEW" ) . 
                          add ( "FIXED" ) . 
                          add ( "BROKEN" ) . 
                          add ( "DEAD" ) ).
                        addField ( "m_Third", CDataTypeDouble () );

  CDataTypeStruct  d = CDataTypeStruct () .
                        addField ( "m_Length", CDataTypeInt () ) .
                        addField ( "m_Status", CDataTypeEnum () . 
                          add ( "NEW" ) . 
                          add ( "FIXED" ) . 
                          add ( "BROKEN" ) . 
                          add ( "DEAD" ) ).
                        addField ( "m_Ratio", CDataTypeInt () );
  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "}") );

  assert ( whitespaceMatch ( b, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "}") );

  assert ( whitespaceMatch ( c, "struct\n"
    "{\n"
    "  int m_First;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Second;\n"
    "  double m_Third;\n"
    "}") );

  assert ( whitespaceMatch ( d, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  int m_Ratio;\n"
    "}") );

  assert ( a != b );
  assert ( a == c );
  assert ( a != d );
  assert ( a . field ( "m_Status" ) == CDataTypeEnum () . add ( "NEW" ) . add ( "FIXED" ) . add ( "BROKEN" ) . add ( "DEAD" ) );
  assert ( a . field ( "m_Status" ) != CDataTypeEnum () . add ( "NEW" ) . add ( "BROKEN" ) . add ( "FIXED" ) . add ( "DEAD" ) );
  assert ( a != CDataTypeInt() );
  assert ( whitespaceMatch ( a . field ( "m_Status" ), "enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  DEAD\n"
    "}") );

  CDataTypeStruct aOld = a;
  b . addField ( "m_Other", CDataTypeDouble ());

  a . addField ( "m_Sum", CDataTypeInt ());

  assert ( a != aOld );
  assert ( a != c );
  assert ( aOld == c );
  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  int m_Sum;\n"
    "}") );

  assert ( whitespaceMatch ( b, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  double m_Other;\n"
    "}") );

  c . addField ( "m_Another", a . field ( "m_Status" ));

  assert ( whitespaceMatch ( c, "struct\n"
    "{\n"
    "  int m_First;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Second;\n"
    "  double m_Third;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Another;\n"
    "}") );

  d . addField ( "m_Another", a . field ( "m_Ratio" ));

  assert ( whitespaceMatch ( d, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  int m_Ratio;\n"
    "  double m_Another;\n"
    "}") );

  assert ( a . getSize () == 20 );
  assert ( b . getSize () == 24 );
  try
  {
    a . addField ( "m_Status", CDataTypeInt () );
    assert ( "addField: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Duplicate field: m_Status"sv );
  }

  try
  {
    cout << a . field ( "m_Fail" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Unknown field: m_Fail"sv );
  }

  try
  {
    CDataTypeEnum en;
    en . add ( "FIRST" ) .
         add ( "SECOND" ) .
         add ( "FIRST" );
    assert ( "add: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Duplicate enum value: FIRST"sv );
  }

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
