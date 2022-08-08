#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

//=================================================================================================
// a dummy exception class, keep this implementation
class InvalidDateException : public invalid_argument
{
  public:
    InvalidDateException ( )
      : invalid_argument ( "invalid date or format" )
    {
    }
};

class CDate {
public:
    CDate (int y, int m, int d) {
        if (m > 12 || d > 31 || m <= 0 || d <= 0 || y <= 0) {
            throw InvalidDateException();
        } else if ( (m == 4 || m == 6 || m == 9 || m == 11 ) && d > 30) {
            throw  InvalidDateException();
        } else if ( (( m == 2) && d > 28 && !(leapYearCheck(y))) ||
            (( m == 2) && d > 29 && (leapYearCheck(y))) ) {
            throw  InvalidDateException();
        } else {
          this->year = y;
          this->month = m;
          this->day = d;
          
        }
    }

    // ~ CDate () {
    //     delete;
    // }

    CDate operator + (int days) {
        if (days == 0) return *this;
        return daysToDate(dateToDays()+days);
    }
    
    CDate operator - (int days) {
        if (days == 0) return *this;
        return daysToDate(dateToDays()-days);
    }

    int  operator - (CDate & rhs) {
        int left = dateToDays();
        int right = rhs.dateToDays();
        
        if ( left > right ) {
            return left - right;
        } else {
            return right - left;
        }
    }

    CDate operator ++  () {
        *this = daysToDate(dateToDays() + 1);
        return *this;
    }

    CDate operator ++ (int) {
        CDate temp = *this;
        *this =  daysToDate(dateToDays() + 1);
        return temp;
    }

    CDate operator --  () {
        *this = daysToDate(dateToDays() - 1);
        return *this;
    }

    CDate operator -- (int) {
        CDate temp = *this;
        *this =  daysToDate(dateToDays() - 1);
        return temp;
    }
    

    bool operator ==  (CDate & rhs) {
        return ( year == rhs.year && month == rhs.month && day == rhs.day); 
    }

    bool operator !=  (CDate & rhs) {
        return !(*this == rhs); 
    }

    bool operator <  (CDate & rhs) {
        if (*this == rhs) return false;

        if ( year == rhs.year) {
            if ( month == rhs.month) {
                return day < rhs.day;
            }
            return month < rhs.month;
        }
        return year < rhs.year;
    }

    bool operator <=  (CDate & rhs) {
        if (*this == rhs) return true;

        if ( year == rhs.year) {
            if ( month == rhs.month) {
                return day < rhs.day;
            }
            return month < rhs.month;
        }
        return year < rhs.year;
    }

    bool operator >  (CDate & rhs) {
        return !( *this == rhs || *this < rhs);
    }

    bool operator >=  (CDate & rhs) {
        return ( *this == rhs || *this > rhs);
    }

    int leapYearCounter (const int & year, const int month) const  {
        int y = year;
        if (month <= 2) y --;
        return (y/4) - (y/100) + (y / 400);
    }

    int dateToDays () const {
        const int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        const int daysInYear = 365;

        int daysCount = ( (year * daysInYear) + day);
        
        for (unsigned int i = 0; i < month - 1; i++) {
            daysCount += daysInMonth[i];
         }

        return daysCount += leapYearCounter(year,month);
    }

    bool leapYearCheck (int year) {
        return (( (year % 4 == 0) && (year % 100!= 0) ) || ((year % 400 == 0) )); 
    }


    CDate daysToDate (int days) {
        int daysInMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        const int daysInYear = 365;
        int firstYearsInDays = 730484;
        
        int year = 2000;
        days -= firstYearsInDays;

        
        while ( days > 365)
        {
            if ( leapYearCheck(year) ) days--;
            days -= daysInYear;
            year++;
        }
        
        int month = 1;
        int day = 0;

        if ( days == 0) {
            month = 12;
            day = 31;
            year --;
        } else {
            if ( leapYearCheck(year) ) daysInMonth[2] = 29;
            while ( days > daysInMonth[month]){
                days -= daysInMonth[month++];
            }
            day += days;
        }

        CDate node (year,month,day);
        return node;
        }

