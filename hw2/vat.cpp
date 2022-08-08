#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream> 
#include <iomanip> 
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CVATRegister
{
  public:

    class Company {
    public:
        string name;
        string addr;
        string taxID;
        string nameOrg;
        string addrOrg;
        int income = 0;
        Company (string compName, string compAddr, string comTaxID, string nameOriginal, string addrOriginal) 
        :name(compName), addr(compAddr), taxID(comTaxID), nameOrg(nameOriginal), addrOrg(addrOriginal) {}
        
        Company (string compName, string compAddr) 
        :name(compName), addr(compAddr) {}
        
        Company (string comTaxID) 
        :taxID(comTaxID) {}

        bool operator < ( const Company & rhs) const {
            if ( name == rhs.name) {
                if (addr < rhs.addr) {
                return true;
                } else {
                return false;
                }
            }
            return name < rhs.name;
        }
    };




                  // CVATRegister   ( void );
                  ~CVATRegister  ( void ) {
                    for (size_t i = 0 ; i < m_Company.size(); i ++) {
                      delete m_Company[i];
                    }
                  };
    bool          newCompany     ( const string    & name,
                                   const string    & addr,
                                   const string    & taxID );
    bool          cancelCompany  ( const string    & name,
                                   const string    & addr );
    bool          cancelCompany  ( const string    & taxID );
    bool          invoice        ( const string    & taxID,
                                   unsigned int      amount );
    bool          invoice        ( const string    & name,
                                   const string    & addr,
                                   unsigned int      amount );
    bool          audit          ( const string    & name,
                                   const string    & addr,
                                   unsigned int    & sumIncome ) const;
    bool          audit          ( const string    & taxID,
                                   unsigned int    & sumIncome ) const;
    bool          firstCompany   ( string          & name,
                                   string          & addr ) const;
    bool          nextCompany    ( string          & name,
                                   string          & addr ) const;
    unsigned int  medianInvoice  ( void ) const;
    

    bool addViaTaxID ( Company * node);
    
  private:
    vector <Company*> m_Company;
    vector  <Company*> m_CompanyTax;
    vector <unsigned int> m_Invoice;

};

bool CVATRegister:: addViaTaxID (Company * node) {

  auto comparator = [](const Company* lhs, const Company* rhs) {
      return lhs->taxID < rhs->taxID;
  };

  auto nodePos = lower_bound(m_CompanyTax.begin(), m_CompanyTax.end(), node, comparator);

  if ( nodePos == m_CompanyTax.end() ) {
    m_CompanyTax.push_back(node);
  } else if ((*nodePos)->taxID != node->taxID ) {
    m_CompanyTax.insert(nodePos, node);
  } else {
    return false;
  }

  return true;
}

bool CVATRegister:: newCompany (const string & name, const string & addr, const string & taxID) {
  string compName = name;
  string compAddr = addr;
  transform ( compName.begin(), compName.end(), compName.begin(), ::tolower);
  transform ( compAddr.begin(), compAddr.end(), compAddr.begin(), ::tolower);

  Company* node = new Company(compName,compAddr, taxID, name, addr );



  auto comparator = [](const Company* lhs, const Company* rhs) 
  {
      return *lhs < *rhs;
  };

  auto nodePos = lower_bound(m_Company.begin(), m_Company.end(),node, comparator);

  if ( nodePos == m_Company.end()) {
    m_Company.push_back(node);
  } else if ( (*nodePos)->name != node->name || (*nodePos)->addr != node->addr) {
    m_Company.insert(nodePos, node);
  } else {
    delete node;
    return false;
  }
  addViaTaxID(node);
  return true;
}

bool CVATRegister:: cancelCompany  ( const string    & name, const string    & addr ) {
  string compName = name;
  string compAddr = addr;
  transform ( compName.begin(), compName.end(), compName.begin(), ::tolower);
  transform ( compAddr.begin(), compAddr.end(), compAddr.begin(), ::tolower);

  Company* node = new Company(compName, compAddr);
  
  auto comparator = [](const Company* lhs, const Company* rhs) 
  {
      return *lhs < *rhs;
  };
  
  auto nodePos = lower_bound(m_Company.begin(), m_Company.end(),node, comparator);

  if (nodePos == m_CompanyTax.end() || (*nodePos)->name != node->name || (*nodePos)->addr != node->addr) {
    delete node;
    return false;
  } else {
    delete node;
    delete (*nodePos);
    m_Company.erase (nodePos);
    m_CompanyTax = m_Company;
    sort(m_CompanyTax.begin(), m_CompanyTax.end(), [](Company* a, Company* b) { 
    return a->taxID < b->taxID;
    });
  }
  return true;
}

