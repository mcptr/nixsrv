import sys
# import argparse
from lib import yami
import threading
from concurrent.futures import ThreadPoolExecutor
import json

server_address = "tcp://127.0.0.1:9876"


def call(agent, mod, route, params):
	with client_agent.send(server_address, mod, route, params) as msg:
		msg.wait_for_completion(4)
		state = msg.get_state()
		if state[0] == yami.OutgoingMessage.REPLIED:
			print("RAW:", threading.current_thread(), msg.get_reply())
			# response = (json.loads(msg.get_reply()["response"]))
			# return json.dumps(response, indent=4)
		elif state[0] == yami.OutgoingMessage.REJECTED:
			print(
				"The message has been rejected:",
				msg.get_exception_msg()
			)
		else:
			print("The message has been abandoned.")


with yami.Agent({"tcp_nonblocking" : 1}) as client_agent:
	with ThreadPoolExecutor(max_workers=6) as executor:
		for x in range(0, 1):
			future = executor.submit(
				call, client_agent,
				"stats", "get", {
					"reset" : False,
					"messages_sent" : 666,
					"messages_received" : 666,
					"sessages_received" : 666,
				}
			)
			future.result()
