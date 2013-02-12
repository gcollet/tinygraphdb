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

#include "tinygraphdb.h"

using namespace tinygraphdb;

/*
 * Basic parsing functions
 */

// Test if a string is an integer //
bool is_int(const std::string & s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
		++it;
    return !s.empty() && it == s.end();
}

// Remove spaces at the beginning and at the end of a std::string //
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

/*
 * Node methods
 */

void Node ::  print()
{
	std::cout << "(" << _type << ")" << _unique_id;
	std::map<std::string, std::string>::iterator it = _properties.begin();
	if (it != _properties.end()) {
		std::cout << "{" << it->first << "=\"" << it->second << "\"";
		it++;
		for (; it != _properties.end(); it++) {
			std::cout << ',' << it->first << "=\"" << it->second << "\"";
		}
		std::cout << "}";
	}
	std::cout << "\n";
}

void Node :: printFull()
{
	print();
	for (std::set<Arc*>::iterator it = _arc_out.begin(); it != _arc_out.end(); it++) {
		std::cout << "-[" << (*it)->type() << "]->";
		(*it)->toNode()->print();
	}
}
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

std::set<Node *> Node :: getNodeFromArcOfType (std::string type)
{
	std::set<Node *> out_node;
	for (std::set<Arc *>::iterator it = _arc_in.begin(); it != _arc_in.end(); it++)
		if ((*it)->type().compare(type) == 0)
			out_node.insert((*it)->toNode());
	for (std::set<Arc *>::iterator it = _arc_out.begin(); it != _arc_out.end(); it++)
		if ((*it)->type().compare(type) == 0)
			out_node.insert((*it)->toNode());
	return out_node;
}

void Node ::  print(std::ofstream & outfile)
{
	outfile << "(" << _type << ")" << _unique_id;
	std::map<std::string, std::string>::iterator it = _properties.begin();
	if (it != _properties.end()) {
		outfile << "{" << it->first << "=\"" << it->second << "\"";
		it++;
		for (; it != _properties.end(); it++) {
			outfile << ',' << it->first << "=\"" << it->second << "\"";
		}
		outfile << "}";
	}
	outfile << "\n";
}

bool Node :: hasProp (const std::string & prop_name, const std::string & prop_value)
{
	if (_properties.find(prop_name) != _properties.end()) {
		return _properties[prop_name].compare(prop_value) == 0;
	}
	return false;
}

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

/*
 * Arc methods
 */
void Arc :: print ()
{
	std::cout << _from_node->unique_id() << "->[(" << _type << ")";
	std::map<std::string, std::string>::iterator it = _properties.begin();
	if (it != _properties.end()) {
		std::cout << "{" << it->first << "=\"" << it->second << "\"";
		it++;
		for (; it != _properties.end(); it++) {
			std::cout << ',' << it->first << "=\"" << it->second << "\"";
		}
		std::cout << "}";
	}
	std::cout << "]->" << _to_node->unique_id() << "\n";
}

void Arc :: print (std::ofstream & outfile)
{
	outfile << _from_node->unique_id() << "->[(" << _type << ")";
	std::map<std::string, std::string>::iterator it = _properties.begin();
	if (it != _properties.end()) {
		outfile << "{" << it->first << "=\"" << it->second << "\"";
		it++;
		for (; it != _properties.end(); it++) {
			outfile << ',' << it->first << "=\"" << it->second << "\"";
		}
		outfile << "}";
	}
	outfile << "]->" << _to_node->unique_id() << "\n";
}

/*
 * Policy methods
 */

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

bool Policy :: isValid (std::string from_type, std::string arc_link, std::string to_type)
{
	for (int i = 0; i < _from_type.size(); i++)
		if (from_type.compare(_from_type[i]) == 0 && to_type.compare(_to_type[i]) == 0 && arc_link.compare(_arc_link[i]) == 0)
			return true;
	return false;
}

