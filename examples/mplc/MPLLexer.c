#include "MPLLexer.h"

static List_t *tokens;
static uint32_t lineNum;

static Parse Parser_Comment(String_t *code) {
	Parse line(String_t *code) {
		Parse open(String_t *code) {
			return Primitive.Char.Char('{', code);
		}

		Parse nonClose(String_t *code) {
			return Primitive.Char.NoneOf(
				String.New(u8"}"),
				code
			);
		}

		Parse content(String_t *code) {
			return Parser.Many0(nonClose, code);
		}

		Parse close(String_t *code) {
			return Primitive.Char.Char('}', code);
		}

		return Parser.Bind3(
			open,
			content,
			close,
			code
		);
	}

	Parse block(String_t *code) {
		Parse open(String_t *code) {
			return Primitive.String.Match(
				String.New(u8"/*"),
				code
			);
		}

		Parse nonClose(String_t *code) {
			return Primitive.String.UnMatch(
				String.New(u8"*/"),
				code
			);
		}

		Parse content(String_t *code) {
			return Parser.Many0(nonClose, code);
		}

		Parse close(String_t *code) {
			return Primitive.String.Match(
				String.New(u8"*/"),
				code
			);
		}

		return Parser.Bind3(
			open,
			content,
			close,
			code
		);
	}

	return Parser.Choise(line, block, code);
}

static Parse Parser_Separator(String_t *code) {
	Parse space_tab(String_t *code) {
		List_t *seps = List.New();
		seps->Add(seps, String.New(u8" "));
		seps->Add(seps, String.New(u8"\t"));

		return Primitive.String.OneOf(
			seps,
			code
		);
	}

	Parse newline(String_t *code) {
		List_t *nls = List.New();
		nls->Add(nls, String.New(u8"\r\n"));
		nls->Add(nls, String.New(u8"\r"));
		nls->Add(nls, String.New(u8"\n\r"));
		nls->Add(nls, String.New(u8"\n"));

		Parse prs = Primitive.String.OneOf(
			nls,
			code
		);

/****************************************/
		if (prs.Reply == Ok) lineNum++;
/****************************************/

		return prs;
	}

	return Parser.Choise3(
		space_tab,
		newline,
		Parser_Comment,
		code
	); 
}

static Parse Parser_Digit(String_t *code) {
	return Primitive.Char.Digit(code);
}

static Parse Parser_Alphabet(String_t *code) {
	return Primitive.Char.Letter(code);
}

static Parse Parser_Symbol(String_t *code) {
	List_t *syms = List.New();
	syms->Add(syms, String.New(u8"+"));
	syms->Add(syms, String.New(u8"-"));
	syms->Add(syms, String.New(u8"*"));
	syms->Add(syms, String.New(u8"="));
	syms->Add(syms, String.New(u8"<>"));
	syms->Add(syms, String.New(u8"<="));
	syms->Add(syms, String.New(u8"<"));
	syms->Add(syms, String.New(u8">="));
	syms->Add(syms, String.New(u8">"));
	syms->Add(syms, String.New(u8"("));
	syms->Add(syms, String.New(u8")"));
	syms->Add(syms, String.New(u8"["));
	syms->Add(syms, String.New(u8"]"));
	syms->Add(syms, String.New(u8"."));
	syms->Add(syms, String.New(u8","));
	syms->Add(syms, String.New(u8":="));
	syms->Add(syms, String.New(u8":"));
	syms->Add(syms, String.New(u8";"));

	Parse prs = Primitive.String.OneOf(
		syms,
		code
	);

/****************************************/
	if (prs.Reply == Ok)
		tokens->Add(tokens, Token.New(prs.Precipitate, Token_Symbol, lineNum));
/****************************************/

	return prs;
}

