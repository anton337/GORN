all:
	make clean;
	g++ -std=c++11 HashTableDaemon.cpp -o HashTableDaemon -lboost_thread -lboost_system -lpthread;
	./script.sh 32000

clean:
	rm -rf HashTableDaemon
