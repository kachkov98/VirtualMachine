#define IS_ZERO(x) (fabs(x) < DBL_EPSILON)
#define IS_MORE_ZERO(x) (x > DBL_EPSILON)
#define IS_LESS_ZERO(x) (x < -DBL_EPSILON)

#define COMMA ,

DEF_CMD (HALT,  0, 1, EMP, { out << "Program finished successfully" << std::endl; return true; })
DEF_CMD (SCAN,  1, 2, REG, { out << "Enter a real number: "; in >> registers[program[cmd_counter + 1]]; cmd_counter += 2; })
DEF_CMD (PRINT, 2, 2, REG, { out << "Register " << (unsigned int)program[cmd_counter + 1] << ": " << registers[program[cmd_counter + 1]] << std::endl; cmd_counter += 2; })
DEF_CMD (PUSH,  3, 9, VAL, { calculations.push (*(double*)(program + cmd_counter + 1));  cmd_counter += 9; })
DEF_CMD (POP,   4, 1, EMP, { calculations.pop (); cmd_counter += 1; })
DEF_CMD (FETCH, 5, 2, REG, { calculations.push (registers[program[cmd_counter + 1]]); cmd_counter += 2; })
DEF_CMD (STORE, 6, 2, REG, { registers[program[cmd_counter + 1]] = calculations.pop (); cmd_counter += 2; }; )
DEF_CMD (DUP,   7, 1, EMP, { double a = calculations.pop (); calculations.push (a); calculations.push (a); cmd_counter += 1; })
DEF_CMD (SWAP,  8, 1, EMP, { double a COMMA b; a = calculations.pop (); b = calculations.pop (); calculations.push (a); calculations.push (b); cmd_counter += 1; })
DEF_CMD (INC,   9, 1, EMP, { calculations.push (calculations.pop () + 1);  cmd_counter += 1; })
DEF_CMD (DEC,  10, 1, EMP, { calculations.push (calculations.pop () - 1);  cmd_counter += 1; })
DEF_CMD (ADD,  11, 1, EMP, { double a COMMA b; a = calculations.pop (); b = calculations.pop (); calculations.push (a + b); cmd_counter += 1; })
DEF_CMD (SUB,  12, 1, EMP, { double a COMMA b; a = calculations.pop (); b = calculations.pop (); calculations.push (b - a); cmd_counter += 1; })
DEF_CMD (MUL,  13, 1, EMP, { double a COMMA b; a = calculations.pop (); b = calculations.pop (); calculations.push (a * b); cmd_counter += 1; })
DEF_CMD (DIV,  14, 1, EMP, { double a COMMA b; a = calculations.pop (); b = calculations.pop (); calculations.push (b / a); cmd_counter += 1; })
DEF_CMD (MOD,  15, 1, EMP, { double a COMMA b; a = calculations.pop (); b = calculations.pop (); calculations.push (fmod (b, a)); cmd_counter += 1; })
DEF_CMD (TRUNC,16, 1, EMP, { calculations.push (trunc (calculations.pop ())); cmd_counter += 1; })
DEF_CMD (ROUND,17, 1, EMP, { calculations.push (round (calculations.pop ())); cmd_counter += 1; })
DEF_CMD (ABS,  18, 1, EMP, { calculations.push (fabs (calculations.pop ())); cmd_counter += 1; })
DEF_CMD (NEG,  19, 1, EMP, { calculations.push (-1 * calculations.pop ()); cmd_counter += 1; })
DEF_CMD (SQR,  20, 1, EMP, { double a = calculations.pop (); calculations.push (a * a); cmd_counter += 1; })
DEF_CMD (SQRT, 21, 1, EMP, { calculations.push (sqrt (calculations.pop())); cmd_counter += 1; })
DEF_CMD (POW,  22, 1, EMP, { double a COMMA b; a = calculations.pop (); b = calculations.pop (); calculations.push (pow (b COMMA a)); cmd_counter += 1; })
DEF_CMD (SIN,  23, 1, EMP, { calculations.push (sin (calculations.pop ()));  cmd_counter += 1; })
DEF_CMD (COS,  24, 1, EMP, { calculations.push (cos (calculations.pop ()));  cmd_counter += 1; })
DEF_CMD (TAN,  25, 1, EMP, { calculations.push (tan (calculations.pop ()));  cmd_counter += 1; })
DEF_CMD (ASIN, 26, 1, EMP, { calculations.push (asin (calculations.pop ()));  cmd_counter += 1; })
DEF_CMD (ACOS, 27, 1, EMP, { calculations.push (acos (calculations.pop ()));  cmd_counter += 1; })
DEF_CMD (ATAN, 28, 1, EMP, { calculations.push (atan (calculations.pop ()));  cmd_counter += 1; })
DEF_CMD (LN,   29, 1, EMP, { calculations.push (log (calculations.pop ()));  cmd_counter += 1; })
DEF_CMD (RAND, 30, 1, EMP, { calculations.push (rand () / RAND_MAX); cmd_counter += 1; })
DEF_CMD (SGN,  31, 1, EMP, { double a = calculations.pop () COMMA cmp = 0.0; if (IS_MORE_ZERO (a)) cmp = 1.0; if (IS_LESS_ZERO (a)) cmp = -1.0; calculations.push (cmp); cmd_counter += 1; })
DEF_CMD (CMP,  32, 1, EMP, { double a = calculations.pop () COMMA b = calculations.pop () COMMA  cmp = 0.0; if (IS_MORE_ZERO (b - a)) cmp = 1.0; if (IS_LESS_ZERO (b - a)) cmp = -1.0; calculations.push (cmp); cmd_counter += 1; })
DEF_CMD (JZ,   33, 5, JMP, { if (IS_ZERO (calculations.pop ())) cmd_counter = *(size_t*)(program + cmd_counter + 1); else cmd_counter += 5; })
DEF_CMD (JNZ,  34, 5, JMP, { if (!IS_ZERO (calculations.pop ())) cmd_counter = *(size_t*)(program + cmd_counter + 1); else cmd_counter += 5; })
DEF_CMD (JMZ,  35, 5, JMP, { if (IS_MORE_ZERO (calculations.pop ())) cmd_counter = *(size_t*)(program + cmd_counter + 1); else cmd_counter += 5; })
DEF_CMD (JLZ,  36, 5, JMP, { if (IS_LESS_ZERO (calculations.pop ())) cmd_counter = *(size_t*)(program + cmd_counter + 1); else cmd_counter += 5; })
DEF_CMD (JMP,  37, 5, JMP, { cmd_counter = *(size_t*)(program + cmd_counter + 1); })
DEF_CMD (CALL, 38, 5, JMP, { func_calls.push (cmd_counter + 5); cmd_counter = *(size_t*)(program + cmd_counter + 1); })
DEF_CMD (RET,  39, 1, EMP, { cmd_counter = func_calls.pop (); })
DEF_CMD (BREAK,40, 1, EMP, { std::cout << "Program generated break error" << std::endl; return false; })

#undef IS_ZERO
#undef IS_MORE_ZERO
#undef IS_LESS_ZERO

#undef COMMA

