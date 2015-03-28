import sys
import argparse
from lib import yami
import threading
import concurrent.futures
import json

default_requests = 1
default_concurrency = 1
default_server = "tcp://127.0.0.1:9876"

tsprint_lock = threading.Lock()

def tsprint(msg):
	with tsprint_lock:
		print(msg)

def call(agent, **kwargs):
	server = kwargs.pop("server")
	mod = kwargs.pop("module")
	route = kwargs.pop("route")
	data = kwargs.pop("data", {})
	verbose = kwargs.pop("verbose", False)

	with agent.send(server, mod, route, data) as msg:
		msg.wait_for_completion(600)
		state = msg.get_state()
		if state[0] == yami.OutgoingMessage.REPLIED:
			reply = msg.get_reply()
			response_data =  reply["response"] if "response" in reply else str({})
			if verbose:
				tsprint("# Finished (%d / %s)" % (reply["response_id"], threading.current_thread().getName()))
			if verbose:
				response = json.loads(response_data)
				return(json.dumps(response, indent=4))
		elif state[0] == yami.OutgoingMessage.REJECTED:
			tsprint("The message has been rejected:",
					msg.get_exception_msg())
		else:
			tsprint("The message has been abandoned.")

def run(requests, pool_size=1, **kwargs):
	print("Creating yami agent")
	with yami.Agent() as agent:
		if pool_size > requests:
			print("Lowering pool size to number of requests (%d)" % requests)
			pool_size = requests
		print("Executing %d request(s) with %d worker(s)" % (requests, pool_size))
		with concurrent.futures.ThreadPoolExecutor(max_workers=pool_size) as executor:
			futures = {executor.submit(call, agent, **kwargs): x for x in range(0, requests)}
			for future in concurrent.futures.as_completed(futures):
				if kwargs.get("verbose"):
					print(future.result())

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Call remote procedure")
	parser.add_argument("-c", "--concurrency", metavar="C",
						type=int, action="store",  default=default_concurrency,
						help="concurrency (default: %d)" % default_concurrency)

	parser.add_argument("-f", "--input",
						type=argparse.FileType('r'), action="store", nargs="?",
						help="remote module route")

	parser.add_argument("-m", "--module",
						type=str, action="store", required=True,
						help="remote module")

	parser.add_argument("-n", "--requests", metavar="N",
						type=int, action="store", default=default_requests,
						help="number of requests (default %d)" % default_requests)

	parser.add_argument("-r", "--route",
						type=str, action="store", required=True,
						help="remote module route")

	parser.add_argument("-s", "--server",
						type=str, action="store", default=default_server,
						help="server address (default: %s)" % default_server)

	parser.add_argument("-v", "--verbose", action="store_true", default=False,
						help="verbose")


	args = parser.parse_args()

	data = {}

	if args.input:
		data = json.loads((args.input.read() or str(data)))

	call_params = {
		"server" : args.server,
		"module" : args.module,
		"route" : args.route,
		"data" : data,
		"verbose" : args.verbose
	}

	run(args.requests, args.concurrency, **call_params)
