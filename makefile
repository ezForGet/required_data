TARGET = a.out


CC = g++ 


CFLAGS = -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format matrix.cpp main.cpp

SRC = main.cpp matrix.cpp


OBJ = $(SRC:.cpp=.o)


$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)


main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp


matrix.o: matrix.cpp
	$(CC) $(CFLAGS) -c matrix.cpp

# Цель для запуска тестов
test: $(TARGET)
	@echo "Запуск тестов..."
	@for test in tests/*.sh; do \
		echo "Выполняется $$test..."; \
		bash $$test || exit 1; \
	done
	@echo "Все тесты выполнены успешно!"

clean:
	rm -f $(OBJ) $(TARGET)