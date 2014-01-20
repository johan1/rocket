#include "Tree.h"

#ifdef _ROCKET_TREE_TEST_

#include <string>
#include <iostream>

namespace rocket { namespace util {
namespace {
	void test() {
		Tree<std::string> tree;
		Tree<std::string>::Node n1("Hello");
		Tree<std::string>::Node n2(" child");
		Tree<std::string>::Node n3(" child");

		tree.add(n1);
		n1.add(n2);
		n2.add(n3);

//		tree.add(n2, n1);
//		tree.add(n3, n2);

/*
		auto lambda = [](Tree<std::string>::Node &n) -> bool {
			std::cout << n.element << std::endl;
			return false;
		};
*/
//		n2.traverse() {}

		tree.traverse([](Tree<std::string>::Node &n) -> bool {
			std::cout << n.get() << std::endl;
			return false;
		});

		n2.remove();
//		tree.traverse(lambda);
	}
}

/*
	Renderable {
		virtual void render(Canvas &canvas) = 0;
	};

	Node<Renderable>
	RenderNode

	transform.set(T) { // setPosition, setRotation, setScale
		M = T;

		VM = I, V = M^(-1); ??

		M = M_p * T
		for (child : children) {

		}
	}

	transform.apply(T) { // e.g. move, rotate, scale
		M = M * T;
	}

*/

}}

#endif
