#include "schedim.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <queue>
#include <string>

using namespace schedim;

namespace {

class SchedImStringKey : public SchedImKey {
public:
  explicit SchedImStringKey(const std::string &s) : s_{s} {}

  bool operator==(const SchedImKey &other) const final { return true; }

  const std::string &string() const final { return s_; }

private:
  std::string s_;
};

class SchedImTaskBase : public SchedImTask {
public:
  explicit SchedImTaskBase(std::unique_ptr<SchedImKey> &&key)
      : key_{std::move(key)} {}

  const SchedImKey &key() const noexcept final { return *key_; }

private:
  std::unique_ptr<SchedImKey> key_;
};

class SchedImHolderBase : public SchedImHolder {
public:
  virtual std::ostream &os_info(std::ostream &) const = 0;

  friend std::ostream &operator<<(std::ostream &os,
                                  const SchedImHolderBase &holder) {
    return holder.os_info(os);
  }
};

class SchedImQueueHolder : public SchedImHolderBase {
public:
  void Push(SchedImTask &task) final { queue_.push(&task); }

  SchedImTask &Pop() final {
    SchedImTask *task = queue_.front();
    queue_.pop();
    return *task;
  }

  std::size_t Size() noexcept final { return queue_.size(); }

  bool Empty() const noexcept final { return queue_.empty(); }

  std::ostream &os_info(std::ostream &os) const final {
    std::queue<SchedImTask *> queue = queue_;

    os << "head[";
    while (!queue.empty()) {
      os << " " << queue.front()->key().string();
      queue.pop();
    }
    os << " ]";

    return os;
  }

private:
  std::queue<SchedImTask *> queue_;
};

class SchedImNSUpperBoundUnit : public SchedImNSUnit {
public:
  bool operator>=(const SchedImNSUnit &) const noexcept final { return true; }

  bool operator==(const SchedImNSUnit &other) const noexcept final {
    return this == &other;
  }
} static upperBoundUnit;

class SchedImNSPositiveUnit : public SchedImNSUnit {
public:
  explicit SchedImNSPositiveUnit(std::size_t value) : value_{value} {}

  bool operator>=(const SchedImNSUnit &other) const noexcept final {
    if (&upperBoundUnit == &other) {
      return false;
    } else {
      return value_ >= static_cast<const SchedImNSPositiveUnit &>(other).value_;
    }
  }

  bool operator==(const SchedImNSUnit &other) const noexcept final {
    if (&upperBoundUnit == &other) {
      return false;
    } else {
      return value_ == static_cast<const SchedImNSPositiveUnit &>(other).value_;
    }
  }

private:
  std::size_t value_;
};

class SchedImComputeBase : public SchedImCompute {
public:
  explicit SchedImComputeBase(const SchedImNSUnit &runtime_limit)
      : runtime_limit_{&runtime_limit}, task_{nullptr} {}

  void Attach(SchedImTask &task) final { task_ = &task; }

  SchedImTask &Detach() final {
    register SchedImTask *task = task_;
    task_ = nullptr;
    return *task;
  }

private:
  const SchedImNSUnit *runtime_limit_;
  SchedImTask *task_;
};

class SchedImSchedulerBase : public SchedImScheduler {
public:
  explicit SchedImSchedulerBase(
      std::unique_ptr<SchedImHolder> &&ready_holder,
      std::vector<std::unique_ptr<SchedImTask>> &&tasks)
      : ready_holder_{std::move(ready_holder)}, tasks_{std::move(tasks)} {}

  const SchedImHolder &ready_holder() const noexcept final {
    return *ready_holder_;
  }

  std::ostream &os_info(std::ostream &os) const final {
    os << "Tasks length: " << tasks_.size() << std::endl << "Tasks: ";
    std::transform(tasks_.cbegin(), tasks_.cend(),
                   std::ostream_iterator<std::string>(os, " "),
                   [](const std::unique_ptr<SchedImTask> &task) {
                     return task->key().string();
                   });
    os << std::endl
       << "Holder ready: "
       << static_cast<SchedImHolderBase &>(*ready_holder_.get()) << std::endl;
    return os;
  }

private:
  std::unique_ptr<SchedImHolder> ready_holder_;
  const std::vector<std::unique_ptr<SchedImTask>> tasks_;
};

} // namespace

std::unique_ptr<SchedImTask> SchedImTask::MakeNamed(const std::string &name) {
  return std::make_unique<SchedImTaskBase>(
      std::make_unique<SchedImStringKey>(name));
}

std::unique_ptr<SchedImHolder> SchedImHolder::MakeFIFO() {
  return std::make_unique<SchedImQueueHolder>();
}

std::unique_ptr<SchedImNSUnit> SchedImNSUnit::Make(std::size_t n) {
  return std::make_unique<SchedImNSPositiveUnit>(n);
}

const SchedImNSUnit &SchedImNSUnit::MAX() { return upperBoundUnit; }

std::unique_ptr<SchedImCompute> SchedImCompute::Make() {
  return std::make_unique<SchedImComputeBase>(SchedImNSUnit::MAX());
}

std::unique_ptr<SchedImCompute>
SchedImCompute::Make(const SchedImNSUnit &unit) {
  return std::make_unique<SchedImComputeBase>(unit);
}

SchedImSchedulerBuilder::SchedImSchedulerBuilder() {}

SchedImSchedulerBuilder::~SchedImSchedulerBuilder() {
  if (!tasks_.empty()) {
    std::cerr << "Not empty tasks" << std::endl;
  }
}

SchedImSchedulerBuilder &
SchedImSchedulerBuilder::AppendTask(std::unique_ptr<SchedImTask> &&task) {
  tasks_.push_back(std::move(task));
  return *this;
}

std::unique_ptr<SchedImScheduler> SchedImSchedulerBuilder::Build() {
  std::vector<std::unique_ptr<SchedImTask>> tasks;
  tasks.reserve(tasks_.size());
  std::transform(
      std::make_move_iterator(tasks_.begin()),
      std::make_move_iterator(tasks_.end()), std::back_inserter(tasks),
      [](std::unique_ptr<SchedImTask> task) { return std::move(task); });
  tasks_.clear();

  auto ready_holder = SchedImHolder::MakeFIFO();
  std::for_each(tasks.cbegin(), tasks.cend(),
                [&ready_holder](const std::unique_ptr<SchedImTask> &task) {
                  ready_holder->Push(*task);
                });

  return std::make_unique<SchedImSchedulerBase>(std::move(ready_holder),
                                                std::move(tasks));
}
