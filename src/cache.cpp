#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

unsigned get_blockAddress(unsigned block,int offset){
	block = block >> offset;
	return block;
}

unsigned get_tag(unsigned block,int offset,int index){
	block = block >> (offset + index);
	return block;
}

int calculate_index(int cache_size,int block_size){
	int answer=0,index=0;
	answer = cache_size/block_size;
	while(answer != 1){
		answer = answer / 2;
		index++;
	}
	return index;
}

int calculate_wordSet(int block_size){
	int wordSet=0;;
	while(block_size != 1){
		block_size = block_size / 2;
		wordSet++;
	}
	return wordSet;
}

int main(){
	vector <unsigned> input;
	string data;
	int counter=0,width=32;
	int cache_size,block_size;
	int associativity,algorithm;
	int tag,index,offset,index_bits;	

	ifstream inFile;
	inFile.open("./src/trace1.txt");

	//Check for error
	if(inFile.fail()){
		cout << "Error Opening File" << endl;
		exit(1);
	}

	inFile >> cache_size >> block_size >> associativity >> algorithm;
	cache_size = cache_size * 1024;
	while(!inFile.eof()){
		inFile >> data;		  
		input.push_back(stoull(data,nullptr,0));
		counter++;		
	}

	index_bits = calculate_index(cache_size,block_size);
	offset = calculate_wordSet(block_size); //WO + BO
	index = cache_size / block_size;
	cout << "The index is " << index_bits << endl;
	
	int cache[index];
	for(int i=0;i<index;i++)
		cache[i] = -1;
	
	for(int i=0;i<counter;i++){	
		int blockAddress = get_blockAddress(input[i],offset);
		if(cache[blockAddress % index] == -1){
			cout << -1 << endl;
			cache[blockAddress % index] = get_tag(input[i],offset,index_bits);
		}
		else if(cache[blockAddress % index] == get_tag(input[i],offset,index_bits))
		{
			cout << -1 << endl;			
		}
		else if(cache[blockAddress % index] != get_tag(input[i],offset,index_bits)){						
			cout <<  std::dec << cache[blockAddress % index] << endl;
			cache[blockAddress % index] = get_tag(input[i],offset,index_bits);
		}
	}
	
	// //Print out for checking
    // cout << cache_size << endl << block_size << endl << associativity << endl << algorithm << endl;
	// for(int i=0;i<counter;i++){
	// 	cout << std::bitset<32>(input[i]) << endl;
	// }	
	
	inFile.close();	
	return 0;	

}


