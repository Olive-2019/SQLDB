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
	SyntaxTree tree(lexer);
	
	while (!stop) {
		cout << PROMPT;
		cout.flush();
		getline(cin, sql);
		cout << sql << endl;
		parse(sql);
	}
	return 0;
}