/*
NFA to DFA conversion and state minimization algorithm
Version 1.0.1
Developed by: Matthew Hahn

This method reads in NFA data from a file, converts it to an DFA and runs a hopcroft minimization algorithm on that DFA

Utilizes:
GraphObject.cpp - Storage for NFA/DFAs
MetaNodeObject.cpp - Storage and creator of meta nodes

Graph ReadIn(String) - reads in data from the file in the following format:
//FirstLine (First line must be blank; removes byte order mark for this compiler)
states{}
finalStates{}
startStates{}
alphabet{}
totalTrans=

StartingNode, Alphabet connection, Ending Node (eg. 1, A, 2) (States and connection names must be either alphabet characters, numbers, or 'emp')
#//end of file

Graph NFA_to_DFA(Graph) - takes in an NFA and transforms it to a DFA; Returns the DFA
Graph HopCroft(Graph) - takes in a DFA and minimizes redundent states; Returns the improved Graph
int main() - main method; Controls the flow control and calls other methods
*/

#include<iostream>
#include<vector>
#include<fstream>
#include<stdlib.h>
#include<stdio.h>
#include<algorithm>
#include<math.h>
#include<string>
#include "MetaNodeObject.cpp"

//Reads in raw data from a given text file to set into a graph object
Graph ReadIn(std::string fileName) {
	Graph toReturn;
	std::ifstream G(fileName);
	std::string Line;
	if (G.is_open()) {
		std::getline(G, Line); //Buffer, gets rid of Byte Order Mark

							   //Get Graph Attributes
		for (int i = 1; i<6; i++) {
			std::getline(G, Line);
			std::string toPush;
			if (i == 1) { //Gets States
				for (int j = 7; j<Line.size(); j++) {
					if (Line[j] != ',' && Line[j] != '}')
						toPush.push_back(Line[j]);
					else if (Line[j] == ',' || Line[j] == '}') {
						toReturn.States.push_back(toPush);
						toPush.clear();
					}
				}
			}
			else if (i == 2) { //Gets final state(s)
				for (int k = 12; k<Line.size(); k++) {
					if (Line[k] != ',' && Line[k] != '}')
						toPush.push_back(Line[k]);
					else if (Line[k] == ',' || Line[k] == '}') {
						toReturn.finalStates.push_back(toPush);
						toPush.clear();
					}
				}
			}
			else if (i == 3) { //Gets starting state
				for (int l = 12; l<Line.size(); l++) {
					if (Line[l] != ',' && Line[l] != '}')
						toPush.push_back(Line[l]);
					else if (Line[l] == ',' || Line[l] == '}') {
						toReturn.startState = toPush;
						toPush.clear();
					}
				}
			}
			else if (i == 4) { //Gets alphabet symbols
				for (int m = 9; m<Line.size(); m++) {
					if (Line[m] != ',' && Line[m] != '}')
						toPush.push_back(Line[m]);
					else if (Line[m] == ',' || Line[m] == '}') {
						toReturn.Alphabet.push_back(toPush);
						toPush.clear();
					}
				}
			}
			else if (i == 5) { //Gets total transitions
				for (int n = 11; n <= Line.size(); n++) {
					if (n != Line.size())
						toPush.push_back(Line[n]);
					else if (n == Line.size()) {
						toPush.push_back(Line[n]);
						toReturn.totalTrans = std::stoi(toPush);
						toPush.clear();
					}
				}
			}
		}

		//Gets Graph Data
		while (std::getline(G, Line)) {
			std::vector<std::string> toPushGraph;
			if (Line[0] != '/' && Line[0] != '#' && !Line.empty()) {
				std::string toPush;
				for (int i = 0; i <= Line.size(); i++) {
					if (Line[i] != ',' && i != Line.size())
						toPush.push_back(Line[i]);
					else if (Line[i] == ',') {
						toPushGraph.push_back(toPush);
						toPush.clear();
					}
					else if (i == Line.size()) {
						toPush.push_back(Line[i]);
						toPushGraph.push_back(toPush);
						toPush.clear();
					}
				} toReturn.graphData.push_back(toPushGraph);
			}
		} G.close();
	}
	else
		std::cout << "Critical Error Opening File!";

	//Gets rid of any weird unicode characters or white spaces from the file read-in
	for (int i = 0; i<toReturn.graphData.size(); i++)
		for (int j = 0; j<toReturn.graphData[i].size(); j++)
			if (!isdigit(toReturn.graphData[i][j].back()) && !isalpha(toReturn.graphData[i][j].back()))
				toReturn.graphData[i][j].pop_back();
	return toReturn;
}

