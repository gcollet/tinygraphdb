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
#include <vector>
#include <string>
#include <map>
#include <set>

namespace tinygraphdb
{
	class Arc;
	class Node;
	class GraphDb;

	/*
	 * Node Class
	 */
	class Node
	{
	private:
		int _index;
		std::string _type;
		std::map<std::string, std::string> _properties;
		std::set<const Arc *> _arc_in;
		std::set<const Arc *> _arc_out;
	public:
		explicit Node (const int & index, const std::string & type):_index(index), _type(type) {};
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
		int _index;
		std::string _type;
		Node * _from_node;
		Node * _to_node;
		std::map<std::string, std::string> _properties;

	public:
		explicit Arc (const int & index, const std::string & type, Node * from, Node * to):_index(index), _type(type), _from_node(from), _to_node(to) {};
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

		std::vector<std::string> _from_type;
		std::vector<std::string> _arc_link;
		std::vector<std::string> _to_type;

	public:
		void addNodeType (std::string type);
		void addArcType  (std::string type);

		bool isNodeType (std::string type);
		bool isArcType  (std::string type);
		
		void addConstraint (std::string from_type, std::string arc_link, std::string to_type);
		bool isValid       (std::string from_type, std::string arc_link, std::string to_type);
	};

	/*
	 * Graph Class
	 */
	class GraphDb
	{
	private:
		Policy _policy;
		std::vector<Node> _nodes;
		std::vector<Arc> _arcs;
		
	public:
		explicit GraphDb (Policy & policy):_policy(policy) {};
		~GraphDb () {};
	};

} // namespace tinygraphdb

#endif
