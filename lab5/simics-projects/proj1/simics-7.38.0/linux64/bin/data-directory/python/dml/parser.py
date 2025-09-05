# Parser for DML 1.2
# Copyright (C) 2014-2015 Free Software Foundation, Inc.
 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import os
import re
from .ast import *
from .errors import *

reserved = (
    # Particular to DML (keywords - not allowed as identifiers)
    'CAST', 'DEFINED', 'SIZEOFTYPE', 'TYPEOF', 'UNDEFINED',

    # ANSI C reserved words
    'AUTO', 'BREAK', 'CASE', 'CHAR', 'CONST', 'CONTINUE', 'DEFAULT',
    'DO', 'DOUBLE', 'ELSE', 'ENUM', 'EXTERN', 'FLOAT', 'FOR', 'GOTO',
    'IF', 'INT', 'LONG', 'REGISTER', 'RETURN', 'SHORT', 'SIGNED',
    'SIZEOF', 'STATIC', 'STRUCT', 'SWITCH', 'TYPEDEF', 'UNION',
    'UNSIGNED', 'VOID', 'VOLATILE', 'WHILE',

    # Particular to C99 and C++
    'DELETE',           # C++
    'INLINE',           # C99 and C++
    'NEW',              # C++
    'RESTRICT',         # C99
    'TEMPLATE',         # C++
    'THROW',            # C++
    'TRY',              # C++
    'CATCH',            # C++
    'THIS',             # C++

    # C++ reserved words being reserved by DML for future use
    'CLASS', 'NAMESPACE', 'PRIVATE', 'PROTECTED', 'PUBLIC',
    'USING', 'VIRTUAL'
    )

tokens = reserved + (
    # Literals (identifier, integer constant, hex constant, binary
    # constant, float constant, string constant, char const)
    'ID', 'ICONST', 'HCONST', 'BCONST', 'FCONST', 'SCONST', 'CCONST',

    # Operators (+, -, *, /, %, |, &, ~, ^, <<, >>, ||, &&, !, <, <=, >,
    # >=, ==, !=)
    'PLUS', 'MINUS', 'TIMES', 'DIVIDE', 'MOD',
    'BOR', 'BAND', 'BNOT', 'BXOR', 'LSHIFT', 'RSHIFT',
    'LOR', 'LAND', 'LNOT',
    'LT', 'LE', 'GT', 'GE', 'EQ', 'NE',

    # Assignment (=, *=, /=, %=, +=, -=, <<=, >>=, &=, ^=, |=)
    'EQUALS',

    # Increment/decrement (++, --)
    'PLUSPLUS', 'MINUSMINUS',

    # Structure dereference (->)
    'ARROW',

    # Conditional operator (?)
    'CONDOP',

    # Delimiters ( ) [ ] { } , . ; :
    'LPAREN', 'RPAREN',
    'LBRACKET', 'RBRACKET',
    'LBRACE', 'RBRACE',
    'COMMA', 'PERIOD', 'COLON',

    # DML extensions (non-C)
    'AT',
    'DOLLAR',
    'DOTDOT',
    'HASH',
    )

# Completely ignored characters
t_ignore           = ' \t\r\n\x0c'

# Operators
t_PLUS             = r'\+'
t_MINUS            = r'-'
t_TIMES            = r'\*'
t_DIVIDE           = r'/'
t_MOD              = r'%'
t_BOR              = r'\|'
t_BAND             = r'&'
t_BNOT             = r'~'
t_BXOR             = r'\^'
t_LSHIFT           = r'<<'
t_RSHIFT           = r'>>'
t_LOR              = r'\|\|'
t_LAND             = r'&&'
t_LNOT             = r'!'
t_LT               = r'<'
t_GT               = r'>'
t_LE               = r'<='
t_GE               = r'>='
t_EQ               = r'=='
t_NE               = r'!='
t_EQUALS           = r'='

# ->
t_ARROW            = r'->'

# ?
t_CONDOP           = r'\?'

