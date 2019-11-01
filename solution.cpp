/*
	Project: Simulate computer ALU unit to process instructions
	
	Read the file.txt
	The first line is expected register final answer after all instructions have been executed.
	The second line is initial register values.
	The remaining lines are instructions going to be executed.

	Example file.txt:
	10,21,93,88,67,0,44,89
	2,4,6,88,67,45,44,89
	add $0,$1,$2
	sub $1,$3,$4
	addi $2,$3,5
	b label1
	mul $3,$3,$4
	label1 div $5,$5,$6
	beq $3,$1,label2
	div $5,$5,$6
	bnq $3,$1,label2
	add $0,$1,$2
	sub $1,$3,$4
	label2 addi $2,$3,5
	end
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>

#include <map>

using namespace std;
class solution {

	private:

		bool DEBUG;
		int clck;
		
		// Store all instructions
		vector<string> vect_lines;
		
		// Store all registers
		vector<int>* t_vars;	
		
		// Mapping label to instruction location
		// for jumping case happened in branch instruction
		map <string, int> label_list;

	public :

		solution(ifstream &file_in,int clck_in = 10 ,bool DEBUG_in = false);
		void dbg(const string &msg);
		
		// ALU unit simulation
		vector<int>* alu();
		
		// Hardware clock simulation
		int mips_clock();
		
		// Get integer operands from instruction
		// e.g. : Get 0, 1, 2 from instruction (add $0,$1,$2)
		void opStringToInt(string split_instr, int &opr1i, int &opr2i, int &opr3i, 
			string delim1, string delim2, string delim3);
		
		// Get integer and string operands from instruction
		// e.g. : Get 3, 1, label2 from instruction (beq $3,$1,label2)
		void opStringToInt(string split_instr, int &opr1i, int &opr2i, string &opr3, 
			string delim1, string delim2, string delim3);

};

// Read registers initial values and instructions from text file and then store into vector
solution::solution(ifstream &file_in, int clck_in, bool DEBUG_in) {

	this->clck = clck_in;
	
	t_vars = new vector<int>();
	string initial_registers;
	
	getline(file_in,initial_registers);	

    stringstream sstream(initial_registers);
		
	int int_register;
	
	// Read registers initial values and store into vector
	while (sstream)
	{
		sstream >> int_register;
		t_vars->push_back(int_register);

		if (sstream.peek() == ',')
			sstream.ignore();
		cout << int_register << " ";
 
	}
	cout << endl;
	
	// Read instructions and store into vector
	while (true) 
	{		
		if(file_in.eof())
			break;
		
		string instructions;
		getline(file_in,instructions);	
		
		vect_lines.push_back(instructions);
		
		// skip last empty row
		if (instructions.find("end") != string::npos ) {
			break;
		}	
	}
		
	// Use map to store label locations
	// Mapping: Label -> instruction location
	string labels;
	
	for (int i = 0; i < vect_lines.size() ; i++ ) {

		stringstream sstream2(vect_lines.at(i));
		sstream2 >> labels;
		
		if (labels.find("label") != string::npos ) {
			
			label_list[labels] = i;
			
			string split_instr = vect_lines.at(i);
			string delimiter = " ";
			int next_oprLocation = split_instr.find(delimiter);
			
			vect_lines[i] = split_instr.erase(0, next_oprLocation + delimiter.length());
		}
	}

}

// Get integer operands from instruction
// e.g. : Get 0, 1, 2 from instruction (add $0,$1,$2)
void solution::opStringToInt(string split_instr, int &opr1i, int &opr2i, int &opr3i, string delim1, string delim2, string delim3){
	
	string delimiter = delim1;
	
	int next_oprLocation = split_instr.find(delimiter);
	
	split_instr.erase(0, next_oprLocation + delimiter.length());

	delimiter = delim2;
	next_oprLocation = split_instr.find(delimiter);
	string opr1 = split_instr.substr(0, next_oprLocation);			
	opr1i = stoi(opr1);

	split_instr.erase(0, next_oprLocation + delimiter.length());
	
	delimiter = delim3;
	next_oprLocation = split_instr.find(delimiter);
	string opr2 = split_instr.substr(0, next_oprLocation);	
	opr2i = stoi(opr2);

	
	split_instr.erase(0, next_oprLocation + delimiter.length());
	
	next_oprLocation = split_instr.find(delimiter);
	string opr3 = split_instr.substr(0, next_oprLocation);	
	opr3i = stoi(opr3);

}

// Get integer and string operands from instruction
// e.g. : Get 3, 1, label2 from instruction (beq $3,$1,label2)
void solution::opStringToInt(string split_instr, int &opr1i, int &opr2i, string &opr3, string delim1, string delim2, string delim3){
	
	string delimiter = delim1;
				
	int next_oprLocation = split_instr.find(delimiter);
	
	split_instr.erase(0, next_oprLocation + delimiter.length());

	delimiter = delim2;
	next_oprLocation = split_instr.find(delimiter);
	string opr1 = split_instr.substr(0, next_oprLocation);			
	opr1i = stoi(opr1);
	

	split_instr.erase(0, next_oprLocation + delimiter.length());
	
	delimiter = delim3;
	next_oprLocation = split_instr.find(delimiter);
	string opr2 = split_instr.substr(0, next_oprLocation);	
	opr2i = stoi(opr2);
	
	split_instr.erase(0, next_oprLocation + delimiter.length());
	
	next_oprLocation = split_instr.find(delimiter);
	opr3 = split_instr.substr(0, next_oprLocation);

}

// Simulate the ALU unit
vector<int>* solution::alu(){
	
	// i: current instruction index
	int i = 0;
	int noOfCycle = 0;
	string split_instr;
	
	while(i != -1) {
		if(!mips_clock()) {
			continue;
		} else {
			
			// Process instruction and calculate the result
			// Possible instructions starting words: add, addi, sub, mul, div, b, beq, bnq
			
			cout<< noOfCycle++ << endl;
			cout<< vect_lines.at(i) << endl;
			
			stringstream sstream3(vect_lines.at(i));
			sstream3 >> split_instr;
				
			if (split_instr.compare("add") == 0){
				sstream3 >> split_instr;
				
				int opr1i, opr2i, opr3i;
				
				opStringToInt(split_instr, opr1i, opr2i, opr3i, " $", ",$", ",$");												
				t_vars->at(opr1i) = t_vars->at(opr2i) + t_vars->at(opr3i);	
				
				// point to next instruction
				i++; 
			} else if (split_instr.compare("addi") == 0){
				sstream3 >> split_instr;
				int opr1i, opr2i, opr3i;
				
				opStringToInt(split_instr, opr1i, opr2i, opr3i, " $", ",$", ",");
												
				t_vars->at(opr1i) = t_vars->at(opr2i) + opr3i;
				
				// point to next instruction
				i++; 				
			} else if (split_instr.compare("sub") == 0){
				sstream3 >> split_instr;
				int opr1i, opr2i, opr3i;
				
				opStringToInt(split_instr, opr1i, opr2i, opr3i, " $", ",$", ",$");
												
				t_vars->at(opr1i) = t_vars->at(opr2i) - t_vars->at(opr3i);
				
				// point to next instruction
				i++; 
			} else if (split_instr.compare("mul") == 0){
				sstream3 >> split_instr;
				int opr1i, opr2i, opr3i;
				
				opStringToInt(split_instr, opr1i, opr2i, opr3i, " $", ",$", ",$");
												
				t_vars->at(opr1i) = t_vars->at(opr2i) * t_vars->at(opr3i);
				
				// point to next instruction
				i++; 
			} else if (split_instr.compare("div") == 0){
				sstream3 >> split_instr;
				int opr1i, opr2i, opr3i;
				
				opStringToInt(split_instr, opr1i, opr2i, opr3i, " $", ",$", ",$");
												
				t_vars->at(opr1i) = t_vars->at(opr2i) / t_vars->at(opr3i);
				
				// point to next instruction
				i++; 
			} else if (split_instr.compare("b") == 0){
				// Direct branch case
 
				sstream3 >> split_instr;
				string jumpToLabel = split_instr;
				
				// Jump to the instruction after the label
				i = label_list.find(jumpToLabel)->second;
			} else if (split_instr.compare("beq") == 0){
				sstream3 >> split_instr;
				int opr1i, opr2i;
				string opr3;
				
				opStringToInt(split_instr, opr1i, opr2i, opr3, " $", ",$", ",");
				
				if ( t_vars->at(opr1i) == t_vars->at(opr2i) ) {
					// Jump to the instruction after the label
					i = label_list.find(opr3)->second;
				} else {
					// point to next instruction
					i++; 
				}							
			} else if (split_instr.compare("bnq") == 0){
				sstream3 >> split_instr;
				int opr1i, opr2i;
				string opr3;
				
				opStringToInt(split_instr, opr1i, opr2i, opr3, " $", ",$", ",");
				
				if ( t_vars->at(opr1i) != t_vars->at(opr2i) ) {
					// Jump to the instruction after the label
					i = label_list.find(opr3)->second;
				} else {
					// point to next instruction
					i++; 
				}							
			} else if (split_instr.compare("end") == 0){
				// Set -1 to stop the while loop
				i = -1;						
			}
		}
		
		// Show the final register values
		for (int k = 0; k < t_vars->size(); k++ ){		
			cout << t_vars->at(k) << " ";			
		}
		cout << endl;
	}
	
	return t_vars;
}

// Simulate the hardware clock cycle
int solution::mips_clock() {
	chrono::milliseconds timespan(clck); 

	this_thread::sleep_for(timespan);
	static int cycle = 0;
	if (cycle == 0 )
		cycle = 1;
	else 
		cycle = 0;
	return cycle;
}

