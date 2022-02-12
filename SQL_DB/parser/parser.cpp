#include <iostream>
#include <string>
#include "parser.h"
#include "parser_node.h"
#include "parser_token.h"
#include "parser_lexer.h"
#include "parser_syntaxtree.h"
#include "parser_interp.h"

using namespace std;

static NODE* parse_tree;
bool stop = false;
SyntaxTree* tree;
void parse(string sql) {
	RC errval;
	tree->resetParser(sql);
	try {
		parse_tree = tree->buildSyntaxTree();
	}
	catch (const GeneralError& err)
	{
		cerr << err.what() << endl;
		return;
	}
	if (errval = interp(parse_tree)) { // ½âÎöÓï·¨Ê÷
		if (errval < 0) stop = true;
	}
}
RC RBparse()
{
	stop = false;
	string sql;
	LexerPtr lexer = Lexer::instance();
	tree = new SyntaxTree(lexer);
	sql = "select R1.id,R2.id,R3.name,R4.price from R1,R2,R3,R4 where R1.id>R2.id and R3.id>5 and R4.price=R1.price;";
	
	while (!stop) {
		cout << PROMPT;
		cout.flush();
		getline(cin, sql);
		cout << sql << endl;
		parse(sql);
	}
	return 0;
}