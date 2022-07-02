#include <unordered_map>
#include <rpc>
#include <mutex>

static std::unordered_map<uint64_t, std::PID> names;
static std::mutex lock;

bool publish(std::PID client, uint64_t name) {
	lock.acquire();

	if(names[name]) {
		lock.release();
		return false;
	}

	names[name] = client;
	lock.release();
	return true;
}

std::PID resolve(std::PID client, uint64_t name) {
	IGNORE(client);
	lock.acquire();

	if(!names[name]) {
		lock.release();
		return 0;
	}

	auto ret = names[name];
	lock.release();
	return ret;
}

extern "C" void _start() {
	std::exportProcedure((void*)publish, 1);
	std::exportProcedure((void*)resolve, 1);
	std::enableRPC();
	std::halt();
}
