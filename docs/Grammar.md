SUBLEQ Grammar
==============

Here is the Backus-Naur formal grammar (using PCRE regex syntax) for the SUBLEQ
assembly language:

```
<program> ::= <instruction>+

<instruction> ::= <expression> "," <expression> "," <expression> <EOL>
                | <expression> "," <expression> <EOL>
                | <expression> <EOL>

<expression> ::= <label>? (<number> | "?")
               | <identifier>

<identifier> ::= [^ <EOL> <whitespace> "," ":"]+

<label> ::= <identifier> ":"

<number> ::= <sign>? <digit>+

<sign> ::= "+" | "-"

<digit> ::= ["0"-"9"]

<EOL> ::= ";" | "\n"

<whitespace> ::= " " "\r" "\t"
```
