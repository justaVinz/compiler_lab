# compiler_lab

## Lexer

What is a Lexer doing:
    - reading source code
    - transforming source code to tokens

What Tokens exist:
    - identifier: Names assigned by the programmer
        e.g. variable names like foo, bar
    - keyword: Reserved words of the language.	        
        e.g. if, while, return
    - separator/punctuator: Punctuation characters and paired delimiters.
        e.g. }, (, ;
    - operator: Symbols that operate on arguments and produce results.
        e.g. +, <, =
    - literal: Numeric, logical, textual, and reference literals.	
        e.g. true, 6.02e23, "music"
    - comment:	Line or block comments. Usually discarded.	
        e.g. /* Retrieves user data */, // must be negative 
    - whitespace: Groups of non-printable characters. Usually discarded.

How to: 
    1. read source code as a string
    2. implement dictionary for tokens to identify in a generic way
    3. iterate over source code, identify and save tokens, return list of tokens



## §6.4 Lexical Elements
relevant chapter
## §6.4p4
Max Munch
## §6.4.3 Universal character names
are omitted (this is \U AF23 etc. for some character)
## §6.4.4 Constants
we are restricted to decimal constants, 0 and character constants.  
One important constraint is that the value of the constant should be in range for the type.  
The type of an integer constant is the first of the corresponding list in which its value can
be represented. So 300 is of type short. This isn't relevant to the lexer yet.
## §6.4.5 String Literals
No encoding prefix
## §6.4.6 Punctuators
[ ] ( ) { } .->
 ++ -- & * + - ~ !
 / % << >> < > <= >= == != ^ | && ||
 ? : ; ...
 = *= /= %= +=-= <<= >>= &= ^= |=
 ,  
There are others we omit
## §5.1.1.2 Translation Phases
1 unsure  
2 omitted  
3 lexing sequences of whitespace characters we need to talk about this  
## §6.4p1
keywords, identifiers, constants, string-literals, punctuators.  
No preprocessing tokens
## §6.4.2.1p4
keywords take precedence over identifiers, so if is actually keyword if and id if.

## §6.4.1 Keywords
auto break case char const continue default do double else enum extern float for goto if inline int long register restrict return short signed sizeof static struct switch typedef union unsigned void volatile while _Bool _Complex _Imaginary

## Additional requirements from assignment B
--tokenize I'm not sure how that works in C++ (or anywhere else)

Linear Runtime

2 Runs, remove comments (§6.4.9) first time? "// nocomment-string" is dangerous

ISO-8859-1 encoding

Make sure to output string- or character-constants exactly like they appear in the input stream, so do not
replace escape sequences in the output

Do not intermingle lexing with the output of the tokens. The interface to read tokens will be used again by
the parser. (so lexer prob returns a Vec<Token> or sth like that. Not test.c:1:1: constant 42)
It will prove helpful in the following phase to end the token stream of your lexer with a (not printed) end
of-file token.