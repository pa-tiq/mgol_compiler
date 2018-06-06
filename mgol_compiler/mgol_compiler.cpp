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
#include <stack>

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
using std::stack;
using std::to_string;

struct Tokens
{
	const string num{ "num" };
	const string id{ "id" };
	const string comentario{ "comentario" };
	const string OPR{ "OPR" };
	const string RCB{ "RCB" };
	const string OPM{ "OPM" };
	const string AB_P{ "AB_P" };
	const string FC_P{ "FC_P" };
	const string PT_V{ "PT_V" };
	const string ERRO{ "ERRO" };
	const string EndOfFile{ "EOF" };

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
	const string inteiro{ "int" };
	const string lit{ "lit" };
	const string real{ "real" };
};

multimap<int, tuple<string, string, string>> populateMainTable();
map<int, string> populatefinalStates();
map<pair<char, int>, int> populateLexicalStateMatrix();
pair<int, string> readMgol();

class Lexical
{
	public:
	int lineNum;
	int lineCount;
	string mgolCode;
	map <pair<char, int>, int> stateMatrix;
	multimap<int, tuple<string, string, string>> mainTable;
	map<int, string> finalStates;
	vector<string> ids;

	void initialize()
	{
		pair<int, string> sizeAndCode = readMgol();

		mainTable = populateMainTable();
		stateMatrix = populateLexicalStateMatrix();
		lineCount = sizeAndCode.first;
		mgolCode = sizeAndCode.second;
		finalStates = populatefinalStates();
		lineNum = 1;
	}
};

class Syntatic
{

};

bool reservedWord(string word);
string getToken(int state);
char entry(char c, int state);
string spaces(int previous);
void syntactic();
pair<int, string> nextToken(Lexical *lex);
void syntatic();

int main()
{
	//syntactic();

	Tokens tokens;
	Lexical lex;
	lex.initialize();

	pair<int, string> line_token(0,"");	
	
	while (line_token.first >= 0 && line_token.second != tokens.EndOfFile)
	{
		line_token = nextToken(&lex);
	}

	if (line_token.first > 0 || line_token.second == tokens.EndOfFile)
	{
		cout << endl << endl << "Tabela: " << endl << endl << "         Linha     Lexema                         Token      Tipo" << endl << endl;

		for (auto it = lex.mainTable.begin(); it != lex.mainTable.end(); ++it)
		{
			cout << "             " << it->first << "     " << get<0>(it->second) << spaces(get<0>(it->second).size()) << get<1>(it->second) << "        " << get<2>(it->second) << endl;
		}
	}
	else
	{
		if(!line_token.second.empty())
			cout << endl << "ERRO - linha " << line_token.first*-1 << " - '" <<line_token.second << "'" << endl;
	}

	cout << endl << endl;

	system("pause");
	return 0;
}

void syntactic()
{
	Tokens tokens;
	Lexical lex;
	lex.initialize();
	stack<string> synth;
	int state = -1;
	int stateAux = -1;
	string action = "";
	pair<string,string> reduction("","");
	map<pair<string, int>, int> synthMatrix = populateSyntaticStateMatrix();
	map<int, pair<string, string>> productions = populateProductions();
	pair<int, string> line_token(0, "");

	line_token = nextToken(&lex);

	while (line_token.first >= 0 && line_token.second != tokens.EndOfFile)
	{		
		stateAux = synthMatrix[pair<string, int>(line_token.second, state)];
		action = getNextAction(&stateAux, line_token.second);
		 
		if (action == "shift")
		{
			state = stateAux;
			synth.push(line_token.second);
			synth.push(to_string(state));
			line_token = nextToken(&lex);
		}
		else if (action == "reduce")
		{
			state = stateAux;
			reduction = productions[state];
		}
		else if (action == "accept")
		{

		}

	}
}

string getNextAction(int *state, string token)
{

}

