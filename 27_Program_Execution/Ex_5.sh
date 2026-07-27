# ------------------------------------------------------------------------------
# Test harnass for Exercise 27-5
# ------------------------------------------------------------------------------

echo "Run program with standard buffering on stdout"

./Ex_5 $@

echo "Run program with no buffering on stdout"

./Ex_5 $@ -n
