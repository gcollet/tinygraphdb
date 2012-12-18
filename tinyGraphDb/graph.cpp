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

#include "graph.h"
#include <sstream>

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

void Policy :: addConstraint (std::string from_type, std::string arc_link, std::string to_type)
{
	if (! isArcType(arc_link))
		_arc_type.insert(arc_link);
	if (! isNodeType(from_type))
		_node_type.insert(from_type);
	if (! isNodeType(to_type))
		_node_type.insert(to_type);
	
	_from_type.push_back(from_type);
	_arc_link.push_back(arc_link);
	_to_type.push_back(to_type);
}

bool Policy :: isValid (std::string from_type, std::string arc_link, std::string to_type)
{
	for (int i = 0; i < _from_type.size(); i++)
		if (from_type.compare(_from_type[i]) == 0 && to_type.compare(_to_type[i]) == 0 && arc_link.compare(_arc_link[i]) == 0)
			return true;
	return false;
}

void Policy :: print ()
{
	for (std::set<std::string>::iterator it = _node_type.begin(); it != _node_type.end(); it++) {
		for (int i = 0; i < _from_type.size(); i++) {
			if ((*it).compare(_from_type[i]) == 0) {
			std::cout << _from_type[i] << "->[" << _arc_link[i] << "]->" << _to_type[i] << "\n";
			}
		}
	}
}

void rem_spaces(std::string & str)
{
	int beg = 0;
	while (str[beg] == ' ') {
		beg++;
	}
	str = str.substr(beg);
	int end = (int)str.size() - 1;
	while (str[end] == ' ') {
		end--;
	}
	str = str.substr(0,end + 1);
}

void Policy :: read (std::string fname)
{
	std::string line;
	std::ifstream infile;
	infile.open (fname.c_str());
	while(infile.good()) {
		getline(infile, line);
		int pos = (int) line.find("#");
		if (pos >= 0 && pos < line.size()) {
			line = line.substr(0, pos);
		}
		if (!line.empty()) {
			pos = (int) line.find("->[");
			if (pos < 0 && pos >= line.size()) {
				std::cerr << "Error in policy definition\n  This constraint is not weel formed: " << line << "\n";
				continue;
			}
			std::string from_type = line.substr(0,pos);
			rem_spaces(from_type);
			line = line.substr(pos+3);
			pos = (int) line.find("]->");
			if (pos < 0 && pos >= line.size()) {
				std::cerr << "Error in policy definition\n  This constraint is not weel formed: " << from_type << "->[" << line << "\n";
				continue;
			}
			std::string arc_type = line.substr(0,pos);
			rem_spaces(arc_type);
			std::string to_type = line.substr(pos+3);
			rem_spaces(to_type);
			addConstraint (from_type, arc_type, to_type);
		}
	}
}

/*
 * GraphDb methods
 */

void GraphDb :: addNode(const std::string & unique_id, const std::string & type)
{
	if (_policy.isNodeType(type)) {
		if (_nodes.find(unique_id) == _nodes.end()) {
			std::cerr << "Add node (" << unique_id << ", " << type << ")\n";
			_nodes[unique_id] = Node(unique_id, type);
		} else {
			std::cerr << "Node " << unique_id << " already created-> ignore\n";
		}
	} else {
		std::cerr << "Unknown node type : " << type << " -> ignore\n";
	}
}

void GraphDb :: addArc(Node * node_from, const std::string & type, Node * node_to)
{
	if (_policy.isValid(node_from->type(), type, node_to->type())) {
		std::stringstream unique_id;
		unique_id << node_from << type << node_to;
		if (_arcs.find(unique_id.str()) == _arcs.end()) {
			std::cerr << "Add arc " << node_from->unique_id() << "->[" << type << "]->" << node_to->unique_id()<< "\n";
			_arcs[unique_id.str()] = Arc(unique_id.str(), type, node_from, node_to);
			node_from->addArcOut (&_arcs[unique_id.str()]);
			node_to->addArcIn (&_arcs[unique_id.str()]);
		} else {
			std::cerr << "Arc " << unique_id.str() << " already created-> ignore\n";
		}
	} else {
		std::cerr << "Ignore arc not valid : " << node_from->type() << "->[" << type << "]->" << node_to->type()<< "\n";
	}
}

Node * GraphDb :: node(const std::string & unique_id)
{
	if (_nodes.find(unique_id) != _nodes.end()) {
		return &_nodes[unique_id];
	}
	return NULL;
}

