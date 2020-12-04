//
// STARTER CODE: util.h
//
// This header holds methods for commands 1-6, C, and D for main.cpp
#ifndef UTIL 
#define UTIL 

#include <fstream>
#include <vector>
#include <string>
#include "priorityqueue.h"
#include "hashmap.h"

#pragma once

typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};


// This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
    if (node == nullptr) {
        return;
    }

    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}

//
// This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
    char c;
    int k;
    if (isFile) {
        ifstream inFile(filename);
        inFile.get(c);

        while (!inFile.eof()) {
        	k = (int) c;
            if (!map.containsKey(k)) {
                map.put(k, 1);
            } else {
                map.put(k, map.get(k) + 1);
            }
            inFile.get(c);
        }

        inFile.close();
    } else {
        for (int i = 0; i < (int) filename.size(); i++) {
            c = filename[i];
            k = (int) c;
            if (!map.containsKey(k)) {
                map.put(k, 1);
            } else {
                map.put(k, map.get(k) + 1);
            }
        }
    }
    map.put(PSEUDO_EOF, 1);
}

//
// This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmapF &map) {
    priorityqueue<HuffmanNode*> pq;
    const vector<int> k = map.keys();

    for (int i = 0; i < (int) k.size(); i++) {
        HuffmanNode* newNode = new HuffmanNode;
        newNode->character = k[i];
        newNode->count = map.get(k[i]);
        newNode->zero = nullptr;
        newNode->one = nullptr;
        pq.enqueue(newNode, map.get(k[i]));
    }

    HuffmanNode* zero;
    HuffmanNode* one;
    while (pq.Size() > 1) {
        zero = pq.dequeue();
        one = pq.dequeue();
        HuffmanNode* newNode = new HuffmanNode;
        newNode->character = NOT_A_CHAR;
        newNode->count = zero->count + one->count;
        newNode->zero = zero;
        newNode->one = one;
        pq.enqueue(newNode, newNode->count);
    }
    HuffmanNode* temp = pq.dequeue();
    return temp;
}

//
// Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str, HuffmanNode* prev) {
    if (node == nullptr ) {
    	if (prev == nullptr) {
    		return;
    	}
    	str.pop_back();

    	// emplace it in map if haven't checked child yet
    	if (encodingMap.count(prev->character) == 0) {
    		encodingMap.emplace(prev->character, str);
    	}
    	return;
    }

    // zero
    _buildEncodingMap(node->zero, encodingMap, str.append("0"), node);

    // delete last 0 to go right
    str.pop_back();

    // one
    _buildEncodingMap(node->one, encodingMap, str.append("1"), node);
     // delete last 1 to go back to prev
    str.pop_back();
}

//
// This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap;
    string str = "";
    HuffmanNode* prev = nullptr;
    HuffmanNode* temp = tree;
	_buildEncodingMap(temp, encodingMap, str, prev);

    return encodingMap;
}

//
// This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output,
              int &size, bool makeFile) {
    char c; 
    string result = "";
    string value;

    while (input.get(c)) {
        value = encodingMap.at(c);
        result += value;
      //  if (makeFile) {
      //  	for (char i : value) {
	    	// 	i -= 48;
	    	// 	output.writeBit((int) i);
    		// }
      //  }
    }
    value = encodingMap.at(PSEUDO_EOF);
    result += value;
    if (makeFile) {
    	for (char i : result) {
    		i -= 48;
    		output.writeBit((int) i);
    	}
    }
    size = (int) result.size();
    return result;
}


//
// This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    string result = "";
    int bit;
    char c;
    HuffmanNode* temp = encodingTree;

    bit = input.readBit();
    while (!input.eof()) {
        if (temp->character != NOT_A_CHAR/*temp->zero == nullptr && temp->one == nullptr*/) {
        	c = temp->character;
            if (temp->character == PSEUDO_EOF) {
            	return result;
        	}
            output << c;
            temp = encodingTree;
            result += c;
        }
        if (bit == 0 && temp->zero != nullptr) {
            temp = temp->zero;
        } else if (bit == 1 && temp->one != nullptr){
            temp = temp->one;
        }
        bit = input.readBit();
    }

    return result;
}

//
// This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    int size = 0;
    hashmapF fm;
    ifstream inFile;
    inFile.open(filename);

    // build frequency map
    bool isFile = inFile.is_open();
    inFile.close();
    buildFrequencyMap(filename, isFile, fm);
    // build encoding tree
    HuffmanNode* et = buildEncodingTree(fm);
    // build encoding map
    hashmapE em = buildEncodingMap(et);
    // need to write the map to string at beginning of file

    // encodes the file
    inFile.open(filename);
    ofbitstream output(filename + ".huf");
    output << fm;  // add the frequency map to the file
    string result = encode(inFile, em, output, size, true);
    inFile.close();
	output.close();

	freeTree(et);
    return result;
}

//
// This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    ifbitstream inFile(filename);
    int pos = (int) filename.find(".huf");
    if (pos >= 0) {
        filename = filename.substr(0, pos);
    }
    pos = filename.find(".");
    filename = filename.substr(0, pos);

    // extract header and build frequency map
    hashmapF header;
    inFile >> header;

    // encode tree
    HuffmanNode* et = buildEncodingTree(header);

    // decode
    ofstream output(filename + "_unc.txt");
    string result = decode(inFile, et, output);
    inFile.close();
    output.close();

    freeTree(et);
    return result;
}
#endif UTIL