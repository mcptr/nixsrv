# import sys
import json
import nix.lib.yami as yami


class Result(object):
	def __init__(self):
		self.state = None
		self.message = None

	def is_replied(self):
		return (self.state[0] == yami.OutgoingMessage.REPLIED)

	def is_rejected(self):
		return (self.state[0] == yami.OutgoingMessage.REJECTED)

	def is_abandoned(self):
		return (self.state[0] == yami.OutgoingMessage.ABANDONED)


class NixClient(object):
	def __init__(self, address = None):
		self.__address = address
		self.__agent = yami.Agent({"tcp_nonblocking" : 1})

	def call(self, mod, route, params = None, timeout_ms = 3000):
		params = (params or {})
		with self.__agent.send(
				self.__address,
				mod, route,
				{"message" : json.dumps(params)}) as msg:
			msg.wait_for_completion(timeout_ms)
			result = Result()
			result.state = msg.get_state()

			if result.state[0] == yami.OutgoingMessage.REPLIED:
				result["data"] = json.loads(msg.get_reply()["message"])
			return result