pair<int,string> nextToken(Lexical *lex)
{	
	tuple<string, string, string> mainTableValue;
	struct Tokens tokens;
	char *begin = &lex->mgolCode.front();
	char *beginLexeme = begin;
	string lexeme = "";
	string token = "";
	string error = "";
	int stateAux = -1;
	int state = -1;
	char ent;

	for (char& c : lex->mgolCode)
	{
		if (c == '\0' && stateAux == -1 && state == -1) 
			return pair<int, string>(lex->lineNum, tokens.EndOfFile);

		begin = &c;
		ent = entry(c, state);		
		stateAux = lex->stateMatrix[pair<char, int>(ent, state)];
				
		if (stateAux == -1 && state != -1) break;
		else if (stateAux == -1 && state == -1)
		{
			if (c == '\n') lex->lineNum++;
			beginLexeme++;
		}
		else if (stateAux == 0)
		{
			token = lex->finalStates[state];
			error = lex->finalStates[lex->stateMatrix[pair<char, int>(ent, -1)]];

			if (!token.empty() && !error.empty())
			{
				if (token != error)
				{
					if (token == tokens.id || token == tokens.num)
					{
						if (string(beginLexeme, begin) == tokens.se)
						{
							if (error == tokens.AB_P) break;
						}
						if (error == tokens.PT_V || error == tokens.OPR || error == tokens.OPM || error == tokens.FC_P) break;
					}

					if (token == tokens.AB_P)
					{
						if(error == tokens.id || error == tokens.num) break;
						else return pair<int, string>(lex->lineNum*-1, "Falta um valor ou variavel depois desse Abre Parenteses: " + string(beginLexeme, begin + 1));
					}		
					

					if (token == tokens.OPR || token == tokens.OPM || token == tokens.RCB)
					{
						if(error == tokens.id || error == tokens.num) break;
						else return pair<int, string>(lex->lineNum*-1, "Falta um valor ou variavel aqui: " + string(beginLexeme, begin + 1));
					}
					
				}
				else return pair<int, string>(lex->lineNum*-1, "Token duplicado: "+string(beginLexeme, begin + 1));
			}
			else if (!token.empty() && error.empty()) return pair<int, string>(lex->lineNum*-1, "Caractere invalido: " + string(begin, begin + 1));

			return pair<int, string>(lex->lineNum*-1, "Caractere invalido: "+string(beginLexeme, begin+1));
		}

		state = stateAux;
	} 

	token = lex->finalStates[state];
	lexeme = string(beginLexeme, begin);

	if (!token.empty())
	{		
		lex->mgolCode = string(begin, lex->mgolCode.length());

		if (token == tokens.id)
		{
			if (!reservedWord(lexeme))
			{
				if (std::find(lex->ids.begin(), lex->ids.end(), lexeme) == lex->ids.end())
				{
					lex->ids.push_back(lexeme);
				}
				mainTableValue = tuple<string, string, string>(lexeme, token, "");
				lex->mainTable.emplace(lex->lineNum, mainTableValue);

			}
			else
			{
				token = "PALAVRA RESERVADA";
			}
		}

		cout << endl << "Linha " << lex->lineNum << " [" << token << "] " << lexeme;

		return pair<int, string>(lex->lineNum, token);
	}
	else
	{
		if(state == 16 || state == 18) return pair<int, string>(lex->lineNum*-1, "Vc esqueceu de fechar alguma coisa aqui: "+lexeme.substr(0,7)+"...");
		return pair<int, string>(lex->lineNum*-1, lexeme);
	}
}

pair<int,string> readMgol()
{
	ifstream inmgol("mgol.alg");
	ofstream outmgol("x.alg");

	string mgolCode;
	string rawline;
	int lineCount = 0;

	while (getline(inmgol, rawline))
	{
		lineCount++;
		istringstream iss(rawline);
		cout << lineCount << " " << rawline << endl;
		if (lineCount > 1) rawline = "\n" + rawline;
		mgolCode += rawline;				
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
			word == tokens.lit ||
			word == tokens.real);
}

map<int, string> populatefinalStates()
{
	struct Tokens tokens;

	return 
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
		{ 19,tokens.lit },
		{ 20,tokens.comentario }
	};

}

string getToken(int state)
{
	map <int, string> finalStates = populatefinalStates();

	return finalStates[state];
}

