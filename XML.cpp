#include "XML.h"
#include <stack>

vector<string> split(string str, string del,int cut=0)
{
    vector<string> arr;
    while(str!=del && str!="")
    {
    	if(cut>0)
    	{
    		if(arr.size()==cut)
    		{
    			arr.push_back(str);
    			return arr;
    		}
    	}
        if(str.find(del)>str.length())
        {
            arr.push_back(str);
            break;
        }
        if(str.substr(0, str.find(del)).length())
            arr.push_back(str.substr(0, str.find(del)));
        if((str.find(del)+del.length())<str.length())
            str=str.substr(str.find(del)+del.length());
        else
            str="";
    }
    return arr;
}


Document::Document(const char *addr)
{
	Document::file.open(addr);
}
Document::Document()
{

}
void Document::printMe()
{
	cout<<"version "<<Document::version<<endl;
	cout<<"encoding "<<Document::encoding<<endl;
	traverseElements(&(Document::root), "");
}
Document::~Document()
{
	//Document::file.close(); this has been done already, add a check to do it if not done beforhand
}

void parse(Document *document)
{
	char ch;
	string str;
	string temp;
	while(document->file.get(ch))
	{
		str+=ch; //need to remove all extra spaces
	}
	document->file.close();
	str=split(str, "?>")[0]+"?>\n"+cleanText(split(str, "?>")[1]);
	cout<<str;return ;
	if(split(str, "\n", 1).size()>1)
	{
		temp=split(str, "\n", 1)[0];
		temp=split(temp, ">", 1)[0];
		document->validity= checkHeader(document, temp);
		if(!document->validity)
		{
			return;
		}
	}
	
	if(split(str, "\n", 1).size()>1)
	{
		temp=split(str, "\n", 1)[1];
		document->validity= fetchElements(document, temp);
		if(!document->validity)
		{
			return;
		}
	}
}

bool checkHeader(Document *document, string temp)
{
	string temp1="";
	if(temp.length()<6)
	{
		return false; //a valid xml header must have atleast "<?xml?" i.e. 6 characters
	}
	if(temp.substr(0, 5)!= "<?xml" || temp[temp.length()-1]!= '?')
	{
		return false;
	}
	if(split(temp, "version=\"").size()>1)
	{
		temp1=split(temp, "version=\"")[1];
		temp1=split(temp1, "\"", 1)[0];
		document->version= temp1;
	}
	if(split(temp, "encoding=\"").size()>1)
	{
		temp1=split(temp, "encoding=\"")[1];
		temp1=split(temp1, "\"", 1)[0];
		document->encoding=temp1;
	}
	
	return true;
}

bool fetchElements(Document *document, string temp)
{
	if(!checkElements(temp)) //check to see if the tags all match
	{
		return false;
	}
	recursiveElements(&(document->root), temp);
	return true;
}

bool checkElements(string temp)
{
	stack<string> stk;
	bool check=false;
	string temp1;
	for(int i=0; i<temp.length();)
	{
		temp1="";
		while(temp[i]!='<' && i<temp.length())
		{
			if(temp[i]=='>')
			{
				return false;
			}
			i++;
		}
		i++;
		while(temp[i]!='>' && i<temp.length()) 
		{
			if(temp[i]=='<')
			{
				return false;
			}
			temp1+=temp[i];
			i++;
		}
		if(i>=temp.length())
		{
			break;
		}
		i++;
		if(temp1[0]!='/')
		{
			temp1=split(temp1, " ")[0];
			stk.push(temp1);
		}
		if(temp1[0]=='/')
		{
			if('/'+stk.top()==temp1)
			{
				stk.pop();
			}
			else
			{
				return false;
			}
		}
	}
	return stk.size()==0?true:false;
}

