from util.nix_server import NixServer
from util.nix_client import NixClient

from util.simulator.resolver_client import ResolverClient
from util.simulator.cache_client import CacheClient

import json
import time
import threading
import signal
# import logging

DEVELOPMENT_KEY = "_development_key_"
SERVER_MODULES = ["debug", "cache", "resolver", "job-queue"]
SERVER_MODULES_NAMES = ["Debug", "Cache", "Resolver", "JobQueue", "Status"]

STATUS_PARAMS = {
	"@api_key" : DEVELOPMENT_KEY,
	"server" : True,
	"stats" : True,
	"module_stats" : True,
	"routing" : True,
}





if __name__ == "__main__":
	event = threading.Event()
	signal.signal(signal.SIGINT, lambda: event.set())
	signal.signal(signal.SIGTERM, lambda: event.set())

	threads = []
	with NixServer(modules=SERVER_MODULES) as server:
		for i in range(0, 50):
			for cls in [ResolverClient, CacheClient]:
				worker = cls(server.get_address(), DEVELOPMENT_KEY)
				t = threading.Thread(target=worker.run, args=(event,))
				t.start()
				threads.append(t)

		try:
			client = NixClient(server.get_address())
			while True:
				params = {
					"@api_key" : DEVELOPMENT_KEY,
					"stats" : True,
					"module_stats" : True,
				}
				response = client.call("Status", "status", params)
				print(json.dumps(response.data, indent=4, sort_keys=True))
				time.sleep(1)
		except Exception as e:
			event.set()
			for t in threads:
				print("join")
				t.join()
