//
//  graph.h
//  tinyGraphDb
//
//  Created by Guillaume Collet on 12/12/12.
//

#ifndef __tinyGraphDb__graph__
#define __tinyGraphDb__graph__

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace tinygraphdb
{
	class Arc;
	class Node;
	class Graph;

	/*
	 * Node Class
	 */
	class Node
	{
	private:
		std::string _type;
		std::map<std::string, std::string> _properties;
		std::set<const Arc *> _arc_in;
		std::set<const Arc *> _arc_out;
	public:
		Node () {};
		Node (const std::string & type):_type(type) {};
		~Node () {};

		void addProperty (const std::string & property, const std::string & value);
		void addArcIn (const Arc * in);
		void addArcOut (const Arc * out);

		const std::string property (const std::string & property);
		const std::map<std::string, std::string> & properties () const {return _properties;};
		const std::set<const Arc *> & getArcIn () const {return _arc_in;};
		const std::set<const Arc *> & getArcOut () const {return _arc_out;};
		const std::string & type () const {return _type;};
	};

	/*
	 * Arc Class
	 */
	class Arc
	{
	private:
		std::string _type;
		Node * _from_node;
		Node * _to_node;
		std::map<std::string, std::string> _properties;

	public:
		explicit Arc () {};
		explicit Arc (const std::string & type, Node * from, Node * to):_type(type), _from_node(from), _to_node(to) {};
		~Arc () {};

		void addProperty (const std::string & property, const std::string & value);
		const std::string property (const std::string & property);
		const std::map<std::string, std::string> & properties () const {return _properties;};
		const std::string & type () const {return _type;};
		const Node * from () const {return _from_node;};
		const Node * to () const {return _to_node;};
	};

	/*
	 * Policy class
	 */
	class Policy
	{
	private:
		std::set<std::string> _node_type;
		std::set<std::string> _arc_type;

		std::vector<std::string> _from;
		std::vector<std::string> _to;

	public:
		void addNodeType (std::string type);
		void addArcType  (std::string type);

		bool isNodeType (std::string type);
		bool isArcType  (std::string type);
		
		void addConstraint (std::string from, std::string to);
		bool isValid (std::string from, std::string to);
	};

	/*
	 * Graph Class
	 */
	class Graph
	{
	private:
		Policy _policy;
		std::vector<Node> _nodes;
		std::vector<Arc> _arcs;
		
	public:
		Graph (Policy & policy):_policy(policy) {};
		~Graph () {};
	};

} // namespace tinygraphdb

#endif
