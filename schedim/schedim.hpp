#pragma once

#include <list>
#include <memory>

#define SCHEDIM_CTRAIT(T)                                                      \
public:                                                                        \
  virtual ~T() = default;                                                      \
                                                                               \
protected:                                                                     \
  T() = default;                                                               \
                                                                               \
private:                                                                       \
  T(const T &) = delete;                                                       \
  T(T &&) = default;                                                           \
  T &operator=(const T &) = delete;                                            \
  T &operator=(T &&) = default

namespace schedim {

class SchedImKey {
  SCHEDIM_CTRAIT(SchedImKey);

public:
  virtual bool operator==(const SchedImKey &key) const = 0;
  virtual const std::string &string() const = 0;
};

class SchedImTask {
  SCHEDIM_CTRAIT(SchedImTask);

public:
  virtual const SchedImKey &key() const noexcept = 0;

  static std::unique_ptr<SchedImTask> MakeNamed(const std::string &name);
};

class SchedImDeadlineTask : public SchedImTask {
public:
  std::size_t deadline() const noexcept;
};

class SchedImHolder {
  SCHEDIM_CTRAIT(SchedImHolder);

public:
  virtual void Push(SchedImTask &task) = 0;
  virtual SchedImTask &Pop() = 0;
  virtual std::size_t Size() noexcept = 0;
  virtual bool Empty() const noexcept = 0;

  static std::unique_ptr<SchedImHolder> MakeFIFO();
  static std::unique_ptr<SchedImHolder> MakeBlocked();
  static std::unique_ptr<SchedImHolder> MakeSink();
};

class SchedImNSUnit {
  SCHEDIM_CTRAIT(SchedImNSUnit);

public:
  virtual bool operator>=(const SchedImNSUnit &) const noexcept = 0;
  virtual bool operator==(const SchedImNSUnit &) const noexcept = 0;

  bool operator!=(const SchedImNSUnit &other) const noexcept {
    return not(*this == other);
  }

  bool operator>(const SchedImNSUnit &other) const noexcept {
    return (*this >= other) and (*this != other);
  }

  bool operator<(const SchedImNSUnit &other) const noexcept {
    return not(*this >= other);
  }

  bool operator<=(const SchedImNSUnit &other) const noexcept {
    return not(*this > other);
  }

  static std::unique_ptr<SchedImNSUnit> Make(std::size_t from_positive_integer);
  static const SchedImNSUnit &MAX();
};

class SchedImCompute {
  SCHEDIM_CTRAIT(SchedImCompute);

public:
  virtual void Attach(SchedImTask &task) = 0;
  virtual SchedImTask &Detach() = 0;

  const SchedImNSUnit &runtime_limit() const noexcept;

  static std::unique_ptr<SchedImCompute> Make();
  static std::unique_ptr<SchedImCompute>
  Make(const SchedImNSUnit &runtime_limit);
};

class SchedImScheduler {
  SCHEDIM_CTRAIT(SchedImScheduler);

public:
  virtual const SchedImHolder &ready_holder() const noexcept = 0;
  virtual std::ostream &os_info(std::ostream &) const = 0;
};

class SchedImSchedulerBuilder {
public:
  explicit SchedImSchedulerBuilder();
  ~SchedImSchedulerBuilder();

  SchedImSchedulerBuilder &AppendTask(std::unique_ptr<SchedImTask> &&task);

  std::unique_ptr<SchedImScheduler> Build();

  SchedImSchedulerBuilder(const SchedImSchedulerBuilder &) = delete;
  SchedImSchedulerBuilder &operator=(const SchedImSchedulerBuilder &) = delete;

private:
  std::list<std::unique_ptr<SchedImTask>> tasks_;
};

} // namespace schedim