# Delimiters
t_LPAREN           = r'\('
t_RPAREN           = r'\)'
t_LBRACKET         = r'\['
t_RBRACKET         = r'\]'
t_LBRACE           = r'\{'
t_RBRACE           = r'\}'
t_COMMA            = r','
t_PERIOD           = r'\.'
#t_SEMI             = r';'
t_COLON            = r':'
#t_ELLIPSIS         = r'\.\.\.'

# DML-specific (non-C)
t_DOLLAR = r'\$'
t_DOTDOT = r'\.\.'
t_HASH = r'\#'

# Keywords and identifiers
keywords = {}
for r in reserved:
    keywords[r.lower()] = r

def t_ID(t):
    r'[A-Za-z_]\w*'
    t.type = keywords.get(t.value, 'ID')
    return t

# Floating-point literal
# Keep this before t_INT
def t_FCONST(t):
    r'[0-9]*(\.[0-9]+([eE]-?[0-9]+)?|([eE]-?[0-9]+))'
    t.value = float(t.value)
    return t

# Hexadecimal integer literal
# Keep this before t_INT
def t_HCONST(t):
    r'0x[0-9a-fA-F_]*[0-9a-fA-F]'
    t.value = int(t.value[2:].replace('_', ''), 16)
    return t

# Binary integer literal
# Keep this before t_INT
def t_BCONST(t):
    r'0b[01_]*[01]'
    t.value = int(t.value[2:].replace('_', ''), 2)
    return t

# Integer literal
def t_ICONST(t):
    r'[0-9](?:[0-9_]*[0-9])?'
    t.value = int(t.value.replace('_', ''), 10)
    return t

escapes = {
    '\\': '\\',
    '"': '"',
    'n': '\n',
    'r': '\r',
    't': '\t',
    'b': '\b',
    }

def t_SCONST(t):
    r'"(?:[^\x00-\x1f\x7f"\\]|\\.)*"'
    s = t.value[1:-1].encode('utf-8')

    orig_str = s
    bs = -1
    while True:
        bs = s.find(b'\\', bs + 1)
        if bs < 0:
            break
        c = s[bs + 1]
        if c in escapes:
            s = s[:bs] + escapes[c] + s[bs + 2:]
        elif c == 'x' and re.match(b'[0-9a-fA-F][0-9a-fA-F]', s[bs + 2:]):
            code = int(s[bs + 2 : bs + 4], 16)
            if code >= 0x80 and re.search(b'[\x80-\xff]', orig_str):
                raise ESYNTAX(t, "Hex escape above \\x7f in Unicode string")
            s = s[:bs] + chr(code) + s[bs + 4:]
        else:
            raise ESYNTAX(t, "unrecognised character escape sequence")
    t.value = s
    return t

char_escapes = {
    '\\': '\\',
    "'": "'",
    'n': '\n',
    'r': '\r',
    't': '\t',
    'b': '\b',
    }

def t_CCONST(t):
    r"'(?:[^\x00-\x1f\x7f-\xff'\\]|\\.)[^']*'"
    c = t.value[1:-1]
    if c[0] == '\\':
        if c[1:] in char_escapes:
            c = char_escapes[c[1:]]
        else:
            raise ESYNTAX(t, "unrecognised character escape sequence")
    elif len(c) != 1:
        raise ESYNTAX(t, "character constants must be a single ASCII char")
    t.value = ord(c)
    return t

# Comments
def t_comment(t):
    r'/\*([^*]|\*+[^/*])*\*+/'
    t.lineno += t.value.count('\n')

def t_cppcomment(t):
    r'//.*'

def t_error(t):
    raise ESYNTAX(t, "illegal character")

# Build the lexer
from ply import lex
lexer = lex.lex()

# Parsing rules

def p_expression(p):
    'expression : assignment'
    p[0] = p[1]

def p_assignment(p):
    'assignment : conditional EQUALS assignment'
    p[0] = ast_assignment(p[1], p[3])

def p_assignment2(p):
    'assignment : conditional'
    p[0] = p[1]

def p_conditional(p):
    'conditional : logical_or CONDOP expression COLON conditional'
    p[0] = ast_conditional(p[1], p[3], p[5])

def p_conditional2(p):
    'conditional : logical_or'
    p[0] = p[1]

