#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main(){
	vector <string> input;
	string String;
	int counter=0;
	ifstream inFile;
	inFile.open("trace1.txt");

	//Check for error
	if(inFile.fail()){
		cout << "Error Opening File" << endl;
		exit(1);
	}

	while(!inFile.eof()){
		inFile >> String;
		input.push_back(String);
		counter++;		
	}


	for(int i=0;i<counter-1;i++)
		cout << input[i] << endl;
	inFile.close();
	return 0;	

}