Graph NFA_to_DFA(Graph NFA) {
	Graph DFA;  //Storage for the newly generated DFA object
	std::vector<std::vector<int>> Left; //Storage for left table column
	std::vector<std::vector<int>> Right;  //Storage for right table column
	bool toLoop = true; //Boolean to check for loop exit condition
	int CIndex = 0;     //Int to track which left string to process

						//Generates the relational table (Left and Right) using the starting (meta)node
	Node FirstNode(NFA, 0);
	Left.push_back(FirstNode.PNode);
	while (toLoop) {
		for (int i = 1; i<NFA.Alphabet.size(); i++) {
			Right.push_back(std::vector<int>());
			for (int j = 0; j<NFA.relMatrix.size(); j++)
				if (std::find(Left[CIndex].begin(), Left[CIndex].end(), j + stoi(NFA.States[0])) != Left[CIndex].end())
					for (int k = 0; k<NFA.relMatrix[j].size(); k++) {
						if (NFA.relMatrix[j][k] == NFA.Alphabet[i]) {
							Node MNode(NFA, k);
							for (int l = 0; l<MNode.PNode.size(); l++)
								if (!(std::find(Right.back().begin(), Right.back().end(), MNode.PNode[l]) != Right.back().end()))
									Right.back().push_back(MNode.PNode[l]);
							std::sort(Right.back().begin(), Right.back().end());
						}
					}
		} if (Left[CIndex] == Left.back()) { toLoop = false; }
		for (int k = 0; k<Right.size(); k++) {
			if (!(std::find(Left.begin(), Left.end(), Right[k]) != Left.end()))
				if (!Right[k].empty()) {
					Left.push_back(Right[k]);
					toLoop = true;
				}
		} CIndex++;
	}

	//Sets basic data for the new DFA graph object
	int TransitionN = 0;                   //Sets number of transitions
	for (int i = 0; i<Right.size(); i++)
		if (!(Right[i].empty())) TransitionN++;
	DFA.totalTrans = TransitionN;
	for (int j = 0; j<Left.size(); j++) {       //Sets States
		std::string toPush = "";
		for (int k = 0; k<Left[j].size(); k++)
			toPush.append(std::to_string(Left[j][k]));
		DFA.States.push_back(toPush);
	}
	DFA.startState = DFA.States[0];        //Sets startState
	for (int k2 = 1; k2<NFA.Alphabet.size(); k2++) //Sets Alphabet
		DFA.Alphabet.push_back(NFA.Alphabet[k2]);
	for (int l = 0; l<DFA.States.size(); l++) { //Sets finalStates
		for (int m = 0; m<NFA.finalStates.size(); m++)
			if (DFA.States[l].find(NFA.finalStates[m]) != std::string::npos)
				DFA.finalStates.push_back(DFA.States[l]);
	}

	//Uses the left and right 2d vectors to generate the transition data of the DFA graph object
	for (int n = 0; n<Left.size(); n++) {
		for (int o = 0; o<DFA.Alphabet.size(); o++) {
			if (!(Right[(n*DFA.Alphabet.size()) + o].empty())) {
				DFA.graphData.push_back(std::vector<std::string>()); //Creates transition
				std::string toPushLeft = "";
				for (int p = 0; p<Left[n].size(); p++)
					toPushLeft.append(std::to_string(Left[n][p]));
				DFA.graphData.back().push_back(toPushLeft);          //Pushes original node
				for (int q = 0; q<DFA.Alphabet.size(); q++)
					if (o % DFA.Alphabet.size() == q) DFA.graphData.back().push_back(DFA.Alphabet[q]);   //Pushes alphabet value for transition
				std::string toPushRight = "";
				for (int r = 0; r<Right[(n*DFA.Alphabet.size()) + o].size(); r++)
					toPushRight.append(std::to_string(Right[(n*DFA.Alphabet.size()) + o][r]));            //Pushes node being transitioned to
				DFA.graphData.back().push_back(toPushRight);
			}
		}
	}
	return DFA;
}

