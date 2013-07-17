TinyGraphDb is a tiny graph database engine written in C++

I will need help to develop it because I will use it in my personal project 
but I don't know what are the needs for other projects.

Feel free to participate !

In version 1.2, you can create a graph database with nodes and arcs (vertex and edges) under a given policy. The policy defines the types of nodes (strictly) and the type of arc between two nodes.

Example:

protein	has id	db_id

This policy defines an arc of type "has id" from a node of type "protein" to a node of type "db_id". If you try to create an arc of type "has id" from a node of type "db_id" to a node of type "protein", an exception is thrown. Terms are separated by tabulations.

Then, nodes are written in this format (tab_separated):

node_type	id	prop_name1	prop_value1	prop_name2	prop_value2	…

Relations are written in this format (tab separated):

node_id1	arc_type	node_id2	prop_value1	prop_name2	prop_value2	…


TODOs:

Add some algorithms like graph traversal, map reduce, etc…
These algorithms can be used but not directly (you have to code it).

Add a graphDb with no policy (no constraints on arc and node types)

Licence:

Copyright (c) 2013 Guillaume Collet

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