void Policy :: print ()
{
	std::cout << "Policy\n";
	for (std::set<std::string>::iterator it = _node_type.begin(); it != _node_type.end(); it++) {
		for (int i = 0; i < _from_type.size(); i++) {
			if ((*it).compare(_from_type[i]) == 0) {
			std::cout << _from_type[i] << "->[" << _arc_link[i] << "]->" << _to_type[i] << "\n";
			}
		}
	}
}

void Policy :: print (std::ofstream & outfile)
{
	outfile << "Policy\n";
	for (std::set<std::string>::iterator it = _node_type.begin(); it != _node_type.end(); it++) {
		for (int i = 0; i < _from_type.size(); i++) {
			if ((*it).compare(_from_type[i]) == 0) {
				outfile << _from_type[i] << "->[" << _arc_link[i] << "]->" << _to_type[i] << "\n";
			}
		}
	}
}

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
		if (!line.empty()) {
			rem_spaces(line);
			if (line.compare("Database") == 0) {
				break;
			}
			pos = (int) line.find("->[");
			if (pos < 0 || pos >= line.size()) {
				std::cerr << "Error in policy definition\n  This constraint is not well formed: " << line << "\n";
				continue;
			}
			std::string from_type = line.substr(0,pos);
			rem_spaces(from_type);
			line = line.substr(pos+3);
			pos = (int) line.find("]->");
			if (pos < 0 || pos >= line.size()) {
				std::cerr << "Error in policy definition\n  This constraint is not well formed: " << from_type << "->" << line << "\n";
				continue;
			}
			std::string arc_type = line.substr(0,pos);
			rem_spaces(arc_type);
			std::string to_type = line.substr(pos+3);
			rem_spaces(to_type);
			addConstraint (from_type, arc_type, to_type);
		}
	}
	infile.close();
}

/*
 * GraphDb methods
 */

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

/*
 * Add a node to the database
 * Check the type is valid and the unique id has not already been seen
 */
void GraphDb :: addNode(const int & unique_id, const std::string & type, const std::map<std::string, std::string> & properties)
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

/*
 * Add an arc to the Database
 * Check if each node exists
 * Check if the type of nodes is compatible with the type of the arc
 * Generate a unique_id (concatenate from_id, arc_type and to_id)
 * Check the unique id has not already been seen
 */
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

/*
 * Read a node from a file line : (type)unique_id{prop_name="prop_value",prop_name="prop_value",...}
 */
void GraphDb :: readNode (std::string line)
{
	try {
		int beg_prop = (int) line.find("{");
		std::string node_def = line;
		if (beg_prop >= 0 && beg_prop < line.size()) {
			node_def = line.substr(0,beg_prop);
		}
		
		// read node_type
		std::string node_type = readType(node_def);
		
		// read node unique id
		int node_id = readId(node_def);
		
		// Check if type and id empty
		if (node_type.empty()) {
			std::stringstream error_message;
			error_message << "Type is empty";
			throw std::runtime_error(error_message.str());
		}
		
		// read node properties
		std::map<std::string, std::string> node_properties;
		int end_prop = (int) line.rfind("}");
		if (beg_prop >= 0 || beg_prop < line.size() || end_prop >= 0 || end_prop < line.size()) {
			node_properties = readProperties (line.substr(beg_prop + 1, end_prop - beg_prop - 1));
		}
		
		// create the node
		addNode(node_id, node_type, node_properties);
		
	} catch (std::exception & e) {
		throw;
	}
}

