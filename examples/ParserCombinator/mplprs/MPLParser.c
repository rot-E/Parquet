#include "MPLParser.h"
/*
static SeqAnswer_t Parser_VarName(String_t *s, Processor_t *p) {
	SeqAnswer_t r = Parser_Name(s, p);

	if (r.Reply == Reply.Ok)
		printf(u8" %s{var}", String.GetPrimitive(r.Precipitate));

	return r;
}

static SeqAnswer_t Parser_ProcName(String_t *s, Processor_t *p) {
	return Parser_Name(s, p);
}

static SeqAnswer_t Parser_StdType(String_t *s, Processor_t *p) {
	Seq_t *types = Seq.New();
	types->Add(types, String.New(u8"integer"));
	types->Add(types, String.New(u8"boolean"));
	types->Add(types, String.New(u8"char"));

	SeqAnswer_t r = Parsers.String.OneOf(types, s, p);

	if (r.Reply == Reply.Ok)
		printf(u8" %s", String.GetPrimitive(r.Precipitate));

	return r;
}

static SeqAnswer_t Parser_AddOpr(String_t *s, Processor_t *p) {
	Seq_t *adds = Seq.New();
	adds->Add(adds, String.New(u8"or"));
	adds->Add(adds, String.New(u8"+"));
	adds->Add(adds, String.New(u8"-"));

	return Parsers.String.OneOf(adds, s, p);
}

static SeqAnswer_t Parser_MulOpr(String_t *s, Processor_t *p) {
	Seq_t *muls = Seq.New();
	muls->Add(muls, String.New(u8"and"));
	muls->Add(muls, String.New(u8"div"));
	muls->Add(muls, String.New(u8"*"));

	return Parsers.String.OneOf(muls, s, p);
}

static SeqAnswer_t Parser_Const(String_t *s, Processor_t *p) {
	SeqAnswer_t false_kwd(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"false"), s, p);
	}

	SeqAnswer_t true_kwd(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"true"), s, p);
	}

	return Choise4(
		Parser_UInt,
		false_kwd,
		true_kwd,
		Parser_String,

		s, p
	);
}

static SeqAnswer_t Parser_Expr(String_t *s, Processor_t *p);
static SeqAnswer_t Parser_Var(String_t *s, Processor_t *p) {
	SeqAnswer_t arrayIndexX(String_t *s, Processor_t *p) {
		SeqAnswer_t arrayIndex(String_t *s, Processor_t *p) {
			SeqAnswer_t open(String_t *s, Processor_t *p) {
				return Parsers.Char.Match('[', s, p);
			}

			SeqAnswer_t close(String_t *s, Processor_t *p) {
				return Parsers.Char.Match(']', s, p);
			}

			return Bind3(
				open, Parser_Expr, close,

				s, p
			);
		}

		return Combinator.Possibly(arrayIndex, s, p);
	}

	return Bind(
		Parser_VarName, arrayIndexX,

		s, p
	);
}

static SeqAnswer_t Parser_Factor(String_t *s, Processor_t *p) {
	SeqAnswer_t exprZ(String_t *s, Processor_t *p) {
		SeqAnswer_t open(String_t *s, Processor_t *p) {
			return Parsers.Char.Match('(', s, p);
		}

		SeqAnswer_t close(String_t *s, Processor_t *p) {
			return Parsers.Char.Match(')', s, p);
		}

		return Bind3(
			open, Parser_Expr, close,

			s, p
		);
	}

	SeqAnswer_t invFactor(String_t *s, Processor_t *p) {
		SeqAnswer_t not(String_t *s, Processor_t *p) {
			return Parsers.String.Match(String.New(u8"not"), s, p);
		}

		return Bind(
			not, Parser_Factor,

			s, p
		);
	}

	SeqAnswer_t stdTexprZ(String_t *s, Processor_t *p) {
		SeqAnswer_t open(String_t *s, Processor_t *p) {
			return Parsers.Char.Match('(', s, p);
		}

		SeqAnswer_t close(String_t *s, Processor_t *p) {
			return Parsers.Char.Match(')', s, p);
		}

		return Bind4(
			Parser_StdType, open, Parser_Expr, close,

			s, p
		);
	}

	return Choise5(
		Parser_Var,
		Parser_Const,
		exprZ,
		invFactor,
		stdTexprZ,

		s, p
	);
}

static SeqAnswer_t Parser_Term(String_t *s, Processor_t *p) {
	SeqAnswer_t adtnTerms0(String_t *s, Processor_t *p) {
		SeqAnswer_t adtnTerm(String_t *s, Processor_t *p) {
			return Bind(
				Parser_MulOpr, Parser_Factor,

				s, p
			);
		}

		return Many0(adtnTerm, s, p);
	}

	return Bind(
		Parser_Factor, adtnTerms0,

		s, p
	);
}

static SeqAnswer_t Parser_LeftPart(String_t *s, Processor_t *p) {
	return Parser_Var(s, p);
}

static SeqAnswer_t Parser_SimpleExpr(String_t *s, Processor_t *p) {
	SeqAnswer_t plus_or_minus(String_t *s, Processor_t *p) {
		SeqAnswer_t plus(String_t *s, Processor_t *p) {
			return Parsers.Char.Match('+', s, p);
		}

		SeqAnswer_t minus(String_t *s, Processor_t *p) {
			return Parsers.Char.Match('-', s, p);
		}

		return Choise(
			plus,
			minus,

			s, p
		);
	}

	SeqAnswer_t plus_or_minusX(String_t *s, Processor_t *p) {
		return Combinator.Possibly(plus_or_minus, s, p);
	}

	SeqAnswer_t adtnAdds0(String_t *s, Processor_t *p) {
		SeqAnswer_t adtnAdd(String_t *s, Processor_t *p) {
			return Bind(
				Parser_AddOpr, Parser_Term,

				s, p
			);
		}

		return Many0(adtnAdd, s, p);
	} 

	return Bind3(
		plus_or_minusX, Parser_Term, adtnAdds0,

		s, p
	);
}

static SeqAnswer_t Parser_RelOpr(String_t *s, Processor_t *p) {
	Seq_t *rels = Seq.New();
	rels->Add(rels, String.New(u8"<>"));
	rels->Add(rels, String.New(u8"<="));
	rels->Add(rels, String.New(u8">="));
	rels->Add(rels, String.New(u8"="));
	rels->Add(rels, String.New(u8"<"));
	rels->Add(rels, String.New(u8">"));

	return Parsers.String.OneOf(rels, s, p);
}

static SeqAnswer_t Parser_Expr(String_t *s, Processor_t *p) {
	SeqAnswer_t adtnSufExpr0(String_t *s, Processor_t *p) {
		SeqAnswer_t adtnSufExpr(String_t *s, Processor_t *p) {
			return Bind(
				Parser_RelOpr, Parser_SimpleExpr,

				s, p
			);
		}

		return Many0(adtnSufExpr, s, p);
	}

	return Bind(
		Parser_SimpleExpr, adtnSufExpr0,

		s, p
	);
}

static SeqAnswer_t Parser_AssignStmt(String_t *s, Processor_t *p) {
	SeqAnswer_t assign(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8":="), s, p);
	}

	return Bind3(
		Parser_LeftPart, assign, Parser_Expr,

		s, p
	);
}

static SeqAnswer_t Parser_Stmt(String_t *s, Processor_t *p);
static SeqAnswer_t Parser_CondStmt(String_t *s, Processor_t *p) {
	SeqAnswer_t if_kwd(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"if"), s, p);
	}

	SeqAnswer_t then_kwd(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"then"), s, p);
	}

	SeqAnswer_t elseSecX(String_t *s, Processor_t *p) {
		SeqAnswer_t elseSec(String_t *s, Processor_t *p) {
			SeqAnswer_t else_kwd(String_t *s, Processor_t *p) {
				return Parsers.String.Match(String.New(u8"else"), s, p);
			}

			return Bind(
				else_kwd, Parser_Stmt,

				s, p
			);
		}

		return Combinator.Possibly(elseSec, s, p);
	}

	return Bind5(
		if_kwd, Parser_Expr, then_kwd, Parser_Stmt, elseSecX,

		s, p
	);
}

static SeqAnswer_t Parser_IterStmt(String_t *s, Processor_t *p) {
	SeqAnswer_t while_kwd(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"while"), s, p);
	}

	SeqAnswer_t do_kwd(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"do"), s, p);
	}

	return Bind4(
		while_kwd, Parser_Expr, do_kwd, Parser_Stmt,

		s, p
	);
}

static SeqAnswer_t Parser_ExitStmt(String_t *s, Processor_t *p) {
	return Parsers.String.Match(String.New(u8"break"), s, p);
}

static SeqAnswer_t Parser_Exprs(String_t *s, Processor_t *p) {
	SeqAnswer_t adtnExprs0(String_t *s, Processor_t *p) {
		SeqAnswer_t adtnExpr(String_t *s, Processor_t *p) {
			SeqAnswer_t comma(String_t *s, Processor_t *p) {
				return Parsers.Char.Match(',', s, p);
			}

			return Bind(
				comma, Parser_Expr,
				s, p
			);
		}

		return Many0(adtnExpr, s, p);
	}

	return Bind(
		Parser_Expr, adtnExprs0,

		s, p
	);
}

static SeqAnswer_t Parser_CallStmt(String_t *s, Processor_t *p) {
	SeqAnswer_t call(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"if"), s, p);
	}

	SeqAnswer_t paramX(String_t *s, Processor_t *p) {
		SeqAnswer_t param(String_t *s, Processor_t *p) {
			SeqAnswer_t open(String_t *s, Processor_t *p) {
				return Parsers.Char.Match('(', s, p);
			}

			SeqAnswer_t close(String_t *s, Processor_t *p) {
				return Parsers.Char.Match(')', s, p);
			}

			return Bind3(
				open, Parser_Exprs, close,

				s, p
			);
		}

		return Combinator.Possibly(param, s, p);
	}

	return Bind3(
		call, Parser_ProcName, paramX,

		s, p
	);
}

static SeqAnswer_t Parser_RetStmt(String_t *s, Processor_t *p) {
	return Parsers.String.Match(String.New(u8"return"), s, p);
}

static SeqAnswer_t Parser_InputStmt(String_t *s, Processor_t *p) {
	SeqAnswer_t embeddedInst(String_t *s, Processor_t *p) {
		Seq_t *insts = Seq.New();
		insts->Add(insts, String.New(u8"readln"));
		insts->Add(insts, String.New(u8"read"));

		return Parsers.String.OneOf(insts, s, p);
	}

	SeqAnswer_t paramX(String_t *s, Processor_t *p) {
		SeqAnswer_t param(String_t *s, Processor_t *p) {
			SeqAnswer_t open(String_t *s, Processor_t *p) {
				return Parsers.Char.Match('(', s, p);
			}

			SeqAnswer_t adtnVars0(String_t *s, Processor_t *p) {
				SeqAnswer_t adtnVar(String_t *s, Processor_t *p) {
					SeqAnswer_t comma(String_t *s, Processor_t *p) {
						return Parsers.Char.Match(',', s, p);
					}

					return Bind(
						comma, Parser_Var,

						s, p
					);
				}

				return Many0(adtnVar, s, p);
			}

			SeqAnswer_t close(String_t *s, Processor_t *p) {
				return Parsers.Char.Match(')', s, p);
			}

			return Bind4(
				open, Parser_Var, adtnVars0, close,

				s, p
			);
		}

		return Combinator.Possibly(param, s, p);
	}

	return Bind(
		embeddedInst, paramX,

		s, p
	);
}

static SeqAnswer_t Parser_OutputStmt(String_t *s, Processor_t *p) {
	SeqAnswer_t number(String_t *s, Processor_t *p) {
		SeqAnswer_t optX(String_t *s, Processor_t *p) {
			SeqAnswer_t opt(String_t *s, Processor_t *p) {
				SeqAnswer_t colon(String_t *s, Processor_t *p) {
					return Parsers.Char.Match(':', s, p);
				}

				return Bind(
					colon, Parser_UInt,

					s, p
				);
			}

			return Combinator.Possibly(opt, s, p);
		}

		return Bind(
			Parser_Expr, optX,

			s, p
		);
	}

	return Choise(
		number,
		Parser_String,

		s, p
	);
}

static SeqAnswer_t Parser_OutputFmt(String_t *s, Processor_t *p) {
	SeqAnswer_t embeddedInst(String_t *s, Processor_t *p) {
		Seq_t *insts = Seq.New();
		insts->Add(insts, String.New(u8"writeln"));
		insts->Add(insts, String.New(u8"write"));

		return Parsers.String.OneOf(insts, s, p);
	}

	SeqAnswer_t paramX(String_t *s, Processor_t *p) {
		SeqAnswer_t param(String_t *s, Processor_t *p) {
			SeqAnswer_t open(String_t *s, Processor_t *p) {
				return Parsers.Char.Match('(', s, p);
			}

			SeqAnswer_t adtnOutputFmts0(String_t *s, Processor_t *p) {
				SeqAnswer_t adtnOutputFmt(String_t *s, Processor_t *p) {
					SeqAnswer_t comma(String_t *s, Processor_t *p) {
						return Parsers.Char.Match(',', s, p);
					}

					return Bind(
						comma, Parser_OutputFmt,

						s, p
					);
				}

				return Many0(adtnOutputFmt, s, p);
			}

			SeqAnswer_t close(String_t *s, Processor_t *p) {
				return Parsers.Char.Match(')', s, p);
			}

			return Bind4(
				open, Parser_OutputFmt, adtnOutputFmts0, close,

				s, p
			);
		}

		return Combinator.Possibly(param, s, p);
	}

	return Bind(
		embeddedInst, paramX,

		s, p
	);
}

static SeqAnswer_t Parser_Stmt(String_t *s, Processor_t *p);
static SeqAnswer_t Parser_CompoundStmt(String_t *s, Processor_t *p) {
	SeqAnswer_t begin(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"begin"), s, p);
	}

	SeqAnswer_t adtnStmts(String_t *s, Processor_t *p) {
		SeqAnswer_t semicolon(String_t *s, Processor_t *p) {
			return Parsers.Char.Match(';', s, p);
		}

		SeqAnswer_t adtnStmt(String_t *s, Processor_t *p) {
			return Bind(
				semicolon, Parser_Stmt,

				s, p
			);
		}

		return Many0(adtnStmt, s, p);
	}

	SeqSeqAnswer_t end(Seq_t *seq, Processor_t *p) {
		SeqAnswer_t end_p(String_t *s, Processor_t *p) {
			return Parsers.String.Match(String.New(u8"end"), s, p);
		}

		SeqSeqAnswer_t r = SeqParsers.Complete(end_p, seq, p);
		****************************************
		if (r.Reply == Reply.Ok)
			printf(u8"end");
		****************************************

		return r;
	}

	return Bind4(
		begin, Parser_Stmt, adtnStmts, end,
		
		s, p
	);
}

static SeqAnswer_t Parser_EmptyStmt(String_t *s, Processor_t *p) {
	return Basis.Ok(s, p);
}

static SeqAnswer_t Parser_Stmt(String_t *s, Processor_t *p) {
	return Choise10(
		Parser_AssignStmt,
		Parser_CondStmt,
		Parser_IterStmt,
		Parser_ExitStmt,
		Parser_CallStmt,
		Parser_RetStmt,
		Parser_InputStmt,
		Parser_OutputStmt,
		Parser_CompoundStmt,
		Parser_EmptyStmt,

		s, p
	);
}

static SeqAnswer_t Parser_VarNames(String_t *s, Processor_t *p) {
	SeqAnswer_t adtnVarNames(String_t *s, Processor_t *p) {
		SeqAnswer_t comma(String_t *s, Processor_t *p) {
			SeqAnswer_t r = Parsers.Char.Match(',', s, p);

			if (r.Reply == Reply.Ok)
				printf(u8",");

			return r;
		}

		return Bind(
			comma, Parser_VarName,

			s, p
		);
	}

	return Bind(Parser_VarName, adtnVarNames, s, p);
}

static SeqAnswer_t Parser_ArrType(String_t *s, Processor_t *p) {
	SeqAnswer_t array(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"array"), s, p);
	}

	SeqAnswer_t open(String_t *s, Processor_t *p) {
		return Parsers.Char.Match('[', s, p);
	}

	SeqAnswer_t close(String_t *s, Processor_t *p) {
		return Parsers.Char.Match(']', s, p);
	}

	SeqAnswer_t of(String_t *s, Processor_t *p) {
		return Parsers.String.Match(String.New(u8"of"), s, p);
	}

	return Bind6(
		array, open, Parser_UInt, close, of, Parser_StdType,

		s, p
	);
}

static SeqAnswer_t Parser_Type(String_t *s, Processor_t *p) {
	return Combinator.Choise(
		Parser_StdType,
		Parser_ArrType,

		s, p
	);
}

static SeqAnswer_t Parser_VarDecl(String_t *s, Processor_t *p) {
	SeqAnswer_t var(String_t *s, Processor_t *p) {
		SeqAnswer_t r = Parsers.String.Match(String.New(u8"var"), s, p);

		if (r.Reply == Reply.Ok)
			printf(u8"\tvar");

		return r;
	}

	SeqAnswer_t VarDecl(String_t *s, Processor_t *p) {
		SeqAnswer_t colon(String_t *s, Processor_t *p) {
			SeqAnswer_t r = Parsers.Char.Match(':', s, p);

			if (r.Reply == Reply.Ok)
				printf(u8" :");

			return r;
		}

		SeqAnswer_t semicolon(String_t *s, Processor_t *p) {
			SeqAnswer_t r = Parsers.Char.Match(';', s, p);

			if (r.Reply == Reply.Ok)
				printf(u8";\n");

			return r;
		}

		return Bind4(
			Parser_VarNames, colon, Parser_Type, semicolon,

			s, p
		);
	}

	SeqAnswer_t VarDecls(String_t *s, Processor_t *p) {
		return Many0(VarDecl, s, p);
	}

	return Bind3(
		var, VarDecl,
		VarDecls,

		s, p
	);
}

static SeqAnswer_t Parser_FormalParam(String_t *s, Processor_t *p) {
	SeqAnswer_t open(String_t *s, Processor_t *p) {
		return Parsers.Char.Match('(', s, p);
	}

	SeqAnswer_t colon(String_t *s, Processor_t *p) {
		return Parsers.Char.Match(':', s, p);
	}

	SeqAnswer_t adtnParams(String_t *s, Processor_t *p) {
		SeqAnswer_t semicolon(String_t *s, Processor_t *p) {
			return Parsers.Char.Match(';', s, p);
		}

		SeqAnswer_t adtnParam(String_t *s, Processor_t *p) {
			return Bind4(
				semicolon, Parser_VarNames, colon, Parser_Type,

				s, p
			);
		}

		return Many0(adtnParam, s, p);
	}

	SeqAnswer_t close(String_t *s, Processor_t *p) {
		return Parsers.Char.Match(')', s, p);
	}

	return Bind6(
		open, Parser_VarNames, colon, Parser_Type,
		adtnParams, close,

		s, p
	);
}
*/
static SeqAnswer_t SeqParser_SubProgDecl(Seq_t *seq, Processor_t *p) {
	SeqAnswer_t procedure(Seq_t *seq, Processor_t *p) {
		return SeqParsers.Match(String.New(u8"procedure"), seq, p);
	}

	SeqAnswer_t SeqParser_FormalParamX(Seq_t *seq, Processor_t *p) {
		return SeqCombinator.Possibly(SeqParser_FormalParam, seq, p);
	}

	SeqAnswer_t semicolon(Seq_t *seq, Processor_t *p) {
		return SeqParsers.Match(String.New(u8";"), seq, p);
	}

	SeqAnswer_t SeqParser_VarDeclX(Seq_t *seq, Processor_t *p) {
		return SeqCombinator.Possibly(SeqParser_VarDecl, seq, p);
	}

	SeqAnswer_t decl(Seq_t *seq, Processor_t *p) {
		return SeqCombinator.Bind4(
			procedure, SeqParser_ProcName, SeqParser_FormalParamX, semicolon,
			seq, p
		);
	}

	return SeqCombinator.Bind4(
		decl,
			SeqParser_VarDeclX,
			SeqParser_CompoundStmt,
		semicolon,
		seq, p
	);
}

