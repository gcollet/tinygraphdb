//
//  main.cpp
//  tinyGraphDb
//
//  Created by Guillaume Collet on 12/12/12.
//

#include <iostream>
#include "graph.h"

int main(int argc, const char * argv[])
{
	// Define the graph policy //
	tinygraphdb::Policy policy;
	
	// Node types //
	policy.addNodeType("pathway");
	policy.addNodeType("reaction");
	
	// Entity nodes
	policy.addNodeType("molecule");
	policy.addNodeType("protein");
	policy.addNodeType("complex");
	policy.addNodeType("dna");
	policy.addNodeType("rna");
	
	// Misc data
	policy.addNodeType("organism");
	policy.addNodeType("data_source");
	policy.addNodeType("data_id");
	policy.addNodeType("name");
	
	
	// A pathway contains reactions and entities
	policy.addArcType("contains");
	policy.addConstraint("pathway", "contains");
	policy.addConstraint("contains", "reaction");
	policy.addConstraint("contains", "molecule");
	policy.addConstraint("contains", "protein");
	policy.addConstraint("contains", "complex");
	policy.addConstraint("contains", "dna");
	policy.addConstraint("contains", "rna");

	// A reaction consumes entities //
	policy.addArcType("consumes");
	policy.addConstraint("reaction", "consumes");
	policy.addConstraint("consumes", "molecule");
	policy.addConstraint("consumes", "complex");
	policy.addConstraint("consumes", "protein");
	policy.addConstraint("consumes", "dna");
	policy.addConstraint("consumes", "rna");
	
	policy.addArcType("is_consumed_by");
	policy.addConstraint("is_consumed_by", "reaction");
	policy.addConstraint("molecule", "is_consumed_by");
	policy.addConstraint("complex", "is_consumed_by");
	policy.addConstraint("protein", "is_consumed_by");
	policy.addConstraint("dna", "is_consumed_by");
	policy.addConstraint("rna", "is_consumed_by");
	
	// A reaction products entities //
	policy.addArcType("products");
	policy.addConstraint("reaction", "products");
	policy.addConstraint("products", "molecule");
	policy.addConstraint("products", "complex");
	policy.addConstraint("products", "protein");
	policy.addConstraint("products", "dna");
	policy.addConstraint("products", "rna");
	
	policy.addArcType("is_modulated_by");
	
	
	policy.addConstraint("reaction", "is_in");
	policy.addConstraint("is_in", "pathway");
	
	
	policy.addArcType("is_producted_by");
	
	policy.addArcType("modulates");
	
	policy.addArcType("is_referenced_by");
	policy.addArcType("is_in");
	
	
	
}

