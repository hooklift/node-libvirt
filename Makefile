REPORTER ?= spec
TESTS = $(shell find ./test/* -name "*.test.js")
NPM_BIN = ./node_modules/.bin

jshint:
	$(NPM_BIN)/jshint lib test

fixjsstyle:
	fixjsstyle -r lib -r test --strict --jslint_error=all

test:	jshint
	@if [ "$$GREP" ]; then \
		$(NPM_BIN)/mocha --globals setImmediate,clearImmediate --check-leaks --colors -t 10000 --reporter $(REPORTER) -g "$$GREP" $(TESTS); \
	else \
		$(NPM_BIN)/mocha --globals setImmediate,clearImmediate --check-leaks --colors -t 10000 --reporter $(REPORTER) $(TESTS); \
	fi

.PHONY: jshint fixjsstyle test