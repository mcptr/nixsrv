#include "base.hxx"
#include <nix/message.hxx>
#include <string>
#include <iostream>

int main()
{
	using nix::Message;
	using std::string;

	UnitTest unit;

	unit.test_case(
		"Basic", 
		[](TestCase& test)
		{
			Message m;
			std::string expected("{}\n");
			test.equals(m.to_string(), expected, "is empty");
			
			m.set("string", "value");
			m.set("int", 123);
			expected = "{\"int\":123,\"string\":\"value\"}\n";
			test.equals(m.to_string(), expected, "to_string()");
		}
	);

	unit.test_case(
		"Copy message", 
		[](TestCase& test)
		{
			Message m1;
			m1.set("string", "value");
			Message m2 = m1;
			
			std::string excected = "{\"string\":\"value\"}\n";
			test.equals(m2.to_string(), excected, "copied message to_string()");
		}
	);


	return unit.run();
}
