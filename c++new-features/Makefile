LDFLAGS += -pthread

all: move_semantics strategy_pattern producer_consumer

move_semantics: move_semantics.cpp

strategy_pattern: strategy_pattern.cpp

producer_consumer: producer_consumer.cpp

thr_sanitize:
	g++ -pthread -fsanitize=thread  producer_consumer.cpp
	./a.out
