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

RC RBparse()
{
	RC errval;
	stop = false;
	string buffer;
	LexerPtr lexer = Lexer::instance();
	SyntaxTree tree(lexer);
	//string sql = "insert into Rel1(2, 3.3, Lam);";
	string sql = "select * from Rel1;";
	//string sql = "select id,Rel.id from Rel1,Rel2 where Rel1.id>Rel2.id;";
	//string sql = "select * from Rel1 where Rel1.id=5;";
	while (!stop) {
		cout << PROMPT;
		cout.flush();
		cout << sql << endl;
		buffer = sql;
 	    tree.resetParser(buffer);
		try { 
			parse_tree = tree.buildSyntaxTree();
		}
		catch (const GeneralError& err)
		{
			cerr << err.what() << endl;
			break;
		}
		if (errval = interp(parse_tree)) { // ½âÎöÓï·¨Ê÷
			if (errval < 0) stop = true;
		}
		return 0;
	}
	return 0;
}