    unsigned int year, month, day;
};

bool leapYearCheck (int year) {
    return (( (year % 4 == 0) && (year % 100!= 0) ) || ((year % 400 == 0) )); 
}

std:: ostream&  operator << (std:: ostream& out , CDate & other) {
    out << other.year << "-" << std::setfill ('0') << std::setw (2) << other.month <<"-"<< std::setfill ('0') << std::setw (2) << other.day;

    return out;
}


std:: istream&  operator >> (std::istream& in , CDate & other) {
    int m_year, m_month, m_day;
    char sep1;
    char sep2;
    
    if (!(in >> m_year >> sep1 >> m_month >> sep2 >> m_day))  {
      in.setstate(std::ios_base::failbit);
      return in;
    }
    
    if (sep1 != '-' || sep2 != '-' || m_month > 12 ||m_month <= 0 || m_day > 31 || m_day <= 0 || m_year <= 0) {
        in.setstate(std::ios_base::failbit);
        return in;

    }

    if ( (m_month == 4 || m_month == 6 || m_month == 9 || m_month == 11 ) && m_day > 30) {
        in.setstate(std::ios_base::failbit);
        return in;
    } 

    if ( (( m_month == 2 && !(leapYearCheck(m_year))) && m_day > 28 ) ||
        (( m_month == 2  && (leapYearCheck(m_year))) && m_day > 29) ) {
        in.setstate(std::ios_base::failbit);
        return in;
    }

    other.year = m_year;
    other.month = (m_month);
    other.day = (m_day);

    return in;
}


//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
ios_base & ( * date_format ( const char * fmt ) ) ( ios_base & x )
{
  return [] ( ios_base & ios ) -> ios_base & { return ios; };
}
//=================================================================================================

