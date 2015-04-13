import sys
import os
import signal
import random
import time
import errno
from subprocess import Popen, PIPE


class NixServer(object):
	def __init__(self, **kwargs):
		self.__server_process = None
		self.__project_root = os.getenv("PROJECT_ROOT")
		self.__address = kwargs.pop("address", None)
		self.__builtin_modules = kwargs.pop("modules", [])
		port = random.randint(65500, 65535)
		if not self.__address:
			self.__address = "tcp://127.0.0.1:%d" % port
		self.__pidfile = "/tmp/nix.test-%d.pid" % os.getpid()
		self.__nodename = "unittest-node-01-%d" % port

	def get_address(self):
		return self.__address

	def get_nodename(self):
		return self.__nodename

	def __enter__(self):
		executable = os.path.join(self.__project_root, "bin", "NIX")
		cmd = [
			executable, "-F", "-D", "-v",
			"--nodename", self.__nodename,
			"-c", self.__project_root + "/etc/devel.ini",
			"-A", self.__address,
			"--pidfile", self.__pidfile,
			"--no-close-fds",
			"--development-mode"
		]

		for module in self.__builtin_modules:
			cmd.append("--enable-" + module.lower())
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
		print("### NixServer: Server ready")
		return self

	def __exit__(self, tp, value, tb):
		with open(self.__pidfile, "r") as fh:
			pid = int(fh.read())
			wait = 500
			try:
				print("### Stopping server", pid)
				os.kill(pid, signal.SIGTERM)
				is_alive = True
				while wait and is_alive:
					try:
						is_alive = (os.kill(pid, 0) is None)
						if is_alive:
							time.sleep(0.02)
							wait -= 1
						else:
							break
					except OSError as e:
						if e.errno == errno.ESRCH:
							break
				if os.kill(pid, 0) is None:
					print("Killing with SIGKILL")
					os.kill(pid, signal.SIGKILL)
				print("### NixServer: Server stopped")
			except OSError as e:
				if e.errno != errno.ESRCH:
					raise
