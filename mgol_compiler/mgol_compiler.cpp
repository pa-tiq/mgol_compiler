// mgol_compiler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <regex>
#include <ctype.h>
#include <algorithm>

using std::cout;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::string;
using std::getline;
using std::istringstream;
using std::endl;
using std::map;
using std::multimap;
using std::pair;
using std::tuple;
using std::vector;
using std::get;

struct Tokens
{
	const string num{ "num" };
	const string id{ "id" };
	const string comentario{ "comentário" };
	const string OPR{ "OPR" };
	const string RCB{ "RCB" };
	const string OPM{ "OPM" };
	const string AB_P{ "AB_P" };
	const string FC_P{ "FC_P" };
	const string PT_V{ "PT_V" };
	const string ERRO{ "ERRO" };

	const string inicio{ "inicio" };
	const string varinicio{ "varinicio" };
	const string varfim{ "varfim" };
	const string leia{ "leia" };
	const string escreva{ "escreva" };
	const string se{ "se" };
	const string entao{ "entao" };
	const string senao{ "senao" };
	const string fimse{ "fimse" };
	const string fim{ "fim" };
	const string inteiro{ "inteiro" };
	const string literal{ "literal" };
	const string real{ "real" };
};

bool reservedWord(string word);
map<int, string> populatefinalStates();
string getToken(int state);
map<pair<char, int>, int> populateStateMatrix();
multimap<int, tuple<string, string, string>> populateMainTable();
char entry(char c);
pair<int,string> readMgol();
string spaces(int previous);

int main()
{
	//line number, lexeme, token, type
	multimap<int, tuple<string, string, string>> mainTable = populateMainTable();
	map <pair<char, int>, int> stateMatrix = populateStateMatrix();
	pair<int,string> SizeAndCode = readMgol();	
	string mgolCode = SizeAndCode.second;
	pair<int, string> token(0,"");
	
	while (token.second != "ERROR")
	{
		token = nextToken(stateMatrix, &mainTable, &mgolCode);
	}

	if (token.second != "ERROR")
	{
		cout << endl << endl << "Tabela: " << endl << endl << "         Linha     Lexema                         Token      Tipo" << endl << endl;

		for (auto it = mainTable.begin(); it != mainTable.end(); ++it)
		{
			cout << "             " << it->first << "     " << get<0>(it->second) << spaces(get<0>(it->second).size()) << get<1>(it->second) << "        " << get<2>(it->second) << endl;
		}
	}
	else
	{
		cout << endl << "ERRO: linha " << token.first << endl;
	}

	cout << endl << endl;

	system("pause");
	return 0;
}

pair<int,string> nextToken(map <pair<char, int>, int> stateMatrix, multimap<int, tuple<string, string, string>> *mainTable, string* mgolCode)
{
	
	tuple<string, string, string> mainTableValue;
	struct Tokens tokens;
	int i = 0;
	char *begin = mgolCode;
	char *end;
	string token = "";
	string lexeme = "";
	string word = "";
	vector<string> ids;
	int lineNum = 1;
	int state = 0;
	int stateAux = 0;

	for (char& c : mgolCode)
	{

	}

	for (it = mgolCode.begin(); it != mgolCode.end(); ++it)
	{
		i = 0;
		word = *it;
		begin = &word[i];
		end = begin;

		do
		{
			if (state == 18)
			{
				if (*(end + 1) == 0)
				{
					lexeme += string(begin, end + 1);
					lexeme += " ";
				}
				if (word[i] == '"')
				{
					stateAux = stateMatrix[pair<char, int>(entry(word[i]), state)];
					lexeme += string(begin, end + 1);
				}
			}
			else if (state == 16)
			{
				if (*(end + 1) == 0)
				{
					lexeme += string(begin, end + 1);
					lexeme += " ";
				}
				if (word[i] == '}')
				{
					stateAux = stateMatrix[pair<char, int>(entry(word[i]), state)];
					lexeme += string(begin, end + 1);
				}
			}
			else
			{
				if ((state == 2 || state == 4) && (word[i] == 'e' || word[i] == 'E'))
				{
					stateAux = stateMatrix[pair<char, int>(word[i], state)];
				}
				else
					stateAux = stateMatrix[pair<char, int>(entry(word[i]), state)];
			}
			end = &word[++i];

			if ((stateAux == 0 || *end == 0) && state != 18 && state != 16)
			{
				if (stateAux == 0 && state == 0) return lineNum;

				if (stateAux == 0)
				{
					if (state != 19 && state != 20 && stateAux != 19 && stateAux != 20) lexeme = string(begin, --end);
					else --end;
					begin = end;
					--i;
				}
				else
				{
					if (state != 19 && state != 20 && stateAux != 19 && stateAux != 20) lexeme = string(begin, end);
					state = stateAux;
				}

				if (!lexeme.empty())
				{
					token = getToken(state);
					if (token.empty()) return lineNum;

					mainTableValue = tuple<string, string, string>(lexeme, token, "");
					if (!reservedWord(lexeme))
					{
						cout << endl << "[LEXEMA] " << lexeme << " em linha " << lineNum << endl;
						if (token == tokens.id)
						{
							if (std::find(ids.begin(), ids.end(), lexeme) == ids.end())
							{
								ids.push_back(lexeme);
								(*mainTable).emplace(lineNum, mainTableValue);
							}
						}
					}
					else cout << endl << "[PALAVRA RESERVADA] " << lexeme << " em linha " << lineNum << endl;
					lexeme = "";
					state = 0;
				}
				else
					return lineNum;

			}
			else
			{
				state = stateAux;
			}

			if (stateAux == -1)
			{
				state = 0;
				stateAux = 0;
				begin = end;
			}

		} while (0 != *end);

		if (lCount[lineNum] == 1)
		{
			lineNum++;
		}
		else
		{
			lCount[lineNum]--;
		}

	}

	if (state != 0) return lineNum;

	return -1;
}

