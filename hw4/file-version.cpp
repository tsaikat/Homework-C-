#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <iostream>
using namespace std;
#endif /* __PROGTEST__ */


/*  CData
    it stores the actual data in a vector like container &
    handles the memory deallocation like a shared pointers */

class CData {
 public:
    CData ();
    ~ CData ();
    void removeRef();
    void addRef();
    uint32_t getRef();
    bool isFree();
    CData & operator = ( const CData & other);
    uint8_t& operator [] ( const uint32_t index) const;
    void push_back(const uint8_t data);
    void insert (const uint32_t index, const uint8_t data);
    void truncate(const uint32_t index);
    void pop_back();
    uint32_t size() const;
	CData& deepCopy (const CData & other, uint32_t pos);
 
 private:
    uint8_t* arr;
    uint32_t capacity, mSize, ref;
};

    CData::CData () : arr (new uint8_t[1]), capacity(1), mSize (0), ref (1)	{}
    CData::~ CData () { delete[] arr; }
    void CData::removeRef() { ref--;}
    void CData::addRef() { ref++;}
    uint32_t CData::getRef() {return ref;}
    bool CData::isFree() { return --ref == 0;}
    CData& CData::operator = ( const CData & other) {
        if (this == &other) return *this;
        uint8_t* temp = new uint8_t [other.mSize];
        memcpy(temp, other.arr, sizeof(other.arr[0]) * other.mSize);
        capacity = other.mSize;
        mSize = other.mSize;
        delete[] arr;
        arr = temp;
        return *this;
    }

	CData& CData::deepCopy (const CData & other, uint32_t pos) {
		if (this == &other) return *this;
        uint8_t* temp = new uint8_t [pos];
        memcpy(temp, other.arr, sizeof(other.arr[0]) * pos);
        capacity = pos;
        mSize = pos;
        delete[] arr;
        arr = temp;
        return *this;
	}




    uint8_t& CData::operator [] ( const uint32_t index) const { return arr[index]; }
    void CData::push_back(const uint8_t data) {
        if (mSize == capacity) {
			uint32_t size = (capacity * 1.2) + 1;
			cout << size << endl;
            uint8_t* temp = new uint8_t[size];
            memcpy(temp, arr, sizeof(arr[0]) * mSize);
            delete[] arr;
            capacity  = size;
            arr = temp;
        }
        arr[mSize++] = data;
    }
	void CData::insert (const uint32_t index, const uint8_t data) {
		if (index == capacity || index == mSize) {
			push_back(data);
		} else {
			arr[index] = data;
        }
	}
    void CData::truncate(const uint32_t index) {mSize = index; }
	void CData::pop_back() { mSize--; }
	uint32_t CData::size() const { return mSize; }

/*  snapShots
    It stores the snapshots of the file in a singly linkedlist */

class snapShots {
 public:
    snapShots();
    ~snapShots();
    void addRef();
    bool isFree();
    bool empty();
    void clear();
	
	
	// Veribles
	uint32_t pos, ref, fSize;
    CData * data;
    snapShots* next;
};
    snapShots::snapShots () : ref (1) { next = nullptr; }
    snapShots::~snapShots () {}
    void snapShots::addRef () {
        ref++;
        data->addRef();
    }
    bool snapShots::isFree() { 
        if (data->isFree() ) delete data;
        return --ref == 0;
    }
    bool snapShots::empty() { return --ref == 0; }
    void snapShots::clear () {
        if (next != nullptr) {
            next->clear();
            if (next->isFree() ) delete next;
        }
        return;
    }

/*  CFile
    This class simulates a file. writes/reads stors/undo file versions */
        
class CFile {
  public:
    CFile ( void );    
    ~ CFile ( void );
    void clear ();
    CFile ( const CFile & other );
    CFile & operator= ( CFile & other);
    bool seek ( uint32_t offset );
    uint32_t read ( uint8_t * dst, uint32_t bytes );
    uint32_t write ( const uint8_t * src, uint32_t bytes );
    void truncate ( void );
    uint32_t fileSize ( void ) const;
    void addVersion ( void );
    bool undoVersion ( void );

