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
  virtual string getType () const = 0;
  virtual void printType (ostream& oss) const = 0;
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
  virtual CDataType& field (const string & name) const {
    throw invalid_argument("Cannot use field() for type: " + getType() );
  }
  virtual CDataType& element () const { 
    throw invalid_argument("Cannot use element() for type: " + getType() );
  } ;
  
};

// ******************************* INT *******************************************
class CDataTypeInt : public CDataType {
  public:
  CDataTypeInt() {};
  size_t getSize() const override { return 4; }
  string getType() const override { return {"int"}; }
  void printType (ostream& oss) const override { oss << "int";}
  CDataType * clone() const override { return new CDataTypeInt(*this); }

};

// ******************************* DOUBLE *******************************************
class CDataTypeDouble : public CDataType{
  public:
  CDataTypeDouble() {};
  CDataType * clone() const override { return new CDataTypeDouble(*this); }
  size_t getSize() const override { return 8; }
  string getType() const override { return ("double"); }
  void printType (ostream& oss) const override { oss << "double";}

};


// ******************************* ENUM *******************************************
class CDataTypeEnum : public CDataType {
  public:
  CDataTypeEnum() {};
  size_t getSize() const override { return 4; }
  string getType() const override { 
    string str;
    str += "enum\n{\n";
		for (auto it =  mEnum.begin(); it !=  mEnum.end(); it++) {
			str += *it;
			if ((it + 1) == mEnum.end())  { str += "\n";
			} else {str += ",\n";}
		}
		str += "}";
    return str;
  }

  bool operator == (const CDataType & other) const override {
    CDataTypeEnum  const* x = dynamic_cast<CDataTypeEnum const *> (&other);
    if (x == nullptr) return false;
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
    if (it != mEnum.end() ) throw invalid_argument("Duplicate enum value: " + value ); 
    mEnum.push_back(value);
    return *this;
  }

  
	void printType (ostream& oss) const override { oss << getType(); }
  CDataType * clone() const override { return new CDataTypeEnum(*this); }

  private:
  vector <string> mEnum;
};

// ******************************* STRUCT ****************************************
class CDataTypeStruct : public CDataType {
  public:
  CDataTypeStruct() {};

  CDataTypeStruct(const CDataTypeStruct & other) {
		for (auto &it : other.fieldList) {
			fieldList.push_back(make_pair(it.first, it.second->clone() ));
		}
	}
  
  ~CDataTypeStruct() {
    for (auto it = fieldList.begin(); it != fieldList.end(); it++) {
      delete (it->second);
    }
  }
    size_t getSize () const override {
		size_t totalSize = 0;
		for (auto it = fieldList.begin(); it != fieldList.end(); it++) {
			totalSize += it->second->getSize();
		}
		return totalSize;
	}
  string getType() const override {
    ostringstream oss;
		oss << "struct\n" << "{\n";
		for (auto it = fieldList.begin(); it != fieldList.end(); it++) {
      string str = it->second->getType();
      if ( find_arr_ptr( str) && findSpace(str) ) {
        string str1, str2;

        auto i = str.begin();
        auto iter = i;
        while (i != str.end() ) {  if (*i++ == '}') iter = i;}
        i = str.begin();
        while (i != iter ) {  str1 += *i++;}

        while (!isspace(*iter) ) {
          str1 += *iter++;
        }

        while (iter != str.end() ) {
          str2 += *iter++;
        }  
        oss << str1 << it->first << str2 <<  ";\n";    
      } else {
        oss << *it->second << it->first << ";\n";

      }
		}
		oss << "}";
    return oss.str(); 
    }

  void printType(ostream& oss) const override {
    oss << getType();
	}

  bool find_arr_ptr (const string& mType) const {
    auto i = mType.begin();
    auto it = i;
    while (i != mType.end() ) {  if (*i++ == '}') it = i;}

    while (it != mType.end() ) {  if (*it++ == '*' || *it == '[' ) return true;}
    return false;
  }

  bool findSpace (const string& mType) const {
    // size_t i = 0;
    // while (i != mType.size() && i+1 != mType.size()) {
    //   if ( mType[i++] == ' ' && mType[i--] == ' ' ) return true;
    //   i++;
    // }
    // return false;

    auto i = mType.begin();
    auto it = i;
    while (i != mType.end() ) {  if (*i++ == '}') it = i;}

    while (it != mType.end() ) {  if (isspace(*it++)) return true;}
    return false;
  }


