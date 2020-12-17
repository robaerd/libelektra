BASE_DIR=$1
VERSION=$2
SUFFIX=$3

log_strace() {
	CONTEXT=$1
	mkdir $BASE_DIR/$VERSION/$CONTEXT

	strace -o $BASE_DIR/$VERSION/$CONTEXT/mount.strace kdb mount file.ecf user:/release_test
	strace -o $BASE_DIR/$VERSION/$CONTEXT/file.strace kdb file user:/release_test/b
	strace -o $BASE_DIR/$VERSION/$CONTEXT/set.strace kdb set user:/release_test/b
	strace -o $BASE_DIR/$VERSION/$CONTEXT/get.strace kdb get user:/release_test/b
	strace -o $BASE_DIR/$VERSION/$CONTEXT/rm.strace kdb rm user:/release_test/b
	strace -o $BASE_DIR/$VERSION/$CONTEXT/umount.strace kdb umount user:/release_test
}

run_log_tests() {
	CONTEXT=$1
	mkdir $BASE_DIR/$VERSION/$CONTEXT

	KDB=kdb kdb run_all -v 2>&1 | tee $BASE_DIR/$VERSION/$CONTEXT/run_all
	check_test_amount $BASE_DIR/$VERSION/$CONTEXT/run_all

	KDB=kdb-full kdb-full run_all > $BASE_DIR/$VERSION/$CONTEXT/run_all_full 2>&1
	check_test_amount $BASE_DIR/$VERSION/$CONTEXT/run_all_full

	KDB=kdb-static kdb-static run_all > $BASE_DIR/$VERSION/$CONTEXT/run_all_static 2>&1
	check_test_amount $BASE_DIR/$VERSION/$CONTEXT/run_all_static
}

log_strace "strace-${SUFFIX}"
run_log_tests "test-${SUFFIX}"