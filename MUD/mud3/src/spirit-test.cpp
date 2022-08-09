#include <iostream>
#include <string>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

int main(int argc, char **argv) {
	namespace qi = boost::spirit::qi;

	int quantity = 0;
	std::string item = "";
	int itemNumber = 0;
	std::string container = "";
	int containerNumber = 0;

	std::string str("sword from backpack");
	std::string::iterator first = str.begin();

	bool res = false;

	// 'get item' rule
	res = qi::phrase_parse(first, str.end(),
				(
					qi::lexeme[+qi::alpha]
				),
				' ',
				item
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get item' grammar" << std::endl;
		std::cout << "Item: " << item << std::endl;
		return 0;
	}

	first = str.begin();

	// 'get 20 items' rule
	res = qi::phrase_parse(first, str.end(),
					(
						qi::int_
						>> qi::lexeme[+qi::alpha]
					),
					' ',
					quantity,
					item
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get 20 items' rule" << std::endl;
		return 0;
	}

	first = str.begin();

	// 'get item 2' rule
	res = qi::phrase_parse(first, str.end(),
					(
						qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ',
					item,
					itemNumber
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get item 2' rule" << std::endl;
		return 0;
	}

	first = str.begin();

	// 'get 20 item 2' rule
	res = qi::phrase_parse(first, str.end(),
					(
						qi::int_
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ',
					quantity,
					item,
					itemNumber
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get 20 item 2' rule" << std::endl;
		return 0;
	}

	first = str.begin();

	// 'get item from container' rule
	res = qi::phrase_parse(first, str.end(),
						// begin grammar
						(
							qi::lexeme[+qi::alpha]
							>> "from"
							>> qi::lexeme[+qi::alpha]
						),
						' ',
						item,
						container
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get item from container' rule" << std::endl;
		return 0;
	}

	first = str.begin();

	// 'get 20 items from container' rule
	res = qi::phrase_parse(first, str.end(),
					(
						qi::int_
						>> qi::lexeme[+qi::alpha]
						>> "from"
						>> qi::lexeme[+qi::alpha]
					),
					' ',
					quantity,
					item,
					container
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get 20 items from container' rule" << std::endl;
		return 0;
	}

	first = str.begin();

	// 'get item from container 2' rule
	res = qi::phrase_parse(first, str.end(),
					(
						qi::lexeme[+qi::alpha]
						>> "from"
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ',
					item,
					container,
					containerNumber
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get item from container 2' rule" << std::endl;
		return 0;
	}

	first = str.begin();

	// 'get 20 items from container 2' rule
	res = qi::phrase_parse(first, str.end(),
					(
						qi::int_
						>> qi::lexeme[+qi::alpha]
						>> "from"
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ',
					quantity,
					item,
					container,
					containerNumber
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get 20 items from container 2' rule" << std::endl;
		return 0;
	}

	first = str.begin();

	// 'get item 2 from container 2' rule
	res = qi::phrase_parse(first, str.end(),
					(
						qi::lexeme[+qi::alpha]
						>> qi::int_
						>> "from"
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ',
					item,
					itemNumber,
					container,
					containerNumber
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get item 2 from container 2' rule" << std::endl;
		return 0;
	}

	first = str.begin();

	// 'get 20 items 2 from container 3' rule
	res = qi::phrase_parse(first, str.end(),
					(
						qi::int_
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
						>> "from"
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ',
					quantity,
					item,
					itemNumber,
					container,
					containerNumber
	);

	if(res && first == str.end()) {
		std::cout << "Matched 'get 20 items 2 from container 3' rule" << std::endl;
		return 0;
	}

	std::cout << "Didn't match any rules." << std::endl;
	return 0;
}