  CDataTypeStruct& addField (const string & name, const CDataType & type) {
		for (auto it = fieldList.begin(); it != fieldList.end(); it++) {
			if (it->first == name) throw invalid_argument("Duplicate field: " + name);
		}
		fieldList.push_back ( make_pair(name, type.clone() ));
    return *this;
  }
  CDataType & field (const string& name) const override {
    
    auto it = fieldList.begin();
    for (; it < fieldList.end(); it++) {
      if (it->first == name ) break;
    }

    if (it == fieldList.end() ) throw invalid_argument("Unknown field: " + name);
    
    return *it->second;
  }

  bool operator == (const CDataType & other) const override {
    CDataTypeStruct  const* x = dynamic_cast<CDataTypeStruct const *> (&other);
    if (x == nullptr) return false;
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

  CDataType * clone() const override { return new CDataTypeStruct(*this); }

  private:
  vector <pair <string, CDataType*> > fieldList;
};


// ******************************* ARRAY ****************************************


class CDataTypeArray : public CDataType {
  public:
  size_t mSize;
  string mType;
  shared_ptr<CDataType> mData;

  CDataTypeArray ( size_t size, const CDataType& type)
  : mSize(size), mType(type.getType() ), mData(type.clone()) {
    if ( findSpace() ) {
      place();
    } else {
      mType += "   [" + to_string(mSize) + "]";
    }
  };

  void place () {
    string temp;

    auto i = mType.begin();
    auto it = i;
    while (i != mType.end() ) {  if (*i++ == '}')  it = i;}
    i = mType.begin();
    while (i != it ) {  temp += *i++;}

    while (it != mType.end() ) {
      if (isspace(*it) ) {
        temp+= "   [" + to_string(mSize) + "]";
        while (it != mType.end() ) {
          temp += *it++;   
        }
        mType = temp;
        return;
      }
      temp += *it++;
    }
  }

  bool findSpace () {
    auto i = mType.begin();
    auto it = i;
    while (i != mType.end() ) {  if (*i++ == '}') it = i;}

    while (it != mType.end() ) {  if (isspace(*it++)) return true;}
    return false;
  }

  string getType() const override { return mType;}  
  size_t getSize () const override { return (mSize * mData->getSize() ); }
  CDataType& element () const override{ return *mData; };

  void printType (ostream& oss) const override { oss << mType; }

  CDataType * clone() const override { return new CDataTypeArray(*this); }



};

//// ******************************* POINTER ****************************************
class CDataTypePtr : public CDataType { 
  public:
  string mType;
  shared_ptr<CDataType> mData;

  CDataTypePtr (const CDataType& type)
  : mType(type.getType() ), mData(type.clone() ) {
    if (countPtr()  > 1 ) {
      place("(*   )");
    } else {
      if (find_arr_ptr()) {
        place("*   ");
      } else {
      mType += "*";
      }
    }
  };

  bool find_arr_ptr () const {
    auto i = mType.begin();
    auto it = i;
    while (i != mType.end() ) {  if (*i++ == '}') it = i;}

    while (it != mType.end() ) {  if (*it++ == '*' || *it == '[' ) return true;}
    return false;
  }

  void place (const string & str) {
    string temp;

    auto i = mType.begin();
    auto it = i;
    while (i != mType.end() ) {  if (*i++ == '}')  it = i;}
    i = mType.begin();
    while (i != it ) {  temp += *i++;}

    while (it != mType.end() ) {
      if (isspace(*it) ) {
        temp += str;
        while (it != mType.end() ) {
          temp += *it++;   
        }
        mType = temp;
        return;
      }
      temp += *it++;
    }
  }

  size_t countPtr () {
    size_t count = 0;

    auto i = mType.begin();
    auto it = i;
    while (i != mType.end() ) {  if (*i++ == '}') it = i;}

    while (it != mType.end() ) {if ((*it++) == '*')  count++; }
    return count;
  }