def p_logical_or(p):
    'logical_or : logical_or LOR logical_and'
    p[0] = ast_binary(p[1], p[2], p[3])

def p_logical_or2(p):
    'logical_or : logical_and'
    p[0] = p[1]

def p_logical_and(p):
    'logical_and : logical_and LAND bit_or'
    p[0] = ast_binary(p[1], p[2], p[3])

def p_logical_and2(p):
    'logical_and : bit_or'
    p[0] = p[1]

def p_bit_or(p):
    'bit_or : bit_or BOR bit_xor'
    p[0] = ast_binary(p[1], p[2], p[3])

def p_bit_or2(p):
    'bit_or : bit_xor'
    p[0] = p[1]

def p_bit_xor(p):
    'bit_xor : bit_xor BXOR bit_and'
    p[0] = ast_binary(p[1], p[2], p[3])

def p_bit_xor2(p):
    'bit_xor : bit_and'
    p[0] = p[1]

def p_bit_and(p):
    'bit_and : bit_and BAND equality'
    p[0] = ast_binary(p[1], p[2], p[3])

def p_bit_and2(p):
    'bit_and : equality'
    p[0] = p[1]

def p_equality(p):
    '''equality : equality EQ relational
                | equality NE relational'''
    p[0] = ast_binary(p[1], p[2], p[3])

def p_equality2(p):
    'equality : relational'
    p[0] = p[1]

def p_relational(p):
    '''relational : relational LT shift
                  | relational GT shift
                  | relational LE shift
                  | relational GE shift'''
    p[0] = ast_binary(p[1], p[2], p[3])

def p_relational2(p):
    'relational : shift'
    p[0] = p[1]

def p_shift(p):
    '''shift : shift LSHIFT additive
             | shift RSHIFT additive'''
    p[0] = ast_binary(p[1], p[2], p[3])

def p_shift2(p):
    'shift : additive'
    p[0] = p[1]

def p_additive(p):
    '''additive : additive PLUS multiplicative
                | additive MINUS multiplicative'''
    p[0] = ast_binary(p[1], p[2], p[3])

def p_additive2(p):
    'additive : multiplicative'
    p[0] = p[1]

def p_multiplicative(p):
    '''multiplicative : multiplicative TIMES unary
                      | multiplicative DIVIDE unary
                      | multiplicative MOD unary'''
    p[0] = ast_binary(p[1], p[2], p[3])

def p_multiplicative2(p):
    'multiplicative : unary'
    p[0] = p[1]

def p_unary(p):
    '''unary : SIZEOF unary
             | MINUS unary
             | PLUS unary
             | LNOT unary
             | BNOT unary
             | BAND unary
             | TIMES unary
             | DEFINED unary
             | HASH unary'''
    p[0] = ast_unary(p[1], p[2])

def p_unary2(p):
    'unary : postfix'
    p[0] = p[1]

def p_member(p):
    '''postfix : postfix PERIOD objident
               | postfix ARROW objident'''
    p[0] = ast_member(p[1], p[2], p[3])

def p_index(p):
    'postfix : postfix LBRACKET expression RBRACKET'
    p[0] = ast_indexed(p[1], p[3])

# TODO: cast

def p_postfix2(p):
    'postfix : primary'
    p[0] = p[1]

def p_int(p):
    '''primary : ICONST
               | HCONST
               | BCONST
               | CCONST'''
    p[0] = ast_int(p[1])

def p_float(p):
    'primary : FCONST'
    p[0] = ast_float(p[1])

def p_string(p):
    'primary : SCONST'
    p[0] = ast_string(p[1])

def p_undefined(p):
    'primary : UNDEFINED'
    p[0] = ast_undefined()

def p_objectref(t):
    'primary : DOLLAR objident'
    t[0] = ast_objectref(t[2])

def p_expression_ident(p):
    'primary : objident'
    p[0] = ast_var(p[1])

def p_primary_paren(p):
    'primary : LPAREN expression RPAREN'
    p[0] = p[2]

def p_arg_empty(p):
    'arguments : '
    p[0] = []

def p_arg_one(p):
    'arguments_nonempty : postfix'
    p[0] = [p[1]]

