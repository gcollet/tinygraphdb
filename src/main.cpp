/*
 * Tinygraphdb version 1.2
 *
 * Copyright (c) 2010 Guillaume Collet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "tinygraphdb.h"

int main(int argc, const char * argv[])
{
	// Create a small policy where:
	// - compounds participate to a reaction as left or right elements.
	tinygraphdb::Policy policy;
	
	policy.addNodeType("compound");
	policy.addNodeType("reaction");
	
	policy.addConstraint("compound","is left of","reaction");
	policy.addConstraint("compound","is right of","reaction");
	
	// Create a graph database based on this policy
	tinygraphdb::GraphDb graph_db(policy);
	
	// Create a new node with valid type
	int n1;
	try {
		n1 = graph_db.newNode("compound", std::map<std::string,std::string>());
	} catch (std::exception & e) {
		std::cerr << e.what() << " -> ignore\n";
	}
	
	// Create a new node with invalid type caught by an exception
	try {
		graph_db.newNode("dna", std::map<std::string,std::string>());
	} catch (std::exception & e) {
		std::cerr << e.what() << " -> ignore\n";
	}
		
	// Create a compound with a property
	std::map<std::string,std::string> property;
	property["name"] = "PYRUVATE";
	int n2;
	try {
		n2 = graph_db.newNode("compound", property);
	} catch (std::exception & e) {
		std::cerr << e.what() << " -> ignore\n";
	}
	
	// Create a reaction
	int r1;
	try {
		r1 = graph_db.newNode("reaction", std::map<std::string,std::string>());
	} catch (std::exception & e) {
		std::cerr << e.what() << " -> ignore\n";
	}
	
	// Create a "is left of" relation between n1 and r1
	try {
		graph_db.addArc(n1, "is left of", r1, std::map<std::string,std::string>());
	} catch (std::exception & e) {
		std::cerr << e.what() << " -> ignore\n";
	}
	
	// Create an invalid "is left of" relation between r1 and n2
	try {
		graph_db.addArc(r1, "is left of", n1, std::map<std::string,std::string>());
	} catch (std::exception & e) {
		std::cerr << e.what() << " -> ignore\n";
	}
	
	// Print the database on standard output
	std::cerr << "\n";
	graph_db.print();
	
	// Print the database on output file
	graph_db.save("output.txt");
	
	// Print statistics
	std::cerr << "\nnb nodes : " << graph_db.nbNode() << "\nnb arcs : " << graph_db.nbArc() << "\n";
	return 0;
}

