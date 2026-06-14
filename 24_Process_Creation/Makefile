.PHONY: all test_1 test_2 test_3 test_4 test_5

all: test_1 test_2 test_3 test_4 test_5

test_1:
	@echo "There are eight processes created:"
	@echo "Parent executes fork()"
	@echo "Parent and child_1 execute fork()"
	@echo "Parent, child_1, child_2, and child_1_1 execute fork()"
	@echo "Now Parent, child_1, child_2, child_3, child_1_1, child_1_2, child_2_1, and child_1_1_1 are executing."
	@echo ""
	@echo "The process tree is:"
	@echo "Parent +-> child_1 +-> child_1_1 -> child_1_1_1"
	@echo "       |           +-> child_1_2"
	@echo "       +-> child_2  -> child_2_1"
	@echo "       +-> child_3"