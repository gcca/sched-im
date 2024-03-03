#include <gtest/gtest.h>

#include "schedim.hpp"

TEST(SchedImTaskBaseTest, InitialName) {
  auto task_a = schedim::SchedImTask::MakeNamed("task");
  auto task_b = schedim::SchedImTask::MakeNamed("task");
  EXPECT_EQ(task_a->key(), task_b->key());
}

TEST(SchedImQueueHolderTest, WalkThrough) {
  auto task_1 = schedim::SchedImTask::MakeNamed("task-1");
  auto task_2 = schedim::SchedImTask::MakeNamed("task-2");
  auto task_3 = schedim::SchedImTask::MakeNamed("task-3");

  auto queue = schedim::SchedImHolder::MakeFIFO();

  EXPECT_TRUE(queue->Empty());
  EXPECT_FALSE(queue->Size());

  queue->Push(*task_1);
  queue->Push(*task_2);
  queue->Push(*task_3);

  EXPECT_FALSE(queue->Empty());
  EXPECT_EQ(queue->Size(), 3);
}

TEST(SchedImNSUnitTest, CompareUnits) {
  auto unit_1 = schedim::SchedImNSUnit::Make(10);
  auto unit_2 = schedim::SchedImNSUnit::Make(15);
  auto unit_3 = schedim::SchedImNSUnit::Make(10);
  auto unit_4 = &schedim::SchedImNSUnit::MAX();

  EXPECT_LT(*unit_1, *unit_2);
  EXPECT_LE(*unit_1, *unit_2);

  EXPECT_GT(*unit_2, *unit_1);
  EXPECT_GE(*unit_2, *unit_1);

  EXPECT_NE(*unit_1, *unit_2);
  EXPECT_NE(*unit_2, *unit_1);

  EXPECT_EQ(*unit_1, *unit_3);
  EXPECT_EQ(*unit_3, *unit_1);

  EXPECT_GE(*unit_4, *unit_3);
  EXPECT_GT(*unit_4, *unit_3);

  EXPECT_LE(*unit_3, *unit_4);
  EXPECT_LT(*unit_3, *unit_4);

  EXPECT_NE(*unit_3, *unit_4);
  EXPECT_NE(*unit_4, *unit_3);
}

TEST(SchedImComputeTest, SwitchTask) {
  auto compute = schedim::SchedImCompute::Make();

  auto task = schedim::SchedImTask::MakeNamed("task");
  compute->Attach(*task);

  auto &detached_task = compute->Detach();

  EXPECT_EQ(task.get(), &detached_task);
}

TEST(SchedImSchedulerBuilderTest, AppendWays) {
  auto builder = schedim::SchedImSchedulerBuilder();

  auto task_1 = schedim::SchedImTask::MakeNamed("task-1");
  auto task_2 = schedim::SchedImTask::MakeNamed("task-2");
  auto task_3 = schedim::SchedImTask::MakeNamed("task-3");

  builder.AppendTask(std::move(task_1));
  builder.AppendTask(std::move(task_2));
  builder.AppendTask(std::move(task_3));

  auto scheduler = builder.Build();

  std::ostringstream oss;
  scheduler->os_info(oss);
  EXPECT_EQ(oss.str(), "Tasks length: 3\n"
                       "Tasks: task-1 task-2 task-3 \n"
                       "Holder ready: head[ task-1 task-2 task-3 ]\n");
}
