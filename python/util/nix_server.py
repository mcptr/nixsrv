import os
import sys
import signal
import random
import time
import errno
from subprocess import Popen, PIPE


class NixServer(object):
	def __init__(self, address = None):
		self.__server_process = None
		self.project_root = os.getenv("PROJECT_ROOT")
		self.__address = None
		if not self.__address:
			port = random.randint(65500, 65535)
			self.__address = "tcp://127.0.0.1:%d" % port
		self.__pidfile = "/tmp/nix.test-%d.pid" % os.getpid()

	def get_address(self):
		return self.__address

	def __enter__(self):
		executable = os.path.join(self.project_root, "bin", "NIX")
		cmd = [
			executable, "-F", "-D", "-v",
			"--enable-resolver", "-A", self.__address,
			"--pidfile", self.__pidfile,
			"--no-close-fds",
			"--development-mode"
		]
		self.__server_process = Popen(cmd, stdout=PIPE, shell=False)
		print("")
		wait = 3
		while not os.path.isfile(self.__pidfile):
			time.sleep(1)
			wait -= 1

		if not os.path.isfile(self.__pidfile):
			raise Exception("Server startup failed")

		wait = 10
		try:
			with open(self.__pidfile, "r") as fh:
				pid = int(fh.read())
				while wait and (os.kill(pid, 0) is not None):
					time.sleep(1)
					wait -= 1
				if os.kill(pid, 0) is not None:
					raise Exception("Server startup failed")
		except Exception as e:
			print(e)
			raise
		print("### NixServer: Server ready")
		return self

	def __exit__(self, tp, value, tb):
		with open(self.__pidfile, "r") as fh:
			pid = int(fh.read())
			wait = 500
			try:
				os.kill(pid, signal.SIGTERM)
				is_alive = True
				while wait and is_alive:
					try:
						is_alive = (os.kill(pid, 0) is None)
						if is_alive:
							time.sleep(0.01)
							print("### Waiting for server to stop (%d)" % pid)
							wait -= 1
						else:
							break
					except OSError as e:
						if e.errno == errno.ESRCH:
							print("### NixServer: Server stopped")
							break

				if os.kill(pid, 0) is None:
					print("Killing with SIGKILL")
					os.kill(pid, signal.SIGKILL)
			except OSError as e:
				if e.errno != errno.ESRCH:
					raise