void GraphDb :: readArc (std::string line)
{
	try {
		// read from_node_def
		int end_from = (int) line.find("->[");
		if (end_from < 0 || end_from >= line.size()) {
			std::stringstream error_message;
			error_message << "Error: from_node is missing in arc definition: " << line;
			throw std::runtime_error(error_message.str());
		}
		// read from type
		//std::string from_type = readType(line.substr(0,end_from));
		// read from id
		int from_node_id = readId(line.substr(0,end_from));
		
		// read to_node_def
		int beg_to = (int) line.find("]->") + 3;
		if (beg_to < 0 || beg_to >= line.size()) {
			std::stringstream error_message;
			error_message << "Error: to_node is missing in arc definition: " << line;
			throw std::runtime_error(error_message.str());
		}
		// read to type
		//std::string to_type = readType(line.substr(beg_to));
		// read from id
		int to_node_id = readId(line.substr(beg_to));
		
		// read arc type
		std::string arc_type = readType(line.substr(end_from + 3, beg_to - end_from - 6));
		
		// Read arc properties
		std::map<std::string, std::string> arc_properties;
		int beg_prop = (int) line.find("{");
		int end_prop = (int) line.find("}");
		if (beg_prop >= 0 || beg_prop < line.size() || end_prop >= 0 || end_prop < line.size()) {
			arc_properties = readProperties (line.substr(beg_prop + 1, end_prop - beg_prop - 1));
		}
		
		// Add the arc to the database
		addArc(from_node_id, arc_type, to_node_id, arc_properties);
		
	} catch (std::exception & e) {
		throw;
	}
}

std::map<std::string, std::string> GraphDb :: readProperties (std::string line)
{
	std::map<std::string, std::string> tmp_prop;
	try {
		while (!line.empty()) {
			if (line[0] == ',') {
				line = line.substr(1);
			}
			int pos = (int) line.find('"');
			if (pos < 0 || pos >= line.size()) {
				std::stringstream error_message;
				error_message << "Cannot find property value in \'" << line << "\'";
				throw std::runtime_error(error_message.str());
			}
			pos = (int) line.find('"', pos + 1);
			if (pos < 0 || pos >= line.size()) {
				std::stringstream error_message;
				error_message << "Cannot find property value in \'" << line << "\'";
				throw std::runtime_error(error_message.str());
			}
			
			std::string sub = line.substr(0, pos + 1);
			std::string prop_name = readPropName(sub);
			std::string prop_value = readPropValue(sub);
			if (!prop_name.empty() && !prop_value.empty()) {
				tmp_prop[prop_name] = prop_value;
			}
			line = line.substr(pos + 1);
		}
	} catch (std::exception & e) {
		throw;
	}
	return tmp_prop;
}

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
	while(infile.good() && line.compare("Database") != 0) {
		getline(infile, line);
		rem_spaces(line);
	}
	while(infile.good()) {
		getline(infile, line);
		rem_spaces(line);
		if (!line.empty() && line[0] != '#') {
			rem_spaces(line);
			if (line.compare("Policy") == 0) {
				break;
			}
			int pos = (int) line.find("->[");
			if (pos < 0 || pos >= line.size()) {
				try {
					readNode(line);
				} catch (std::exception & e) {
					std::cerr << "Read node: " << e.what() << " -> ignore node\n";
				}
			} else {
				try {
					readArc(line);
				} catch (std::exception & e) {
					std::cerr << "Read arc: " << e.what() << " -> ignore arc\n";
				}
			}
		}
	}
	infile.close();
}

void GraphDb :: save (std::string fname)
{
	std::ofstream outfile (fname);
	
	_policy.print(outfile);
	outfile << "\nDatabase\n\n# Nodes\n\n";
	for (std::map<int, Node>::iterator it = _nodes.begin(); it != _nodes.end(); it++) {
		it->second.print(outfile);
	}
	outfile << "\n# Relations\n\n";
	for (std::map<std::string, Arc>::iterator it = _arcs.begin(); it != _arcs.end(); it++) {
		it->second.print(outfile);
	}
	
}
void GraphDb :: print ()
{
	_policy.print();
	std::cout << "\nDatabase\n\n# Nodes\n\n";
	for (std::map<int, Node>::iterator it = _nodes.begin(); it != _nodes.end(); it++) {
		it->second.print();
	}
	std::cout << "\n# Relations\n\n";
	for (std::map<std::string, Arc>::iterator it = _arcs.begin(); it != _arcs.end(); it++) {
		it->second.print();
	}
	
}

