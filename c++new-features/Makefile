CXX = g++-7
CPPFLAGS += -std=c++17
LDFLAGS += -pthread

all: move_semantics strategy_pattern producer_consumer check_atomic_shared_ptr sleep_for spinlock timer_events partial_specialization

move_semantics: move_semantics.cpp

strategy_pattern: strategy_pattern.cpp

producer_consumer: producer_consumer.cpp

check_atomic_shared_ptr: check_atomic_shared_ptr.cpp

sleep_for: sleep_for.cpp

spinlock: spinlock.cpp

timer_events: timer_events.cpp

shared_mutex: shared_mutex.cpp

partial_specialization: partial_specialization.cpp

spec: spec.cpp
	g++ -pthread spec.cpp -o spec -lgtest

thr_sanitize:
	g++ -pthread -fsanitize=thread  producer_consumer.cpp
	./a.out
