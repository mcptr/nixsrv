#include <yami4-cpp/yami.h>

#include <cstdlib>
#include <iostream>

#include <ctime>

void handle_message(yami::outgoing_message & om)
{
    const yami::message_state state = om.get_state();
    if (state == yami::transmitted) {
		std::cout << "TRANSMITTED" << std::endl;
    }
    else if (state == yami::replied) {
		const yami::parameters & reply =
			om.get_reply();
		std::cout << "REPLIED" << std::endl;
	}
	else if (state == yami::rejected) {
		std::cout << "The message has been rejected: "
				  << om.get_exception_msg()
				  << std::endl;
	}
	else {
		std::cout << "The message has been abandoned."
				  << std::endl;;
	}

}


int main(int argc, char * argv[])
{
    const std::string server_address = "tcp://127.0.0.1:9876";

	yami::agent client_agent;

	yami::parameters params;
	for(int i = 0; i < 1; i++) {
		std::unique_ptr<yami::outgoing_message> om(
			client_agent.send(server_address, "Debug", "debug_async", params)
		);
		
		om->wait_for_completion();
		handle_message(*om);
	}

	return 0;
}
