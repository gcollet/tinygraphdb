/*
 * Tinygraphdb version 1.0
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
	// - proteins catalyse reactions.
	// - compounds participate to a reaction as left or right elements.
	tinygraphdb::Policy policy;
	
	policy.addNodeType("compound");
	policy.addNodeType("protein");
	policy.addNodeType("reaction");
	
	policy.addConstraint("compound","is left of","reaction");
	policy.addConstraint("compound","is right of","reaction");
	
	policy.addConstraint("protein","catalyses","reaction");
	policy.addConstraint("reaction","is catalysed by","protein");
	
	// Create a graph database based on this policy
	
	
	
	return 0;
}

