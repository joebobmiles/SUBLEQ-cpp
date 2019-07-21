SUBLEQ Grammar
==============

Here is the Backus-Naur formal grammar for the SUBLEQ assembly language:

```
<line> ::= <expression> "," <expression> "," <expression> <EOL>

<expression> ::= <number> | "?"

<number> ::= <sign> <digit>+

<digit> ::= "0"-"9"

<EOL> ::= ";" | "\n"
```
