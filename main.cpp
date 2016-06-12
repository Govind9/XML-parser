#include "XML.h"

int main()
{
	Document document("input2.xml");
	parse(&document);
	if(!document.validity)
	{
		cout<<"Document not valid\n";
		return 1;
	}	
	document.printMe();
	return 0;
}
