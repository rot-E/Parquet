#include <Scraper.h>

Parse declVar(String_t *s) {
	Parse prs = Primitive.String.Match(String.New(u8"var"), s);
	if (prs.Reply == Err) return Parser.makeErr(s);
	String_t *precip = prs.Precipitate;

	prs = Parser.Bind(
		Primitive.Char.Space,
		Primitive.Char.Lower,
		prs.Subsequent
	);
	if (prs.Reply == Err) return Parser.makeErr(s);
	precip = String.Concat(precip, prs.Precipitate);

	prs = Parser.Many(Primitive.Char.Letter, prs.Subsequent);
	if (prs.Reply == Err) return Parser.makeErr(s);
	precip = String.Concat(precip, prs.Precipitate);

	prs = Primitive.Char.Char(';', prs.Subsequent);
	if (prs.Reply == Err) return Parser.makeErr(s);
	precip = String.Concat(precip, prs.Precipitate);

	return (Parse){
		.Reply			= Ok,
		.Precipitate	= precip,
		.Subsequent		= prs.Subsequent,
	};
}

void main() {
	String_t *s = String.New(u8"var mIkO352;");
	Parser.ParseTest(declVar, s);
}