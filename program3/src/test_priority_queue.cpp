#include <iostream>
#include <sstream>
#include <algorithm>                 // std::random_shuffle
#include "ics46goody.hpp"
#include "gtest/gtest.h"
#include "array_priority_queue.hpp"  // must leave in, even if testing another PQ type
#include "heap_priority_queue.hpp"

typedef ics::HeapPriorityQueue<std::string> PriorityQueueType;
typedef ics::HeapPriorityQueue<int>         PriorityQueueType2;

int test_size  = ics::prompt_int ("Enter large scale test size");
int trace      = ics::prompt_bool("Trace large scale test",false);
int speed_size = ics::prompt_int ("Enter large scale speed test size");


bool lt_string (const std::string& a, const std::string& b) {return a > b;}
bool gt_string (const std::string& a, const std::string& b) {return a < b;}
bool gt_int    (const int& a, const int& b) {return a < b;}

class PriorityQueueTest : public ::testing::Test {
protected:
    virtual void SetUp()    {}
    virtual void TearDown() {}
};


void load(PriorityQueueType& q, std::string values) {
  std::string* v = new std::string[values.size()];
  for (unsigned i=0; i<values.size(); ++i)
    v[i] = values[i];
  for (unsigned i=0; i<values.size(); ++i)
    q.enqueue(v[i]);
  delete[] v;
}


::testing::AssertionResult unload(PriorityQueueType& q, std::string values) {
  std::string* v = new std::string[values.size()];
  for (unsigned i=0; i<values.size(); ++i)
    v[i] = values[i];
  for (unsigned i=0; i<values.size(); ++i)
    if (v[i] != q.dequeue())
      return ::testing::AssertionFailure();
  delete[] v;
  return ::testing::AssertionSuccess();
}



TEST_F(PriorityQueueTest, empty) {
  PriorityQueueType q(gt_string);
  ASSERT_TRUE(q.empty());
}


TEST_F(PriorityQueueTest, size) {
  PriorityQueueType q(gt_string);
  ASSERT_EQ(0,q.size());
}


TEST_F(PriorityQueueTest, peek) {
  PriorityQueueType q(gt_string);
  ASSERT_THROW(q.peek(),ics::EmptyError);
}