  private:    
    CData * data;
    uint32_t mPos;
	uint32_t mFileSize;
    snapShots * history = nullptr;
    
};
    CFile::CFile ( void ) : data (new CData () ), mPos(0), mFileSize(0) {}
    CFile::~ CFile ( void ) {
        if (data->isFree() ) delete data;
        if (history != nullptr ) {
          history->clear();
          if (history->isFree() ) delete history;
        }
    }
    void CFile::clear () {
        //clears the files and deallocate memories
        if (data->isFree() ) delete data;
        if (history != nullptr ) {
            history->clear();
            if (history->isFree() ) delete history;
        }   
    }
    CFile::CFile ( const CFile & other ) {
        data = other.data;
        data->addRef();
        mPos = other.mPos;
		mFileSize = other.mFileSize;
        history = other.history;
        snapShots * curNode = history;
        while ( curNode != nullptr) {
            curNode->addRef();
            curNode = curNode->next;
        }
    }
    CFile& CFile::operator= ( CFile & other) {
        if (this == &other) return *this;
        // first free memory from current CFile
        clear();
        // assign data from other CFile
        data = other.data;
        data->addRef();
        mPos = other.mPos;
        history = other.history;
        snapShots * curNode = history;
        while ( curNode != nullptr) {
            curNode->addRef();
            curNode = curNode->next;
        }
        return *this;
    }
    uint32_t CFile::write ( const uint8_t * src, uint32_t bytes ) {
        // if file has mutiple references, we clone the file before writing on it. (copy-on-write)
		uint32_t i = 0;
		if (mPos == data->size() ) {
			for (; i < bytes; i++) {
				data->push_back(src[i]);
				mPos++;
			}
			mFileSize = data->size();
		} else {
			if ( data->getRef() > 1) {
				CData * temp = new CData();
				// *(temp) = *(data);  // does deep copy
				temp->deepCopy(*data, mFileSize); 
				data->removeRef();
				data = temp;
			}
			for (; i < bytes; i++) {
				data->insert(mPos++, src[i]);
			}
			if (mPos > mFileSize) mFileSize = mPos;			
		}


        // uint32_t i = 0;
        // for (; i < bytes; i++) {
        //     if (mPos == data->size() ) {
		// 		data->push_back(src[i]);
		// 		mPos++;
        //     } else {
		// 		if ( data->getRef() > 1) {
		// 			CData * temp = new CData();
		// 			*(temp) = *(data);  // does deep copy 
		// 			data->removeRef();
		// 			data = temp;
		// 			mFileSize += data->size();
		// 		}
        //     	data->insert(mPos++, src[i]);
        //     }
        // }
        return i;
    }
    uint32_t CFile::read ( uint8_t * dst, uint32_t bytes ) {
        if (mPos == mFileSize ) return 0;   
        uint32_t readBytes = 0;
		// cout << "starts::>>" <<mFileSize <<" >> " << bytes <<" :";
        while (mPos < mFileSize && readBytes < bytes) {
            dst[readBytes++] = (*data)[mPos++];
			// cout << (int)(dst[readBytes-1]) <<" ,";
        }
		// cout << endl;
        return readBytes;
    }
    bool CFile::seek ( uint32_t offset ) {
        if ( offset > mFileSize || offset < 0 ) return false;
        if ( mPos == offset ) return true;
        mPos = offset;
        return true;
    }
    void CFile::truncate ( void ) {
		mFileSize = mPos;

        // if ( data->getRef() > 1) {
		// 	mFileSize = mPos;
        //     // CData * temp = new CData();
		// 	// temp->trunCopy(*data, mPos);
        //     // // *(temp) = *(data); // does deep copy
        //     // data->removeRef();
        //     // data = temp;
        // } else {
        // 	data->truncate(mPos);
		// }
    }
    uint32_t CFile::fileSize ( void ) const { 
		// return data->size();
		return mFileSize;
	 }
    void CFile:: addVersion ( void ) {
        snapShots * temp = new snapShots();
        temp->data = data;
        temp->pos = mPos;
		temp->fSize = mFileSize; 
        temp->next = history;
        history = temp;
        data->addRef();
    }
    bool CFile::undoVersion ( void ) {
        if ( history == nullptr ) return false;
        if (data->isFree() ) delete data;
        data = history->data;
        mPos = history->pos;
		mFileSize = history->fSize;		
        snapShots* temp = history->next;
        if (history->empty() ) delete history;
        history = temp;
        return true;
    }


//***************************************************************************************************************

#ifndef __PROGTEST__
bool               writeTest                               ( CFile           & x,
                                                             const initializer_list<uint8_t> & data,
                                                             uint32_t          wrLen )
{
  return x . write ( data . begin (), data . size () ) == wrLen;
}

