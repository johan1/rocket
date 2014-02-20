#ifndef _ROCKET_TREE_H_
#define _ROCKET_TREE_H_

#include "Assert.h"

#include <memory>
#include <stack>
#include <vector>

namespace rocket { namespace util {

typedef unsigned int TreeNodeId;

// TODO: Maybe we can add some threading policy,
// Traversal stack need to be synchronized if used by multiple threads...
template <typename T>
class Tree {
public:
	class Node {
	friend class Tree;
	public:
		// Add accessor to element. Ok since client does not have access to root node, and are unable to create free nodes...
		T& get() {
			return *element; 
		}

	private:
		Node() {} // Root node...
		Node(std::shared_ptr<Node> const& parent) : element(new T()), parent(parent) {}
		Node(std::shared_ptr<Node> const& parent, T const& element) : element(new T(element)), parent(parent) {}

		std::unique_ptr<T> element;
		std::vector<std::shared_ptr<Node>> children;
		std::shared_ptr<Node> parent;
	};

	Tree() : root(new Node()) {}

	std::shared_ptr<Node> add(T const& element) {
		return add(root, element);
	}

	std::shared_ptr<Node> add(std::shared_ptr<Node> const& parent, T const& element) {
		auto node = std::shared_ptr<Node>(new Node(parent, element));
		parent->children.push_back(node);
		node->parent = parent;

		return node;
	}

	void remove(std::shared_ptr<Node> const& node) {
		removeSubTree(node);

		auto& parent = node->parent;
		for (auto& child : node->children) {
			child->parent = parent;
		}
		parent->children.insert(node->children.begin(), node->children.end());
	}

	void removeSubTree(std::shared_ptr<Node> const& node) {
		auto &siblings = node->parent.children;
		siblings.erase(std::remove(siblings.begin(), siblings.end(), node), siblings.end());
	}

	template <typename Visitor>
	std::shared_ptr<Node> traverse(Visitor &&visitor) {
		return traverse(root, visitor);
	}

	template <typename Visitor>
	std::shared_ptr<Node> traverse(std::shared_ptr<Node> const& node, Visitor &&visitor) {
		std::shared_ptr<Node> resultNode;
		traverseStack.push_back(&node);
		
		while (!traverseStack.empty()) {
			auto n = traverseStack.back();
			traverseStack.pop_back();

			if (visitor(*n)) {
				resultNode = n;
				break;
			}

			for (auto c : n->children) {
				traverseStack.push_back(c);
			}
		}
		
		traverseStack.clear();
		return resultNode;
	}

private:
	std::deque<std::shared_ptr<Node>> traverseStack;

	std::shared_ptr<Node> root;
};

}

using namespace util;
}

#endif