//order: fcijbdegabh
TEST_F(PriorityQueueTest, enqueue) {
  PriorityQueueType q(gt_string);
  ASSERT_EQ(1,q.enqueue("f"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(1, q.size());
  ASSERT_EQ("f",q.peek());

  ASSERT_EQ(1,q.enqueue("c"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(2, q.size());
  ASSERT_EQ("c",q.peek());

  ASSERT_EQ(1,q.enqueue("i"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(3, q.size());
  ASSERT_EQ("c",q.peek());

  ASSERT_EQ(1,q.enqueue("j"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(4, q.size());
  ASSERT_EQ("c",q.peek());

  ASSERT_EQ(1,q.enqueue("b"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(5, q.size());
  ASSERT_EQ("b",q.peek());

  ASSERT_EQ(1,q.enqueue("d"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(6, q.size());
  ASSERT_EQ("b",q.peek());

  ASSERT_EQ(1,q.enqueue("e"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(7, q.size());
  ASSERT_EQ("b",q.peek());

  ASSERT_EQ(1,q.enqueue("g"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(8, q.size());
  ASSERT_EQ("b",q.peek());

  ASSERT_EQ(1,q.enqueue("a"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(9, q.size());
  ASSERT_EQ("a",q.peek());

  ASSERT_EQ(1,q.enqueue("h"));
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(10, q.size());
  ASSERT_EQ("a",q.peek());
}


TEST_F(PriorityQueueTest, operator_eq_ne) {// == and !=
  PriorityQueueType q1(gt_string),q2(gt_string);
  ASSERT_EQ(q1,q2);

  q1.enqueue("a");
  ASSERT_NE(q1,q2);

  q1.enqueue("b");
  ASSERT_NE(q1,q2);

  q1.enqueue("c");
  ASSERT_NE(q1,q2);

  q2.enqueue("c");
  ASSERT_NE(q1,q2);

  q2.enqueue("b");
  ASSERT_NE(q1,q2);

  q2.enqueue("a");
  ASSERT_EQ(q1,q2);

  q2.enqueue("d");
  ASSERT_NE(q1,q2);

  q2.enqueue("e");
  ASSERT_NE(q1,q2);

  q1.enqueue("e");
  ASSERT_NE(q1,q2);

  q1.enqueue("d");
  ASSERT_EQ(q1,q2);

  q1.enqueue("x");
  ASSERT_NE(q1,q2);

  q1.enqueue("y");
  ASSERT_NE(q1,q2);

  ASSERT_EQ(q1,q1);
  ASSERT_EQ(q2,q2);
}


TEST_F(PriorityQueueTest, operator_stream_insert) {// <<
  std::ostringstream value;
  PriorityQueueType q(gt_string);
  value << q;
  ASSERT_EQ("priority_queue[]:highest", value.str());

  value.str("");
  q.enqueue("c");
  value << q;
  ASSERT_EQ("priority_queue[c]:highest", value.str());

  value.str("");
  q.enqueue("b");
  value << q;
  ASSERT_EQ("priority_queue[c,b]:highest", value.str());

  value.str("");
  q.enqueue("d");
  value << q;
  ASSERT_EQ("priority_queue[d,c,b]:highest", value.str());

  value.str("");
  q.enqueue("a");
  value << q;
  ASSERT_EQ("priority_queue[d,c,b,a]:highest", value.str());

  value.str("");
  q.enqueue("e");
  value << q;
  ASSERT_EQ("priority_queue[e,d,c,b,a]:highest", value.str());
}


TEST_F(PriorityQueueTest, enqueue_iterator_param) {
  PriorityQueueType q(gt_string),q1(gt_string);
  load(q1,"fcijbdegah");
  q.enqueue(q1.ibegin(),q1.iend());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(10, q.size());
  ASSERT_EQ(q,q1);
}


TEST_F(PriorityQueueTest, clear) {
  PriorityQueueType q(gt_string);
  q.clear();
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
  ASSERT_THROW(q.peek(),ics::EmptyError);

  load(q,"f");
  q.clear();
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
  ASSERT_THROW(q.peek(),ics::EmptyError);

  load(q,"fc");
  q.clear();
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
  ASSERT_THROW(q.peek(),ics::EmptyError);

  load(q,"fci");
  q.clear();
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
  ASSERT_THROW(q.peek(),ics::EmptyError);

  load(q,"fcij");
  q.clear();
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
  ASSERT_THROW(q.peek(),ics::EmptyError);

  load(q,"fcijb");
  q.clear();
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
  ASSERT_THROW(q.peek(),ics::EmptyError);
}


TEST_F(PriorityQueueTest, dequeue1) {
  PriorityQueueType q(gt_string);
  load(q,"fcijbdegah");
  ASSERT_EQ("a",q.dequeue());
  ASSERT_EQ("b",q.dequeue());
  ASSERT_EQ("c",q.dequeue());
  ASSERT_EQ("d",q.dequeue());
  ASSERT_EQ("e",q.dequeue());
  ASSERT_EQ("f",q.dequeue());
  ASSERT_EQ("g",q.dequeue());
  ASSERT_EQ("h",q.dequeue());
  ASSERT_EQ("i",q.dequeue());
  ASSERT_EQ("j",q.dequeue());
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
  ASSERT_THROW(q.peek(),ics::EmptyError);
}


TEST_F(PriorityQueueTest, dequeue2) {
  PriorityQueueType q(gt_string);
  load(q,"fcijbdegah");
  ASSERT_EQ("a",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(9, q.size());
  ASSERT_EQ("b", q.peek());

  ASSERT_EQ("b",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(8, q.size());
  ASSERT_EQ("c", q.peek());

  ASSERT_EQ("c",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(7, q.size());
  ASSERT_EQ("d", q.peek());

  ASSERT_EQ("d",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(6, q.size());
  ASSERT_EQ("e", q.peek());

  ASSERT_EQ("e",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(5, q.size());
  ASSERT_EQ("f", q.peek());

  ASSERT_EQ("f",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(4, q.size());
  ASSERT_EQ("g", q.peek());

  ASSERT_EQ("g",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(3, q.size());
  ASSERT_EQ("h", q.peek());

  ASSERT_EQ("h",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(2, q.size());
  ASSERT_EQ("i", q.peek());

  ASSERT_EQ("i",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(1, q.size());
  ASSERT_EQ("j", q.peek());

  ASSERT_EQ("j",q.dequeue());
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
  ASSERT_THROW(q.peek(),ics::EmptyError);
}


TEST_F(PriorityQueueTest, assignment) {
  PriorityQueueType q1(nullptr),q2(gt_string);
  load(q2,"fcijb");
  q1 = q2;
  ASSERT_EQ(q1,q2);

  q2.clear();
  load(q2,"ab");
  q1 = q2;
  ASSERT_EQ(q1,q2);

  q2.clear();
  load(q2,"fcijbdegabh");
  q1 = q2;
  ASSERT_EQ(q1,q2);

}


TEST_F(PriorityQueueTest, iterator_simple) {
  std::string qvalues[] ={"a","b","c","d","e","f","g","h","i","j"};
  PriorityQueueType q(gt_string);
  load(q,"fcijbdegah");

  //for-each iterator (using .begin/.end)
  int i = 0;
  for (std::string x : q) {
    ASSERT_EQ(qvalues[i++],x);
  }
  ASSERT_EQ(10,q.size());

  //explicit iterator (using .ibegin/.iend and ++it)
  i = 0;
  for (ics::Iterator<std::string>& it = q.ibegin(); it != q.iend(); ++it)
    ASSERT_EQ(qvalues[i++],*it);
  ASSERT_EQ(10,q.size());

  //explicit iterator (using .ibegin/.iend and it++)
  i = 0;
  for (ics::Iterator<std::string>& it = q.ibegin(); it != q.iend(); it++)
    ASSERT_EQ(qvalues[i++],*it);
  ASSERT_EQ(10,q.size());

  //all these iterations didn't change the queue
  i = 0;
  ASSERT_EQ(10,q.size());
  while (!q.empty())
    ASSERT_EQ(qvalues[i++],q.dequeue());
  ASSERT_EQ(0,q.size());
}


TEST_F(PriorityQueueTest, iterator_erase) {
  std::string qvalues[] ={"a","b","c","d","e","f","g","h","i","j"};
  PriorityQueueType q(gt_string);
  load(q,"fcijbdegah");
  ics::Iterator<std::string>& it = q.ibegin();

  ASSERT_EQ("a", it.erase());
  ASSERT_THROW(it.erase(),ics::CannotEraseError);
  ASSERT_THROW(*it,ics::IteratorPositionIllegal);

  ++it; //to "b"

  ++it; //to "c"
  ASSERT_EQ("c", it.erase());
  ASSERT_THROW(it.erase(),ics::CannotEraseError);
  ASSERT_THROW(*it,ics::IteratorPositionIllegal);
  ++it; //to "d"
  ASSERT_EQ("d", it.erase());
  ASSERT_THROW(it.erase(),ics::CannotEraseError);
  ASSERT_THROW(*it,ics::IteratorPositionIllegal);

  ++it; //to "e"

  ++it; //to "f"
  ASSERT_EQ("f", it.erase());
  ASSERT_THROW(it.erase(),ics::CannotEraseError);
  ASSERT_THROW(*it,ics::IteratorPositionIllegal);

  ++it; //to "g"
  ++it; //to "h"

  ++it; //to "i"
  ASSERT_EQ("i", it.erase());
  ASSERT_THROW(it.erase(),ics::CannotEraseError);
  ASSERT_THROW(*it,ics::IteratorPositionIllegal);
  ++it; //to "j"
  ASSERT_EQ("j", it.erase());
  ASSERT_THROW(it.erase(),ics::CannotEraseError);
  ASSERT_THROW(*it,ics::IteratorPositionIllegal);

  ++it; //beyond last
  ASSERT_THROW(it.erase(),ics::CannotEraseError);
  ASSERT_THROW(*it,ics::IteratorPositionIllegal);
  std::string remaining[] ={"b","e","g","h"};
  int i = 0;
  for (std::string x : q)
    ASSERT_EQ(remaining[i++],x);


  //erase all in the queue
  q.clear();
  load(q,"fcijbdegah");
  i = 0;
  for (ics::Iterator<std::string>& it = q.ibegin(); it != q.iend(); ++it)
    ASSERT_EQ(qvalues[i++],it.erase());
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0,q.size());
}


TEST_F(PriorityQueueTest, iterator_erase_heap_special_case) {
  PriorityQueueType q(gt_string);
  load(q,"adbefc");
  for (ics::Iterator<std::string>& i = q.ibegin(); i != q.iend(); ++i)
    if (*i == "f") {
      i.erase();
      break;
    }
  ASSERT_TRUE(unload(q,"abcde"));
}


TEST_F(PriorityQueueTest, iterator_exception_concurrent_modification_error) {
  PriorityQueueType q(gt_string);
  load(q,"fcijbdegabh");
  ics::Iterator<std::string>& it = q.ibegin();

  q.dequeue();
  ASSERT_THROW(it.erase(),ics::ConcurrentModificationError);
  ASSERT_THROW(++it,ics::ConcurrentModificationError);
  ASSERT_THROW(it++,ics::ConcurrentModificationError);
  ASSERT_THROW(*it,ics::ConcurrentModificationError);
}


TEST_F(PriorityQueueTest, constructors) {
  PriorityQueueType q(gt_string);
  load(q,"fcijbdegah");
  ASSERT_TRUE(unload(q,"abcdefghij"));

  load(q,"fcijbdegah");
  PriorityQueueType q2(q);
  ASSERT_EQ(q,q2);
  ASSERT_TRUE(unload(q, "abcdefghij"));
  ASSERT_NE(q,q2);
  ASSERT_TRUE(unload(q2,"abcdefghij"));
  ASSERT_EQ(q,q2);

  load(q,"fcijbdegah");
  PriorityQueueType q3({"a","b","c","d","e","f","g","h","i","j"},lt_string);   //Ordered oppositely
  ASSERT_TRUE(unload(q, "abcdefghij"));
  ASSERT_NE(q,q3);
  ASSERT_TRUE(unload(q3,"jihgfedcba"));
  ASSERT_NE(q,q3);

  load(q,"fcijbdegah");
  PriorityQueueType q4(q.ibegin(),q.iend(),lt_string);   //Ordered oppositely
  ASSERT_TRUE(unload(q, "abcdefghij"));
  ASSERT_NE(q,q4);
  ASSERT_TRUE(unload(q4,"jihgfedcba"));
  ASSERT_NE(q,q4);
}


TEST_F(PriorityQueueTest, large_scale) {
  PriorityQueueType2 lq(gt_int);
  ics::ArrayPriorityQueue<int> lq_ref(gt_int);


  std::vector<int> values;
  for (int i=0; i<test_size; ++i)
    values.push_back(i);
   std::random_shuffle(values.begin(),values.end());


   for (int test=1; test<=5; ++test) {
    int enqueued = 0;
    int dequeued = 0;
    while (dequeued != test_size) {
      int to_enqueue = ics::rand_range(0,test_size-enqueued);
      if (trace)
        std::cout << "Enqueue " << to_enqueue << std::endl;
      for (int i=0; i <to_enqueue; ++i) {
        lq_ref.enqueue(values[enqueued]);
        ASSERT_EQ(1,lq.enqueue(values[enqueued++]));
      }
      ics::Iterator<int>& i = lq_ref.ibegin();
      for (int v : lq) {
        ASSERT_EQ(*i,v);
        ++i;
      }

      int to_dequeue = ics::rand_range(0,enqueued-dequeued);
      if (trace)
        std::cout << "Dequeue " << to_dequeue << std::endl;
      for (int i=0; i <to_dequeue; ++i) {
        ASSERT_EQ(lq_ref.peek(),lq.peek());
        ASSERT_EQ(lq_ref.dequeue(),lq.dequeue());
        ++dequeued;
      }
      ics::Iterator<int>& j = lq_ref.ibegin();
      for (int v : lq) {
        ASSERT_EQ(*j,v);
        ++j;
      }
    }
  }
  ASSERT_TRUE(lq.empty());
  ASSERT_EQ(0,lq.size());

}


TEST_F(PriorityQueueTest, large_scale_speed1) {
  PriorityQueueType2 lq(gt_int);

  std::vector<int> values;
  for (int i=0; i<speed_size; ++i)
    values.push_back(i);
  std::random_shuffle(values.begin(),values.end());

  for (int i=0; i <speed_size; ++i)
        lq.enqueue(values[i]);
  for (int i=0; i <speed_size; ++i)
        lq.dequeue();
}


TEST_F(PriorityQueueTest, large_scale_speed2) {
  PriorityQueueType2 lq(gt_int);

  std::vector<int> values;
  for (int i=0; i<speed_size; ++i)
    values.push_back(i);
  std::random_shuffle(values.begin(),values.end());

  for (int test=1; test<=5; ++test) {
    int enqueued = 0;
    int dequeued = 0;
    while (dequeued != speed_size) {
      int to_enqueue = ics::rand_range(0,speed_size-enqueued);
      for (int i=0; i <to_enqueue; ++i)
        lq.enqueue(values[enqueued++]);
      int to_dequeue = ics::rand_range(0,enqueued-dequeued);
      for (int i=0; i <to_dequeue; ++i) {
        lq.dequeue();
        ++dequeued;
      }
    }
  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