bool               readTest                                ( CFile           & x,
                                                             const initializer_list<uint8_t> & data,
                                                             uint32_t          rdLen )
{
  uint8_t  tmp[100];
  uint32_t idx = 0;

  if ( x . read ( tmp, rdLen ) != data . size ())
    return false;
  for ( auto v : data )
    if ( tmp[idx++] != v )
      return false;
  return true;
}



int main ( void )
{
  CFile f0;
  assert ( writeTest ( f0, { 10, 20, 30 }, 3 ) );
  assert ( f0 . fileSize () == 3 );
  assert ( writeTest ( f0, { 60, 70, 80 }, 3 ) );
  assert ( f0 . fileSize () == 6 );
  assert ( f0 . seek ( 2 ));
  assert ( writeTest ( f0, { 5, 4 }, 2 ) );
  assert ( f0 . fileSize () == 6 );
  assert ( f0 . seek ( 1 ));
  assert ( readTest ( f0, { 20, 5, 4, 70, 80 }, 7 ));
  assert ( f0 . seek ( 3 ));
  f0 . addVersion();
  assert ( f0 . seek ( 6 ));
  assert ( writeTest ( f0, { 100, 101, 102, 103 }, 4 ) );
  f0 . addVersion();
  assert ( f0 . seek ( 5 ));
  CFile f1 ( f0 );
  f0 . truncate ();
  assert ( f0 . seek ( 0 ));
  assert ( readTest ( f0, { 10, 20, 5, 4, 70 }, 20 ));
  assert ( f0 . undoVersion () );
  assert ( f0 . seek ( 0 ));
  assert ( readTest ( f0, { 10, 20, 5, 4, 70, 80, 100, 101, 102, 103 }, 20 ));
  assert ( f0 . undoVersion () );
  assert ( f0 . seek ( 0 ));
  assert ( readTest ( f0, { 10, 20, 5, 4, 70, 80 }, 20 ));
  assert ( !f0 . seek ( 100 ));
  assert ( writeTest ( f1, { 200, 210, 220 }, 3 ) );
  assert ( f1 . seek ( 0 ));
  assert ( readTest ( f1, { 10, 20, 5, 4, 70, 200, 210, 220, 102, 103 }, 20 ));
  assert ( f1 . undoVersion () );
  assert ( f1 . undoVersion () );
  assert ( readTest ( f1, { 4, 70, 80 }, 20 ));
  assert ( !f1 . undoVersion () );



  CFile f3;
  assert(writeTest(f3, {2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 8, 3, 4, 6, 8, 9,54, 3, 6}, 20) );
  assert ( f3 . seek ( 2 ));
  assert(writeTest(f3, {2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13}, 24) );
  
  assert ( f3 . seek ( 10 ));
  
  
  assert(writeTest(f3, {2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5 , 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5, 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5}, 75) );
  assert ( f3 . seek ( 10 ));

  assert ( f3 . seek ( 20 ));
  assert ( f3 . seek ( 60 ));

  assert(writeTest(f3, {254, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5 , 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5, 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5}, 75) );
  assert ( f3 . seek ( 100 ));
  f3 . addVersion();

    assert(writeTest(f3, {254, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5 , 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5, 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5}, 75) );
  f3 . addVersion();

  assert ( f3 . seek ( 150 ));
  f3 . addVersion();

  assert(writeTest(f3, {254, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5 , 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5, 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5}, 75) );

  f3 . addVersion();

  
  assert(writeTest(f3, {254, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5 , 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5, 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5}, 75) );
  assert ( f3 . seek ( 200 ));
  f3 . addVersion();

  assert(writeTest(f3, {254, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5 , 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5, 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5}, 75) );
  f3 . addVersion();

  assert ( f3 . seek ( 250 ));
  f3 . addVersion();

  assert(writeTest(f3, {254, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5 , 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5, 2, 45, 6, 3, 5, 6, 7, 3, 5 , 7, 7, 1, 2, 3, 4, 5, 6,7, 8, 9, 10, 11, 12, 13, 5}, 75) );

  f3 . addVersion();
  assert ( f3 . undoVersion () );
  assert ( f3 . undoVersion () );
  assert ( f3 . undoVersion () );
  assert ( f3 . undoVersion () );

  f0 = f3 = f1;

  CFile x (f3);


  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
