#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>

using namespace std;

vector<string> split(string, string, int);

class Attribute
{
public:
	string name;
	string value;
};

class Element
{
public:
	string tag;
	string data;
	vector <Attribute> attributes;
	vector <Element> children;
};

class Document
{
public:
	ifstream file;
	string version;
	string encoding;
	Element root;
	bool validity;
	Document();
	Document(const char *);
	void printMe();
	~Document();
};

void parse(Document *);

bool checkHeader(Document *, string);

bool fetchElements(Document *, string);

bool checkElements(string);

void recursiveElements(Element *, string);

void traverseElements(Element *, string);

string filter(string);

string cleanText(string);

string filter(string, string);