map<pair<char, int>, int> populateLexicalStateMatrix()
{
	//pair<entry,current state> destination state
	return
	{
		{ pair<char,int>('S',-1),-1 },
		{ pair<char,int>('S',1),-1 },
		{ pair<char,int>('S',1),-1 },
		{ pair<char,int>('S',2),-1 },
		{ pair<char,int>('S',4),-1 },
		{ pair<char,int>('S',7),-1 },
		{ pair<char,int>('S',8),-1 },
		{ pair<char,int>('S',9),-1 },
		{ pair<char,int>('S',10),-1 },
		{ pair<char,int>('S',11),-1 },
		{ pair<char,int>('S',12),-1 },
		{ pair<char,int>('S',13),-1 },
		{ pair<char,int>('S',14),-1 },
		{ pair<char,int>('S',15),-1 },
		{ pair<char,int>('S',17),-1 },
		{ pair<char,int>('S',19),-1 },
		{ pair<char,int>('S',20),-1 },

		{ pair<char,int>(';',1),-1 },
		{ pair<char,int>(';',2),-1 },
		{ pair<char,int>(';',4),-1 },
		{ pair<char,int>(';',7),-1 },
		{ pair<char,int>(';',19),-1 },
			   
		{ pair<char,int>('L',-1),1 },
		{ pair<char,int>('L',1),1 },
		{ pair<char,int>('_',1),1 },		
			   
		{ pair<char,int>('D',-1),2 },
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
			   
		{ pair<char,int>('(',-1),13 },
		{ pair<char,int>(')',-1),14 },
		{ pair<char,int>(';',-1),15 },
		{ pair<char,int>('+',-1),17 },
		{ pair<char,int>('*',-1),17 },
		{ pair<char,int>('-',-1),17 },
		{ pair<char,int>('/',-1),17 },
			   
		{ pair<char,int>('>',-1),9 },
		{ pair<char,int>('<',-1),10 },
		{ pair<char,int>('=',-1),9 },
		{ pair<char,int>('>',10),12 },
		{ pair<char,int>('=',10),12 },
		{ pair<char,int>('=',9),12 },
		{ pair<char,int>('-',10),11 },			   
		
		{ pair<char,int>('"',-1),18 },
		{ pair<char,int>('A',18),18 },
		{ pair<char,int>('"',18),19 },
		{ pair<char,int>('{',-1),16 },
		{ pair<char,int>('A',16),16 },
		{ pair<char,int>('}',16),20 },

	};

}