Graph HopCroft(Graph DFA) {
	std::vector<std::vector<std::string>> hopTable;    //hopTable data storage
	std::vector<std::vector<std::string>> newHopTable; //Temporary storage for hopTable data
	std::vector<std::pair<int, int>> splicingIndexes;  //Indexes of where to differentiate nodes
	std::vector<std::vector<std::string>> tempTable;   //Temporary storage to fill with Alphabet connections
	std::vector<std::pair<int, int>> tempIndexes;      //Temporary storage to keep track of individual slices (Beginning, End)
	std::vector<std::string> Nodes;                    //Used as temporary storage for each node value (For Find())
	std::vector<char> nodeNumbers;                     //Keeps track of number of slices
	bool toLoop = true;

	//Generates the hop table
	for (int i = 0; i<DFA.States.size(); i++) { //Fills hopTable with no connections by default for each node
		hopTable.push_back(std::vector<std::string>());
		for (int j = 0; j<DFA.Alphabet.size(); j++)
			hopTable.back().push_back("None");
	}
	for (int k = 0; k<hopTable.size(); k++) {   //Fills hopTable with appropriate node connections
		for (int l = 0; l<DFA.graphData.size(); l++) {
			if (DFA.graphData[l][0] == DFA.States[k]) {
				ptrdiff_t pos = find(DFA.Alphabet.begin(),
					DFA.Alphabet.end(), DFA.graphData[l][1]) - DFA.Alphabet.begin();
				hopTable[k][pos] = DFA.graphData[l][2];
			}
		}
	} for (int m = 0; m<DFA.States.size(); m++) hopTable[m].insert(hopTable[m].begin(), DFA.States[m]);

	//Generates 0-Equivelance Classes
	splicingIndexes.push_back(std::make_pair(newHopTable.size(), -1));
	for (int m = 0; m<DFA.States.size(); m++) //Non-final states
		if (!(std::find(DFA.finalStates.begin(), DFA.finalStates.end(), DFA.States[m]) != DFA.finalStates.end()))
			newHopTable.push_back(hopTable[m]);
	splicingIndexes.back().second = newHopTable.size() - 1;
	for (int n = 0; n<DFA.States.size(); n++) //Final States
		if (std::find(DFA.finalStates.begin(), DFA.finalStates.end(), DFA.States[n]) != DFA.finalStates.end())
			newHopTable.push_back(hopTable[n]);
	splicingIndexes.push_back(std::make_pair(splicingIndexes.back().second + 1, newHopTable.size() - 1));
	hopTable = newHopTable;
	newHopTable.clear();

	//Generates the rest of the N-Equivelance Classes
	while (toLoop) { //Loops until table cannot be divided further
					 //Sets the connection values to Alphabet letters based on the slices
		int Decider = splicingIndexes.size();
		tempTable = hopTable;
		for (int i = 0; i<splicingIndexes.size(); i++) nodeNumbers.push_back((char)(65 + i));
		for (int j = 0; j < tempTable.size(); j++) Nodes.push_back(tempTable[j][0]);
		for (int k = 0; k<tempTable.size(); k++)
			for (int l = 1; l<tempTable[k].size(); l++)
				for (int m = 0; m<splicingIndexes.size(); m++) {
					std::vector<std::string> toFind;
					if (tempTable[k][l].size() >= 2) tempTable[k][l].pop_back();
					for (int p = splicingIndexes[m].first; p<splicingIndexes[m].second + 1; p++) toFind.push_back(Nodes[p]);
					if (std::find(toFind.begin(), toFind.end(), tempTable[k][l]) != toFind.end())
						tempTable[k][l] = nodeNumbers[m];
				}
		//Splicts the hoptable into new slices based on the Alphabet letters (None is ignored)
		tempIndexes = splicingIndexes;
		splicingIndexes.clear();
		for (int i = 0; i<tempIndexes.size(); i++) {
			for (int j = tempIndexes[i].first; j <= tempIndexes[i].second; j++) {
				Nodes.clear();
				for (int i2 = 0; i2<newHopTable.size(); i2++) Nodes.push_back(newHopTable[i2][0]);
				if (!(std::find(Nodes.begin(), Nodes.end(), tempTable[j][0]) != Nodes.end())) {
					splicingIndexes.push_back(std::make_pair(Nodes.size(), -1));
					for (int k = tempIndexes[i].first; k <= tempIndexes[i].second; k++)
						if (tempTable[k][1] == tempTable[j][1] && tempTable[k][2] == tempTable[j][2])
							newHopTable.push_back(hopTable[k]);
					splicingIndexes.back().second = newHopTable.size() - 1;
				}
			}
		} 
		if (Decider = splicingIndexes.size()) toLoop = false;
		hopTable = newHopTable;
	}

	Nodes.clear();
	for (int j = 0; j < hopTable.size(); j++) Nodes.push_back(hopTable[j][0]);

	//Generates a new graph object based on the new data
	Graph Min_DFA;
	Min_DFA.Alphabet = DFA.Alphabet; //Sets the alphabet of the new minimized graph
	for (int g = 0; g<splicingIndexes.size(); g++) { //Sets the new States/finalStates/startState for the graph
		std::string toAdd = "";
		bool isFirst = false; bool isFinal = false;
		for (int h = splicingIndexes[g].first; h <= splicingIndexes[g].second; h++) {
			toAdd.append(hopTable[h][0]);
			if (hopTable[h][0] == DFA.startState)
				isFirst = true;
			if (std::find(DFA.finalStates.begin(), DFA.finalStates.end(),
				hopTable[h][0]) != DFA.finalStates.end())
				isFinal = true;
		}
		Min_DFA.States.push_back(toAdd);
		if (isFirst) Min_DFA.startState = toAdd;
		if (isFinal) Min_DFA.finalStates.push_back(toAdd);
	}
	for (int i = 0; i<splicingIndexes.size(); i++) { //Sets the graph data based on the hopTable
		std::vector<std::string> toAdd; //Storage for the 3 parts of connection data (Starting Node, Connection Data, Ending Node)
		toAdd.push_back(std::string());
		for (int j = splicingIndexes[i].first; j <= splicingIndexes[i].second; j++) { //Gets the Starting Node data
			toAdd.front().append(hopTable[j][0]);
		} for (int z = 0; z<Min_DFA.Alphabet.size(); z++) toAdd.push_back(std::string()); //Sets blank data for each alphabet symbol
		for (int l = 1; l<Min_DFA.Alphabet.size() + 1; l++) {  //Determines which new meta node the ending node is a part of
			int starter; int finisher;
			if (hopTable[splicingIndexes[i].first][l] != "None") { //Doesn't attempt to find ending node data if node has no connection
				int pos = find(Nodes.begin(), Nodes.end(), hopTable[splicingIndexes[i].first][l]) - Nodes.begin();
				for (int m = 0; m<splicingIndexes.size(); m++) {
					if (pos >= splicingIndexes[m].first && pos <= splicingIndexes[m].second) {
						starter = splicingIndexes[m].first;
						finisher = splicingIndexes[m].second;
					}
				}
				std::string toAppend = "";
				for (int n = starter; n <= finisher; n++) {
					toAppend.append(Nodes[n]);
				}
				toAdd[l].append(toAppend);
			}
			else
				toAdd[l].append("None");  //Sets that connection to none if no connection
		}
		for (int l = 1; l <= Min_DFA.Alphabet.size(); l++) { //For every possible connection
			if (toAdd[l] != "None") { //If connection exists
				Min_DFA.graphData.push_back(std::vector<std::string>()); //Add a blank connection for graphData
				Min_DFA.graphData.back().push_back(toAdd[0]);            //Adds the starting Node for that connection
				Min_DFA.graphData.back().push_back(Min_DFA.Alphabet[l - 1]); //Adds the alphabet symbol for that connection
				Min_DFA.graphData.back().push_back(toAdd[l]);            //Adds the ending node
			}
		}
	} Min_DFA.totalTrans = Min_DFA.graphData.size();
	return Min_DFA;
}

int main() {
	Graph NFA = ReadIn("Data/Graph1.txt");
	if (!NFA.checkValidity())
		printf("Validity Error. Closing...");
	else {
		NFA.generateMatrix();
		Graph DFA = NFA_to_DFA(NFA);
		if (!DFA.checkValidity())
			printf("Validity Error. Closing...");
		else {
			Graph Min_DFA = HopCroft(DFA);
			std::cout << "DFA:\n"; DFA.showGraphData();
			std::cout << "\n\nMinimized DFA:\n"; Min_DFA.showGraphData();
		}
	}

	std::cin.get();
	return 0;
}
