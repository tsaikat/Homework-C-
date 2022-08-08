#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CDate
{   
  int mDate;
  public:
    CDate ( int y, int m, int d )
    : mDate (10000 * y + 100 * m + d) {
    }
    int operator () () const {
      return mDate;
    }
};

struct pairs
{
  int date;
  int count;

  pairs ( int & d, int & c)
  : date(d), count(c) {}
};

class CProduct { 
  public:
  string mName;
  int mLen;
  map <int , pairs*> mStorage;
  CProduct (string& name, int & date, int & cnt) 
  : mName(name), mLen(name.length()) {
    pairs* node = new pairs(date, cnt);
    mStorage.emplace(date, node);
  }
  
  ~ CProduct () {
    for (auto &pos : mStorage) {
      delete pos.second;
    }
  }
   
};

class CSupermarket
{
  public:
    // default constructor

  ~ CSupermarket () {
    for (auto &pos : mProducts) {
      delete pos.second;
    }
  }

    CSupermarket () {}
    CSupermarket & store ( string name, CDate expiryDate, int count ) {
      int expDate = expiryDate();
      auto product = mProducts.find(name);
      //if product already exist and not
      if( product != mProducts.end() ) {
        if ( product->second->mStorage.find(expDate) != product->second->mStorage.end() ) {
            product->second->mStorage[expDate]->count += count;
        } else {
            pairs* node = new pairs (expDate, count);
            product->second->mStorage.emplace(expDate, node);
        }
      } else {
          CProduct* node = new CProduct(name, expDate, count);
          mProducts.emplace(name, node);
          // mProducts[name] = node;
      }
      return *this;
    }

    bool hammingDist(string &str1, string &str2) const {
        int i = 0, count = 0;

        while (str1[i] != '\0') {
            if (str1[i] != str2[i]) count++;
            if (count > 1) return false;
            i++;
        }
        return count == 1; // can also just return true let's see
    }
    

    // CProduct * findProduct (string & name ) {
    //   auto product = mProducts.find(name);
    //   if ( product != mProducts.end() ) {
    //     return product->second;
    //   } else {
    //     int typoMatch = 0;
    //     CProduct* matchedProduct = nullptr;
    //     string str;
    //     // search for posible match with typo
    //     for ( auto i = mProducts.begin(); i != mProducts.end(); i++) {
    //       str = i->first;
    //       if (str.length() == name.length()) {
    //         if (hammingDist(str, name) ) {
    //           typoMatch++;
    //           if (typoMatch > 1) return nullptr;
    //           matchedProduct = i->second; 
    //         }
    //       }
    //     }
    //     return matchedProduct;
    //   }
    // }

    CProduct * findProduct (string & name ) {
      auto product = mProducts.find(name);
      if ( product != mProducts.end() ) {
        return product->second;
      } else {
        int typoMatch = 0;
        CProduct* matchedProduct = nullptr;
        string str;
        int nameLen = name.length();
        // search for posible match with typo
        for ( auto i = mProducts.begin(); i != mProducts.end(); i++) {
          str = i->first;
          if (i->second->mLen == nameLen) {
            if (hammingDist(str, name) ) {
              typoMatch++;
              if (typoMatch > 1) return nullptr;
              matchedProduct = i->second; 
            }
          }
        }
        return matchedProduct;
      }
    }

    void sell ( list<pair<string,int> > & shoppingList ) {
      vector <CProduct*> products;
      for ( auto iterList = shoppingList.begin(); iterList != shoppingList.end(); iterList++) {
        products.push_back(findProduct( iterList->first ) );
      }

      int iter = 0;
      for ( auto sList = shoppingList.begin(); sList != shoppingList.end(); ) { 
        CProduct * product =  products[iter++];
        if ( product != nullptr ) {
          // sell them and remove from storeage and shopping list
          auto storage = product->mStorage;
          if (storage.empty() ) {
            sList++;
            continue;
          }
          for ( auto i = storage.begin(); i != storage.end(); ) { 
            
            if (i->second->count < sList->second) {
              sList->second -= i->second->count;
              i->second->count = 0; // need more thoughts here. I don't know why the erase isn't working properly and i need this
              i = storage.erase(i); //poping the empty date and count from the product >> PROBLEM!!
              if ( i == storage.end() ) {
                mProducts.erase (product->mName);
                delete (product);
                sList++;
                break;
              }
              continue;
            } else if ( i->second->count > sList->second) {
              i->second->count -= sList->second;
              sList = shoppingList.erase(sList);
              break;
            } else {
              i = storage.erase(i);
              sList = shoppingList.erase(sList);
              break;
            }

          }
        } else {
          sList++;
        }
      }
      
    }