map<pair<string, int>, int> populateSyntaticStateMatrix()
{
	Tokens tokens;
	//pair<entry,current state> destination state
	return
	{

	{ pair<string,int>("P",-1), 1 },
	{ pair<string,int>(tokens.inicio,-1),2 },

	{ pair<string,int>("V",2),3 },
	{ pair<string,int>(tokens.varinicio,2),4 },

	{ pair<string,int>("A",3),5 },
	{ pair<string,int>("ES",3),6 },
	{ pair<string,int>("CMD",3),7 },
	{ pair<string,int>("COND",3),8 },
	{ pair<string,int>("CABEÇALHO",3),13 },
	{ pair<string,int>(tokens.fim,3),9 },
	{ pair<string,int>(tokens.escreva,3),10 },
	{ pair<string,int>(tokens.leia,3),11 },
	{ pair<string,int>(tokens.id,3),12 },
	{ pair<string,int>(tokens.se,3),-1 },

	{ pair<string,int>("LV",4),15 },
	{ pair<string,int>("D",4),16 },
	{ pair<string,int>(tokens.varfim,4),17 },
	{ pair<string,int>(tokens.id,4),18 },

	{ pair<string,int>("A",6),19 },
	{ pair<string,int>("ES",6),6 },
	{ pair<string,int>("CMD",6),7 },
	{ pair<string,int>("COND",6),8 },
	{ pair<string,int>("CABEÇALHO",6),13 },
	{ pair<string,int>(tokens.fim,6),9 },
	{ pair<string,int>(tokens.escreva,6),10 },
	{ pair<string,int>(tokens.leia,6),11 },
	{ pair<string,int>(tokens.id,6),12 },
	{ pair<string,int>(tokens.se,6),14 },

	{ pair<string,int>("A",7),20 },
	{ pair<string,int>("ES",7),6 },
	{ pair<string,int>("CMD",7),7 },
	{ pair<string,int>("COND",7),8 },
	{ pair<string,int>("CABEÇALHO",7),13 },
	{ pair<string,int>(tokens.fim,7),9 },
	{ pair<string,int>(tokens.escreva,7),10 },
	{ pair<string,int>(tokens.leia,7),11 },
	{ pair<string,int>(tokens.id,7),12 },
	{ pair<string,int>(tokens.se,7),14 },

	{ pair<string,int>("A",8),21 },
	{ pair<string,int>("ES",8),6 },
	{ pair<string,int>("CMD",8),7 },
	{ pair<string,int>("COND",8),8 },
	{ pair<string,int>("CABEÇALHO",8),13 },
	{ pair<string,int>(tokens.fim,8),9 },
	{ pair<string,int>(tokens.escreva,8),10 },
	{ pair<string,int>(tokens.leia,8),11 },
	{ pair<string,int>(tokens.id,8),12 },

	{ pair<string,int>(tokens.se,8),14 },

	{ pair<string,int>("ARG",10),22 },
	{ pair<string,int>(tokens.lit,10),23 },
	{ pair<string,int>(tokens.num,10),24 },
	{ pair<string,int>(tokens.id,10),25 },

	{ pair<string,int>(tokens.id,11),26 },

	{ pair<string,int>(tokens.RCB,12),27 },

	{ pair<string,int>("ES",13),29 },
	{ pair<string,int>("CMD",13),7 },
	{ pair<string,int>("CORPO",13),7 },
	{ pair<string,int>("COND",13),8 },
	{ pair<string,int>("CABEÇALHO",13),13 },
	{ pair<string,int>(tokens.fim,13),9 },
	{ pair<string,int>(tokens.escreva,13),10 },
	{ pair<string,int>(tokens.leia,13),11 },
	{ pair<string,int>(tokens.id,13),12 },
	{ pair<string,int>(tokens.se,13),14 },
	
	{ pair<string,int>(tokens.AB_P,14),33 },

	{ pair<string,int>(tokens.varfim,16),17 },
	{ pair<string,int>(tokens.id,16),18 },
	{ pair<string,int>("D",16),16 },
	{ pair<string,int>("LV",16),34 },

	{ pair<string,int>(tokens.PT_V,17),35 },

	{ pair<string,int>(tokens.inteiro,18),37 },
	{ pair<string,int>(tokens.real,18),38 },
	{ pair<string,int>(tokens.lit,18),39 },
	{ pair<string,int>("TIPO",18),36 },

	{ pair<string,int>(tokens.PT_V,22),40 },

	{ pair<string,int>(tokens.PT_V,26),58 },

	{ pair<string,int>(tokens.id,27),43 },
	{ pair<string,int>(tokens.num,27),44 },
	{ pair<string,int>("LD",27),47 },
	{ pair<string,int>("OPRD",27),42 },

	{ pair<string,int>("CORPO",29),45 },
	{ pair<string,int>("ES",29),29 },
	{ pair<string,int>("CMD",29),30 },
	{ pair<string,int>("COND",29),31 },
	{ pair<string,int>("CABEÇALHO",29),13 },
	{ pair<string,int>(tokens.fimse,29),32 },
	{ pair<string,int>(tokens.escreva,29),10 },
	{ pair<string,int>(tokens.leia,29),11 },
	{ pair<string,int>(tokens.id,29),12 },
	{ pair<string,int>(tokens.se,29),14 },

	{ pair<string,int>("CORPO",30),46 },
	{ pair<string,int>("ES",30),29 },
	{ pair<string,int>("CMD",30),30 },
	{ pair<string,int>("COND",30),31 },
	{ pair<string,int>("CABEÇALHO",30),13 },
	{ pair<string,int>(tokens.fimse,30),32 },
	{ pair<string,int>(tokens.escreva,30),10 },
	{ pair<string,int>(tokens.leia,30),11 },
	{ pair<string,int>(tokens.id,30),12 },
	{ pair<string,int>(tokens.se,30),14 },

	{ pair<string,int>("CORPO",31),47 },
	{ pair<string,int>("ES",31),29 },
	{ pair<string,int>("CMD",31),30 },
	{ pair<string,int>("COND",31),31 },
	{ pair<string,int>("CABEÇALHO",31),13 },
	{ pair<string,int>(tokens.fimse,31),32 },
	{ pair<string,int>(tokens.escreva,31),10 },
	{ pair<string,int>(tokens.leia,31),11 },
	{ pair<string,int>(tokens.id,31),12 },
	{ pair<string,int>(tokens.se,31),14 },

	{ pair<string,int>(tokens.id,33),43 },
	{ pair<string,int>(tokens.num,33),44 },
	{ pair<string,int>("CORPO",33),49 },
	{ pair<string,int>("ËXP_R",33),48 },

	{ pair<string,int>(tokens.PT_V,36),50 },

	{ pair<string,int>(tokens.PT_V,41),51 },

	{ pair<string,int>(tokens.OPM,42),52 },

	{ pair<string,int>(tokens.FC_P,48),53 },

	{ pair<string,int>(tokens.OPR,49),54 },

	{ pair<string,int>("OPRD",52),55 },
	{ pair<string,int>(tokens.id,52),43 },
	{ pair<string,int>(tokens.num,52),44 },

	{ pair<string,int>(tokens.entao,53),56 },

	{ pair<string,int>("OPRD",54),57 },
	{ pair<string,int>(tokens.id,54),43 },
	{ pair<string,int>(tokens.num,54),44 },	

	};

}

