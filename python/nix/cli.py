#!/usr/bin/python3

import sys
# import argparse
from lib import yami
import threading
from concurrent.futures import ThreadPoolExecutor
import json

server_address = "tcp://127.0.0.1:9876"


def call(agent, mod, route, params):
	print(">> Calling %s/%s::%s" % (server_address, mod, route))
	with client_agent.send(
			server_address,
			mod, route,
			{"message" : json.dumps(params)}
	) as msg:
		msg.wait_for_completion(5000)
		state = msg.get_state()
		if state[0] == yami.OutgoingMessage.REPLIED:
			print("RAW:", threading.current_thread(), msg.get_reply())
			response = (json.loads(msg.get_reply()["message"]))
			if "@error_code" in response:
				print("Error: %d, %s" % (response["@error_code"], response["@error"]))
			else:
				print(json.dumps(response, indent=4))
			pass
		elif state[0] == yami.OutgoingMessage.REJECTED:
			print(
				"The message has been rejected:",
				msg.get_exception_msg()
			)
		else:
			print("The message has been abandoned.")


params = {
	"@id" : "Job",
	"@version" : 1,
	"@error_code" : 123,
	"@error" : "some error msg string",
	"@api_key" : "key_public",
	"content" : json.dumps({"value" : "zażółć gęślą jaźń"})
}

with yami.Agent() as client_agent:
	with ThreadPoolExecutor(max_workers=10) as executor:
		for x in range(0, 1):
			future = executor.submit(
				call, client_agent,
				"Debug", "debug_async", {"interval_ms" : 3456}
			)
			future.result()