bool CVATRegister:: invoice ( const string & name, const string & addr, unsigned int amount ){
  string compName = name;
  string compAddr = addr;
  transform ( compName.begin(), compName.end(), compName.begin(), ::tolower);
  transform ( compAddr.begin(), compAddr.end(), compAddr.begin(), ::tolower);

  Company* node = new Company(compName, compAddr);
  
  auto comparator = [](const Company* lhs, const Company* rhs) 
  {
      return *lhs < *rhs;
  };
  auto nodePos = lower_bound(m_Company.begin(), m_Company.end(),node, comparator);

  if (nodePos == m_CompanyTax.end() || (*nodePos)->name != node->name || (*nodePos)->addr != node->addr) {
    delete node;
    return false;
  } else {
    delete node;
    (*nodePos)->income += amount;
    m_Invoice.push_back(amount);
  }
  return true;
}



bool CVATRegister:: cancelCompany  ( const string  & taxID ) {
  Company* node = new Company(taxID);
  
  auto comparator = [](const Company* lhs, const Company* rhs) 
  {
      return lhs->taxID < rhs->taxID;
  };

  auto nodePos = lower_bound(m_CompanyTax.begin(), m_CompanyTax.end(),node, comparator);

  if (nodePos == m_CompanyTax.end() || (*nodePos)->taxID != node->taxID) {
    delete node;
    return false;
  } else {
    delete node;
    delete (*nodePos);
    m_CompanyTax.erase (nodePos);
    m_Company = m_CompanyTax;
    sort(m_Company.begin(), m_Company.end(), [](Company* a, Company* b) { return *a < *b;});

  }
  return true;
}


bool CVATRegister:: invoice ( const string & taxID, unsigned int  amount ) {
  Company* node = new Company(taxID);
  
  auto comparator = [](const Company* lhs, const Company* rhs) 
  {
      return lhs->taxID < rhs->taxID;
  };

  auto nodePos = lower_bound(m_CompanyTax.begin(), m_CompanyTax.end(),node, comparator);

  if (nodePos == m_CompanyTax.end() || (*nodePos)->taxID != node->taxID) {
    delete node;
    return false;
  } else {
    delete node;
    (*nodePos)->income += amount;
    m_Invoice.push_back(amount);
  }
  return true;
}


bool CVATRegister:: audit ( const string & name, const string & addr, unsigned int & sumIncome ) const{
  string compName = name;
  string compAddr = addr;
  transform ( compName.begin(), compName.end(), compName.begin(), ::tolower);
  transform ( compAddr.begin(), compAddr.end(), compAddr.begin(), ::tolower);

  Company* node = new Company(compName, compAddr);
  
  auto comparator = [](const Company* lhs, const Company* rhs) 
  {
      return *lhs < *rhs;
  };
  auto nodePos = lower_bound(m_Company.begin(), m_Company.end(),node, comparator);

  if (nodePos == m_Company.end() || (*nodePos)->name != node->name || (*nodePos)->addr != node->addr) {
    delete node;
    return false;
  } else {
    delete node;
    sumIncome = (*nodePos)->income;
  }
  return true;
}

bool CVATRegister:: audit ( const string & taxID, unsigned int & sumIncome ) const {
  Company* node = new Company(taxID);
  
  auto comparator = [](const Company* lhs, const Company* rhs) 
  {
      return lhs->taxID < rhs->taxID;
  };

  auto nodePos = lower_bound(m_CompanyTax.begin(), m_CompanyTax.end(),node, comparator);

  if (nodePos == m_CompanyTax.end() || (*nodePos)->taxID != node->taxID) {
    delete node;
    return false;
  } else {
    delete node;
    sumIncome = (*nodePos)->income;
  }
  return true;
}


unsigned int CVATRegister:: medianInvoice  ( void ) const {
  std::vector<unsigned int> m_Median ( m_Invoice );
  sort ( m_Median.begin(), m_Median.end());
  
  size_t size = m_Median.size();
  unsigned int median = 0;

  if ( size != 0 ) {
    if (size % 2 == 0) {
      unsigned int x = m_Median[(size/2)-1];
      unsigned int y = m_Median[(size/2)];
      x > y ? median = x : median = y;
    } else {
      median = m_Median[size / 2];
    }
  } else {
      median = 0;
  }
  return median;
}


bool CVATRegister:: firstCompany   ( string   & name, string   & addr ) const {
  if (m_Company.empty()) {
    return false;
  } else {
    name = m_Company[0]->nameOrg;
    addr = m_Company[0]->addrOrg;
    return true;
  }
}

bool CVATRegister:: nextCompany ( string & name, string  & addr ) const {
  string compName = name;
  string compAddr = addr;
  transform ( compName.begin(), compName.end(), compName.begin(), ::tolower);
  transform ( compAddr.begin(), compAddr.end(), compAddr.begin(), ::tolower);

  Company* node = new Company(compName, compAddr);
  
  auto comparator = [](const Company* lhs, const Company* rhs) 
  {
      return *lhs < *rhs;
  };
  auto nodePos = lower_bound(m_Company.begin(), m_Company.end(),node, comparator);

  if (nodePos == m_Company.end() || (nodePos+1) == m_Company.end() || (*nodePos)->name != node->name || (*nodePos)->addr != node->addr) {
    delete node;
    return false;
  } else {
    delete node;
    name = (*(nodePos+1))->nameOrg;
    addr = (*(nodePos+1))->addrOrg;
  }
  return true;
}


