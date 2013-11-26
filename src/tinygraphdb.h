/* 
 * Tinygraphdb version 1.3
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

#ifndef __tinyGraphDb__graph__
#define __tinyGraphDb__graph__

#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>

void rem_spaces(std::string & str);
void rem_tab(std::string & str);
std::vector<std::string> chomp_line (const std::string & line, char sep);

namespace tinygraphdb
{
	class Arc;
	class Node;
	class GraphDb;
	class Policy;
	
	
	/*******************************************************************************
	 * Node Class
	 *
	 * _unique_id  : A node is uniquely identified by its _unique_id
	 * _type       : A node has a type which will be used to check policy
	 * _properties : A node has properties (pairs of strings: name and value)
	 *               Each property name is unique for a given node
	 * _arc_in(out): A node has a set of output arcs and a set of input arcs
	 *******************************************************************************/
	class Node
	{
	private:
		int _unique_id;
		std::string _type;
		std::map<std::string, std::set<std::string> > _properties;
		std::set<Arc *> _arcs;

	public:
		// Constructor & destructor //
		Node () {};
		explicit Node (const int & unique_id, const std::string & type, const std::map<std::string, std::set<std::string> > & properties):_unique_id(unique_id), _type(type), _properties(properties) {};
		~Node () {};

		// Adders //
		void addArc (Arc * arc) {_arcs.insert(arc);};
		void addProperty (const std::string & property, const std::string & value) {_properties[property].insert(value);};
		
		// Eraser //
		void eraseArc (const std::string & arc_id);
		void eraseProperty (const std::string & property) {_properties.erase(property);};
		void eraseProperty (const std::string & property, const std::string & value) {_properties[property].erase(_properties[property].find(value));};
		
		// Getters //
		const int & unique_id () const;
		const std::string & type () const;
		const std::set<std::string> & property (const std::string & property) const;
		const std::map<std::string, std::set<std::string> >& properties () const;
		const std::set<Arc *> & arcs () const {return _arcs;};
		
		std::set<Arc *> getArcOfType(const std::string & type);
		std::set<Node *> getNodeFromArcOfType (std::string type);
		
		// Checkers //
		bool hasArcOfType (std::string type);
		bool hasArcOfTypeToNode (std::string type, Node * node);
		bool hasProp (const std::string & prop_name, const std::string & prop_value);
		bool hasProp (const std::string & prop_name);
		
		// Printers //
		void print();
		void print (std::ofstream & outfile);
	};

	
	/*******************************************************************************
	 * Arc Class
	 *
	 * _unique_id : An arc is uniquely identified by its _unique_id
	 * _type      : An arc has a type which will be used to check policy
	 * _properties: An arc has properties (pairs of strings)
	 * _from_node : An arc has an input node (only one)
	 * _to_node   : An arc has an output node (only one)
	 *
	 *******************************************************************************/
	class Arc
	{
	private:
		std::string _unique_id;
		std::string _type;
		std::map<std::string, std::set<std::string> > _properties;
		Node * _from_node;
		Node * _to_node;
		
	public:
		// Constructor & destructor //
		Arc () {};
		explicit Arc (const std::string & unique_id, const std::string & type, const std::map<std::string, std::set<std::string> > & properties, Node * from, Node * to):_unique_id(unique_id), _type(type), _properties(properties), _from_node(from), _to_node(to) {};
		~Arc () {};

		// Adders //
		void addProperty (const std::string & property, const std::string & value) {_properties[property].insert(value);};
		
		// Getters //
		const std::string & unique_id () const;
		const std::string & type () const;
		const std::set<std::string> & property (const std::string & property) const;
		const std::map<std::string, std::set<std::string> > & properties () const;
		Node * fromNode ();
		Node * toNode ();
		
		// Printers //
		void print();
		void print (std::ofstream & outfile);
	};

	
	/*******************************************************************************
	 * Policy class
	 *
	 * _node_type : A policy defines the available node types (wrong types are ignored)
	 * _arc_type  : A policy defines the available arc types (wrong types are ignored)
	 *
	 * (_from_type, arc_type, to_type) : A policy defines triplets of authorized links 
	 *                                   from a node type to a node type through an
	 *                                   arc_type
	 *
	 *******************************************************************************/
	class Policy
	{
	private:
		std::set<std::string> _node_type;
		std::set<std::string> _arc_type;

		std::vector<std::string> _from_type;
		std::vector<std::string> _arc_link;
		std::vector<std::string> _to_type;

	public:
		// Adders //
		void addNodeType (std::string type);
		void addArcType  (std::string type);
		void addConstraint (std::string from_type, std::string arc_link, std::string to_type);
		
		// Getters //
		const std::set<std::string> & getNodeType () const;
		const std::set<std::string> & getArcType () const;
		const std::vector<std::string> & getFromType () const;
		const std::vector<std::string> & getToType () const;
		const std::vector<std::string> & getLinkType () const;
		
		// Checkers //
		bool isNodeType (std::string type);
		bool isArcType (std::string type);
		bool isValid (std::string from_type, std::string arc_link, std::string to_type);
		
		// Printers //
		void print ();
		void print (std::ofstream & outfile);
		
		// Readers //
		void read (std::string fname);
	};
	
	/*******************************************************************************
	 * Interface to the graph database (not used yet)
	 *******************************************************************************/
	class GraphDbInterface
	{
	public:
		GraphDbInterface () {};
		virtual ~GraphDbInterface () {};
		
		virtual int newNode (const std::string & type, const std::map<std::string, std::set<std::string> > & properties) = 0;
		virtual void newNodeWithId (const int & unique_id, const std::string & type, const std::map<std::string, std::set<std::string> > & properties) = 0;
		virtual void addArc  (const int & from_id, const std::string & type, const int & to_id, const std::map<std::string, std::set<std::string> >& properties) = 0;
		
		virtual Node * getNode (int node_id) = 0;
		virtual std::set<Node *> getNodesOfType (std::string type) = 0;
		
		virtual int nbNode() = 0;
		
		virtual void save (std::string fname) = 0;
		virtual void print () = 0;
	};

	
	/*******************************************************************************
	 * GraphDb Class
	 *
	 * _policy : A graphdb defines a policy to check type consistency
	 * _nodes  : A graphdb has a set of nodes
	 * _arcs   : A graphdb has a set of arcs
	 *
	 * For quick search, it also contains:
	 * _types : types to set of id
	 *******************************************************************************/
	class GraphDb : public GraphDbInterface
	{
	private:
		Policy _policy;
		
		std::map<int, Node> _nodes;
		std::map<std::string, Arc>  _arcs;
		
		std::map<std::string, std::set<int> > _node_types;
		std::map<std::string, std::map<std::string, std::set<int> > > _props;
		std::map<std::string, std::set<int> > _rev_props; // prop_value, set of nodes having the property
		
		// Private readers
		void readNode (std::string line);
		void readArc (std::string line);
		std::map<std::string, std::string> readProperties (std::string line);
		
	public:
		// Constructor & destructor //
		explicit GraphDb (Policy policy): _policy(policy) {};
		explicit GraphDb (std::string fname);
		~GraphDb () {};
		
		// Adders //
		int newNode (const std::string & type, const std::map<std::string, std::set<std::string> > & properties);
		void newNodeWithId (const int & unique_id, const std::string & type, const std::map<std::string, std::set<std::string> >& properties);
		void addArc  (const int & from_id, const std::string & type, const int & to_id, const std::map<std::string, std::set<std::string> >& properties);
		
		// Getters //
		std::set<Node *> allNodes ();
		Node * getNode (int node_id);
		std::set<Node *> getNodesOfType (std::string type);
		
		std::set<Node *> getNodesWithProperty (std::string prop_name);
		std::set<Node *> getNodesWithProperty (std::string prop_name, std::string prop_value);
		
		std::set<Node *> getNodesWithPropertyValue (std::string prop_value);
		
		std::set<Node *> getNodesOfTypeWithProperty (std::string type, std::string prop_name);
		std::set<Node *> getNodesOfTypeWithProperty (std::string type, std::string prop_name, std::string prop_value);
		
		void eraseNode (int node_id);
		
		const Policy & policy () const;
		int nbNode ();
		int nbArc ();
		
		// Printers //
		void save (std::string fname);
		void print();
	};

} // namespace tinygraphdb

#endif