pair<int,string> readMgol()
{
	ifstream inmgol("mgol.alg");
	ofstream outmgol("x.alg");

	string mgolCode;
	string rawline;
	int lineCount = 1;

	while (getline(inmgol, rawline))
	{
		istringstream iss(rawline);
		mgolCode += rawline;
		lineCount++;
		cout << lineCount << " " << rawline << endl;
	}

	return pair<int,string>(lineCount,mgolCode);
}

bool reservedWord(string word)
{
	struct Tokens tokens;
	return (word == tokens.inicio ||
			word == tokens.varinicio ||
			word == tokens.varfim ||
			word == tokens.leia ||
			word == tokens.escreva ||
			word == tokens.se ||
			word == tokens.entao ||
			word == tokens.senao ||
			word == tokens.fimse ||
			word == tokens.fim ||
			word == tokens.inteiro ||
			word == tokens.literal ||
			word == tokens.real);
}

map<int, string> populatefinalStates()
{
	struct Tokens tokens;
	map <int, string> m;

	m =
	{
		{ 1,tokens.id },
		{ 2,tokens.num },
		{ 4,tokens.num },
		{ 7,tokens.num },
		{ 8,tokens.fim },
		{ 9,tokens.OPR },
		{ 10,tokens.OPR },
		{ 11,tokens.RCB },
		{ 12,tokens.OPR },
		{ 13,tokens.AB_P },
		{ 14,tokens.FC_P },
		{ 15,tokens.PT_V },
		{ 17,tokens.OPM },
		{ 19,tokens.literal },
		{ 20,tokens.comentario }
	};

	return m;
}

string getToken(int state)
{
	map <int, string> finalStates = populatefinalStates();

	return finalStates[state];
}

map<pair<char, int>, int> populateStateMatrix()
{
	//pair<entry,current state> destination state
	map<pair<char, int>, int> m;

	m =
	{
		{ pair<char,int>('\t',0),-1 },

		{ pair<char,int>('L',0),1 },
		{ pair<char,int>('L',1),1 },
		{ pair<char,int>('_',1),1 },

		{ pair<char,int>('D',0),2 },
		{ pair<char,int>('D',1),1 },
		{ pair<char,int>('D',2),2 },
		{ pair<char,int>('D',3),4 },
		{ pair<char,int>('D',4),4 },
		{ pair<char,int>('D',5),7 },
		{ pair<char,int>('D',6),7 },
		{ pair<char,int>('D',7),7 },

		{ pair<char,int>('E',2),5 },
		{ pair<char,int>('E',4),5 },
		{ pair<char,int>('e',2),5 },
		{ pair<char,int>('e',4),5 },
		{ pair<char,int>('.',2),3 },
		{ pair<char,int>('-',5),6 },
		{ pair<char,int>('+',5),6 },

		{ pair<char,int>('(',0),13 },
		{ pair<char,int>(')',0),14 },
		{ pair<char,int>(';',0),15 },
		{ pair<char,int>('+',0),17 },
		{ pair<char,int>('*',0),17 },
		{ pair<char,int>('-',0),17 },
		{ pair<char,int>('/',0),17 },

		{ pair<char,int>('>',0),9 },
		{ pair<char,int>('<',0),10 },
		{ pair<char,int>('=',0),9 },
		{ pair<char,int>('>',10),12 },
		{ pair<char,int>('=',10),12 },
		{ pair<char,int>('=',9),12 },
		{ pair<char,int>('-',10),11 },

		{ pair<char,int>('"',0),18 },
		{ pair<char,int>('"',18),19 },
		{ pair<char,int>('{',0),16 },
		{ pair<char,int>('}',16),20 },

	};

	return m;
}

multimap<int, tuple<string, string, string>> populateMainTable()
{
	struct Tokens tokens;
	// line number, lexeme, token, type
	multimap<int, tuple<string, string, string>> m;

	m =
	{
		{ 0, tuple<string, string, string>(tokens.inicio, tokens.inicio, " ") },
		{ 0, tuple<string, string, string>(tokens.varinicio, tokens.varinicio, " ") },
		{ 0, tuple<string, string, string>(tokens.varfim, tokens.varfim, " ") },
		{ 0, tuple<string, string, string>(tokens.leia, tokens.leia, " ") },
		{ 0, tuple<string, string, string>(tokens.escreva, tokens.escreva, " ") },
		{ 0, tuple<string, string, string>(tokens.se, tokens.se, " ") },
		{ 0, tuple<string, string, string>(tokens.entao, tokens.entao, " ") },
		{ 0, tuple<string, string, string>(tokens.senao, tokens.senao, " ") },
		{ 0, tuple<string, string, string>(tokens.fimse, tokens.fimse, " ") },
		{ 0, tuple<string, string, string>(tokens.inteiro, tokens.inteiro, " ") },
		{ 0, tuple<string, string, string>(tokens.literal, tokens.literal, " ") },
		{ 0, tuple<string, string, string>(tokens.real, tokens.real, " ") },
		{ 0, tuple<string, string, string>(tokens.fim, tokens.fim, " ") },
	};

	return m;
}

char entry(char c)
{
	if (isalpha(c)) return 'L';
	if (isdigit(c)) return 'D';
	else return c;
}

string spaces(int previous)
{
	int sp = 30;
	int i = 0;
	string spac = " ";

	for (i = 0; i < sp - previous; i++)
	{
		spac += " ";
	}

	return spac;
}




