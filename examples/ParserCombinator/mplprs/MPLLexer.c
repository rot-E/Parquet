#include "MPLLexer.h"

static String_t *TokenTypeString(Token_t *t) {
	switch (t->GetType(t)) {
		case Token_Name:		return String.New(u8"Name");
		case Token_Keyword:		return String.New(u8"Keyword");
		case Token_UInt:		return String.New(u8"UInt");
		case Token_String:		return String.New(u8"String");
		case Token_Symbol:		return String.New(u8"Symbol");
		default:				return String.New(u8"Unknown");
	}
}

static Answer_t Parser_Comment(String_t *s, Processor_t *p) {
	Answer_t line(String_t *s, Processor_t *p) {
		Answer_t open(String_t *s, Processor_t *p) {
			return Parsers.Char.Match('{', s, p);
		}

		Answer_t content(String_t *s, Processor_t *p) {
			Answer_t nonClose(String_t *s, Processor_t *p) {
				return Parsers.Char.UnMatch('}', s, p);
			}

			return Combinator.Many0(nonClose, s, p);
		}

		Answer_t close(String_t *s, Processor_t *p) {
			return Parsers.Char.Match('}', s, p);
		}

		return Combinator.Bind3(
			open, content, close,
			s, p
		);
	}

	Answer_t block(String_t *s, Processor_t *p) {
		Answer_t open(String_t *s, Processor_t *p) {
			return Parsers.String.Match(
				String.New(u8"/*"),
				s, p
			);
		}

		Answer_t content(String_t *s, Processor_t *p) {
			Answer_t nonClose(String_t *s, Processor_t *p) {
				return Parsers.String.UnMatch(
					String.New(u8"*/"),
					s, p
				);
			}

			return Combinator.Many0(nonClose, s, p);
		}

		Answer_t close(String_t *s, Processor_t *p) {
			return Parsers.String.Match(
				String.New(u8"*/"),
				s, p
			);
		}

		return Combinator.Bind3(
			open, content, close,
			s, p
		);
	}

	return Combinator.Choise(line, block, s, p);
}

static Answer_t Parser_Separator(String_t *s, Processor_t *p) {
	Answer_t space_or_tab(String_t *s, Processor_t *p) {
		return Combinator.Choise(
			Parsers.Char.Space,
			Parsers.Char.Tab,

			s, p
		);
	}

	Answer_t newline(String_t *s, Processor_t *p) {
		List_t *nls = List.New();
		nls->Add(nls, String.New(u8"\r\n"));
		nls->Add(nls, String.New(u8"\r"));
		nls->Add(nls, String.New(u8"\n\r"));
		nls->Add(nls, String.New(u8"\n"));

		Answer_t result = Parsers.String.OneOf(
			nls,
			s, p
		);
		/****************************************/
		if (result.Reply == Reply.Ok)
			(((TokenCollector_t *)(p)))->NewLine(p);
		/****************************************/

		return result;
	}

	return Combinator.Choise3(
		space_or_tab,
		newline,
		Parser_Comment,
		s, p
	); 
}

static Answer_t Parser_Digit(String_t *s, Processor_t *p) {
	return Parsers.Char.Digit(s, p);
}

static Answer_t Parser_Alphabet(String_t *s, Processor_t *p) {
	return Parsers.Char.Letter(s, p);
}

static Answer_t Parser_Symbol(String_t *s, Processor_t *p) {
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

	Answer_t result = Parsers.String.OneOf(
		syms,
		s, p
	);
	/****************************************/
	if (result.Reply == Reply.Ok)
		((TokenCollector_t *)(p))->Add(p, result.Precipitate, Token_Symbol);
	/****************************************/

	return result;
}

static Answer_t Parser_String(String_t *s, Processor_t *p) {
	Answer_t apostrophe(String_t *s, Processor_t *p) {
		return Parsers.Char.Match('\'', s, p);
	}

	Answer_t content(String_t *s, Processor_t *p) {
		Answer_t nonApostrophe(String_t *s, Processor_t *p) {
			return Parsers.Char.NoneOf(
				String.New(u8"'"),
				s, p
			);
		}

		return Combinator.Many0(nonApostrophe, s, p);
	}

	Answer_t result = Combinator.Bind3(
		apostrophe,
		content,
		apostrophe,
		s, p
	);
	/****************************************/
	if (result.Reply == Reply.Ok)
		((TokenCollector_t *)(p))->Add(p, result.Precipitate, Token_String);
	/****************************************/

	return result;
}

static Answer_t Parser_UInt(String_t *s, Processor_t *p) {
	Answer_t result = Combinator.Many1(Parser_Digit, s, p);
	/****************************************/
	if (result.Reply == Reply.Ok)
		((TokenCollector_t *)(p))->Add(p, result.Precipitate, Token_UInt);
	/****************************************/

	return result;
}

static Answer_t Parser_Keyword(String_t *s, Processor_t *p) {
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
	keywords->Add(keywords, String.New(u8"readln"));
	keywords->Add(keywords, String.New(u8"read"));
	keywords->Add(keywords, String.New(u8"writeln"));
	keywords->Add(keywords, String.New(u8"write"));
	keywords->Add(keywords, String.New(u8"true"));
	keywords->Add(keywords, String.New(u8"false"));
	keywords->Add(keywords, String.New(u8"break"));

	Answer_t result = Parsers.String.OneOf(
		keywords,
		s, p
	);
	/****************************************/
	if (result.Reply == Reply.Ok)
		((TokenCollector_t *)(p))->Add(p, result.Precipitate, Token_Keyword);
	/****************************************/

	return result;
}

static Answer_t Parser_Name(String_t *s, Processor_t *p) {
	Answer_t Alpha_or_Num(String_t *s, Processor_t *p) {
		return Combinator.Choise(
			Parser_Alphabet,
			Parser_Digit,
			s, p
		);
	}

	Answer_t AlphaNums0(String_t *s, Processor_t *p) {
		return Combinator.Many0(Alpha_or_Num, s, p);
	}

	Answer_t result = Combinator.Bind(
		Alpha_or_Num, AlphaNums0,
		s, p
	);
	/****************************************/
	if (result.Reply == Reply.Ok)
		((TokenCollector_t *)(p))->Add(p, result.Precipitate, Token_Name);
	/****************************************/

	return result;
}

static Answer_t Parser_Token(String_t *s, Processor_t *p) {
	return Combinator.Choise5(
		Parser_Symbol,
		Parser_Keyword,
		Parser_UInt,
		Parser_String,
		Parser_Name,
		s, p
	);
}

static Answer_t Parser_Program(String_t *s, Processor_t *p) {
	Answer_t Separator_or_Token(String_t *s, Processor_t *p) {
		return Combinator.Choise(
			Parser_Separator,
			Parser_Token,
			s, p
		);
	}

	return Combinator.Many0(Separator_or_Token, s, p);
}

_MPLLexer MPLLexer = {
	.TokenTypeString	= TokenTypeString,

	.Parser_Comment		= Parser_Comment,
	.Parser_Separator	= Parser_Separator,
	.Parser_Digit		= Parser_Digit,
	.Parser_Alphabet	= Parser_Alphabet,
	.Parser_Symbol		= Parser_Symbol,
	.Parser_String		= Parser_String,
	.Parser_UInt		= Parser_UInt,
	.Parser_Keyword		= Parser_Keyword,
	.Parser_Name		= Parser_Name,
	.Parser_Token		= Parser_Token,
	.Parser_Program		= Parser_Program,
};