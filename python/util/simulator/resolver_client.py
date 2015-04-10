import threading
import random

from util.nix_client import NixClient


class ResolverClient(object):
	_registered_nodes = {}
	_lock = threading.RLock()
	_module = "Resolver"

	def __init__(self, server_address, api_key):
		self._server_address = server_address
		self._api_key = api_key

	def run(self, event):
		client = NixClient(self._server_address, verbose=False)
		loop = 0
		while not event.is_set():
			for loop in range(0, 20):
				if loop == 20:
					self.remove_own_nodes(client)
					continue
				elif loop % 4 == 0:
					self.register_node(client, loop)
				self.resolve_random_node(client)
		print("Signaled")

	def register_node(self, client, loop):
		nodename = "node-%d-%d" % (loop, threading.current_thread().ident)
		address = threading.current_thread().ident
		params = {
			"@api_key" : self._api_key,
			"node" : nodename,
			"address" : address
		}
		response = client.call(self._module, "bind", params, 2000)
		if response.is_status_fail():
			# print(response.status_msg())
			pass
		self._lock.acquire()
		self._registered_nodes[nodename] = address
		self._lock.release()

	def resolve_random_node(self, client):
		self._lock.acquire()
		nodename = random.sample(self._registered_nodes.keys(), 1)[0]
		self._lock.release()
		address = self._registered_nodes.get(nodename)
		params = {
			"@api_key" : self._api_key,
			"node" : nodename,
		}
		response = client.call(self._module, "resolve", params, 2000)
		if response.is_status_fail() or response.data.get("address") != address:
			# print(response.status_msg())
			pass

	def remove_own_nodes(self, client):
		self._lock.acquire()
		nodes = filter(
			lambda k:
			k.endswith(
				str(threading.current_thread().ident),
				self._registered_nodes.keys()))
		for n in nodes:
			params = {
				"@api_key" : self._api_key,
				"nodename" : n,
			}
			client.send_one_way(self._module, "unbind", params)
			del self._registered_nodes[n]
		self._lock.release()
