import threading
import random
import time

from util.nix_client import NixClient


class CacheClient(object):
	def __init__(self, server_address, api_key):
		self._server_address = server_address
		self._api_key = api_key

	def run(self, event):
		client = NixClient(self._server_address, verbose=False)
		cache_key = "cache-key-%d-" % threading.current_thread().ident
		cache_value = "Random value %d: " % threading.current_thread().ident
		while not event.is_set():
			time.sleep(random.randint(1, 2) / 100.0)
			# print("this is", threading.current_thread().ident)

			params = {
				"@api_key" : self._api_key,
				"key" : "test",  # cache_key,
				"value" : "test",  # cache_value,
			}
			response = client.call("Cache", "store", params, 2000)
			if response.is_status_fail():
				print("%s %d" % ("Store failed", threading.current_thread().ident))
			response = client.call("Cache", "retrieve", params, 5000)
			if response.is_status_fail():
				print("Retrieve failed", response.data)
			print(response.data)
			response.data.get(cache_key) == cache_value
			# client.send_one_way("Cache", "remove", params)
		print("CacheClient finished")