void recursiveElements(Element *element, string str)
{
	string tag;
	tag=split(str, "<", 1)[0];
	tag=split(tag, ">", 1)[0];
	//also deal with spaces
	element->tag=tag;
	element->data="";
	
	int noOfOpenTags=0;
	int noOfCloseTags=0;
	for(int i=0;i<str.length();i++)
	{
		if(str[i]=='<')
			noOfOpenTags++;
		if(str[i]=='>')
			noOfCloseTags++;
	}
	if(noOfOpenTags==2 && noOfCloseTags) //str contains simple data
	{
		element->data= str.substr(element->tag.length()+2, str.length()-5-2*element->tag.length());
		string s="";
		for(int i=0;i<element->data.length();i++)
		{
			if(element->data[i]=='\n')
			{
				s+="\\n";
				continue;
			}
			if(element->data[i]=='\r')
			{
				s+="\\r";
				continue;
			}
			if(element->data[i]=='\t')
			{
				s+="\\t";
				continue;
			}
			s+=element->data[i];
		}
		element->data=s;
		return;
	}
	
	string temp=split(str, "</"+tag+">")[0];
	temp= split(temp, "<"+tag+">")[0];
	
	while(true)
	{
		tag=split(temp, "<", 1)[0];
		tag=split(tag, ">", 1)[0];
		string temp1= split(temp, "</"+tag+">", 1)[0];
		temp1+= "</"+tag+">";
		if(split(temp, "</"+tag+">", 1).size()<2)
		{
			Element e;
			recursiveElements(&e, temp);
			element->children.push_back(e);
			break;
		}
		else
		{
			temp= split(temp, "</"+tag+">", 1)[1];
		}
		Element e;
		recursiveElements(&e, temp1);
		element->children.push_back(e);
	}
}


void traverseElements(Element *element, string indent)
{
	cout<<indent<<element->tag<<endl;
	if(element->data!="")
		cout<<indent<<"   "<<element->data<<endl;
	for(int i=0; i<element->children.size(); i++)
	{
		traverseElements(&(element->children[i]), indent+"   ");
	}
	cout<<indent<<"/"<<element->tag<<endl;
}


string filter(string str, string del)
{
	string retString="";
	vector<string> temp=split(str, del);
	for(int i=0; i<temp.size(); i++)
	{
		retString+=temp[i];
	}
	return retString;
}
string cleanText(string str)
{
	string retString="";
	string tag;
	tag=split(str, ">", 1)[0];
	tag=split(tag, "<", 1)[1];
	tag=split(tag, " ")[0]; //separating attributes from the tag
	
	int noOfOpenTags=0;
	int noOfCloseTags=0;

	for(int i=0;i<str.length();i++)
	{
		if(str[i]=='<')
			noOfOpenTags++;
		if(str[i]=='>')
			noOfCloseTags++;
	}
	if(noOfOpenTags==2 && noOfCloseTags==2) //str contains simple data
	{
		retString+="<"+split(str, "<",1)[1];
		return retString;
	}
	
	string temp;
	temp=split(str, ">", 1)[0];
	if(split(temp, "<").size()>1)
		temp=split(temp, "<")[1];
	retString+="<"+temp+">";
	
	temp=split(str, "</"+tag+">")[0];
	for(int i=temp.length()-1;i>=0;i--)
	{
		if(temp[i]=='>')
		{
			temp=temp.substr(0, i+1);
			break;
		}
	}
	if(split(temp, "<"+tag+">", 1).size()>1)
		temp= split(temp, "<"+tag+">")[1];
	temp= split(temp, "<"+tag+">")[0];
	//cout<<temp; return "";
	//while(true)
	for(int i=0;;i++)
	{
		//cout<<temp<<endl<<endl;
		tag=split(temp, ">", 1)[0];
		tag=split(tag, "<", 1)[1];
		tag=split(tag, " ")[0];
		string temp1= split(temp, "</"+tag+">", 1)[0];
		temp1+= "</"+tag+">";
		//cout<<split(temp, "</"+tag+">", 1).size()<<endl<<endl;
		//if(i==4)
			//return "";
		if(split(temp, "</"+tag+">", 1).size()<2)
		{	
			//cout<<temp; return "";
			retString+=cleanText(temp);
			break;
		}
		temp= split(temp, "</"+tag+">", 1)[1];
		retString+=cleanText(temp1);
		//cout<<temp<<endl<<endl;
		//cout<<retString<<endl;return "";
	}
	tag=split(str, ">", 1)[0];
	tag=split(tag, "<", 1)[1];
	tag=split(tag, " ")[0];
	retString+="</"+tag+">";
	return retString;
}


