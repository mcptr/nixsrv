#!../bin/python3.4 -tt

import unittest
from util.nix_server import NixServer
from util.nix_client import NixClient


class TestResolver(unittest.TestCase):
	def setUp(self):
		self.server = NixServer()
		self.server.start()
		self.server_address = self.server.get_address()

	def tearDown(self):
		self.server.stop()

	def test_resolver(self):
		client = NixClient(self.server_address)
		result = client.call("Resolver", "list_routes", {}, 2000)
		print(result.state)
		self.assertFalse(result.is_rejected(), "not rejected")
		self.assertFalse(result.is_abandoned(), "not abandoned")
		self.assertTrue(result.is_replied(), "is replied")


if __name__ == "__main__":
	runner = unittest.TextTestRunner(verbosity=9)
	unittest.main(verbosity=9)