map<int, pair<string,string>> populateProductions()
{
	struct Tokens tokens;
	return
	{
	{1,pair<string,string>("P'","P")},
	{2,pair<string,string>("P",tokens.inicio+" V"+" A")},
	{3,pair<string,string>("V",tokens.varinicio + " LV")},
	{4,pair<string,string>("LV","D LV")},
	{5,pair<string,string>("LV",tokens.varfim+" ;")},
	{6,pair<string,string>("D",tokens.id+" TIPO ;")},
	{7,pair<string,string>("TIPO",tokens.inteiro)},
	{8,pair<string,string>("TIPO",tokens.real)},
	{9,pair<string,string>("TIPO",tokens.lit)},
	{10,pair<string,string>("A","ES A")},
	{11,pair<string,string>("ES",tokens.leia + " "+tokens.id+" ;")},
	{12,pair<string,string>("ES",tokens.escreva + " ARG ;")},
	{13,pair<string,string>("ARG",tokens.lit)},
	{14,pair<string,string>("ARG",tokens.num)},
	{15,pair<string,string>("ARG",tokens.id)},
	{16,pair<string,string>("A","CMD A")},
	{17,pair<string,string>("CMD",tokens.id+" "+tokens.RCB+" LD ;")},
	{18,pair<string,string>("LD","OPRD "+tokens.OPM+" OPRD")},
	{19,pair<string,string>("LD","OPRD")},
	{20,pair<string,string>("OPRD",tokens.id)},
	{21,pair<string,string>("OPRD",tokens.num)},
	{22,pair<string,string>("A","COND A")},
	{23,pair<string,string>("COND","CABEÇALHO CORPO")},
	{24,pair<string,string>("CABEÇALHO",tokens.se+" "+tokens.AB_P+" EXP_R "+tokens.FC_P+" "+tokens.entao)},
	{25,pair<string,string>("EXP_R","OPRD "+tokens.OPR+" OPRD")},
	{26,pair<string,string>("CORPO","ES CORPO")},
	{27,pair<string,string>("CORPO","CMD CORPO")},
	{28,pair<string,string>("CORPO","COND CORPO")},
	{29,pair<string,string>("CORPO",tokens.fimse)},
	{30,pair<string,string>("A",tokens.fim)},
	};

}

multimap<int, tuple<string, string, string>> populateMainTable()
{
	struct Tokens tokens;
	// line number, lexeme, token, type
	return	
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
		{ 0, tuple<string, string, string>(tokens.lit, tokens.lit, " ") },
		{ 0, tuple<string, string, string>(tokens.real, tokens.real, " ") },
		{ 0, tuple<string, string, string>(tokens.fim, tokens.fim, " ") },
	};
}

char entry(char c, int state)
{
	if (state == 2 || state == 4)
	{
		if (c == 'e') return c;
		if (c == 'E') return c;
	}

	if (state == 18)
	{
		if (c != '"') return 'A';
		else return c;
	}

	if (state == 16)
	{
		if (c != '}') return 'A';
		else return c;
	}

	if (c == '\n' || c == ' ' || c == '\t' || c == '\0') return 'S';

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