#ifndef __PROGTEST__
int main ( void )
{
  ostringstream oss;
  istringstream iss;

  CDate a ( 2000, 1, 2 );
  CDate b ( 2010, 2, 3 );
  CDate c ( 2004, 2, 10 );
  oss . str ("");
  oss << a;
  assert ( oss . str () == "2000-01-02" );
  oss . str ("");
  oss << b;
  assert ( oss . str () == "2010-02-03" );
  oss . str ("");
  oss << c;
  assert ( oss . str () == "2004-02-10" );
  a = a + 1500;
  oss . str ("");
  oss << a;
  assert ( oss . str () == "2004-02-10" );
  b = b - 2000;
  oss . str ("");
  oss << b;
  assert ( oss . str () == "2004-08-13" );
  assert ( b - a == 185 );
  assert ( ( b == a ) == false );
  assert ( ( b != a ) == true );
  assert ( ( b <= a ) == false );
  assert ( ( b < a ) == false );
  assert ( ( b >= a ) == true );
  assert ( ( b > a ) == true );
  assert ( ( c == a ) == true );
  assert ( ( c != a ) == false );
  assert ( ( c <= a ) == true );
  assert ( ( c < a ) == false );
  assert ( ( c >= a ) == true );
  assert ( ( c > a ) == false );
  a = ++c;
  oss . str ( "" );
  oss << a << " " << c;
  assert ( oss . str () == "2004-02-11 2004-02-11" );
  a = --c;
  oss . str ( "" );
  oss << a << " " << c;
  assert ( oss . str () == "2004-02-10 2004-02-10" );
  a = c++;
  oss . str ( "" );
  oss << a << " " << c;
  assert ( oss . str () == "2004-02-10 2004-02-11" );
  a = c--;
  oss . str ( "" );
  oss << a << " " << c;
  assert ( oss . str () == "2004-02-11 2004-02-10" );
  iss . clear ();
  iss . str ( "2015-09-03" );
  assert ( ( iss >> a ) );
  oss . str ("");
  oss << a;
  assert ( oss . str () == "2015-09-03" );
  a = a + 70;
  oss . str ("");
  oss << a;
  assert ( oss . str () == "2015-11-12" );

  CDate d ( 2000, 1, 1 );
  try
  {
    CDate e ( 2000, 32, 1 );
    assert ( "No exception thrown!" == nullptr );
  }
  catch ( ... )
  {
  }
  iss . clear ();
  iss . str ( "2000-12-33" );
  assert ( ! ( iss >> d ) );
  oss . str ("");
  oss << d;
  assert ( oss . str () == "2000-01-01" );
  iss . clear ();
  iss . str ( "2000-11-31" );
  assert ( ! ( iss >> d ) );
  oss . str ("");
  oss << d;
  assert ( oss . str () == "2000-01-01" );
  iss . clear ();
  iss . str ( "2000-02-29" );
  assert ( ( iss >> d ) );
  oss . str ("");
  oss << d;
  assert ( oss . str () == "2000-02-29" );
  iss . clear ();
  iss . str ( "2001-02-29" );
  assert ( ! ( iss >> d ) );
  oss . str ("");
  oss << d;
  assert ( oss . str () == "2000-02-29" );

  // //-----------------------------------------------------------------------------
  // // bonus test examples
  // //-----------------------------------------------------------------------------
  // CDate f ( 2000, 5, 12 );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // oss . str ("");
  // oss << date_format ( "%Y/%m/%d" ) << f;
  // assert ( oss . str () == "2000/05/12" );
  // oss . str ("");
  // oss << date_format ( "%d.%m.%Y" ) << f;
  // assert ( oss . str () == "12.05.2000" );
  // oss . str ("");
  // oss << date_format ( "%m/%d/%Y" ) << f;
  // assert ( oss . str () == "05/12/2000" );
  // oss . str ("");
  // oss << date_format ( "%Y%m%d" ) << f;
  // assert ( oss . str () == "20000512" );
  // oss . str ("");
  // oss << date_format ( "hello kitty" ) << f;
  // assert ( oss . str () == "hello kitty" );
  // oss . str ("");
  // oss << date_format ( "%d%d%d%d%d%d%m%m%m%Y%Y%Y%%%%%%%%%%" ) << f;
  // assert ( oss . str () == "121212121212050505200020002000%%%%%" );
  // oss . str ("");
  // oss << date_format ( "%Y-%m-%d" ) << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-01-1" );
  // assert ( ! ( iss >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-1-01" );
  // assert ( ! ( iss >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-001-01" );
  // assert ( ! ( iss >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-01-02" );
  // assert ( ( iss >> date_format ( "%Y-%m-%d" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2001-01-02" );
  // iss . clear ();
  // iss . str ( "05.06.2003" );
  // assert ( ( iss >> date_format ( "%d.%m.%Y" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2003-06-05" );
  // iss . clear ();
  // iss . str ( "07/08/2004" );
  // assert ( ( iss >> date_format ( "%m/%d/%Y" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2004-07-08" );
  // iss . clear ();
  // iss . str ( "2002*03*04" );
  // assert ( ( iss >> date_format ( "%Y*%m*%d" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2002-03-04" );
  // iss . clear ();
  // iss . str ( "C++09format10PA22006rulez" );
  // assert ( ( iss >> date_format ( "C++%mformat%dPA2%Yrulez" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2006-09-10" );
  // iss . clear ();
  // iss . str ( "%12%13%2010%" );
  // assert ( ( iss >> date_format ( "%%%m%%%d%%%Y%%" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2010-12-13" );

  // CDate g ( 2000, 6, 8 );
  // iss . clear ();
  // iss . str ( "2001-11-33" );
  // assert ( ! ( iss >> date_format ( "%Y-%m-%d" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "29.02.2003" );
  // assert ( ! ( iss >> date_format ( "%d.%m.%Y" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "14/02/2004" );
  // assert ( ! ( iss >> date_format ( "%m/%d/%Y" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "2002-03" );
  // assert ( ! ( iss >> date_format ( "%Y-%m" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "hello kitty" );
  // assert ( ! ( iss >> date_format ( "hello kitty" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "2005-07-12-07" );
  // assert ( ! ( iss >> date_format ( "%Y-%m-%d-%m" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "20000101" );
  // assert ( ( iss >> date_format ( "%Y%m%d" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-01-01" );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