    list<pair<string,int>> expired ( CDate date ) const {
      int endDate = date();
      map <string, int > expiredList;
      for (auto i = mProducts.begin(); i != mProducts.end(); i++) {
         for (auto j = i->second->mStorage.begin(); j != i->second->mStorage.end(); j++) {
           if (j->first < endDate) {
             expiredList[i->first] += j->second->count;
           } else {
             break;
           }
         }
        }
         list<pair<string,int> > finalList;
        for ( auto i = expiredList.begin(); i != expiredList.end(); i++) {
          finalList.emplace_back(make_pair((i->first), (i->second) ));
        }

        finalList.sort([](auto const& a, auto const& b) {
          return a.second > b.second;
          });

          return finalList;

    }

  private:
    unordered_map < string, CProduct* > mProducts;
    // map < int, CProduct* > mProducts;
};

#ifndef __PROGTEST__
int main ( void )
{
  CSupermarket s;
  s . store ( "bread", CDate ( 2016, 4, 30 ), 100 )
    . store ( "butter", CDate ( 2016, 5, 10 ), 10 )
    . store ( "beer", CDate ( 2016, 8, 10 ), 50 )
    . store ( "bread", CDate ( 2016, 4, 25 ), 100 )
    . store ( "okey", CDate ( 2016, 7, 18 ), 5 );

  list<pair<string,int> > l0 = s . expired ( CDate ( 2018, 4, 30 ) );
  assert ( l0 . size () == 4 );
  assert ( ( l0 == list<pair<string,int> > { { "bread", 200 }, { "beer", 50 }, { "butter", 10 }, { "okey", 5 } } ) );

  list<pair<string,int> > l1 { { "bread", 2 }, { "Coke", 5 }, { "butter", 20 } };
  s . sell ( l1 );
  assert ( l1 . size () == 2 );
  assert ( ( l1 == list<pair<string,int> > { { "Coke", 5 }, { "butter", 10 } } ) );

  list<pair<string,int> > l2 = s . expired ( CDate ( 2016, 4, 30 ) );
  assert ( l2 . size () == 1 );
  assert ( ( l2 == list<pair<string,int> > { { "bread", 98 } } ) );

  list<pair<string,int> > l3 = s . expired ( CDate ( 2016, 5, 20 ) );
  assert ( l3 . size () == 1 );
  assert ( ( l3 == list<pair<string,int> > { { "bread", 198 } } ) );

  list<pair<string,int> > l4 { { "bread", 105 } };
  s . sell ( l4 );
  assert ( l4 . size () == 0 );
  assert ( ( l4 == list<pair<string,int> > {  } ) );

  list<pair<string,int> > l5 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l5 . size () == 3 );
  assert ( ( l5 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 } } ) );

  s . store ( "Coke", CDate ( 2016, 12, 31 ), 10 );

  list<pair<string,int> > l6 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 }, { "Cokes", 1 } };
  s . sell ( l6 );
  assert ( l6 . size () == 3 );
  assert ( ( l6 == list<pair<string,int> > { { "cake", 1 }, { "cuke", 1 }, { "Cokes", 1 } } ) );

  list<pair<string,int> > l7 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l7 . size () == 4 );
  assert ( ( l7 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 7 }, { "okey", 5 } } ) );

  s . store ( "cake", CDate ( 2016, 11, 1 ), 5 );

  list<pair<string,int> > l8 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 } };
  s . sell ( l8 );
  assert ( l8 . size () == 2 );
  assert ( ( l8 == list<pair<string,int> > { { "Cake", 1 }, { "coke", 1 } } ) );

  list<pair<string,int> > l9 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l9 . size () == 5 );
  assert ( ( l9 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 }, { "cake", 3 } } ) );

  list<pair<string,int> > l10 { { "cake", 15 }, { "Cake", 2 } };
  s . sell ( l10 );
  assert ( l10 . size () == 2 );
  assert ( ( l10 == list<pair<string,int> > { { "cake", 12 }, { "Cake", 2 } } ) );

  list<pair<string,int> > l11 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l11 . size () == 4 );
  assert ( ( l11 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 } } ) );

  list<pair<string,int> > l12 { { "Cake", 4 } };
  s . sell ( l12 );
  assert ( l12 . size () == 0 );
  assert ( ( l12 == list<pair<string,int> > {  } ) );

  list<pair<string,int> > l13 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l13 . size () == 4 );
  assert ( ( l13 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 }, { "Coke", 2 } } ) );

  list<pair<string,int> > l14 { { "Beer", 20 }, { "Coke", 1 }, { "bear", 25 }, { "beer", 10 } };
  s . sell ( l14 );
  assert ( l14 . size () == 1 );
  assert ( ( l14 == list<pair<string,int> > { { "beer", 5 } } ) );

  s . store ( "ccccb", CDate ( 2019, 3, 11 ), 100 )
    . store ( "ccccd", CDate ( 2019, 6, 9 ), 100 )
    . store ( "dcccc", CDate ( 2019, 2, 14 ), 100 );

  list<pair<string,int> > l15 { { "ccccc", 10 } };
  s . sell ( l15 );
  assert ( l15 . size () == 1 );
  assert ( ( l15 == list<pair<string,int> > { { "ccccc", 10 } } ) );



  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
