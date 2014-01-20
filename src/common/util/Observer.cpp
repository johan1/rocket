#include "Observer.h"

#ifdef _TEST_OBSERVER_

#include <iostream>

namespace rocket { namespace util {

namespace {
	class ObservableString : public Observable<ObservableString> {
	public:
		void setString(std::string const& str) {
			this->str = str;
			notify();
		}
		
		std::string const& getString() const { return str; }

	private:
		std::string str;
	};

	void test() {
		ObservableString obsStr;
		Observer<ObservableString> observer([](ObservableString const& obsStr) {
			std::cout << "Hello " << obsStr.getString() << std::endl;
		});
		obsStr.addObserver(&observer); 

		obsStr.setString("Apa");

		obsStr.removeObserver(&observer);

		obsStr.setString("Apa2");
	}
}

}}

int main() {
	rocket::util::test();

	return 0;
}

#endif
