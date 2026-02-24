#include "black_board.h"

namespace bbd {
// BlackBoard构造函数实现
BlackBoard::BlackBoard() : thread_pool_(std::thread::hardware_concurrency()) {}
BlackBoard::BlackBoard(size_t thread_num) : thread_pool_(thread_num) {}

Handler BlackBoard::Register(const std::string& name, std::shared_ptr<FunctionBase> func_ptr) {
	uint64_t id = reinterpret_cast<uint64_t>(func_ptr.get());
	{
		std::lock_guard<std::mutex> lk(rpc_mutex_);
		rpc_func_[name] = func_ptr;
	}
	return {name, id};
}

bool BlackBoard::UnSub(Handler handler) {
	std::lock_guard<std::mutex> lk(datas_mutex_);
	if (datas_.count(handler.name)) {
		datas_[handler.name].UnSub(handler.id);
		return true;
	}
	return false;
}

bool BlackBoard::UnRegister(Handler handler) {
	std::lock_guard<std::mutex> lk(rpc_mutex_);
	auto it = rpc_func_.find(handler.name);
	if (it != rpc_func_.end() && reinterpret_cast<uint64_t>(it->second.get()) == handler.id) {
		rpc_func_.erase(it);
		return true;
	}
	return false;
}

std::shared_ptr<BlackBoard> BlackBoard::Create(const std::string& name, size_t thread_num) {
	std::lock_guard<std::mutex> lk(boards_mutex_);
	auto board = std::make_shared<BlackBoard>(thread_num);
	boards_[name] = board;
	return board;
}

void BlackBoard::Clear() {
	std::lock_guard<std::mutex> lk1(datas_mutex_);
	std::lock_guard<std::mutex> lk2(rpc_mutex_);
	datas_.clear();
	rpc_func_.clear();
}

// 其他成员函数如有需要可继续补充

} // namespace bbd
