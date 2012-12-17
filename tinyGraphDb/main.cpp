/* Copyright (c) 2010 Guillaume Collet
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

#include <iostream>
#include "graph.h"

int main(int argc, const char * argv[])
{
	// Define the graph policy //
	tinygraphdb::Policy policy;
	
	/*
	 * Names 
	 *
	 * Names are used to identify biological data
	 * One piece of data can have multiple names
	 *
	 * A name has only one property which is a string
	 */
	policy.addNodeType("name");
	
	
	/*
	 * Data source
	 *
	 * A piece of data has been integrated from one or many databases
	 * The data_source node keeps information about the database
	 * The data_id keeps the id of the piece of data in a given database
	 */
	policy.addNodeType("data_source");
	policy.addNodeType("data_id");
	
	policy.addConstraint("data_id", "is in db", "data_source");
	policy.addConstraint("data_source", "contains", "data_id");
	
	
	/*
	 * Organism
	 */
	policy.addNodeType("organism");
	
	// Organism has names
	policy.addConstraint("organism", "has name", "name");
	policy.addConstraint("name", "is name of", "organism");
	
	
	/*
	 * Pathway
	 */
	policy.addNodeType("pathway");
	
	// A pathway has names
	policy.addConstraint("pathway", "has name", "name");
	policy.addConstraint("name", "is name of", "pathway");
	
	// A pathway is referenced in data_sources through a data_id
	policy.addConstraint("pathway", "has id", "data_id");
	policy.addConstraint("data_id", "refers", "pathway");
	
	
	/*
	 * Reaction
	 */
	policy.addNodeType("reaction");
	
	// A reaction has names
	policy.addConstraint("reaction", "has name", "name");
	policy.addConstraint("name", "is name of", "reaction");
	
	// A reaction is referenced in data_sources through a data_id
	policy.addConstraint("reaction", "has id", "data_id");
	policy.addConstraint("data_id", "refers", "reaction");
	
	
	/*
	 * Molecule
	 */
	policy.addNodeType("molecule");

	// A molecule has names
	policy.addConstraint("molecule", "has name", "name");
	policy.addConstraint("name", "is name of", "molecule");
	
	// A molecule is referenced in data_sources through a data_id
	policy.addConstraint("molecule", "has id", "data_id");
	policy.addConstraint("data_id", "refers", "molecule");
	
	
	/*
	 * Protein
	 */
	policy.addNodeType("protein");
	
	// A protein has names
	policy.addConstraint("protein", "has name", "name");
	policy.addConstraint("name", "is name of", "protein");
	
	// A protein is referenced in data_sources through a data_id
	policy.addConstraint("protein", "has id", "data_id");
	policy.addConstraint("data_id", "refers", "protein");
	
	
	/*
	 * Dna
	 */
	policy.addNodeType("dna");
	
	// A dna has names
	policy.addConstraint("dna", "has name", "name");
	policy.addConstraint("name", "is name of", "dna");
	
	// A dna is referenced in data_sources through a data_id
	policy.addConstraint("dna", "has id", "data_id");
	policy.addConstraint("data_id", "refers", "dna");
	
	
	/*
	 * Rna
	 */
	policy.addNodeType("rna");
	
	// A dna has names
	policy.addConstraint("rna", "has name", "name");
	policy.addConstraint("name", "is name of", "rna");
	
	// A rna is referenced in data_sources through a data_id
	policy.addConstraint("rna", "has id", "data_id");
	policy.addConstraint("data_id", "refers", "rna");
	
	/*
	 * Complex
	 */
	policy.addNodeType("complex");
	
	// A dna has names
	policy.addConstraint("complex", "has name", "name");
	policy.addConstraint("name", "is name of", "complex");
	
	// A complex is referenced in data_sources through a data_id
	policy.addConstraint("complex", "has id", "data_id");
	policy.addConstraint("data_id", "refers", "complex");
	
	// A complex contains entities//
	policy.addConstraint("complex", "contains", "molecule");
	policy.addConstraint("complex", "contains", "protein");
	policy.addConstraint("complex", "contains", "complex");
	policy.addConstraint("complex", "contains", "dna");
	policy.addConstraint("complex", "contains", "rna");
	
	policy.addConstraint("molecule", "is in complex", "complex");
	policy.addConstraint("protein",  "is in complex", "complex");
	policy.addConstraint("complex",  "is in complex", "complex");
	policy.addConstraint("dna",      "is in complex", "complex");
	policy.addConstraint("rna",      "is in complex", "complex");

	
	// An organism contains reactions, pathways and entities
	policy.addConstraint("organism", "contains", "molecule");
	policy.addConstraint("organism", "contains", "protein");
	policy.addConstraint("organism", "contains", "complex");
	policy.addConstraint("organism", "contains", "dna");
	policy.addConstraint("organism", "contains", "rna");
	
	policy.addConstraint("molecule", "is in organism", "organism");
	policy.addConstraint("protein",  "is in organism", "organism");
	policy.addConstraint("complex",  "is in organism", "organism");
	policy.addConstraint("dna",      "is in organism", "organism");
	policy.addConstraint("rna",      "is in organism", "organism");
	
	// A pathway contains reactions and entities
	policy.addConstraint("pathway", "contains", "reaction");
	policy.addConstraint("pathway", "contains", "molecule");
	policy.addConstraint("pathway", "contains", "protein");
	policy.addConstraint("pathway", "contains", "complex");
	policy.addConstraint("pathway", "contains", "dna");
	policy.addConstraint("pathway", "contains", "rna");
	
	policy.addConstraint("reaction", "is in pathway", "pathway");
	policy.addConstraint("pathway",  "is in pathway", "pathway");
	policy.addConstraint("molecule", "is in pathway", "pathway");
	policy.addConstraint("protein",  "is in pathway", "pathway");
	policy.addConstraint("complex",  "is in pathway", "pathway");
	policy.addConstraint("dna",      "is in pathway", "pathway");
	policy.addConstraint("rna",      "is in pathway", "pathway");
	
	// A reaction consumes entities //
	policy.addConstraint("reaction", "consumes", "molecule");
	policy.addConstraint("reaction", "consumes", "protein");
	policy.addConstraint("reaction", "consumes", "complex");
	policy.addConstraint("reaction", "consumes", "dna");
	policy.addConstraint("reaction", "consumes", "rna");
	
	policy.addConstraint("molecule", "is consumed by", "reaction");
	policy.addConstraint("complex",  "is consumed by", "reaction");
	policy.addConstraint("protein",  "is consumed by", "reaction");
	policy.addConstraint("dna",      "is consumed by", "reaction");
	policy.addConstraint("rna",      "is consumed by", "reaction");
	
	// A reaction products entities //
	policy.addConstraint("reaction", "products", "molecule");
	policy.addConstraint("reaction", "products", "protein");
	policy.addConstraint("reaction", "products", "complex");
	policy.addConstraint("reaction", "products", "dna");
	policy.addConstraint("reaction", "products", "rna");
	
	policy.addConstraint("molecule", "is produced by", "reaction");
	policy.addConstraint("complex",  "is produced by", "reaction");
	policy.addConstraint("protein",  "is produced by", "reaction");
	policy.addConstraint("dna",      "is produced by", "reaction");
	policy.addConstraint("rna",      "is produced by", "reaction");
	
	// A reactions is modulated by entities //
	policy.addConstraint("reaction", "is modulated by", "molecule");
	policy.addConstraint("reaction", "is modulated by", "complex");
	policy.addConstraint("reaction", "is modulated by", "protein");
	policy.addConstraint("reaction", "is modulated by", "dna");
	policy.addConstraint("reaction", "is modulated by", "rna");
	
	policy.addConstraint("molecule", "modulates", "reaction");
	policy.addConstraint("complex",  "modulates", "reaction");
	policy.addConstraint("protein",  "modulates", "reaction");
	policy.addConstraint("dna",      "modulates", "reaction");
	policy.addConstraint("rna",      "modulates", "reaction");
	
	tinygraphdb::GraphDb tmp_graph (policy);
	
	tmp_graph.addNode("Protein23874839", "protein");
	tmp_graph.addNode("CONSUerkn", "reaction");
	
	tmp_graph.addArc(tmp_graph.node("CONSUerkn"), "consumes", tmp_graph.node("Protein23874839"));
	tmp_graph.addArc(tmp_graph.node("Protein23874839"), "is produced by", tmp_graph.node("CONSUerkn"));
	
	
	const std::map<std::string, tinygraphdb::Arc> & arcs = tmp_graph.getArcs();
	for (std::map<std::string, tinygraphdb::Arc>::const_iterator it = arcs.begin(); it != arcs.end(); it++) {
		std::cout << it->first << "\n";
	}
	
	
	//policy.print();
}