#ifndef __PROGTEST__
int               main           ( void )
{
  string name, addr;
  unsigned int sumIncome;

  CVATRegister b1;
  assert ( b1 . newCompany ( "ACME", "Thakurova", "666/666" ) );
  assert ( b1 . newCompany ( "ACME", "Kolejni", "666/666/666" ) );
  assert ( b1 . newCompany ( "Dummy", "Thakurova", "123456" ) );
  assert ( b1 . invoice ( "666/666", 2000 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "666/666/666", 3000 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "123456", 4000 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "aCmE", "Kolejni", 5000 ) );
  assert ( b1 . medianInvoice () == 4000 );
  assert ( b1 . audit ( "ACME", "Kolejni", sumIncome ) && sumIncome == 8000 );
  assert ( b1 . audit ( "123456", sumIncome ) && sumIncome == 4000 );
  assert ( b1 . firstCompany ( name, addr ) && name == "ACME" && addr == "Kolejni" );
  assert ( b1 . nextCompany ( name, addr ) && name == "ACME" && addr == "Thakurova" );
  assert ( b1 . nextCompany ( name, addr ) && name == "Dummy" && addr == "Thakurova" );
  assert ( ! b1 . nextCompany ( name, addr ) );
  assert ( b1 . cancelCompany ( "ACME", "KoLeJnI" ) );
  assert ( b1 . medianInvoice () == 4000 );
  assert ( b1 . cancelCompany ( "666/666" ) );
  assert ( b1 . medianInvoice () == 4000 );
  assert ( b1 . invoice ( "123456", 100 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "123456", 300 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "123456", 200 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "123456", 230 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "123456", 830 ) );
  assert ( b1 . medianInvoice () == 830 );
  assert ( b1 . invoice ( "123456", 1830 ) );
  assert ( b1 . medianInvoice () == 1830 );
  assert ( b1 . invoice ( "123456", 2830 ) );
  assert ( b1 . medianInvoice () == 1830 );
  assert ( b1 . invoice ( "123456", 2830 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "123456", 3200 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . firstCompany ( name, addr ) && name == "Dummy" && addr == "Thakurova" );
  assert ( ! b1 . nextCompany ( name, addr ) );
  assert ( b1 . cancelCompany ( "123456" ) );
  assert ( ! b1 . firstCompany ( name, addr ) );

  CVATRegister b2;
  assert ( b2 . newCompany ( "ACME", "Kolejni", "abcdef" ) );
  assert ( b2 . newCompany ( "Dummy", "Kolejni", "123456" ) );
  assert ( ! b2 . newCompany ( "AcMe", "kOlEjNi", "1234" ) );
  assert ( b2 . newCompany ( "Dummy", "Thakurova", "ABCDEF" ) );
  assert ( b2 . medianInvoice () == 0 );
  assert ( b2 . invoice ( "ABCDEF", 1000 ) );
  assert ( b2 . medianInvoice () == 1000 );
  assert ( b2 . invoice ( "abcdef", 2000 ) );
  assert ( b2 . medianInvoice () == 2000 );
  assert ( b2 . invoice ( "aCMe", "kOlEjNi", 3000 ) );
  assert ( b2 . medianInvoice () == 2000 );
  assert ( ! b2 . invoice ( "1234567", 100 ) );
  assert ( ! b2 . invoice ( "ACE", "Kolejni", 100 ) );
  assert ( ! b2 . invoice ( "ACME", "Thakurova", 100 ) );
  assert ( ! b2 . audit ( "1234567", sumIncome ) );
  assert ( ! b2 . audit ( "ACE", "Kolejni", sumIncome ) );
  assert ( ! b2 . audit ( "ACME", "Thakurova", sumIncome ) );
  assert ( ! b2 . cancelCompany ( "1234567" ) );
  assert ( ! b2 . cancelCompany ( "ACE", "Kolejni" ) );
  assert ( ! b2 . cancelCompany ( "ACME", "Thakurova" ) );
  assert ( b2 . cancelCompany ( "abcdef" ) );
  assert ( b2 . medianInvoice () == 2000 );
  assert ( ! b2 . cancelCompany ( "abcdef" ) );
  assert ( b2 . newCompany ( "ACME", "Kolejni", "abcdef" ) );
  assert ( b2 . cancelCompany ( "ACME", "Kolejni" ) );
  assert ( ! b2 . cancelCompany ( "ACME", "Kolejni" ) );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
