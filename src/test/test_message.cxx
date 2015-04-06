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
		"Null_t value", 
		[](TestCase& test)
		{
			Message m;
			std::string key = "key";
			m.set_null(key);
			std::string expected("{\"key\":null}\n");
			test.equals(m.to_string(), expected, "to_string()");
			test.equals(m.is_null(key), true, "is null?");

			Message::Null_t null_value;
			m.set(key, null_value);
			test.equals(m.is_null(key), true, "is null?");

			// 2DO: cannot get(k, Null_t)
		}
	);

	unit.test_case(
		"Object_t value", 
		[](TestCase& test)
		{
			Message m;
			std::string key = "key";
			m.set_object(key);
			std::string expected("{\"key\":{}}\n");
			test.equals(m.to_string(), expected, "to_string()");
			test.equals(m.is_object(key), true, "is object?");

			Message::Object_t object_value;
			m.set(key, object_value);
			test.equals(m.is_object(key), true, "is object?");
		}
	);

	unit.test_case(
		"Array_t_t value", 
		[](TestCase& test)
		{
			Message m;
			std::string key = "key";
			m.set_array(key);
			std::string expected("{\"key\":[]}\n");
			test.equals(m.to_string(), expected, "to_string()");
			test.equals(m.is_array(key), true, "is array?");

			Message::Array_t array_value;
			m.set(key, array_value);
			test.equals(m.is_array(key), true, "is array?");
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

	unit.test_case(
		"set() suite", 
		[](TestCase& test) {
			Message m;

			bool bool_v = true;
			int int_v = 123;
			std::string string_v(u8"重新发布 Dive Into Python"); 
			double double_v = 123.0102030405;
			long long long_long_v =  123456789123456789LL;
			m.set("bool", bool_v);
			m.set("int", 123);
			m.set("string", string_v);
			m.set("double", double_v);
			m.set("long long", long_long_v);

			test.equals(m.get("bool", false), bool_v, "bool");
			test.equals(m.get("int", 0), int_v, "int");
			test.equals(m.get("string", ""), string_v, "string");
			test.equals(m.get("double", 0.0), double_v, "double");
			test.equals(m.get("long long", 0LL), long_long_v, "long long");
		}
	);


	unit.test_case(
		"Parse json (ctor)", 
		[](TestCase& test)
		{
			std::string json = "{\"level1\":{\"level2\":{\"first\":\"value\"}}}\n";
			Message m(json);

			test.equals(m.to_string(), json, "parsed and to_string()");

			std::string retrieved = m.get("level1.level2.first", "");
			std::string expected = "value";
			test.equals(retrieved,
						expected,
						"get value from parsed json");
		}
	);

	unit.test_case(
		"Object", 
		[](TestCase& test)
		{
			Message m;
			m.set("level1.level2.first", "value");
			std::string expected("{\"level1\":{\"level2\":{\"first\":\"value\"}}}\n");
			test.equals(
				m.to_string(),
				expected,
				"dotted field created and set"
			);
			m.set("level1.level2.first", 12345);
			expected = ("{\"level1\":{\"level2\":{\"first\":12345}}}\n");
			test.equals(
				m.to_string(),
				expected,
				"dotted field overwritten"
			);

			m.set_null("level1.level2.second");
			expected = ("{\"level1\":{\"level2\":{\"first\":12345,\"second\":null}}}\n");
			test.equals(
				m.to_string(),
				expected,
				"set null in an exiting object"
			);

			m.set_null("level1.level2");
			expected = ("{\"level1\":{\"level2\":null}}\n");
			test.equals(
				m.to_string(),
				expected,
				"set null in an exiting object (delete object)"
			);

			m.clear();
			expected = ("{}\n");
			test.equals(m.to_string(), expected, "clear");
		}
	);

	unit.test_case(
		"Array", 
		[](TestCase& test)
		{
			Message m;
			m.set_array("array1");
			std::string expected("{\"array1\":[]}\n");
			test.equals(m.to_string(), expected, "set_array()");

			m.append("array1", 1);
			m.append("array1", "one");
			expected = "{\"array1\":[1,\"one\"]}\n";
			test.equals(m.to_string(), expected, "append()");
		}
	);

	return unit.run();
}
