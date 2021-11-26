#include <Parquet.h>

#include "MPLLexer.h"
#include "MPLParser.h"
#include "Printer.h"

String_t *Token2String(any *item) {
	return Token.GetEntity(item);
}

void main(const int32_t argc, uint8_t *argv[]) {
	/* check args */
	if (argc != 2) {
		printf("Usage:  $ ./mplprs [FILE]\n\n");
		exit(EXIT_FAILURE);
	}

	/* tokenise */
	Answer_t r = Invoker.Invoke(
		MPLLexer.Parser_Program,
		String.FromFile(argv[1]),
		TokenCollector.New()
	);
	TokenCollector_t *collector = (TokenCollector_t *)(r.Processor);

	if (r.Reply == Reply.Err) {
		printf("\e[91m[error]\e[0m tokenise failed at line %d.\n", collector->GetLine(collector));
		printf("\e[4m                                                                      \e[0m\n");
		printf("\e[2m%s\e[0m", String.GetPrimitive(r.Precipitate));
		printf("\e[1m\e[3m\e[4m\e[6m%c\e[0m\n", String.GetCharAt(r.Subsequent, 0));
		exit(EXIT_FAILURE);
	}

	/* parse */
	SeqAnswer_t sr = SeqInvoker.Invoke(
		MPLParser.SeqParser_Program,
		Seq.FromList(
			Token2String,
			TokenCollector.Get(collector)
		),
		Printer.New()
	);
	Printer_t *printer = (Printer_t *)(sr.Processor);

	if (sr.Reply == Reply.Err) return;

	printer->Dump(printer);

/*	if (!r.Reply == Reply.Err) {
		printf("\e[91m[error]\e[0m parse failed at line %d.\n\n", res.ErrorLine);
		printf("\e[2m%s\e[0m", String.GetPrimitive(res.Precipitate));
		//printf("\e[1m\e[3m\e[4m\e[6m%c\e[0m\n", String.GetCharAt(res.Subsequent, 0));
		printf("\e[1m\e[3m\e[4m\e[6m%c\e[0m", String.GetCharAt(res.Subsequent, 0));
		printf("\e[2m%s\n", String.GetPrimitive(String.Substring(res.Subsequent, 1, String.GetLength(res.Subsequent))));
		exit(EXIT_FAILURE);
	}

	printf("\e[2m%s\e[0m", String.GetPrimitive(res.Precipitate));
	printf("\e[1m\e[3m\e[4m\e[6m%c\e[0m\n", String.GetCharAt(res.Subsequent, 0));*/
}
