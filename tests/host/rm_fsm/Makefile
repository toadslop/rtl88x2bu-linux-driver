ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
	-I$(HOST_INC) -DHOST_RM_FSM_TEST -DCONFIG_RTW_80211K
.PHONY: all test clean
all test: test_rm_fsm
	./test_rm_fsm rm_fsm_vectors.json
test_rm_fsm: test_rm_fsm.c rm_fsm_c_oracle.c rm_fsm_queue_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rm_fsm.c rm_fsm_c_oracle.c rm_fsm_queue_oracle.c $(HOST_JSON)
clean:
	rm -f test_rm_fsm
