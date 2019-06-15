#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <algorithm>

using namespace std;
ifstream inFile;
ofstream outFile;	
vector <int> result;

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

int printResult(vector<int> result,int size){
	for(int i=0;i<size;i++){
		if(i != size -1)
			outFile << result[i] << endl;
		else
			outFile << result[i];				
	}
}
/* 
bool isFull(vector<unsigned> cache,int blockNumber,int tagValue){
	for(int i=0;i<blockNumber;i++){
		if(cache[0][i] == 0){			
			return false;
		}
	}
	return true;
}

bool isHit(vector<unsigned>cache,int blockNumber,int tagValue){
	
}
*/
int main(int argc,char *argv[]){
	vector <unsigned> input;
	string data;
	int counter=0,width=32,checking_counter=0;	
	int x,largest = 0;
	int cache_size,block_size;
	int associativity,algorithm;
	int tag,index,offset,index_bits;
	int blockAddress,tagValue,blockNum;	
	double missRate=0;

	
	inFile.open("trace4.txt"); 
	outFile.open("trace4.out");
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
	//Initial
	index_bits = calculate_index(cache_size,block_size);
	offset = calculate_wordSet(block_size); //WO + BO
	index = cache_size / block_size;
	tag = 32 - offset - index_bits;
	
	vector <unsigned> cache [index];

	if(associativity == 0 && algorithm == 0){ //directedMapped_FIFO		
			for(int i=0;i<index;i++){		
				cache[i].push_back(0);
			}				
			for(int i=0;i<counter;i++){	
		
				blockAddress = get_blockAddress(input[i],offset);
				tagValue = get_tag(input[i],offset,index_bits);
				blockNum = blockAddress % index;				
				if(cache[blockNum][0] == tagValue){
					result.push_back(-1);
					cache[blockNum][0] = tagValue;
				}
				else if(cache[blockNum][0] == 0){
					result.push_back(-1);
					cache[blockNum][0] = tagValue;			
				}
				else if(cache[blockNum][0] != tagValue){						
					result.push_back(cache[blockNum][0]);
					cache[blockNum][0] = tagValue;
				}				
				
			}
			for(int i=0;i<result.size();i++){
				if(i != result.size() -1)
					outFile << result[i] << endl;
				else
					outFile << result[i];				
			}
	} //end directedMapped_FIFO
	///////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////Algorithm要記得改回去/////////////////////////////////////////////	
	///////////////////////////////////////////////////////////////////////////////////////////
	if(associativity == 1 && (algorithm == 0 || algorithm == 2)){ //4-way_FIFO or LRU
		int FIFO[4] = {0};
		for(int i=0;i<index;i++){		
			cache[i].push_back(0);cache[i].push_back(0);cache[i].push_back(0);cache[i].push_back(0);
		}
		for(int i=0;i<counter;i++){	
			blockAddress = get_blockAddress(input[i],offset);
			tagValue = get_tag(input[i],offset,index_bits);
			blockNum = blockAddress % index;	
			for(int columnNumber = 0;columnNumber < 4;columnNumber++){
				if(tagValue == cache[blockNum][columnNumber]){
					result.push_back(-1);
					//if(algorithm == 2) // algorithm == 1 == LRU            ///////////////這裏也是
					//	FIFO[columnNumber] = 0;
					for(int num = 0;num < 4;num++){
						if(cache[blockNum][num] != 0)
							FIFO[num]++;
					}
					goto exit_4way_FIFO;
				}
			}
			for(int columnNumber = 0;columnNumber < 4;columnNumber++){
				if(cache[blockNum][columnNumber] == 0){
					result.push_back(-1);
					cache[blockNum][columnNumber] = tagValue;
					FIFO[columnNumber] = 0;
					for(int num = 0;num < 4;num++){
						if(cache[blockNum][num] != 0)
							FIFO[num]++;
					}
					goto exit_4way_FIFO;
				}
			}
			{
				largest = distance(FIFO, max_element(FIFO,FIFO + (sizeof(FIFO)/sizeof(int))));
				result.push_back(cache[blockNum][largest]);
				cache[blockNum][largest] = tagValue;
				FIFO[largest] = 0;
				for(int num = 0;num < 4;num++){
					if(cache[blockNum][num] != 0)
						FIFO[num]++;
				}
				goto exit_4way_FIFO;
			}
			exit_4way_FIFO:;
		}
		printResult(result,result.size());
	} //end 4way_FIFO

	if(associativity == 2 && (algorithm == 0 || algorithm == 1)){ //Fully_LRU
			int blockNumber = cache_size / block_size;
			index_bits = 0;
			int LRU[blockNumber];		
			for(int i=0;i<blockNumber;i++)
				LRU[i] = 0; //use this array to determine which data is gonna replace
			for(int i=0;i<blockNumber;i++)
				cache[0].push_back(0); //initialize cache						
			for(int i=0;i<counter;i++){							
				tagValue = get_tag(input[i],offset,index_bits);
				
				for(int k=0;k<blockNumber;k++){
					if(tagValue == cache[0][k]){
						result.push_back(-1);
						if(algorithm == 1) //algorithm == 1 == LRU
							LRU[k] = 0;
						for(int num = 0;num < blockNumber;num++){
							if(cache[0][num] != 0)
								LRU[num]++;
						}
						goto exit_LRU;
					}
				}
				for(int k=0;k<blockNumber;k++){
					if(cache[0][k] == 0){
						result.push_back(-1);
						cache[0][k] = tagValue;
						LRU[k] = 0;
						for(int num = 0;num < blockNumber;num++){
							if(cache[0][num] != 0)
								LRU[num]++;
						}
						goto exit_LRU;
					}
				}				
				{						
					largest = distance(LRU, max_element(LRU,LRU + (sizeof(LRU)/sizeof(int))));
					result.push_back(cache[0][largest]);
					cache[0][largest] = tagValue;
					LRU[largest] = 0;
					for(int num = 0;num < blockNumber;num++){
						if(cache[0][num] != 0)
							LRU[num]++;
					}
					goto exit_LRU;
				}																										
				
				exit_LRU:;
			}
			printResult(result,result.size());
	}	//end Fully_LRU			
	inFile.close();	
	outFile.close();
	cout << "Finish" << endl;
	return 0;	
}