static SeqAnswer_t SeqParser_Block(Seq_t *seq, Processor_t *p) {
	SeqAnswer_t VarDecl_or_SubProgDecl(Seq_t *seq, Processor_t *p) {
		return SeqCombinator.Choise(
			SeqParser_VarDecl,
			SeqParser_SubProgDecl,
			seq, p
		);
	}

	SeqAnswer_t Repeat_VarDecl_or_SubProgDecl(Seq_t *seq, Processor_t *p) {
		return SeqCombinator.Many0(VarDecl_or_SubProgDecl, seq, p);
	}

	return SeqCombinator.Bind(
		Repeat_VarDecl_or_SubProgDecl,
		SeqParser_CompoundStmt,
		seq, p
	);
}

static SeqAnswer_t SeqParser_Program(Seq_t *seq, Processor_t *p) {
	SeqAnswer_t program(Seq_t *seq, Processor_t *p) {
		SeqAnswer_t r = SeqParsers.Match(String.New(u8"program"), seq, p);

		/****************************************/
		if (r.Reply == Reply.Ok)
			printf(u8"program");
		/****************************************/

		return r;
	}

	SeqAnswer_t name(Seq_t *seq, Processor_t *p) {
		Answer_t name_p(String_t *s, Processor_t *p) {
			return Combinator.Many1(MPLLexer.Parser_Name, s, p);
		}

		SeqAnswer_t r = SeqParsers.Complete(name_p, seq, p);
		/****************************************/
		if (r.Reply == Reply.Ok)
			printf(u8" %s", String.GetPrimitive(
				(seq->GetStringiser(seq))(Seq.GetLast(seq))
			));
		/****************************************/

		return r;
	}

	SeqAnswer_t semicolon(Seq_t *seq, Processor_t *p) {
		Answer_t semicolon_p(String_t *s, Processor_t *p) {
			return Parsers.Char.Match(';', s, p);
		}

		SeqAnswer_t r = SeqParsers.Complete(semicolon_p, seq, p);
		/****************************************/
		if (r.Reply == Reply.Ok)
			printf(u8";\n");
		/****************************************/

		return r;
	}

	SeqAnswer_t dot(Seq_t *seq, Processor_t *p) {
		Answer_t dot_p(String_t *s, Processor_t *p) {
			return Parsers.Char.Match('.', s, p);
		}

		SeqAnswer_t r = SeqParsers.Complete(dot_p, seq, p);
		/****************************************/
		if (r.Reply == Reply.Ok)
			printf(u8".");
		/****************************************/

		return r;
	}

	return SeqCombinator.Bind5(
		program, name, semicolon,
		SeqParser_Block, dot,
		seq, p
	);
}

_MPLParser MPLParser = {
	.SeqParser_Program	= SeqParser_Program,
};