  size_t getSize () const override { return 8; }
  CDataType& element () const override { return *mData; }
  string getType() const override { return mType; }
  void printType (ostream& oss) const override { oss << mType; }
  CDataType * clone() const override { return new CDataTypePtr(*this); }

};






// -------------------------------------------------------------------------------------//

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
  assert ( a != CDataTypeInt() );
  assert ( whitespaceMatch ( a . field ( "m_Status" ), "enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  DEAD\n"
    "}") );

  b . addField ( "m_Other", a );

  a . addField ( "m_Sum", CDataTypeInt ());

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
    "  struct\n"
    "  {\n"
    "    int m_Length;\n"
    "    enum\n"
    "    {\n"
    "      NEW,\n"
    "      FIXED,\n"
    "      BROKEN,\n"
    "      DEAD\n"
    "    } m_Status;\n"
    "    double m_Ratio;\n"
    "  } m_Other;\n"
    "}") );

  assert ( whitespaceMatch ( b . field ( "m_Other" ), "struct\n"
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

  assert ( whitespaceMatch ( b . field ( "m_Other" ) . field ( "m_Status" ), "enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  DEAD\n"
    "}") );

  assert ( a . getSize () == 20 );
  assert ( b . getSize () == 32 );
  b . addField ( "m_Other1", b );
  b . addField ( "m_Other2", b );
  b . addField ( "m_Other3", b );
  assert ( b . field ( "m_Other3" ) . field ( "m_Other2" ) . field ( "m_Other1" ) == b . field ( "m_Other1" ) );

  assert ( b . getSize () == 256);

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
    "  struct\n"
    "  {\n"
    "    int m_Length;\n"
    "    enum\n"
    "    {\n"
    "      NEW,\n"
    "      FIXED,\n"
    "      BROKEN,\n"
    "      DEAD\n"
    "    } m_Status;\n"
    "    double m_Ratio;\n"
    "  } m_Other;\n"
    "  struct\n"
    "  {\n"
    "    int m_Length;\n"
    "    enum\n"
    "    {\n"
    "      NEW,\n"
    "      FIXED,\n"
    "      BROKEN,\n"
    "      READY\n"
    "    } m_Status;\n"
    "    double m_Ratio;\n"
    "    struct\n"
    "    {\n"
    "      int m_Length;\n"
    "      enum\n"
    "      {\n"
    "        NEW,\n"
    "        FIXED,\n"
    "        BROKEN,\n"
    "        DEAD\n"
    "      } m_Status;\n"
    "      double m_Ratio;\n"
    "    } m_Other;\n"
    "  } m_Other1;\n"
    "  struct\n"
    "  {\n"
    "    int m_Length;\n"
    "    enum\n"
    "    {\n"
    "      NEW,\n"
    "      FIXED,\n"
    "      BROKEN,\n"
    "      READY\n"
    "    } m_Status;\n"
    "    double m_Ratio;\n"
    "    struct\n"
    "    {\n"
    "      int m_Length;\n"
    "      enum\n"
    "      {\n"
    "        NEW,\n"
    "        FIXED,\n"
    "        BROKEN,\n"
    "        DEAD\n"
    "      } m_Status;\n"
    "      double m_Ratio;\n"
    "    } m_Other;\n"
    "    struct\n"
    "    {\n"
    "      int m_Length;\n"
    "      enum\n"
    "      {\n"
    "        NEW,\n"
    "        FIXED,\n"
    "        BROKEN,\n"
    "        READY\n"
    "      } m_Status;\n"
    "      double m_Ratio;\n"
    "      struct\n"
    "      {\n"
    "        int m_Length;\n"
    "        enum\n"
    "        {\n"
    "          NEW,\n"
    "          FIXED,\n"
    "          BROKEN,\n"
    "          DEAD\n"
    "        } m_Status;\n"
    "        double m_Ratio;\n"
    "      } m_Other;\n"
    "    } m_Other1;\n"
    "  } m_Other2;\n"
    "  struct\n"
    "  {\n"
    "    int m_Length;\n"
    "    enum\n"
    "    {\n"
    "      NEW,\n"
    "      FIXED,\n"
    "      BROKEN,\n"
    "      READY\n"
    "    } m_Status;\n"
    "    double m_Ratio;\n"
    "    struct\n"
    "    {\n"
    "      int m_Length;\n"
    "      enum\n"
    "      {\n"
    "        NEW,\n"
    "        FIXED,\n"
    "        BROKEN,\n"
    "        DEAD\n"
    "      } m_Status;\n"
    "      double m_Ratio;\n"
    "    } m_Other;\n"
    "    struct\n"
    "    {\n"
    "      int m_Length;\n"
    "      enum\n"
    "      {\n"
    "        NEW,\n"
    "        FIXED,\n"
    "        BROKEN,\n"
    "        READY\n"
    "      } m_Status;\n"
    "      double m_Ratio;\n"
    "      struct\n"
    "      {\n"
    "        int m_Length;\n"
    "        enum\n"
    "        {\n"
    "          NEW,\n"
    "          FIXED,\n"
    "          BROKEN,\n"
    "          DEAD\n"
    "        } m_Status;\n"
    "        double m_Ratio;\n"
    "      } m_Other;\n"
    "    } m_Other1;\n"
    "    struct\n"
    "    {\n"
    "      int m_Length;\n"
    "      enum\n"
    "      {\n"
    "        NEW,\n"
    "        FIXED,\n"
    "        BROKEN,\n"
    "        READY\n"
    "      } m_Status;\n"
    "      double m_Ratio;\n"
    "      struct\n"
    "      {\n"
    "        int m_Length;\n"
    "        enum\n"
    "        {\n"
    "          NEW,\n"
    "          FIXED,\n"
    "          BROKEN,\n"
    "          DEAD\n"
    "        } m_Status;\n"
    "        double m_Ratio;\n"
    "      } m_Other;\n"
    "      struct\n"
    "      {\n"
    "        int m_Length;\n"
    "        enum\n"
    "        {\n"
    "          NEW,\n"
    "          FIXED,\n"
    "          BROKEN,\n"
    "          READY\n"
    "        } m_Status;\n"
    "        double m_Ratio;\n"
    "        struct\n"
    "        {\n"
    "          int m_Length;\n"
    "          enum\n"
    "          {\n"
    "            NEW,\n"
    "            FIXED,\n"
    "            BROKEN,\n"
    "            DEAD\n"
    "          } m_Status;\n"
    "          double m_Ratio;\n"
    "        } m_Other;\n"
    "      } m_Other1;\n"
    "    } m_Other2;\n"
    "  } m_Other3;\n"
    "}" ) );

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
    cout << a . field ( "m_Length" ) . field ( "m_Foo" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( whitespaceMatch ( e . what (), "Cannot use field() for type: int" ));
  }

  try
  {
    cout << a . field ( "m_Status" ) . field ( "m_Foo" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( whitespaceMatch ( e . what (), "Cannot use field() for type: enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  DEAD\n"
    "}" ));
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

  CDataTypeArray ar1 ( 10, CDataTypeInt () );
  assert ( whitespaceMatch ( ar1, "int[10]") );
  assert ( whitespaceMatch ( ar1 . element (), "int") );
  CDataTypeArray ar2 ( 11, ar1 );
  assert ( whitespaceMatch ( ar2, "int[11][10]") );
  assert ( whitespaceMatch ( ar2 . element (), "int[10]") );
  assert ( whitespaceMatch ( ar2 . element () . element (), "int") ); 


  
  CDataTypeArray ar3 ( 10, CDataTypeArray ( 20, CDataTypePtr ( CDataTypeInt () ) ) );
  assert ( whitespaceMatch ( ar3, "int*[10][20]") );
  assert ( whitespaceMatch ( ar3 . element (), "int*[20]") );
  assert ( whitespaceMatch ( ar3 . element () . element (), "int*") );
  assert ( whitespaceMatch ( ar3 . element () . element () . element (), "int") );
  CDataTypePtr ar4  ( ar1 . element () );
  assert ( whitespaceMatch ( ar4, "int*") );
  assert ( whitespaceMatch ( ar4 . element (), "int") );
  CDataTypePtr ar5  ( b . field ( "m_Status" ) );
  assert ( whitespaceMatch ( ar5, "enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  READY\n"
    "}*") );
  assert ( whitespaceMatch ( ar5 . element (), "enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  READY\n"
    "}") );
  CDataTypePtr ar6 ( ar3 . element () . element () );
  assert ( whitespaceMatch ( ar6, "int**") );
  assert ( whitespaceMatch ( ar6 . element (), "int*") );
  assert ( whitespaceMatch ( ar6 . element () . element (), "int") );
  CDataTypePtr ar7 ( CDataTypeArray ( 50, ar6 ) );
  assert ( whitespaceMatch ( ar7, "int**(*)[50]") );
  assert ( whitespaceMatch ( ar7 . element (), "int**[50]") );
  assert ( whitespaceMatch ( ar7 . element () . element (), "int**") );
  assert ( whitespaceMatch ( ar7 . element () . element () . element (), "int*") );
  assert ( whitespaceMatch ( ar7 . element () . element () . element () . element (), "int") );
  CDataTypeArray ar8 ( 25, ar7 );
  assert ( whitespaceMatch ( ar8, "int**(*[25])[50]") );
  CDataTypePtr ar9 ( ar8 );
  assert ( whitespaceMatch ( ar9, "int**(*(*)[25])[50]") );
  a . addField ( "m_Ar1", ar1 ) .
      addField ( "m_Ar2", ar2 ) .
      addField ( "m_Ar3", ar3 ) .
      addField ( "m_Ar4", ar4 ) .
      addField ( "m_Ar5", ar5 ) .
      addField ( "m_Ar6", ar6 ) .
      addField ( "m_Ar7", ar7 ) .
      addField ( "m_Ar8", ar8 ) .
      addField ( "m_Ar9", ar9 );
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
    "  int m_Ar1[10];\n"
    "  int m_Ar2[11][10];\n"
    "  int* m_Ar3[10][20];\n"
    "  int* m_Ar4;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  }* m_Ar5;\n"
    "  int** m_Ar6;\n"
    "  int**(* m_Ar7)[50];\n"
    "  int**(* m_Ar8[25])[50];\n"
    "  int**(*(* m_Ar9)[25])[50];\n"
    "}") );
  a . addField ( "m_Madness", CDataTypeArray ( 40, CDataTypePtr ( a ) ) );
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
    "  int m_Ar1[10];\n"
    "  int m_Ar2[11][10];\n"
    "  int* m_Ar3[10][20];\n"
    "  int* m_Ar4;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  }* m_Ar5;\n"
    "  int** m_Ar6;\n"
    "  int**(* m_Ar7)[50];\n"
    "  int**(* m_Ar8[25])[50];\n"
    "  int**(*(* m_Ar9)[25])[50];\n"
    "  struct\n"
    "  {\n"
    "    int m_Length;\n"
    "    enum\n"
    "    {\n"
    "      NEW,\n"
    "      FIXED,\n"
    "      BROKEN,\n"
    "      DEAD\n"
    "    } m_Status;\n"
    "    double m_Ratio;\n"
    "    int m_Sum;\n"
    "    int m_Ar1[10];\n"
    "    int m_Ar2[11][10];\n"
    "    int* m_Ar3[10][20];\n"
    "    int* m_Ar4;\n"
    "    enum\n"
    "    {\n"
    "      NEW,\n"
    "      FIXED,\n"
    "      BROKEN,\n"
    "      READY\n"
    "    }* m_Ar5;\n"
    "    int** m_Ar6;\n"
    "    int**(* m_Ar7)[50];\n"
    "    int**(* m_Ar8[25])[50];\n"
    "    int**(*(* m_Ar9)[25])[50];\n"
    "  }* m_Madness[40];\n"
    "}") );
  assert ( a . field ( "m_Madness" ) . element () . element () . field ( "m_Ar9" ) == a . field ( "m_Ar9" ));
  assert ( a . field ( "m_Madness" ) . element () . element () . field ( "m_Ar9" ) != a . field ( "m_Ar8" ));
  try
  {
    cout << ar2 . field ( "m_Foo" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( whitespaceMatch ( e . what (), "Cannot use field() for type: int[11][10]" ));
  }

  try
  {
    cout << c . element () << endl;
    assert ( "element: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( whitespaceMatch ( e . what (), "Cannot use element() for type: struct\n"
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
    "}" ));
  }

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
