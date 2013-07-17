/*
 * Tinygraphdb version 1.1
 *
 * Copyright (c) 2012-2013 Guillaume Collet
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

using namespace tinygraphdb;

/*******************************************************************************
 * Basic parsing functions
 *******************************************************************************/

std::vector<std::string> chomp_line (const std::string & line, char sep)
{
	std::vector<std::string> list;
	int beg = 0;
	int end = (int) line.find(sep,beg);
	while (end <= line.size()) {
		list.push_back(line.substr(beg,end - beg));
		beg = end + 1;
		end = (int) line.find(sep,beg);
	}
	list.push_back(line.substr(beg));
	return list;
}

// Test if a string is an integer //
bool is_int(const std::string & s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

// Remove spaces at the beginning and at the end of a std::string //
void rem_spaces(std::string & str)
{
	int beg = 0;
	while (str[beg] == ' ') beg++;
	str = str.substr(beg);
	int end = (int)str.size() - 1;
	while (str[end] == ' ') end--;
	str = str.substr(0,end + 1);
}

// Remove tabulations inside a std::string //
void rem_tab(std::string & str)
{
	std::string::iterator it = str.begin();
	while (it != str.end()) {
		if (*it == '\t') {
			it = str.erase(it);
		} else {
			it++;
		}
	}
}


// Read type in a string formed as follows: (type)id //
std::string readType (std::string line)
{
	std::string node_type;
	int beg_type = (int) line.find("(") + 1;
	int end_type = (int) line.find(")");
	if (beg_type < 0 || beg_type >= line.size() || end_type < 0 || end_type >= line.size()) {
		std::stringstream error_message;
		error_message << "Error: no type found in string \"" << line << "\"";
		throw std::runtime_error(error_message.str());
	}
	node_type = line.substr(beg_type, end_type - beg_type);
	rem_spaces(node_type);
	return node_type;
}

// Read id in a string formed as follows: (type)id //
int readId (std::string line)
{
	std::string node_id;
	int beg_id = (int) line.find(")") + 1;
	if (beg_id < 0 || beg_id >= line.size()) {
		std::stringstream error_message;
		error_message << "Error: no id found in string \'" << line << "\'";
		throw std::runtime_error(error_message.str());
	}
	int end_id = (int) line.find("{");
	if (end_id < 0 || end_id >= line.size()) {
		end_id = (int) line.size();
	}
	node_id = line.substr(beg_id, end_id - beg_id);
	rem_spaces(node_id);
	if (! is_int(node_id)) {
		std::stringstream error_message;
		error_message << "Error: the id \'" << node_id << "\' is not an integer";
		throw std::runtime_error(error_message.str());
	}
	return atoi(node_id.c_str());
}

// Read name in a string formed as follows: name="value" // 
std::string readPropName (std::string line)
{
	std::string prop_name;
	int end_name = (int) line.find("=");
	if (end_name < 0 || end_name >= line.size()) {
		std::stringstream error_message;
		error_message << "Error: string \'" << line << "\' is not a property";
		throw std::runtime_error(error_message.str());
	}
	prop_name = line.substr(0, end_name);
	rem_spaces(prop_name);
	
	if (prop_name.find('"') == prop_name.size()) {
		std::stringstream error_message;
		error_message << "Error: property name \'" << prop_name << "\' is not valid (char \" not allowed)";
		throw std::runtime_error(error_message.str());
	}
	if (prop_name.find(',') == prop_name.size()) {
		std::stringstream error_message;
		error_message << "Error: property name \'" << prop_name << "\' is not valid (char , not allowed)";
		throw std::runtime_error(error_message.str());
	}
	
	return prop_name;
}

// Read value in a string formed as follows: name="value" // 
std::string readPropValue (std::string line)
{
	std::string prop_value;
	int beg = (int) line.find('"') + 1;
	int end = (int) line.rfind('"');
	if (beg < 0 || beg >= line.size() || end < 0 || end >= line.size()) {
		std::stringstream error_message;
		error_message << "Error: string \'" << line << "\' is not a property";
		throw std::runtime_error(error_message.str());
	}
	prop_value = line.substr(beg, end - beg);
	rem_spaces(prop_value);
	return prop_value;
}

/*******************************************************************************
 * Node methods
 *******************************************************************************/

// Return the unique id of the node
const int & Node :: unique_id () const
{
	return _unique_id;
}

// Return the node type
const std::string & Node :: type () const
{
	return _type;
}

// Return the value of the given property  (throw an exception if it does not exist)
const std::string & Node :: property (const std::string & property) const
{
	if (_properties.find(property) == _properties.end()) {
		std::stringstream error_message;
		error_message << "Property \"" << property << "\" not found in node << " << unique_id() << "\n";
		throw std::runtime_error(error_message.str());
	}
	return _properties.find(property)->second;
};

// Return all the properties of the node
const std::map<std::string, std::string> & Node :: properties () const
{
	return _properties;
}

// Return the set of input arcs 
const std::set<Arc *> & Node :: ArcIn () const
{
	return _arc_in;
}

// Return the set of output arcs
const std::set<Arc *> & Node :: ArcOut () const
{
	return _arc_out;
}

// Return the set of input arcs of the given type
std::set<Arc *> Node :: getArcInOfType(const std::string & type)
{
	std::set<Arc *> tmp_arcs;
	for (std::set<Arc *>::iterator it = _arc_in.begin(); it != _arc_in.end(); it++) {
		if ((*it)->type().compare(type) == 0){
			tmp_arcs.insert((*it));
		}
	}
	return tmp_arcs;
}

// Return the set of output arcs of the given type
std::set<Arc *> Node :: getArcOutOfType(const std::string & type)
{
	std::set<Arc *> tmp_arcs;
	for (std::set<Arc *>::iterator it = _arc_out.begin(); it != _arc_out.end(); it++) {
		if ((*it)->type().compare(type) == 0){
			tmp_arcs.insert((*it));
		}
	}
	return tmp_arcs;
}

// Return the set of nodes at the end of arcs of the given type
std::set<Node *> Node :: getNodeFromArcOfType (std::string type)
{
	std::set<Node *> out_node;
	for (std::set<Arc *>::iterator it = _arc_in.begin(); it != _arc_in.end(); it++)
		if ((*it)->type().compare(type) == 0)
			out_node.insert((*it)->fromNode());
	for (std::set<Arc *>::iterator it = _arc_out.begin(); it != _arc_out.end(); it++)
		if ((*it)->type().compare(type) == 0)
			out_node.insert((*it)->toNode());
	return out_node;
}

// Return the set of nodes at the end of output arcs of the given type
std::set<Node *> Node :: getNodeFromArcOutOfType (std::string type)
{
	std::set<Node *> out_node;
	for (std::set<Arc *>::iterator it = _arc_out.begin(); it != _arc_out.end(); it++)
		if ((*it)->type().compare(type) == 0)
			out_node.insert((*it)->toNode());
	return out_node;
}

// Return the set of nodes at the end of input arcs of the given type
std::set<Node *> Node :: getNodeFromArcInOfType (std::string type)
{
	std::set<Node *> out_node;
	for (std::set<Arc *>::iterator it = _arc_in.begin(); it != _arc_in.end(); it++)
		if ((*it)->type().compare(type) == 0)
			out_node.insert((*it)->fromNode());
	return out_node;
}

// Check the existence of an arc of the given type
bool Node :: hasArcOfType (std::string type)
{
	for (std::set<Arc *>::iterator it = _arc_in.begin(); it != _arc_in.end(); it++)
		if ((*it)->type().compare(type) == 0)
			return true;
	for (std::set<Arc *>::iterator it = _arc_out.begin(); it != _arc_out.end(); it++)
		if ((*it)->type().compare(type) == 0)
			return true;
	return false;
}

// Check the existence of an arc of the given type from the current node to the given node
bool Node :: hasArcOfTypeToNode (std::string type, Node * node)
{
	for (std::set<Arc *>::iterator it = _arc_in.begin(); it != _arc_in.end(); it++)
		if ((*it)->type().compare(type) == 0)
			if ((*it)->toNode() == node)
				return true;
	for (std::set<Arc *>::iterator it = _arc_out.begin(); it != _arc_out.end(); it++)
		if ((*it)->type().compare(type) == 0)
			return true;
	return false;
}

// Check the existence of the given property with the given value
bool Node :: hasProp (const std::string & prop_name, const std::string & prop_value)
{
	if (_properties.find(prop_name) != _properties.end()) {
		return _properties[prop_name].compare(prop_value) == 0;
	}
	return false;
}

// Check the existence of the given property (does not check the value)
bool Node :: hasProp (const std::string & prop_name)
{
	return _properties.find(prop_name) != _properties.end();
}

// Print the node on the stdout
void Node :: print()
{
	std::cout << _type << "\t" << _unique_id;
	for (std::map<std::string, std::string>::iterator it = _properties.begin(); it != _properties.end(); it++) {
		std::cout << "\t" << it->first << "\t" << it->second;
	}
	std::cout << "\n";
}

// Print the node + the output arcs on stdout
void Node :: printFull()
{
	print();
	for (std::set<Arc*>::iterator it = _arc_out.begin(); it != _arc_out.end(); it++) {
		std::cout << "-[" << (*it)->type() << "]->";
		(*it)->toNode()->print();
	}
}

// Print the node on the given stream
void Node ::  print(std::ofstream & outfile)
{
	outfile << _type << "\t" << _unique_id;
	for (std::map<std::string, std::string>::iterator it = _properties.begin(); it != _properties.end(); it++) {
		outfile << "\t" << it->first << "\t" << it->second;
	}
	outfile << "\n";
}


/*******************************************************************************
 * Arc methods
 *******************************************************************************/

// Add a property to the arc
void Arc :: addProperty (const std::string & property, const std::string & value)
{
	_properties[property] = value;
}

// Return the unique id of the arc
const std::string & Arc :: unique_id () const
{
	return _unique_id;
}

// Return the type of the arc
const std::string & Arc :: type () const
{
	return _type;
}

// Return the value of the given property (throw an exception if it does not exist)
const std::string Arc :: property (const std::string & property) const
{
	if (_properties.find(property) == _properties.end()) {
		std::stringstream error_message;
		error_message << "Property \"" << property << "\" not found in node << " << unique_id() << "\n";
		throw std::runtime_error(error_message.str());
	}
	return _properties.find(property)->second;
}

// Return all the properties of the arc
const std::map<std::string, std::string> & Arc :: properties () const
{
	return _properties;
}

// Return the input node
Node * Arc :: fromNode ()
{
	return _from_node;
}

// Return the ouput node
Node * Arc :: toNode ()
{
	return _to_node;
}

// Print the arc on stdout
void Arc :: print ()
{
	std::cout << _from_node->unique_id() << "\t" << _type << "\t" << _to_node->unique_id();
	for (std::map<std::string, std::string>::iterator it = _properties.begin(); it != _properties.end(); it++) {
		std::cout << "\t" << it->first << "\t" << it->second;
	}
	std::cout << "\n";
}

// Print the arc on the given stream
void Arc :: print (std::ofstream & outfile)
{
	outfile << _from_node->unique_id() << "\t" << _type << "\t" << _to_node->unique_id();
	for (std::map<std::string, std::string>::iterator it = _properties.begin(); it != _properties.end(); it++) {
		outfile << "\t" << it->first << "\t" << it->second;
	}
	outfile << "\n";
}


/*******************************************************************************
 * Policy methods
 *******************************************************************************/

// Add a node type to the policy
void Policy :: addNodeType (std::string type)
{
	_node_type.insert(type);
}

// Add an arc type to the policy
void Policy :: addArcType  (std::string type)
{
	_arc_type.insert(type);
}

// Add a constraint to the policy (node->arc->node)
void Policy :: addConstraint (std::string from_type, std::string arc_link, std::string to_type)
{
	if (! isArcType(arc_link))
		_arc_type.insert(arc_link);
	if (! isNodeType(from_type))
		_node_type.insert(from_type);
	if (! isNodeType(to_type))
		_node_type.insert(to_type);
	
	// Check if the constraint already exists
	for (int i = 0; i < _from_type.size(); i++)
		if (from_type.compare(_from_type[i]) == 0 && to_type.compare(_to_type[i]) == 0 && arc_link.compare(_arc_link[i]) == 0)
			return;
	
	_from_type.push_back(from_type);
	_arc_link.push_back(arc_link);
	_to_type.push_back(to_type);
}

// Return the list of node types
const std::set<std::string> & Policy :: getNodeType () const
{
	return _node_type;
}

// Return the list of arc types
const std::set<std::string> & Policy :: getArcType () const
{
	return _arc_type;
}

// Return the list of from_node types
const std::vector<std::string> & Policy :: getFromType () const
{
	return _from_type;
}

// Return the list of to_node types
const std::vector<std::string> & Policy :: getToType () const
{
	return _to_type;
}

// Return the list of arc_link types
const std::vector<std::string> & Policy :: getLinkType () const
{
	return _arc_link;
}

// Check the existence of the given node type
bool Policy :: isNodeType (std::string type)
{
	return _node_type.find(type) != _node_type.end();
}

// Check the existence of the given arc type
bool Policy :: isArcType (std::string type)
{
	return _arc_type.find(type) != _arc_type.end();
}

// Check the validity of a link (node->arc->node)
bool Policy :: isValid (std::string from_type, std::string arc_link, std::string to_type)
{
	for (int i = 0; i < _from_type.size(); i++)
		if (from_type.compare(_from_type[i]) == 0 && to_type.compare(_to_type[i]) == 0 && arc_link.compare(_arc_link[i]) == 0)
			return true;
	return false;
}

// Print the policy on stdout
void Policy :: print ()
{
	std::cout << "Policy\n";
	for (std::set<std::string>::iterator it = _node_type.begin(); it != _node_type.end(); it++) {
		for (int i = 0; i < _from_type.size(); i++) {
			if ((*it).compare(_from_type[i]) == 0) {
			std::cout << _from_type[i] << "-\t" << _arc_link[i] << "\t" << _to_type[i] << "\n";
			}
		}
	}
}

// Print the policy on the given stream
void Policy :: print (std::ofstream & outfile)
{
	outfile << "Policy\n";
	for (std::set<std::string>::iterator it = _node_type.begin(); it != _node_type.end(); it++) {
		for (int i = 0; i < _from_type.size(); i++) {
			if ((*it).compare(_from_type[i]) == 0) {
				outfile << _from_type[i] << "\t" << _arc_link[i] << "\t" << _to_type[i] << "\n";
			}
		}
	}
}

// Read the policy from the given stream
void Policy :: read (std::string fname)
{
	std::string line;
	std::ifstream infile;
	infile.open (fname.c_str());
	if (!infile.good()) {
		std::cerr << "Cannot open file " << fname << "\n";
		exit(1);
	}
	// Find the Policy keyword to read the policies //
	getline(infile, line);
	rem_spaces(line);
	while(infile.good() && line.compare("Policy") != 0) {
		getline(infile, line);
		rem_spaces(line);
	}
	while(infile.good()) {
		getline(infile, line);
		int pos = (int) line.find("#");
		if (pos >= 0 && pos < line.size()) {
			line = line.substr(0, pos);
		}
		rem_spaces(line);
		if (!line.empty()) {
			if (line.compare("Nodes") == 0 || line.compare("Relations") == 0) {
				break;
			}
			std::vector<std::string> line_element = chomp_line(line, '\t');
			if (line_element.size() < 3) {
				std::cerr << "A policy constraint contains 3 types: node_type, arc_type, node_type -> ignore " << line << "\n";
			}
			std::string from_type = line_element[0];
			rem_spaces(from_type);
			std::string arc_type = line_element[1];
			rem_spaces(arc_type);
			std::string to_type = line_element[2];
			rem_spaces(to_type);
			
			addConstraint (from_type, arc_type, to_type);
		}
	}
	infile.close();
}

/*******************************************************************************
 * GraphDb methods
 *******************************************************************************/

// Read a node from a string : (type)unique_id{prop_name="prop_value",prop_name="prop_value",...}
void GraphDb :: readNode (std::string line)
{
	try {
		std::vector<std::string> line_element = chomp_line(line, '\t');
		if (line_element.size() < 2) {
			std::stringstream error_message;
			error_message << "A node needs at least a type (string) and a unique identifier (int)";
			throw std::runtime_error(error_message.str());
		}
		
		// Read node type and unique id
		std::vector<std::string>::iterator it = line_element.begin();
		std::string node_type = *it;
		rem_spaces(node_type);
		it++;
		int node_id = atoi((*it).c_str());
		it++;
		// read node properties
		std::map<std::string, std::string> node_properties;
		for (; it != line_element.end(); it++) {
			std::string prop_name = *it;
			rem_spaces(prop_name);
			it++;
			if (it == line_element.end()) {
				std::stringstream error_message;
				error_message << "Cannot find property value in \'" << line << "\'";
				throw std::runtime_error(error_message.str());
			}
			std::string prop_value = *it;
			rem_spaces(prop_value);
			node_properties[prop_name] = prop_value;
		}
		
		// create the node
		newNodeWithId(node_id, node_type, node_properties);
		
	} catch (std::exception & e) {
		throw;
	}
}

// Read an arc from a string : id->[type{prop="value",...}]->id
void GraphDb :: readArc (std::string line)
{
	try {
		std::vector<std::string> line_element = chomp_line(line, '\t');
		if (line_element.size() < 3) {
			std::stringstream error_message;
			error_message << "An arc needs at least an input node id (int), a type (string) and an ouput node id (int)";
			throw std::runtime_error(error_message.str());
		}
		
		// Read input id, arc type, output id
		std::vector<std::string>::iterator it = line_element.begin();
		int from_node_id = atoi((*it).c_str());
		it++;
		std::string arc_type = *it;
		rem_spaces(arc_type);
		it++;
		int to_node_id = atoi((*it).c_str());
		it++;
		
		
		// Read arc properties
		std::map<std::string, std::string> arc_properties;
		for (; it != line_element.end(); it++) {
			std::string prop_name = *it;
			rem_spaces(prop_name);
			it++;
			if (it == line_element.end()) {
				std::stringstream error_message;
				error_message << "Cannot find property value in \'" << line << "\'";
				throw std::runtime_error(error_message.str());
			}
			std::string prop_value = *it;
			rem_spaces(prop_value);
			arc_properties[prop_name] = prop_value;
		}
		
		// Add the arc to the database
		addArc(from_node_id, arc_type, to_node_id, arc_properties);
		
	} catch (std::exception & e) {
		throw;
	}
}

// Read properties from a line : prop	value	prop	value...
std::map<std::string, std::string> GraphDb :: readProperties (std::string line)
{
	std::map<std::string, std::string> tmp_prop;
	try {
		std::vector<std::string> line_element = chomp_line(line, '\t');
		for (std::vector<std::string>::iterator it = line_element.begin(); it != line_element.end(); it++) {
			std::string prop_name = *it;
			it++;
			if (it == line_element.end()) {
				std::stringstream error_message;
				error_message << "Cannot find property value in \'" << line << "\'";
				throw std::runtime_error(error_message.str());
			}
			std::string prop_value = *it;
			tmp_prop[prop_name] = prop_value;
		}
	} catch (std::exception & e) {
		throw;
	}
	return tmp_prop;
}

// Create a GraphDb instance from the given file
GraphDb :: GraphDb (std::string fname)
{
	
	_policy.read(fname);
	std::string line;
	std::ifstream infile;
	infile.open (fname.c_str());
	if (!infile.good()) {
		std::cerr << "Cannot open file " << fname << "\n";
		exit(1);
	}
	// Find the Database keyword to read the database //
	getline(infile, line);
	rem_spaces(line);
	bool in_db = false;
	bool in_nodes = false;
	bool in_rel = false;
	while(infile.good()) {
		getline(infile, line);
		rem_spaces(line);
		if (!line.empty() && line[0] != '#') {
			rem_spaces(line);
			if (line.compare("Nodes") == 0) {
				in_db = true;
				in_nodes = true;
				in_rel = false;
			} else if (line.compare("Relations") == 0){
				in_db = true;
				in_rel = true;
				in_nodes = false;
			} else if (in_db) {
				 if (in_nodes) {
					try {
						readNode(line);
					} catch (std::exception & e) {
						std::cerr << "Read node: " << e.what() << " -> ignore node\n";
					}
				 } else if (in_rel){
					 try {
						 readArc(line);
					 } catch (std::exception & e) {
						 std::cerr << "Read arc: " << e.what() << " -> ignore arc\n";
					 }
				 }
			}
		}
	}
	infile.close();
}

// Create a node of given type with the given properties and return its unique id
int GraphDb :: newNode (const std::string & type, const std::map<std::string, std::string> & properties)
{
	if (!_policy.isNodeType(type)) {
		std::stringstream error_message;
		error_message << "Unknown node type \'" << type << "\'";
		throw std::runtime_error(error_message.str());
	}
	int unique_id = (int) _nodes.size();
	if (_nodes.find(unique_id) != _nodes.end()) {
		std::map<int, Node>::iterator it = _nodes.end();
		it--;
		unique_id = it->first + 1;
	}
	_nodes[unique_id] = Node(unique_id, type, properties);
	_node_types[type].insert(unique_id);
	for (std::map<std::string, std::string>::const_iterator it = properties.begin(); it != properties.end(); it++) {
		_props[it->first][it->second].insert(unique_id);
	}
	return unique_id;
}

// Create a node of the given type with the given unique id and the given properties
void GraphDb :: newNodeWithId(const int & unique_id, const std::string & type, const std::map<std::string, std::string> & properties)
{
	if (!_policy.isNodeType(type)) {
		std::stringstream error_message;
		error_message << "Unknown node type \'" << type << "\'";
		throw std::runtime_error(error_message.str());
	}
	if (_nodes.find(unique_id) == _nodes.end()) {
		_nodes[unique_id] = Node(unique_id, type, properties);
		_node_types[type].insert(unique_id);
		for (std::map<std::string, std::string>::const_iterator it = properties.begin(); it != properties.end(); it++) {
			_props[it->first][it->second].insert(unique_id);
		}
	}
}

// Add an arc from from_id to to_id with the given type and the given properties
void GraphDb :: addArc  (const int & from_id, const std::string & type, const int & to_id, const std::map<std::string, std::string> & properties)
{
	// Check from_node existence
	if (_nodes.find(from_id) == _nodes.end()) {
		std::stringstream error_message;
		error_message << "Node \'" << from_id << "\' does not exist";
		throw std::runtime_error(error_message.str());
	}
	Node * node_from = &_nodes[from_id];
	
	// Check to_node existence
	if (_nodes.find(to_id) == _nodes.end()) {
		std::stringstream error_message;
		error_message << "Node \'" << to_id << "\' does not exist";
		throw std::runtime_error(error_message.str());
	}
	Node * node_to = &_nodes[to_id];
	
	// If the arc can exist between the two nodes, create it
	if (!_policy.isValid(node_from->type(), type, node_to->type())) {
		std::stringstream error_message;
		error_message << "Arc not valid : " << node_from->type() << "->[" << type << "]->" << node_to->type();
		throw std::runtime_error(error_message.str());
	}
	
	// If the unique id has been seen, create the new arc
	std::stringstream unique_id;
	unique_id << from_id << type << to_id;
	if (_arcs.find(unique_id.str()) == _arcs.end()) {
		_arcs[unique_id.str()] = Arc(unique_id.str(), type, properties, node_from, node_to);
		node_from->addArcOut (&_arcs[unique_id.str()]);
		node_to->addArcIn (&_arcs[unique_id.str()]);
	}
}

// Return the set of all nodes in the GraphDb
std::set<Node *> GraphDb :: allNodes ()
{
	std::set<Node *>  all;
	for (std::map<int, Node>::iterator it = _nodes.begin(); it != _nodes.end(); it++) {
		all.insert(&it->second);
	}
	return all;
}

// Return a pointer to the node with the given unique id
Node * GraphDb :: getNode (int node_id)
{
	if (_nodes.find(node_id) == _nodes.end()) {
		return NULL;
	}
	return &_nodes[node_id];
}

// Return the set of all nodes of the given type
std::set<Node *> GraphDb :: getNodesOfType (std::string type)
{
	std::set<Node *> nodes;
	if (_node_types.find(type) != _node_types.end()) {
		std::set<int> & node_set = _node_types[type];
		for (std::set<int>::iterator it = node_set.begin(); it != node_set.end(); it++) {
			nodes.insert(&_nodes[*it]);
		}
	}
	return nodes;
}

// Return the set of all nodes of the given type with the given property field
std::set<Node *> GraphDb :: getNodesOfTypeWithProperty (std::string type, std::string prop_name)
{
	std::set<Node *> nodes;
	if (_node_types.find(type) != _node_types.end()) {
		std::set<int> & node_set = _node_types[type];
		for (std::set<int>::iterator it = node_set.begin(); it != node_set.end(); it++) {
			if (_nodes[*it].hasProp(prop_name)) {
				nodes.insert(&_nodes[*it]);
			}
		}
	}
	return nodes;
}

// Return the set of all nodes of the given type with the given property field and property value
std::set<Node *> GraphDb :: getNodesOfTypeWithProperty (std::string type, std::string prop_name, std::string prop_value)
{
	std::set<Node *> nodes;
	if (_props.find(prop_name) != _props.end()) {
		if (_props[prop_name].find(prop_value) != _props[prop_name].end()) {
			for (std::set<int>::iterator it = _props[prop_name][prop_value].begin(); it != _props[prop_name][prop_value].end(); it++) {
				if (_nodes[*it].type().compare(type) == 0) {
					nodes.insert(&_nodes[*it]);
				}
			}
		}
	}
	return nodes;
}

// Return the set of nodes with the given property field
std::set<Node *> GraphDb :: getNodesWithProperty (std::string prop_name)
{
	std::set<Node *> nodes;
	if (_props.find(prop_name) != _props.end()) {
		for (std::map<std::string, std::set<int> >::iterator pit = _props[prop_name].begin(); pit != _props[prop_name].end(); pit++) {
			for (std::set<int>::iterator it = pit->second.begin(); it != pit->second.end(); it++) {
				nodes.insert(&_nodes[*it]);
			}
		}
	}
	return nodes;
}

// Return the set of nodes with the given property field and property value
std::set<Node *> GraphDb :: getNodesWithProperty (std::string prop_name, std::string prop_value)
{
	std::set<Node *> nodes;
	if (_props.find(prop_name) != _props.end()) {
		if (_props[prop_name].find(prop_value) != _props[prop_name].end()) {
			for (std::set<int>::iterator it = _props[prop_name][prop_value].begin(); it != _props[prop_name][prop_value].end(); it++) {
				nodes.insert(&_nodes[*it]);
			}
		}
	}
	return nodes;

}

// Return the policy of the GraphDb
const Policy & GraphDb :: policy () const
{
	return _policy;
}

// Return the number of nodes in the GraphDb
int GraphDb :: nbNode()
{
	return (int) _nodes.size();
}

// Return the number of arcs in the GraphDb
int GraphDb :: nbArc()
{
	return (int) _arcs.size();
}


// Save the GraphDb in the given file
void GraphDb :: save (std::string fname)
{
	std::ofstream outfile;
	outfile.open (fname.c_str());
	
	_policy.print(outfile);
	outfile << "\nNodes\n\n";
	for (std::map<int, Node>::iterator it = _nodes.begin(); it != _nodes.end(); it++) {
		it->second.print(outfile);
	}
	outfile << "\nRelations\n\n";
	for (std::map<std::string, Arc>::iterator it = _arcs.begin(); it != _arcs.end(); it++) {
		it->second.print(outfile);
	}
	
}

// Print the GraphDb on stdout
void GraphDb :: print ()
{
	_policy.print();
	std::cout << "\nNodes\n\n";
	for (std::map<int, Node>::iterator it = _nodes.begin(); it != _nodes.end(); it++) {
		it->second.print();
	}
	std::cout << "\nRelations\n\n";
	for (std::map<std::string, Arc>::iterator it = _arcs.begin(); it != _arcs.end(); it++) {
		it->second.print();
	}
	
}