static Parse Parser_String(String_t *code) {
	Parse apostr(String_t *code) {
		return Primitive.Char.Char('\'', code);
	}

	Parse nonapostr(String_t *code) {
		return Primitive.Char.NoneOf(
			String.New(u8"'"),
			code
		);
	}

	Parse content(String_t *code) {
		return Parser.Many0(nonapostr, code);
	}

	Parse prs = Parser.Bind3(
		apostr,
		content,
		apostr,
		code
	);

/****************************************/
	if (prs.Reply == Ok)
		tokens->Add(tokens, Token.New(prs.Precipitate, Token_String, lineNum));
/****************************************/

	return prs;
}

static Parse Parser_UInt(String_t *code) {
	Parse prs = Parser.Many1(Parser_Digit, code);

/****************************************/
	if (prs.Reply == Ok)
		tokens->Add(tokens, Token.New(prs.Precipitate, Token_UInt, lineNum));
/****************************************/

	return prs;
}

static Parse Parser_Keyword(String_t *code) {
	List_t *keywords = List.New();
	keywords->Add(keywords, String.New(u8"program"));
	keywords->Add(keywords, String.New(u8"var"));
	keywords->Add(keywords, String.New(u8"array"));
	keywords->Add(keywords, String.New(u8"of"));
	keywords->Add(keywords, String.New(u8"begin"));
	keywords->Add(keywords, String.New(u8"if"));
	keywords->Add(keywords, String.New(u8"then"));
	keywords->Add(keywords, String.New(u8"else"));
	keywords->Add(keywords, String.New(u8"procedure"));
	keywords->Add(keywords, String.New(u8"call"));
	keywords->Add(keywords, String.New(u8"while"));
	keywords->Add(keywords, String.New(u8"do"));
	keywords->Add(keywords, String.New(u8"not"));
	keywords->Add(keywords, String.New(u8"or"));
	keywords->Add(keywords, String.New(u8"div"));
	keywords->Add(keywords, String.New(u8"and"));
	keywords->Add(keywords, String.New(u8"char"));
	keywords->Add(keywords, String.New(u8"integer"));
	keywords->Add(keywords, String.New(u8"boolean"));
	keywords->Add(keywords, String.New(u8"read"));
	keywords->Add(keywords, String.New(u8"write"));
	keywords->Add(keywords, String.New(u8"readln"));
	keywords->Add(keywords, String.New(u8"writeln"));
	keywords->Add(keywords, String.New(u8"true"));
	keywords->Add(keywords, String.New(u8"false"));
	keywords->Add(keywords, String.New(u8"break"));

	Parse prs = Primitive.String.OneOf(
		keywords,
		code
	);

/****************************************/
	if (prs.Reply == Ok)
		tokens->Add(tokens, Token.New(prs.Precipitate, Token_Keyword, lineNum));
/****************************************/

	return prs;
}

static Parse Parser_Name(String_t *code) {
	Parse al_num(String_t *code) {
		return Parser.Choise(
			Parser_Alphabet,
			Parser_Digit,
			code
		);
	}

	Parse al_num_Rep(String_t *code) {
		return Parser.Many0(al_num, code);
	}

	Parse prs = Parser.Bind(
		al_num,
		al_num_Rep,
		code
	);

/****************************************/
	if (prs.Reply == Ok)
		tokens->Add(tokens, Token.New(prs.Precipitate, Token_Name, lineNum));
/****************************************/

	return prs;
}

static Parse Parser_Token(String_t *code) {
	Parse prs = Parser.Choise5(
		Parser_Symbol,
		Parser_Keyword,
		Parser_UInt,
		Parser_String,
		Parser_Name,
		code
	);

	return prs;
}

static Parse Parser_Program(String_t *code) {
	Parse tok_sep(String_t *code) {
		return Parser.Choise(
			Parser_Separator,
			Parser_Token,
			code
		);
	}

	return Parser.Many0(tok_sep, code);
}

static List_t *Execute(String_t *code) {
	tokens = List.New();
	lineNum = 1;
	Parser.Invoke(Parser_Program, code);
	return tokens;
}

_MPLLexer MPLLexer = {
	.Execute = Execute,
};