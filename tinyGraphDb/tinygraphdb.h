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

#ifndef __tinyGraphDb__graph__
#define __tinyGraphDb__graph__

#include <iostream>
#include <utility>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace tinygraphdb
{
	class Arc;
	class Node;
	class GraphDb;
	class Policy;
	
	/*
	 * Node Class
	 *
	 * _unique_id  : A node is uniquely identified by its _unique_id
	 * _type       : A node has a type which will be used to check policy
	 * _properties : A node has properties (pairs of strings)
	 * _arc_in(out): A node has a set of output arcs and a set of input arcs
	 */
	class Node
	{
	private:
		int _unique_id;
		std::string _type;
		std::map<std::string, std::string> _properties;
		std::set<Arc *> _arc_in;
		std::set<Arc *> _arc_out;

	public:
		Node () {};
		explicit Node (const int & unique_id, const std::string & type, const std::map<std::string, std::string> & properties):_unique_id(unique_id), _type(type), _properties(properties) {};
		~Node () {};

		void addArcIn (Arc * in) {_arc_in.insert(in);};
		void addArcOut (Arc * out) {_arc_out.insert(out);};
		void addProperty (const std::string & property, const std::string & value) {_properties[property] = value;};
		
		// Getters //
		const int & unique_id () const {return _unique_id;};
		const std::string & type () const {return _type;};
		const std::string & property (const std::string & property) const
		{
			if (_properties.find(property) == _properties.end()) {
				std::stringstream error_message;
				error_message << "Property \"" << property << "\" not found in node << " << unique_id() << "\n";
				throw error_message.str();
			}
			return _properties.find(property)->second;
		};
		const std::map<std::string, std::string> & properties () const {return _properties;};
		const std::set<Arc *> & ArcIn () const {return _arc_in;};
		const std::set<Arc *> & ArcOut () const {return _arc_out;};
		
		bool hasProp (const std::string & prop_name, const std::string & prop_value);
		
		void print();
		void print (std::ofstream & outfile);
	};

	/*
	 * Arc Class
	 *
	 * _unique_id : An arc is uniquely identified by its _unique_id
	 * _type      : An arc has a type which will be used to check policy
	 * _properties: An arc has properties (pairs of strings)
	 * _from_node : An arc has an input node
	 * _to_node   : An arc has an output node
	 *
	 */
	class Arc
	{
	private:
		std::string _unique_id;
		std::string _type;
		std::map<std::string, std::string> _properties;
		Node * _from_node;
		Node * _to_node;
		
	public:
		Arc () {};
		explicit Arc (const std::string & unique_id, const std::string & type, const std::map<std::string, std::string> & properties, Node * from, Node * to):_unique_id(unique_id), _type(type), _properties(properties), _from_node(from), _to_node(to) {};
		~Arc () {};

		void addProperty (const std::string & property, const std::string & value) {_properties[property] = value;};
		
		// Getters //
		const std::string & unique_id () const {return _unique_id;};
		const std::string & type () const {return _type;};
		const std::string property (const std::string & property) const
		{
			if (_properties.find(property) == _properties.end()) {
				std::stringstream error_message;
				error_message << "Property \"" << property << "\" not found in node << " << unique_id() << "\n";
				throw error_message.str();
			}
			return _properties.find(property)->second;
		};
		const std::map<std::string, std::string> & properties () const {return _properties;};
		const Node * fromNode () const {return _from_node;};
		const Node * toNode () const {return _to_node;};
		
		void print();
		void print (std::ofstream & outfile);
	};

	/*
	 * Policy class
	 *
	 * _node_type : A policy defines the available node types (wrong types are ignored)
	 * _arc_type  : A policy defines the available arc types (wrong types are ignored)
	 *
	 * (_from_type, arc_type, to_type) : A policy defines triplets of authorized links 
	 *                                   from a node type to a node type through an
	 *                                   arc_type
	 *
	 */
	class Policy
	{
	private:
		std::set<std::string> _node_type;
		std::set<std::string> _arc_type;

		std::vector<std::string> _from_type;
		std::vector<std::string> _arc_link;
		std::vector<std::string> _to_type;

	public:
		void addNodeType (std::string type) {_node_type.insert(type);};
		void addArcType  (std::string type) {_arc_type.insert(type);};

		const std::set<std::string> & getNodetype () const {return _node_type;};
		
		bool isNodeType (std::string type) {return _node_type.find(type) != _node_type.end();};
		bool isArcType  (std::string type) {return _arc_type.find(type) != _arc_type.end();};
		
		void addConstraint (std::string from_type, std::string arc_link, std::string to_type);
		bool isValid       (std::string from_type, std::string arc_link, std::string to_type);
		
		void print ();
		void print (std::ofstream & outfile);
		void read (std::string fname);
	};
	
	/*
	 * Interface to the graph database
	 */
	class GraphDbInterface
	{
	public:
		GraphDbInterface () {};
		virtual ~GraphDbInterface () {};
		
		virtual void addNode (const int & unique_id, const std::string & type, const std::map<std::string, std::string> & properties) = 0;
		virtual void addArc  (const int & from_id, const std::string & type, const int & to_id, const std::map<std::string, std::string> & properties) = 0;
		
		virtual Node * getNode (int node_id) = 0;
		virtual std::set<Node *> getNodesOfType (std::string type) = 0;
		virtual std::set<Node *> getAllNodes (const std::string & constraint) = 0;
		
		virtual void save (std::string fname) = 0;
		virtual void print () = 0;
	};

	
	/*
	 * GraphDb Class
	 *
	 * _policy : A graphdb defines a policy to check type consistency
	 * _nodes  : A graphdb has a set of nodes
	 * _arcs   : A graphdb has a set of arcs
	 *
	 * For quick search, it also contains:
	 * _types : types to set of id
	 */
	class GraphDb : public GraphDbInterface
	{
	private:
		Policy _policy;
		
		std::map<int, Node> _nodes;
		std::map<std::string, Arc>  _arcs;
		
		std::map<std::string, std::set<int> > _node_types;
		std::map<std::string, std::map<std::string, std::set<int> > > _props;
		
		void readNode (std::string line);
		void readArc  (std::string line);
		std::map<std::string, std::string> readProperties (std::string line);
		
	public:
		explicit GraphDb (std::string fname);
		~GraphDb () {};
		
		// Adders check consistency on the defined policy //
		void addNode (const int & unique_id, const std::string & type, const std::map<std::string, std::string> & properties);
		void addArc  (const int & from_id, const std::string & type, const int & to_id, const std::map<std::string, std::string> & properties);
		
		Node * getNode (int node_id);
		std::set<Node *> getNodesOfType (std::string type);
		std::set<Node *> getAllNodes (const std::string & constraint);
		
		void save (std::string fname);
		void print();
	};

} // namespace tinygraphdb

#endif
