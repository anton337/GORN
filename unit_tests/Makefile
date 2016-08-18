all:
	${GORN_CXX} ${GORN_CXX_FLAGS} -Wno-unused-variable test_system.cpp -o ${GORN_BIN}test_system -I${GORN_UTILS} -lboost_system -lboost_thread -lpthread;
	${GORN_CXX} ${GORN_CXX_FLAGS} test_sort.cpp -o ${GORN_BIN}test_sort -I${GORN_UTILS} -lboost_system -lboost_thread -lboost_filesystem -lpthread;
	${GORN_CXX} ${GORN_CXX_FLAGS} test_crawler.cpp -o ${GORN_BIN}test_crawler -I${GORN_UTILS} -lboost_system -lboost_thread -lboost_filesystem -lpthread;
	${GORN_CXX} ${GORN_CXX_FLAGS} test_serializer.cpp -o ${GORN_BIN}test_serializer -I${GORN_UTILS} -lboost_system -lboost_thread -lboost_filesystem -lpthread;
	