def p_arg_nonempty(p):
    'arguments : arguments_nonempty'
    p[0] = p[1]

def p_arg_many(p):
    'arguments_nonempty : arguments_nonempty COMMA postfix'
    p[0] = p[1] + [p[3]]

def p_funcall(p):
    'postfix : postfix LPAREN arguments RPAREN'
    p[0] = ast_funcall(p[1], p[3])

def p_cast(p):
    'postfix : CAST LPAREN expression COMMA ctypedecl RPAREN'
    p[0] = ast_cast(p[3], p[5])

# ctypedecl is a type without any declared variable

def p_ctypedecl(t):
    'ctypedecl : const_opt basetype ctypedecl_ptr'
    t[0] = "const " * t[1] + t[2] + t[3]


def p_ctypedecl_ptr(t):
    'ctypedecl_ptr : stars ctypedecl_array'
    t[0] = t[1] + t[2]

def p_stars_empty(t):
    'stars : '
    t[0] = ''

def p_stars_const(t):
    'stars : TIMES CONST stars'
    t[0] = '*const' + t[3]

def p_stars(t):
    'stars : TIMES stars'
    t[0] = '*' + t[2]

def p_basetype(t):
    # TODO: struct/layout/bitfields and typeof
    '''basetype : typeident'''
    t[0] = t[1]

def p_typeident(t):
    '''typeident : ident
                 | CHAR
                 | DOUBLE
                 | FLOAT
                 | INT
                 | LONG
                 | SHORT
                 | SIGNED
                 | UNSIGNED
                 | VOID
                 | REGISTER'''
    t[0] = t[1]

def p_ctypedecl_array_simple(t):
    'ctypedecl_array : ctypedecl_simple'
    t[0] = t[1]

def p_ctypedecl_simple_par(t):
    'ctypedecl_simple : LPAREN ctypedecl_ptr RPAREN'
    t[0] = '(' + t[2] + ')'

def p_ctypedecl_simple_none(t):
    'ctypedecl_simple : ' # no variable here
    t[0] = ''

def p_const_opt(t):
    'const_opt : CONST'
    t[0] = True

def p_const_opt_empty(t):
    'const_opt :'
    t[0] = False

def p_expression_slice(t):
    'postfix : postfix LBRACKET expression COLON expression RBRACKET'
    t[0] = ast_slice(t[1], t[3], t[5])

# Object/parameter names may use some additional keywords for now...
def p_objident(t):
    '''objident : ident
                | THIS
                | REGISTER
                | SIGNED
                | UNSIGNED'''
    t[0] = t[1]

def p_ident(t):
    'ident : ID'
    t[0] = t[1]

def p_reserved(t):
    '''ident : CLASS
             | ENUM
             | NAMESPACE
             | PRIVATE
             | PROTECTED
             | PUBLIC
             | RESTRICT
             | UNION
             | USING
             | VIRTUAL
             | VOLATILE'''
    raise ESYNTAX(t, "reserved keyword")

def p_badtoken(t):
    '''ident : THROW
             | STRUCT
             | SIZEOFTYPE
             | TYPEOF
             | STATIC
             | AUTO
             | EXTERN
             | SWITCH
             | CASE
             | DO
             | WHILE
             | FOR
             | INLINE
             | DEFAULT
             | TEMPLATE
             | NEW
             | DELETE
             | CONTINUE
             | BREAK
             | RETURN
             | GOTO
             | CATCH
             | TRY
             | IF
             | ELSE
             | TYPEDEF
             | LBRACE
             | RBRACE
             | MINUSMINUS
             | PLUSPLUS
             | AT
             | COMMA
             | DOTDOT'''
    raise ESYNTAX(t, "invalid keyword/operator")

# Error rule for syntax errors
def p_error(p):
    raise ESYNTAX(p, None)

# Build the parser
from ply import yacc

_parser = None
parsetab_file = os.path.join(os.path.dirname(__file__), 'parsetab')
def get_parser():
    global _parser
    if _parser is None:
        _parser = yacc.yacc(picklefile=parsetab_file)
    return _parser

def parse(expr_str):
    return get_parser().parse(expr_str)
