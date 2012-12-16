//
//  graph.cpp
//  tinyGraphDb
//
//  Created by Guillaume Collet on 12/12/12.
//

#include "graph.h"

using namespace tinygraphdb;


/*
 * Node methods
 */
void Node :: addProperty (const std::string & property, const std::string & value)
{
	_properties[property] = value;
}

void Node :: addArcIn (const Arc * in)
{
	_arc_in.insert(in);
}

void Node :: addArcOut (const Arc * out)
{
	_arc_out.insert(out);
}

const std::string Node :: property (const std::string & property)
{
	return _properties[property];
}


/*
 * Arc methods
 */
void Arc :: addProperty (const std::string & prop, const std::string & value)
{
	_properties[prop] = value;
}

const std::string Arc :: property (const std::string & property)
{
	return _properties[property];
}


/*
 * Policy methods
 */
void Policy :: addNodeType (std::string type)
{
	_node_type.insert(type);
}

void Policy :: addArcType (std::string type)
{
	_arc_type.insert(type);
}

bool Policy :: isNodeType (std::string type)
{
	return _node_type.find(type) != _node_type.end();
}

bool Policy :: isArcType (std::string type)
{
	return _arc_type.find(type) != _arc_type.end();
}

void Policy :: addConstraint (std::string from, std::string to)
{
	_from.push_back(from);
	_to.push_back(to);
}

bool Policy :: isValid (std::string from, std::string to)
{
	for (int i = 0; i < _from.size(); i++)
		if (from.compare(_from[i]) == 0 && to.compare(_to[i]) == 0)
			return true;
	return false;
}

/*
 * Graph methods
 */


