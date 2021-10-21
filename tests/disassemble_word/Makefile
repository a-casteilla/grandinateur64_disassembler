default: run

disassembler:
	cd ../.. ;\
	mv config.h config.h.bak ;\
	sed "s/\/\/#define unit_test_of_disassemble_word/#define \
	unit_test_of_disassemble_word/" < config.h.bak > config.h ;\
	make clean ; \
	make ; \
	mv disassembler tests/disassemble_word/ ; \
	make clean ; \
	mv config.h.bak config.h

run: disassembler
	./disassembler < input_aschex.txt > out.txt
	vimdiff out.txt expected_output.txt

clean:
	rm disassembler out.txt
