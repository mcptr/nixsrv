#!/usr/bin/env python3.4

import sys
# import argparse
import random
from lib import yami
import threading
import concurrent.futures
from concurrent.futures import ThreadPoolExecutor
import json

server_address = "tcp://127.0.0.1:9876"
print_lock = threading.Lock()


def call(agent, mod, route, params):
	with client_agent.send(
			server_address,
			mod, route,
			{"message" : json.dumps(params)}
	) as msg:
		wait_time = 1000
		if "interval_ms" in params:
			wait_time += params.get("interval_ms")
		msg.wait_for_completion(wait_time)
		state = msg.get_state()
		if state[0] == yami.OutgoingMessage.REPLIED:
			# print_lock.acquire()
			# print(
			# 	"RAW (%d): " % (params["msg_id"]),
			# 	threading.current_thread(), msg.get_reply()
			# )
			# # response = (json.loads(msg.get_reply()["message"]))
			# # if "@error_code" in response:
			# # 	print("Error: %d, %s" % (response["@error_code"], response["@error"]))
			# # else:
			# # 	print(json.dumps(response, indent=4))
			# print_lock.release()
			pass
		elif state[0] == yami.OutgoingMessage.REJECTED:
			print("REJECTED", msg.get_exception_msg())
		else:
			print("The message has been abandoned.")
		pass

futures = []

with yami.Agent({"tcp_nonblocking" : 1}) as client_agent:
	with ThreadPoolExecutor(max_workers=100) as executor:
		for x in range(0, 500):
			futures.append(executor.submit(
				call, client_agent,
				"Debug", "debug_async", {
					# "interval_ms" : random.randint(1, 3) * 100,
					"msg_id" : x
				}
			))
		for future in concurrent.futures.as_completed(futures):
			future.result()
