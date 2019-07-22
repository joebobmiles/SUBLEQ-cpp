SUBLEQ Grammar
==============

Here is the Backus-Naur formal grammar for the SUBLEQ assembly language:

```
<line> ::= <expression> "," <expression> "," <expression> <EOL>
         | <expression> "," <expression> <EOL>
         | <expression> <EOL>

<expression> ::= <number> | "?"

<number> ::= <sign>? <digit>+

<sign> ::= "+" | "-"

<digit> ::= "0"-"9"

<EOL> ::= ";" | "\n"
```
