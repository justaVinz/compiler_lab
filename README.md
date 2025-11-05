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