std::set<Node *> GraphDb :: getAllNodes (const std::string & constraint)
{
	std::set<Node *> nodes;
	
	try {
		
		std:: string node_line;
		int first_sep = (int) constraint.find("->[");
		if (first_sep < 0 || first_sep >= constraint.size()) {
			first_sep = (int) constraint.find("<-[");
			if (first_sep < 0 || first_sep >= constraint.size()) {
				first_sep = (int) constraint.size();
			}
		}
		node_line = constraint.substr(0, first_sep);
		
		// read node_type
		std::string node_type = readType(node_line);
		
		// read node unique id
		int node_id = -1;
		try {
			node_id = readId(node_line);
		} catch (std::exception & e) {}
		
		if (node_id >= 0) {
			Node * tmp_node = NULL;
			tmp_node = getNode(node_id);
			if (tmp_node->type().compare(node_type) == 0) {
				nodes.insert(tmp_node);
			}
		} else {
			nodes = getNodesOfType(node_type);
		}
		
	} catch (std::exception & e) {
		throw;
	}
	
	return nodes;
}

Node * GraphDb :: getNode (int node_id)
{
	if (_nodes.find(node_id) == _nodes.end()) {
		std::stringstream error_message;
		error_message << "Cannot find node with id " << node_id;
		throw std::runtime_error(error_message.str());
	}
	return &_nodes[node_id];
}

std::set<Node *> GraphDb :: getNodesOfType (std::string type)
{
	std::set<Node *> nodes;
	if (_node_types.find(type) == _node_types.end()) {
		std::stringstream error_message;
		error_message << "Cannot find node with type (" << type << ")";
		throw std::runtime_error(error_message.str());
	}
	
	std::set<int> & node_set = _node_types[type];
	for (std::set<int>::iterator it = node_set.begin(); it != node_set.end(); it++) {
		nodes.insert(&_nodes[*it]);
	}
	
	return nodes;
}

std::set<Node *> GraphDb :: getNodesOfTypeWithProperty (std::string type, std::string prop_name)
{
	std::set<Node *> nodes;
	if (_node_types.find(type) == _node_types.end()) {
		std::stringstream error_message;
		error_message << "Cannot find node with type (" << type << ")";
		throw std::runtime_error(error_message.str());
	}
	std::set<int> & node_set = _node_types[type];
	for (std::set<int>::iterator it = node_set.begin(); it != node_set.end(); it++) {
		if (_nodes[*it].hasProp(prop_name)) {
			nodes.insert(&_nodes[*it]);
		}
	}
	return nodes;
}


std::set<Node *> GraphDb :: getNodesOfTypeWithProperty (std::string type, std::string prop_name, std::string prop_value)
{
	std::set<Node *> nodes;
	if (_node_types.find(type) == _node_types.end()) {
		std::stringstream error_message;
		error_message << "Cannot find node with type (" << type << ")";
		throw std::runtime_error(error_message.str());
	}
	std::set<int> & node_set = _node_types[type];
	for (std::set<int>::iterator it = node_set.begin(); it != node_set.end(); it++) {
		if (_nodes[*it].hasProp(prop_name, prop_value)) {
			nodes.insert(&_nodes[*it]);
		}
	}
	return nodes;
}

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

int local_similarity (Node * node1, Node * node2)
{
	int similarity = 0;
	// Check properties
	const std::map<std::string, std::string> & properties = node1->properties();
	for (std::map<std::string, std::string>::const_iterator it = properties.begin(); it != properties.end(); it++) {
		if (node2->hasProp(it->first, it->second)) {
			similarity++;
		}
	}
	return similarity;
}

std::set<Node *> GraphDb :: findSimilarNodes(Node * node)
{
	std::set<Node *> similar;
	std::string type = node->type();
	std::map<int, Node>::iterator it = _nodes.begin();
	for (; it != _nodes.end(); it++) {
		int similarity = local_similarity(node, &it->second);
		if (similarity >= 1) {
			similar.insert(&it->second);
			break;
		}
	}
	if (it == _nodes.end()) {
		node->print();
		std::cerr << "not found\n";
	}
	return similar;
}

std::set<Node *> GraphDb :: allNodes ()
{
	std::set<Node *>  all;
	for (std::map<int, Node>::iterator it = _nodes.begin(); it != _nodes.end(); it++) {
		all.insert(&it->second);
	}
	return all;
}





