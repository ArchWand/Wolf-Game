SRC=run.cpp game.cpp

out: ${SRC}
	g++ ${SRC} -o out

.PHONY: debug
debug:
	g++ ${SRC} -o out -g -fsanitize=address

.PHONY: fast
fast:
	g++ ${SRC} -o out -O2

.PHONY: clean
clean:
	rm -f out

.PHONY: run
run: out
	./out gamefile.txt
