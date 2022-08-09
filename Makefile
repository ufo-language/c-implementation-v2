# assumes linter is installed:
# sudo apt install cppcheck
lint:
	cppcheck --enable=all --language=c --std=c11 --platform=unix64 -I src --suppress=missingIncludeSystem -I src --check-config src 2> lintresult.txt
