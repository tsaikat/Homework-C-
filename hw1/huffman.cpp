#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

struct Node {
    char *data = new char[5];
    size_t charSize;
    vector <string> code;
    string unicode;
    Node *left, *right;

    Node() {
        left = right = nullptr;
    }
    bool binToDec() {
        charSize = code.size();
        // data = new char[charSize];
        int dec;
        size_t i = 0;
        for (; i < code.size(); i++) {
            dec = stoi(code[i], 0, 2);
            data[i] =  (char) dec;
            if (dec > 255) return false;
        }
        data[i] = '\0';
        return true;
        // size_t res = 0;
        // for (auto c : code) {
        //     res = res * 2 + c - '0';
        // }
        // data =(char) res;
        // return;
    }
    ~Node () {
        delete left;
        delete right;
        delete[] data;
        // might need to free char
    }
};

class Huffman {
private:
    string inFileName, outFileName;
    int mByte;

    fstream inFile, outFile;
    vector <char> qBits;
    Node *root;

public:
    Huffman(string inFileName, string outFileName) {
        this->inFileName = inFileName;
        this->outFileName = outFileName;
        root = new Node();
    }
    ~ Huffman () {
        delete root;
    }
    bool nextByte() {
        char x;
        if (inFile.get(x)) {
            mByte =(unsigned char) x;
            return true;
        } else {
            return false;
        }
    }


    bool getBits() {
        if (nextByte()) {
            while (mByte > 0) {
                qBits.push_back(mByte % 2 + '0');
                mByte /= 2;
            }
            size_t roundBits = 8 - qBits.size();
            for (size_t i = 0; i < roundBits; i++) {
                qBits.push_back('0');
            }
            return true;
        } else {
            return false;
        }
    }

    void nextBit () {
        qBits.pop_back();
        if (qBits.empty()) getBits();
        return;
    }

    void countUtfByte (bool & flag, int & byteToRead) {
        if (qBits.back() == '1') {
            byteToRead ++;
        } else {
            byteToRead --;
            flag = false;
        }
    }


    bool makeLeaf (Node *curNode) {
        if (qBits.back() == '0') { 
            curNode->code.push_back("");
            for(int i = 0; i < 8; i++) {
                curNode->code[0].push_back(qBits.back());
                curNode->unicode += qBits.back();
                nextBit();
                if (inFile.eof()) return false;
            }
            int unicodeValue = stoi(curNode->unicode, 0, 2);
            if (unicodeValue < 0 || unicodeValue > 127) return false;
            return true;
        } else {
            int byteToRead = 1;
            bool firstByte = true;
            //go through the entire byte
            //check how many consequent 1's in the beginning of the byte (until a 0 shows up)
            // increment byteToRead until a 0 shows up

            // read the next bytes ( they should start with 10, if not return false)
            // once a byte is read decrement byteToRead
            // once all byteToRead are done. return true.

            for(int i = 0; i < byteToRead; i++) {
                curNode->code.push_back("");
                
                for (int k = 0; k < 8; k++) {
                    if ( i > 0) {
                        if ( (k == 0) && qBits.back() != '1') return false;
                        if ( (k == 1) && qBits.back() != '0') return false; 
                    }
                    //storing binary - one byte at a time
                    curNode->code[i].push_back(qBits.back());
                    if (!firstByte && k != 0 && k != 1) {
                        curNode->unicode += qBits.back();
                    }
                    if (firstByte) countUtfByte(firstByte, byteToRead);
                    nextBit();
                    if (qBits.empty()) return false;
                }
                if ( byteToRead > 4 || byteToRead < 2) return false;
            }
            int unicodeValue = stoi(curNode->unicode, 0, 2);

            if ( (byteToRead == 2 && unicodeValue > 2047) || (byteToRead == 3 && unicodeValue > 65535) || (byteToRead == 4 && unicodeValue > 1114111) ) {
                return false;
            }
            if ( (byteToRead == 2 && unicodeValue < 128) || (byteToRead == 3 && unicodeValue < 2048) || (byteToRead == 4 && unicodeValue < 10000) ) {
                return false;
            }

            
            return true;
        }
        return false;       
    }

    bool getTree( Node* curNode) {
        if (qBits.back() == '1') {
            nextBit();
            if(qBits.empty()) return false;
            if (!makeLeaf(curNode)) return false;
            if (!(curNode->binToDec())) return false;
            return true;
        }
        else {
            curNode->left = new Node();
            curNode->right = new Node();
            nextBit();
            if(qBits.empty()) return false;
            if(!(getTree( curNode->left))) return false;
            if (!(getTree( curNode->right))) return false;
            return true; 
        }
        return false;
    }

    Node* traverseTree(Node *curr) {
        if (qBits.back() == '0') {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
        nextBit();
        return curr;
    }

    bool writeFile() {
        Node* curr = root;
        size_t chunk = 0;
        if (qBits.back() == '1') {
            nextBit();
            chunk = 4096;
            while(chunk) {
                curr = traverseTree(curr);
                if (curr == nullptr) return false; 
                if (curr->left == nullptr && curr->right == nullptr) {
                    outFile.write(curr->data, curr->charSize);
                    if (--chunk == 0 && !(inFile.eof())) {
                        if (!writeFile()) return false;
                    }
                    if (chunk > 0 && inFile.eof()) return false;
                    curr = root;
                }
            }            
            
        } else {
            nextBit();
            string binStr = "";
            for (int k = 0; k < 12; k++) {
                binStr += qBits.back();
                nextBit();
                // if (qBits.empty()) break;
            }
            chunk = stoi(binStr, 0, 2);
            //  if (chunk == 0 && binStr.length() == 12 && !(qBits.empty()) && outFile.tellp() < 1 ) return false;


            while(chunk) {
                curr = traverseTree(curr);
                if (curr == nullptr) return false; 
                if (curr->left == nullptr && curr->right == nullptr) {
                    outFile.write(curr->data, curr->charSize);
                    curr = root;
                    if (--chunk > 0 && inFile.eof()) return false;
                    if (chunk == 0 ) return true; 
                }
            }
            return true;
        }
        return true;
    }
    
    bool decompress() {
        inFile.open(inFileName, ios::in | ios::binary);
        if (!inFile) return false;
        if (!inFile.good()) return false;
        if (!getBits()) return false;
        if (!getTree(root)) return false;

        outFile.open(outFileName, ios::out);
        if (!outFile.good()) return false;

        if (!writeFile()) return false;

        if (inFile.bad() || outFile.bad()) return false;
        inFile.close();
        outFile.close();
        return true;
    }
};


bool decompressFile ( const char * inFileName, const char * outFileName )
{
  Huffman x(inFileName, outFileName);
  if (x.decompress()) return true;
  return false;
}

bool compressFile ( const char * inFileName, const char * outFileName )
{
  // keep this dummy implementation (no bonus) or implement the compression (bonus)
  return false;
}
#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
  // todo
  return false;
}

int main ( void )
{
  assert ( decompressFile ( "tests/test0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

  assert ( ! decompressFile ( "tests/test5.huf", "tempfile" ) );


  assert ( decompressFile ( "tests/extra0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra5.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra6.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra7.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra8.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra9.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

  return 0;
}
#endif /* __PROGTEST__ */
